#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

const float TB = 10.0; //Temperatura base del frijol

float tMax = -100.0;   // Valor exagenradamente bajo para capturar la primer lectura
float tMin = 100.0;    // Valor exageradamente alto para capturar la primer lectuea

float ucAcumuladas = 0.0;        // Acumulador para los días
unsigned long tiempoAnterior = 0;
const long DURACION_DIA = 10000; // Un dia se parametrizo como 10 segundos

void setup() {
  Serial.begin(9600);
  
  if (!bmp.begin()) {
    Serial.println("ERROR: No se encontro el BMP180!");
    while (1); 
  }

  Serial.println("BMP180 detectado OK!");
}

void loop() {
  // 1. Lectura de temperatura constante
  float temperaturaActual = bmp.readTemperature(); // °C 

  // 2. Actualización de Máxima y Mínima del "día actual"
  if (temperaturaActual > tMax) tMax = temperaturaActual;
  if (temperaturaActual < tMin) tMin = temperaturaActual;

  // --- MONITOREO EN TIEMPO REAL ---
  Serial.println("--------------------------------------------");
  Serial.print("Temp Actual: "); Serial.print(temperaturaActual, 1); Serial.println(" °C");
  Serial.print("Max del dia: "); Serial.print(tMax, 1); Serial.println(" °C");
  Serial.print("Min del dia: "); Serial.print(tMin, 1); Serial.println(" °C");
  Serial.print("UC Totales Acumuladas: "); Serial.println(ucAcumuladas, 2);

  //INDICADOR DE ETAPA FENOLÓGICA (Evaluando el acumulador general)
  Serial.print("ETAPA FENOLOGICA:   ");
  if (ucAcumuladas < 12.89) {
    Serial.println("Siembra / Pre-Emergencia");
  } else if (ucAcumuladas < 64.41) {
    Serial.println("Emergencia");
  } else if (ucAcumuladas < 91.33) {
    Serial.println("Formacion de guias");
  } else if (ucAcumuladas < 99.31) {
    Serial.println("Floracion");
  } else if (ucAcumuladas < 112.77) {
    Serial.println("Formacion de vaina");
  } else if (ucAcumuladas < 153.24) {
    Serial.println("Llenado de Vainas");
  } else {
    Serial.println("Maduracion");
  }

  // 4. SIMULACIÓN DE FIN DE DÍA (Cada 10 segundos se calcula y se acumula)
  if (millis() - tiempoAnterior >= DURACION_DIA) {
    tiempoAnterior = millis();

    // Calcular Unidades Calor obtenidas EN ESTE DÍA
    float ucDelDia = ((tMax + tMin) / 2.0) - TB;
    if (ucDelDia < 0) ucDelDia = 0; // Evitar valores negativos

    // SUMAR al acumulador global para que la planta crezca
    ucAcumuladas += ucDelDia;

    Serial.println("\n>>> ¡HA PASADO UN DÍA SIMULADO! <<<");
    Serial.print("UC ganadas hoy: "); Serial.println(ucDelDia, 2);
    Serial.println("============================================\n");

    // REINICIAR tMax y tMin para registrar el nuevo día
    tMax = -100.0;
    tMin = 100.0;
  }

  delay(2000); // Intervalo de actualización en pantalla
}