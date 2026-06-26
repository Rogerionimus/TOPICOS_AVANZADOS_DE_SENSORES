#include <Wire.h>
#include <LittleFS.h>

void setup() {
  // Inicializamos el monitor serie a la misma velocidad estándar
  Serial.begin(115200);
  while (!Serial) {
    ; // Esperar a que se abra el monitor serie en la laptop
  }

  Serial.println("\n==================================================");
  Serial.println("   SISTEMA DE RECUPERACIÓN DE CAJA NEGRA - CanSat  ");
  Serial.println("==================================================");

  // Inicializar LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("ERROR: No se pudo montar el sistema de archivos LittleFS.");
    return;
  }

  // Verificar si el archivo existe
  if (!LittleFS.exists("/telemetria.csv")) {
    Serial.println("ERROR: El archivo 'telemetria.csv' no existe en la memoria.");
    return;
  }

  Serial.println("Archivo encontrado. Abriendo y descargando datos...\n");
  Serial.println("--- COPIAR DESDE LA LÍNEA SIGUIENTE ---");

  // Abrir el archivo en modo lectura
  File file = LittleFS.open("/telemetria.csv", FILE_READ);
  if (!file) {
    Serial.println("ERROR: No se pudo abrir el archivo para lectura.");
    return;
  }

  // Leer todo el archivo y escupirlo por el puerto serie
  while (file.available()) {
    Serial.write(file.read());
  }

  file.close();
  
  Serial.println("--- FIN DE LOS DATOS ---");
  Serial.println("==================================================");
  Serial.println("Descarga completada con éxito. Ya puedes desconectar.");
}

void loop() {
  // El loop se queda vacío porque solo queremos descargar los datos una vez al arrancar
}