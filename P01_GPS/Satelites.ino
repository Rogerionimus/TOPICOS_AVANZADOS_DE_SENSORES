#include <SoftwareSerial.h>
#include <TinyGPS++.h>

static const int RXPin = 4; //Parece que se invierten los conectores, pero hay que recordar que la comunicación es por medio de una UART, por eso RX->TX y viceversa.
static const int TXPin = 3;
static const uint32_t GPSBaud = 9600;

SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);

  Serial.println("=== DIAGNOSTICO GPS ===");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (millis() - lastPrint >= 2000) { // cada 2 segundos
    lastPrint = millis();

    Serial.print("Satélites: ");
    Serial.println(gps.satellites.value());

    Serial.print("Fix valido: ");
    Serial.println(gps.location.isValid() ? "SI" : "NO");

    if (gps.location.isValid()) {
      Serial.print("Lat: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Lng: ");
      Serial.println(gps.location.lng(), 6);
    }

    Serial.println("--------------------");
  }
}