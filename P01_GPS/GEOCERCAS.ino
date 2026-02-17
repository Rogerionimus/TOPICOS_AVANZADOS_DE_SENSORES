#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// Configuramos el hardware. Recordar que RX->TX y viceversa
const int GPS_RX = 4;
const int GPS_TX = 3;
const int LED_ALERTA = 13;
const int BTN_CONFIG = 7;

// Establecemos el radio limite de la geocerca, en este caso 30 metros
const double RADIO_LIMITE = 30.0; // Metros

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX, GPS_TX);

double latitudBase, longitudBase;
bool geocercaActiva = false;

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  
  pinMode(LED_ALERTA, OUTPUT);
  pinMode(BTN_CONFIG, INPUT_PULLUP);
  
  Serial.println(F("--- Sistema de Geocerca Activo ---"));
  Serial.println(F("Esperando señal de satélites..."));
}

void loop() {
  // Alimentamos el GPS con los datos del puerto serie
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  // Operará cuando el GPs tenga una posición valida.
  if (gps.location.isValid()) {
    
    verificarBoton();

    if (geocercaActiva) {
      procesarGeocerca();
    } else {
      // Parpadeo lentamente indicando que el GPS está listo pero sin centro definido
      digitalWrite(LED_ALERTA, (millis() / 500) % 2);
    }
  }
}

void verificarBoton() {
  // Si se presiona el botón, se establece (o redefine) el centro, así tendremos el punto de referencia para tomar el radio de 30 metros
  if (digitalRead(BTN_CONFIG) == LOW) {
    latitudBase = gps.location.lat();
    longitudBase = gps.location.lng();
    geocercaActiva = true;
    
    Serial.print(F("NUEVO CENTRO FIJADO: "));
    Serial.print(latitudBase, 6);
    Serial.print(F(", "));
    Serial.println(longitudBase, 6);
    
    for(int i=0; i<5; i++){
      digitalWrite(LED_ALERTA, HIGH); delay(100);
      digitalWrite(LED_ALERTA, LOW); delay(100);
    }
  }
}

void procesarGeocerca() {
  // Usamos la función nativa de la librería para calcular distancia (en.ugar de implementar a mano el algoritmo de haversine)
  double distanciaActual = TinyGPSPlus::distanceBetween(
    gps.location.lat(),
    gps.location.lng(),
    latitudBase,
    longitudBase
  );

  Serial.print(F("Distancia al centro: "));
  Serial.print(distanciaActual);
  Serial.println(F(" m"));

  // Lógica de la alerta
  if (distanciaActual > RADIO_LIMITE) {
    digitalWrite(LED_ALERTA, HIGH); // FUERA DE RANGO
  } else {
    digitalWrite(LED_ALERTA, LOW);  // DENTRO DE RANGO
  }
}