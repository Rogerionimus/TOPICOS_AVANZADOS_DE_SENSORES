// Librerías utilizadas para la comunicación I2C con el MPU6050
// MPU6050.h depende de I2Cdev.h y esta última utiliza Wire.h
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// Creación del objeto que representa al sensor
// La dirección I2C puede ser 0x68 o 0x69 según el estado del pin AD0
MPU6050 mpu;

// Variables donde se almacenan las lecturas crudas
// del acelerómetro y del giroscopio en los ejes X, Y y Z
int accX, accY, accZ;
int gyroX, gyroY, gyroZ;

// Variables empleadas para el cálculo del tiempo y de los ángulos
long tiempoAnterior;
float deltaTiempo;
float anguloX, anguloY;
float anguloX_prev, anguloY_prev;

void setup() {
  Serial.begin(57600);     // Inicialización de la comunicación serial
  Wire.begin();            // Activación del bus I2C
  mpu.initialize();        // Configuración inicial del sensor

  // Verificación de la conexión con el MPU6050
  if (mpu.testConnection()) Serial.println("MPU6050 conectado correctamente");
  else Serial.println("No se pudo iniciar el MPU6050");
}

void loop() {

  // Obtener mediciones del acelerómetro y del giroscopio
  mpu.getAcceleration(&accX, &accY, &accZ);
  mpu.getRotation(&gyroX, &gyroY, &gyroZ);
  
  // Cálculo del intervalo de tiempo entre iteraciones
  deltaTiempo = (millis() - tiempoAnterior) / 1000.0;
  tiempoAnterior = millis();
  
  // Estimación del ángulo usando el acelerómetro
  float angAccelX = atan(accY / sqrt(pow(accX,2) + pow(accZ,2))) * (180.0 / 3.14);
  float angAccelY = atan(-accX / sqrt(pow(accY,2) + pow(accZ,2))) * (180.0 / 3.14);
  
  // Aplicación de filtro complementario combinando giroscopio y acelerómetro

  anguloX = 0.98 * (anguloX_prev + (gyroX / 131) * deltaTiempo) + 0.02 * angAccelX;
  anguloY = 0.98 * (anguloY_prev + (gyroY / 131) * deltaTiempo) + 0.02 * angAccelY;
  
  // Guardar los ángulos actuales para la siguiente iteración
  anguloX_prev = anguloX;
  anguloY_prev = anguloY;

  // Mostrar los resultados en el monitor serial

  Serial.print("X:");
  Serial.print(anguloX);
  Serial.print(" ");
  Serial.print("Y:");
  Serial.println(anguloY);

  delay(10);
}
