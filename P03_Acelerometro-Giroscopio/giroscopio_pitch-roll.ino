// Librerías necesarias para establecer comunicación I2C con el sensor MPU6050
// El archivo MPU6050.h depende de I2Cdev.h y esta usa la librería Wire.h
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// Instancia del objeto que representa al MPU6050
// La dirección puede ser 0x68 o 0x69 dependiendo del estado del pin AD0
MPU6050 mpuSensor;

// Variables para almacenar las lecturas crudas del giroscopio en los tres ejes
int gyroX, gyroY, gyroZ;

// Variables relacionadas con el cálculo del tiempo y los ángulos
long tiempoAnterior, intervalo;
float anguloGiroX, anguloGiroY;
float anguloPrevX, anguloPrevY;

void setup() {
  Serial.begin(57600);     // Inicialización de la comunicación serial
  Wire.begin();            // Activación del bus I2C
  mpuSensor.initialize();  // Configuración inicial del MPU6050

  // Verificación de la comunicación con el sensor
  if (mpuSensor.testConnection()) 
    Serial.println("MPU6050 listo para usar");
  else 
    Serial.println("Fallo en la conexión con el MPU6050");

  tiempoAnterior = millis(); // Guardar el tiempo inicial
}

void loop() {

  // Obtener las velocidades angulares medidas por el giroscopio
  mpuSensor.getRotation(&gyroX, &gyroY, &gyroZ);

  // Cálculo del intervalo de tiempo transcurrido
  intervalo = millis() - tiempoAnterior;
  tiempoAnterior = millis();

  // Integración de la velocidad angular para estimar el ángulo de rotación
  anguloGiroX = (gyroX / 131) * intervalo / 1000.0 + anguloPrevX;
  anguloGiroY = (gyroY / 131) * intervalo / 1000.0 + anguloPrevY;

  // Guardar los valores actuales para la siguiente iteración
  anguloPrevX = anguloGiroX;
  anguloPrevY = anguloGiroY;

  // Mostrar los ángulos calculados en el monitor serial
  //Serial.print("Angulo de rotacion X: ");
  Serial.print(anguloGiroX);
  Serial.print(" ");
  //Serial.print("   Angulo de rotacion Y: ");
  Serial.println(anguloGiroY);

  delay(100); // Pequeña pausa antes de la siguiente medición
}

//Nota: para el giroscopio, por cada iteración hay un aumento de error en la medición debido a la integración de la velocidad angular y la suma del ángulo inicial (DRIFT)