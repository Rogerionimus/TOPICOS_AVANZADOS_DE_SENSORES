// Librerías para la comunicación I2C con el sensor MPU6050
// MPU6050.h requiere I2Cdev.h y esta a su vez depende de Wire.h
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// Creación del objeto que representa al sensor MPU6050
// La dirección puede ser 0x68 o 0x69 según el pin AD0
MPU6050 mpu;

// Variables para almacenar las lecturas crudas del acelerómetro
// correspondientes a los ejes X, Y y Z
int accX, accY, accZ;

void setup() {
  Serial.begin(57600);      // Inicialización de la comunicación serial
  Wire.begin();             // Inicio del bus de comunicación I2C
  mpu.initialize();         // Configuración inicial del sensor

  // Verificación de la conexión con el dispositivo
  if (mpu.testConnection()) Serial.println("MPU6050 conectado correctamente");
  else Serial.println("No se pudo establecer comunicacion con el MPU6050");
}

void loop() {
  // Obtener las mediciones del acelerómetro
  mpu.getAcceleration(&accX, &accY, &accZ);

  // Cálculo de los ángulos de inclinación en los ejes X y Y
  float anguloX = atan(accX / sqrt(pow(accY,2) + pow(accZ,2))) * (180.0 / 3.14);
  float anguloY = atan(accY / sqrt(pow(accX,2) + pow(accZ,2))) * (180.0 / 3.14);

  // Enviar los valores calculados al monitor serial separados por tabulación
  //Serial.print("Angulo eje X: ");
  Serial.print(anguloX);
  Serial.print(" ");
  //Serial.print("  Angulo eje Y: ");
  Serial.println(anguloY);

  delay(10);  // Pequeña pausa antes de la siguiente lectura
}