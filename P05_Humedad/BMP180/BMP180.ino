#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

unsigned long tiempoAnterior = 0;
const long intervalo = 1000; 

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10); 

  Serial.println("--- ARDUINO 2: SENSOR BMP180 ---");

  if (!bmp.begin()) {
    Serial.println("¡Error! No se pudo encontrar el sensor BMP180.");
    while (1) delay(10);
  }
  Serial.println("Sensor BMP180 inicializado correctamente.");
  
  // Encabezado para identificar los datos en este puerto serie
  Serial.println("Tiempo(ms),Temp_BMP180(C)");
}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    float tempBMP = bmp.readTemperature();

    // Imprimir datos separados por comas
    Serial.print(tiempoActual);
    Serial.print(",");
    Serial.println(tempBMP, 2);
  }
}