const int pinSensor = A0;

int valorADC = 0;
float voltaje = 0;

// Para filtrado (promedio)
const int muestras = 10;
int suma = 0;

// Para detección de picos
float voltajeBase = 0;
float voltajeMax = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando medicion MQ-135...");
  delay(3000);

  // Obtener línea base (aire limpio)
  Serial.println("Calculando linea base...");
  for (int i = 0; i < 50; i++) {
    valorADC = analogRead(pinSensor);
    voltaje = valorADC * (5.0 / 1023.0);
    voltajeBase += voltaje;
    delay(100);
  }
  voltajeBase = voltajeBase / 50.0;

  Serial.print("Voltaje base: ");
  Serial.println(voltajeBase, 3);
  Serial.println("----- INICIO DE MEDICION -----");
}

void loop() {

  // Promedio de varias lecturas
  suma = 0;
  for (int i = 0; i < muestras; i++) {
    suma += analogRead(pinSensor);
    delay(10);
  }

  valorADC = suma / muestras;
  voltaje = valorADC * (5.0 / 1023.0);

  // Detectar valor máximo (pico)
  if (voltaje > voltajeMax) {
    voltajeMax = voltaje;
  }

  // Mostrar datos
  Serial.print("Voltaje: ");
  Serial.print(voltaje, 3);
  Serial.print(" V | Base: ");
  Serial.print(voltajeBase, 3);
  Serial.print(" V | Pico: ");
  Serial.print(voltajeMax, 3);
  Serial.print(" V | Delta: ");
  Serial.println(voltaje - voltajeBase, 3);

  delay(200);
}
