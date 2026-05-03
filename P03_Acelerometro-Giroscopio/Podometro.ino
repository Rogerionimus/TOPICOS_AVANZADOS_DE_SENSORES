#include <Wire.h>

const int MPU_addr = 0x68;
float ax, ay, az;
float magnitude, magnitude_old;
int steps = 0;

//configuración de umbrales para la sensibilidad de detección
const float threshold_up = 1.30;   // Umbral para detectar subida (en Gs)
const float threshold_down = 0.90; // Umbral para detectar bajada
bool peak_reached = false;
unsigned long last_step_time = 0;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);

  // Leer y convertir a unidades de gravedad (G)
  // El rango por defecto es +/- 2g (16384 LSB/g)
  ax = (int16_t)(Wire.read() << 8 | Wire.read()) / 16384.0;
  ay = (int16_t)(Wire.read() << 8 | Wire.read()) / 16384.0;
  az = (int16_t)(Wire.read() << 8 | Wire.read()) / 16384.0;

  // Calculamos la magnitud del vector de aceleraciones
  magnitude = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));

  // Lógica de conteo con histéresis y tiempo muerto
  if (magnitude > threshold_up && !peak_reached && (millis() - last_step_time > 350)) {
    peak_reached = true; 
  }
  
  if (magnitude < threshold_down && peak_reached) {
    steps++;
    peak_reached = false;
    last_step_time = millis();
    
    Serial.print("Un paso más! Total: ");
    Serial.println(steps);
  }

  delay(20); // Muestreo a 50Hz
}