#include <Wire.h>

const int MPU_addr = 0x68;
const float COLLISION_THRESHOLD = 3.0; // Umbral en fuerzas G
const int CONFIRMATION_SAMPLES = 2;    // Muestras consecutivas para validar. Así evitamos falsos positivos
int trigger_count = 0;
bool airbag_deployed = false;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);
  
  pinMode(13, OUTPUT); // LED que simula la bolsa de aire
  Serial.begin(115200); // Nos permite un monitoreo a alta velocidad
}

void loop() {
  if (airbag_deployed) return; // Si ya se activó, detener sistema

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);

  float ax = (int16_t)(Wire.read() << 8 | Wire.read()) / 16384.0;
  float ay = (int16_t)(Wire.read() << 8 | Wire.read()) / 16384.0;
  float az = (int16_t)(Wire.read() << 8 | Wire.read()) / 16384.0;

  float g_total = sqrt(ax*ax + ay*ay + az*az);

  // comparador para detección de colisión
  if (g_total > COLLISION_THRESHOLD) {
    trigger_count++;
    if (trigger_count >= CONFIRMATION_SAMPLES) {
      deployAirbag(g_total);
    }
  } else {
    trigger_count = 0; // Resetear si fue solo un pico de ruido
  }
  delay(5); // Muestreo de 200Hz
}

void deployAirbag(float force) {
  digitalWrite(13, HIGH);
  airbag_deployed = true;
  Serial.println("¡¡¡ IMPACTO DETECTADO !!!");
  Serial.print("Fuerza del impacto: ");
  Serial.print(force);
  Serial.println(" G");
}