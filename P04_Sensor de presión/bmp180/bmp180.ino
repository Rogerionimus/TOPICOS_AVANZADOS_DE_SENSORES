#include <Wire.h>
#include <SFE_BMP180.h>

SFE_BMP180 bmp180;

void setup() {
  Serial.begin(9600);

  if (bmp180.begin()) {
    Serial.println("BMP180 inicializado correctamente");
  } else {
    Serial.println("No se pudo inicializar el BMP180");
    while (1);
  }
}

void loop() {
  double T, P;
  char status;

  status = bmp180.startTemperature();
  if (status != 0) {
    delay(status);

    status = bmp180.getTemperature(T);
    if (status != 0) {

      status = bmp180.startPressure(3);
      if (status != 0) {
        delay(status);

        status = bmp180.getPressure(P, T);
        if (status != 0) {

          // Conversión de unidades
          double T_K = T + 273.15;   // Kelvin
          double P_Pa = P * 100;     // Pascales

          // Constante del aire
          double R = 287.05;

          // Cálculo de densidad
          double rho = P_Pa / (R * T_K);

          Serial.print("Temperatura: ");
          Serial.print(T);
          Serial.println(" *C");

          Serial.print("Presión: ");
          Serial.print(P);
          Serial.println(" hPa");

          Serial.print("Densidad del aire: ");
          Serial.print(rho);
          Serial.println(" kg/m^3");
        }
      }
    }
  }

  delay(1000);
}