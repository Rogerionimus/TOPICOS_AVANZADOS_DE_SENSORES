#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuración Del display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración del MPU6050
const int MPU_ADDR = 0x68;
int16_t AcX, AcY, AcZ;

// Establecer la lógica de los cronometros
unsigned long tiemposTotales[6] = {0, 0, 0, 0, 0, 0}; 
unsigned long tiempoUltimaLectura = 0;
int caraActual = -1;

void setup() {
  Serial.begin(9600); // Trabajaremos a 9600 Baudios
  Wire.begin();

  // Inicializar Pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;); // Bloquear si no hay pantalla
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Iniicializar el MPU
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  tiempoUltimaLectura = millis();
}

void loop() {
  leerAcelerometro();

  int caraDetectada = determinarCara();
  unsigned long tiempoAhora = millis();
  unsigned long deltaTiempo = tiempoAhora - tiempoUltimaLectura;
  tiempoUltimaLectura = tiempoAhora;

  // Lógica del sumador de tiempo
  if (caraDetectada != -1) {
    tiemposTotales[caraDetectada] += deltaTiempo;
    caraActual = caraDetectada;
  } else {
    caraActual = -1;
  }

  // Actualizamos la pantalla cada 200ms para no saturar el bus I2C
  static unsigned long ultimaOLED = 0;
  if (millis() - ultimaOLED > 200) {
    actualizarPantalla();
    ultimaOLED = millis();
  }
}

void leerAcelerometro() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  
  if (Wire.available() >= 6) {
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
  }
}

int determinarCara() {
  int umbral = 12000; // Sensibilidad para Arduino
  if (AcZ > umbral)  return 0; // Cara 1
  if (AcZ < -umbral) return 1; // Cara 2
  if (AcY > umbral)  return 2; // Cara 3
  if (AcY < -umbral) return 3; // Cara 4
  if (AcX > umbral)  return 4; // Cara 5
  if (AcX < -umbral) return 5; // Cara 6
  return -1; 
}

void actualizarPantalla() {
  display.clearDisplay();
  
  display.setCursor(15, 0);
  display.print(F("CRONO-DADO"));
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  for (int i = 0; i < 6; i++) {
    int col = (i < 3) ? 0 : 65;
    int row = 15 + (i % 3) * 15;
    
    display.setCursor(col, row);
    if (i == caraActual) display.print(F(">"));
    else display.print(F(" "));
    
    display.print(F("C"));
    display.print(i + 1);
    display.print(F(":"));
    
    unsigned long segTotales = tiemposTotales[i] / 1000;
    display.print(segTotales / 60);
    display.print(F("m"));
    display.print(segTotales % 60);
  }

  display.display();
}