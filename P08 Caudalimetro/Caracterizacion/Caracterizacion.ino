const int sensorPin = 2;
volatile long pulseCount = 0;
unsigned long oldTime = 0;

void pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  Serial.println("Tiempo(ms), Pulsos_registrados"); // Encabezado para Excel
  oldTime = millis();
}

void loop() {
  // Vamos a imprimir cada 5 segundos para que te dé tiempo de anotar el volumen
  if ((millis() - oldTime) > 5000) { 
    detachInterrupt(digitalPinToInterrupt(sensorPin));
    
    Serial.print(millis());
    Serial.print(", ");
    Serial.println(pulseCount);
    
    pulseCount = 0;
    oldTime = millis();
    attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  }
}