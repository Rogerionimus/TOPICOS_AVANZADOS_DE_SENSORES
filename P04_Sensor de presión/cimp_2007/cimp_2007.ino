#include <Wire.h>
#include <SFE_BMP180.h>

SFE_BMP180 bmp180;

// humedad relativa
double HR = 28.57; 

const double R = 8.314472;        // J/(mol·K)
const double Ma = 0.02896546;     // kg/mol
const double Mv = 0.01801528;     // kg/mol

void setup() {
  Serial.begin(9600);

  if (!bmp180.begin()) {
    Serial.println("Error al inicializar BMP180");
    while(1);
  }

  Serial.println("Sistema listo (CIPM-2007)\n");
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

          // Conversiones
          double T_K = T + 273.15;
          double P_Pa = P * 100;

          //  Presión de vapor (Magnus)
          double Ps = 6.1078 * pow(10, (7.5 * T)/(T + 237.3)); // hPa
          double Pv = (HR / 100.0) * Ps; // hPa
          double Pv_Pa = Pv * 100;

          //  Fracción molar
          double xv = Pv_Pa / P_Pa;

          // Factor compresibilidad 
          double Z = 1.0;

          // Densidad del aire 
          double rho = (P_Pa * Ma) / (Z * R * T_K) * (1 - xv * (1 - Mv/Ma));


          Serial.print("Temperatura: ");
          Serial.print(T);
          Serial.println(" °C");

          Serial.print("Presion: ");
          Serial.print(P);
          Serial.println(" hPa");

          Serial.print("Densidad del aire: ");
          Serial.print(rho, 4);
          Serial.println(" kg/m^3");

          Serial.println("--------------------\n");
        }
      }
    }
  }

  delay(2000);
}