#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <Adafruit_BMP085.h>
#include <MPU6050_tockn.h>
#include <TinyGPS++.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_VL53L0X.h> // <-- NUEVO

// --- Tiempos de Muestreo ---
unsigned long lastMPU = 0;   
unsigned long lastBMP = 0;   
unsigned long lastGPS = 0;   
unsigned long lastEnv = 0;   
unsigned long lastLog = 0;   

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

typedef struct struct_telemetria {
  uint32_t tiempo;
  float ax, ay, az;
  float presion, altitud;
  float lat, lon;
  float gas;      // <-- Cambiado a float para Voltaje o aproximación
  float temp_int; // <-- Cambiado a float para °C
  float humedad_digital;
  uint16_t distancia_inf;
} struct_telemetria;

struct_telemetria telemetria;
esp_now_peer_info_t peerInfo;

// --- Sensores ---
Adafruit_BMP085 bmp;
MPU6050 mpu6050(Wire);
Adafruit_AHTX0 aht;    
Adafruit_VL53L0X lox = Adafruit_VL53L0X(); // <-- NUEVO
TinyGPSPlus gps;

bool bmpDisponible = false;
bool ahtDisponible = false;
bool loxDisponible = false; // <-- NUEVO

int adc_temp_raw = 0;
int adc_gas_raw = 0;

float b_presion = 0, b_altitud = 0;
float g_lat = 0, g_lon = 0;

void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // GPS
  
  Wire.begin(21, 22);   // Canal principal (MPU, BMP, VL53)
  Wire1.begin(33, 25);  // Canal secundario (AHT20)
  delay(200);           

  if (!LittleFS.begin(true)) {
    Serial.println("Error al montar LittleFS.");
  } else {
    File file = LittleFS.open("/telemetria.csv", FILE_WRITE);
    if(file) {
      file.println("Tiempo(ms),Ax,Ay,Az,Presion(Pa),Altitud(m),Lat,Lon,Gas(ADC),Temp_Int(ADC),Hum(%),Dist(mm)");
      file.close();
    }
  }

  // Inicializar BMP180
  int intentos = 0;
  while (!bmp.begin() && intentos < 3) { delay(100); intentos++; }
  if (intentos >= 3) bmpDisponible = false;
  else bmpDisponible = true;
  
  // Inicializar MPU6050
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  // Inicializar AHT20
  if (!aht.begin(&Wire1)) ahtDisponible = false;
  else ahtDisponible = true;

  // Inicializar VL53L0X <-- NUEVO
  if (!lox.begin()) {
    Serial.println("VL53L0X no detectado.");
    loxDisponible = false;
  } else {
    Serial.println("VL53L0X en línea.");
    loxDisponible = true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); 
  if (esp_now_init() != ESP_OK) Serial.println("Error ESP-NOW");
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

void loop() {
  unsigned long currentMillis = millis();

  // --- MPU6050 (20Hz) ---
  if (currentMillis - lastMPU >= 50) {
    mpu6050.update();
    telemetria.ax = mpu6050.getAccX();
    telemetria.ay = mpu6050.getAccY();
    telemetria.az = mpu6050.getAccZ();
    lastMPU = currentMillis;
  }

  // --- VL53L0X (10Hz) <-- NUEVO ---
  static unsigned long lastLOX = 0;
  if (currentMillis - lastLOX >= 100) {
    if (loxDisponible) {
      VL53L0X_RangingMeasurementData_t measure;
      lox.rangingTest(&measure, false);
      if (measure.RangeStatus != 4) telemetria.distancia_inf = measure.RangeMilliMeter;
      else telemetria.distancia_inf = 9999;
    } else {
      telemetria.distancia_inf = 0;
    }
    lastLOX = currentMillis;
  }

  // --- BMP180 (5Hz) ---
  if (currentMillis - lastBMP >= 200) {
    if (bmpDisponible) {
      float p = bmp.readPressure();
      if (p > 10000) { 
        b_presion = p;
        b_altitud = bmp.readAltitude(101325);
      } else { b_presion = -1; b_altitud = -1; }
    } else { b_presion = -1; b_altitud = -1; }
    telemetria.presion = b_presion;
    telemetria.altitud = b_altitud;
    lastBMP = currentMillis;
  }

  // --- GPS ---
  while (Serial2.available() > 0) gps.encode(Serial2.read());
  if (currentMillis - lastGPS >= 1000) {
    if (gps.location.isValid()) {
      g_lat = gps.location.lat();
      g_lon = gps.location.lng();
    } else { g_lat = 0.0; g_lon = 0.0; }
    telemetria.lat = g_lat;
    telemetria.lon = g_lon;
    lastGPS = currentMillis;
  }

  // --- Entorno Analógico en Unidades Reales y Digital (0.5Hz) ---
  if (currentMillis - lastEnv >= 2000) {
    // 1. Conversión del LM35 (Temperatura)
    adc_temp_raw = analogRead(35); // Guardamos el valor crudo
    float voltajeLM35 = (adc_temp_raw * 3.3) / 4095.0;
    telemetria.temp_int = voltajeLM35 * 100.0; // Grados Celsius

    // 2. Conversión del MQ-135 (Gas)
    adc_gas_raw = analogRead(34);  // Guardamos el valor crudo
    telemetria.gas = (adc_gas_raw * 5.0) / 4095.0; // Voltaje de señal (0V - 5V)
    
    if (ahtDisponible) {
      sensors_event_t humidity, temp;
      aht.getEvent(&humidity, &temp);
      telemetria.humedad_digital = humidity.relative_humidity;
    } else { telemetria.humedad_digital = -1; }
    lastEnv = currentMillis;
  }

  // --- Almacenamiento, Envío e Impresión Serial Detallada (5Hz) ---
  if (currentMillis - lastLog >= 200) {
    telemetria.tiempo = currentMillis;

    esp_now_send(broadcastAddress, (uint8_t *) &telemetria, sizeof(telemetria));

    File file = LittleFS.open("/telemetria.csv", FILE_APPEND);
    if(file) {
      file.printf("%u,%.2f,%.2f,%.2f,%.1f,%.2f,%.6f,%.6f,%d,%d,%.1f,%u\n", 
                  telemetria.tiempo, telemetria.ax, telemetria.ay, telemetria.az, 
                  telemetria.presion, telemetria.altitud, telemetria.lat, telemetria.lon, 
                  telemetria.gas, telemetria.temp_int, telemetria.humedad_digital, telemetria.distancia_inf);
      file.close();
    }
    
    // =================================================================
    // IMPRESIÓN DETALLADA CON COMPARATIVA ADC VS CONVERTIDO
    // =================================================================
    Serial.print("--- TELEMETRÍA CanSat ["); Serial.print(telemetria.tiempo); Serial.println(" ms] ---");
    
    // Dinámica y Distancia
    Serial.print("  [MPU-6050] Accel X: "); Serial.print(telemetria.ax); 
    Serial.print(" | Y: "); Serial.print(telemetria.ay); 
    Serial.print(" | Z: "); Serial.println(telemetria.az);
    Serial.print("  [VL53L0X]  Distancia al suelo: "); Serial.print(telemetria.distancia_inf); Serial.println(" mm");
    
    // Navegación y Altitud
    Serial.print("  [BMP180]   Presión: "); Serial.print(telemetria.presion); Serial.print(" Pa");
    Serial.print(" | Altitud Relativa: "); Serial.print(telemetria.altitud); Serial.println(" m");
    Serial.print("  [NEO-6M]   Latitud: "); Serial.print(telemetria.lat, 6); 
    Serial.print(" | Longitud: "); Serial.println(telemetria.lon, 6);

    // Entorno, Gases y Comparativa Analógica
    Serial.print("  [AHT20]    Humedad Digital: "); Serial.print(telemetria.humedad_digital, 1); Serial.println(" %");
    
    // LM35 Comparativo
    Serial.print("  [LM35]     Temperatura  -> Crudo ADC: "); Serial.print(adc_temp_raw);
    Serial.print(" | Calculado: "); Serial.print(telemetria.temp_int, 1); Serial.println(" *C");
    
    // MQ-135 Comparativo
    Serial.print("  [MQ-135]   Señal de Gas -> Crudo ADC: "); Serial.print(adc_gas_raw);
    Serial.print(" | Calculado: "); Serial.print(telemetria.gas, 2); Serial.println(" V");
    
    Serial.println("------------------------------------------------");
    // =================================================================

    lastLog = currentMillis;
  }
}