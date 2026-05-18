#include <Wire.h>                // Librería de Arduino para comunicación I2C.
#include <Adafruit_BMP085.h>     // Libreria del sensor.

Adafruit_BMP085 bmp; 

void setup() {
  Serial.begin(9600); 
  if (!bmp.begin()) {
    Serial.println("Error: No se encuentra el sensor BMP180");
    while (1); // Si no inicia, se detiene.
  }
  
  // Encabezado para que sepas qué es cada columna (útil para Excel/MATLAB)
  Serial.println("Tiempo(s),T_BMP(C),T_LM35(C)");
}

void loop() {
  // --- Contador de Tiempo ---
  // Obtenemos el tiempo en segundos (usamos float para tener decimales)
  float tiempo_seg = millis() / 1000.0;

  // --- BMP180 ---
  float T_bmp = bmp.readTemperature(); 

  // --- LM35 (A0) ---
  int lectura = analogRead(A0);
  float voltaje = lectura * (5.0 / 1023.0);
  float T_lm35 = voltaje * 100.0;

  // --- Impresión de los valores ---
  // Formato CSV: Tiempo, Temperatura1, Temperatura2
  Serial.print(tiempo_seg);
  Serial.print(",");
  Serial.print(T_bmp);
  Serial.print(",");
  Serial.println(T_lm35);

  delay(2000); 
}