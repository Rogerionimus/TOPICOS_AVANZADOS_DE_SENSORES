#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 4; // TX del módulo
static const int TXPin = 3; // RX del módulo
static const uint32_t GPSBaud = 9600;

SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;
bool encabezado = false;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  //Titulo de la tabla 
  Serial.println("latitud,longitud");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      Serial.print(gps.location.lat(), 6);
      Serial.print(",");
      Serial.println(gps.location.lng(), 6);
      delay(1000); //Esto hará que tomemos un punto por segundo
    }
  }
}