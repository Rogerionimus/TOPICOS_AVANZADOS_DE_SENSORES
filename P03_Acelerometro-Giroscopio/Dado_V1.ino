#include <Wire.h>

const int MPU_ADDR = 0x68;

//Datos del sensor
int16_t AcX, AcY, AcZ;

//Lógica inicial de cronometros
unsigned long tiemposTotales[6] = {0, 0, 0, 0, 0, 0};
unsigned long tiempoUltimaLectura = 0;
int caraActual = -1;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  tiempoUltimaLectura = millis();
  Serial.println("Crono-Dado iniciado");
}

void loop() {
  leerAcelerometro();

  int caraDetectada = determinarCara();
  unsigned long tiempoAhora = millis();
  unsigned long deltaTiempo = tiempoAhora - tiempoUltimaLectura;
  tiempoUltimaLectura = tiempoAhora;

  // lógica del sumador para el temporizador al estar en una cara
  if (caraDetectada != -1) {
    tiemposTotales[caraDetectada] += deltaTiempo;
    
    //Si el dado cambia de posición o transcurre 1 segundo se imprime el estado.
    static unsigned long ultimoReporte = 0;
    if (caraDetectada != caraActual || (millis() - ultimoReporte > 1000)) {
      caraActual = caraDetectada;
      imprimirTiempos();
      ultimoReporte = millis();
    }
  } else {
    // si el dado está inclinado no se suma tiempo a ninguna cara
    caraActual = -1;
  }

  delay(50);  //muestreo de 20Hz  1/(50x10^-3)
}

void leerAcelerometro() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
}

int determinarCara() {
  int umbral = 12000;
  if (AcZ > umbral)  return 0; // Cara 1
  if (AcZ < -umbral) return 1; // Cara 2
  if (AcY > umbral)  return 2; // Cara 3
  if (AcY < -umbral) return 3; // Cara 4
  if (AcX > umbral)  return 4; // Cara 5
  if (AcX < -umbral) return 5; // Cara 6
  return -1; 
}

void imprimirTiempos() {
  Serial.println("\n===== TIEMPO ACUMULADO POR CARA =====");
  for (int i = 0; i < 6; i++) {
    Serial.print("Cara ");
    Serial.print(i + 1);
    Serial.print(": ");
    
    float segundos = tiemposTotales[i] / 1000.0;
    Serial.print(segundos, 1);
    Serial.println(" seg");
  }
  
  if (caraActual != -1) {
    Serial.print(">>> Cronometrando CARA: ");
    Serial.println(caraActual + 1);
  } else {
    Serial.println(">>> Dado en posición NO VÁLIDA");
  }
}