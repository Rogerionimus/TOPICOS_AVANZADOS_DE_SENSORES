#include <Wire.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

unsigned long tiempoAnterior = 0;
const long intervalo = 1000; 

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10); 

  Serial.println("--- ARDUINO 1: SENSOR AHT20 ---");

  if (!aht.begin()) {
    Serial.println("¡Error! No se pudo encontrar el sensor AHT20.");
    while (1) delay(10);
  }
  Serial.println("Sensor AHT20 inicializado correctamente.");
  
  // Encabezado para identificar los datos en este puerto serie
  Serial.println("Tiempo(ms),Humedad_AHT20(%),Temp_AHT20(C)");
}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    sensors_event_t humedad, tempAHT;
    aht.getEvent(&humedad, &tempAHT); 

    // Imprimir datos separados por comas
    Serial.print(tiempoActual);
    Serial.print(",");
    Serial.print(humedad.relative_humidity, 2);
    Serial.print(",");
    Serial.println(tempAHT.temperature, 2);
  }
}