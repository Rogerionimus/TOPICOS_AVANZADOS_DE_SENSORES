#include <Wire.h>  // Librería para comunicación I2C
#include <Adafruit_VL53L0X.h>  // Librería para el sensor VL53L0X

Adafruit_VL53L0X sensor = Adafruit_VL53L0X();  // Creamos un objeto del sensor

//Filtro EMA
float alpha = 0.25;  // Factor de suavizado (ajustable)
float filteredDistance = 0;  // Valor inicial de la distancia filtrada

void setup() {
  Serial.begin(9600);  // Comunicación serial
  Wire.begin();  // Inicia la comunicación I2C

  if (!sensor.begin()) {  // Verificar si esta encendido
    Serial.println("Error: No se detecta el módulo");  // Error
    while (1);  // Ejecutamos infinitamente para verificar el enecendido
  }
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;  // Arreglo para guardar los datos medidos

  sensor.rangingTest(&measure, false);  // Medimos la distancia

  if (measure.RangeStatus != 4) {  // Si el estado no es error (4), procesa el dato medido
    float rawDistance = measure.RangeMilliMeter;  // Distancia en mm

    //Aplicamos el filtro
    filteredDistance = (alpha * rawDistance) + ((1 - alpha) * filteredDistance);

    //Imprimimos los valores
    Serial.print("Distancia sin filtrar: ");
    Serial.print(rawDistance);
    Serial.print(" mm | Distancia filtrada: ");
    Serial.print(filteredDistance);
    Serial.println(" mm");
  } else {
    Serial.println("Error en la medición"); 
  }

  delay(500);  // Espera 500 ms entre mediciones para no saturar el sensor
}