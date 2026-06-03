const int pinMQ2 = A0;
int aireAmbiente = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinMQ2, INPUT);

  Serial.println("=================================================");
  Serial.println("    DETECTOR DE ACTIVIDAD FÍSICA (MQ-2)         ");
  Serial.println("=================================================");
  Serial.println("Estabilizando sensor en el ambiente... No sople.");
  
  // Espera 5 segundos y promedia el aire de la habitación
  long suma = 0;
  for(int i = 0; i < 5; i++) {
    suma += analogRead(pinMQ2);
    delay(1000);
  }
  aireAmbiente = suma / 5;
  
  Serial.print("Ambiente calibrado. Valor base: ");
  Serial.println(aireAmbiente);
  Serial.println("\n>>> LISTO. Sople al sensor de forma constante durante 5 segundos...");
}

void loop() {
  int maximoDetectado = 0;
  
  // Registra el valor más alto mientras la persona sopla durante un ciclo de 5 segundos
  for(int i = 0; i < 50; i++) { 
    int lecturaActual = analogRead(pinMQ2);
    if(lecturaActual > maximoDetectado) {
      maximoDetectado = lecturaActual;
    }
    delay(100); // Muestreo rápido
  }

  // Calculamos la diferencia entre el soplido máximo y el aire normal
  int incremento = maximoDetectado - aireAmbiente;
  if(incremento < 0) incremento = 0;

  // --- MOSTRAR RESULTADOS DEL TEST ---
  Serial.println("\n=================================================");
  Serial.print("Resultado del soplido (Máximo): "); Serial.println(maximoDetectado);
  Serial.print("Incremento sobre el aire base: "); Serial.println(incremento);
  Serial.print("DIAGNÓSTICO: ");

  // Umbrales de evaluación (Ajustables según las pruebas)
  if (incremento < 15) {
    Serial.println("REPOSO / SIN ACTIVIDAD FÍSICA DETECTADA");
    Serial.println("Exhalación normal. El metabolismo está en niveles base.");
  } 
  else if (incremento >= 15 && incremento < 45) {
    Serial.println("ACTIVIDAD FÍSICA LEVE A MODERADA");
    Serial.println("Se detecta un ligero aumento de gases metabólicos en el aliento.");
  } 
  else {
    Serial.println("¡ACTIVIDAD FÍSICA INTENSA DETECTADA!");
    Serial.println("Metabolismo acelerado. Alta exhalación de compuestos volátiles / sudor.");
  }
  Serial.println("=================================================");

  Serial.println("\nEsperando 10 segundos para limpiar el sensor antes del siguiente test...");
  delay(10000); 
  Serial.println("\n>>> ¡LISTO PARA OTRA PRUEBA! Sople de nuevo...");
}