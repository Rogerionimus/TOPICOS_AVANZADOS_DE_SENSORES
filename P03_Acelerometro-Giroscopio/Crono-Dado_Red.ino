#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuración OLED
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MPU6050 mpu(0x68);

int pitch = 0;
int roll = 0;

// Variables para el cronómetro
unsigned long tiempo_inicio = 0;
String cara_actual = "";
String cara_anterior = "";

///////////////////////////////// Variables Red Neuronal ////////////////
const int node = 2;
const int input = 2;
const int outputs = 2;

float P[input][1];
// Modificar pesos y bias, utilziar las del entrenamiento
float W1[node][input] = {{3.240196361625106, -0.043726993962835},
                         {3.674518515832785, -3.836152739641886}};
   
float b1[node][1] = {{-0.019033024837803},
                     {0.876504827758941}};

float a1[node][1];

float W2[outputs][node] = {{4.708662302993665, 2.906227027194604},
                           {-4.813177284901015, 2.566476262595220}};

float b2[outputs][1] = {{-0.216418141735284},
                        {-0.263706402669589}};
                      
float a2[outputs][1];
float aux = 0.0;
int maxValue = 1023;
int minValue = 0;

void setup() {
  Serial.begin(115200);
  
  Wire.begin(); 
  mpu.initialize();

  // Inicialización de la pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Error al iniciar OLED"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  
  tiempo_inicio = millis();
}

void loop() {
  /////////////////////////////// Primera Capa Entrada ///////////////////
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  float x = ax / 16384.0;
  float y = ay / 16384.0;
  float z = az / 16384.0;
  
  pitch = -(atan2(x, sqrt(y*y + z*z)) * 180.0) / M_PI;
  roll = (atan2(y, z) * 180.0) / M_PI;

  int mappedPitch = map(pitch, -180, 180, 0, 1023);
  int mappedRoll = map(roll, -180, 180, 0, 1023);

  P[0][0] = dataNormalized((float) mappedPitch, minValue, maxValue);
  P[1][0] = dataNormalized((float) mappedRoll, minValue, maxValue);

  ///////////////////////////// Segunda Capa //////////////////////////
    for(int i = 0 ; i < node; i++ ) {  
    aux = 0.0;  
    for(int j = 0 ; j < input ; j++ ) { 
      aux = aux + W1[i][j] * P[j][0];
    }
    a1[i][0] = tansig(aux + b1[i][0]);
  }

  ///////////////////////////// Tercera Capa //////////////////////////
  for(int i = 0 ; i < outputs; i++ ) {  
    aux = 0.0;  
    for(int j = 0 ; j < node ; j++ ) { 
      aux = aux + W2[i][j] * a1[j][0];
    }
    a2[i][0] = tansig(aux + b2[i][0]);
  }

  ///////////////////////////// Lógica de Cara y Cronómetro //////////////////////////
  if (a2[0][0] <= 0 && a2[1][0] <= 0) {
    cara_actual = "Control";
  } else if(a2[0][0] <= 0 && a2[1][0] > 0) {
    cara_actual = "Diseño";
  } else if(a2[0][0] > 0 && a2[1][0] <= 0) {
    cara_actual = "Neumática";
  } else {
    cara_actual = "Autómatas";
  }

  // Si la cara cambió, reiniciamos el cronómetro
  if (cara_actual != cara_anterior) {
    tiempo_inicio = millis();
    cara_anterior = cara_actual;
  }

  unsigned long tiempo_transcurrido = (millis() - tiempo_inicio) / 1000; // Segundos

  // Mostrar en Pantalla OLED
  display.clearDisplay();
  
  // Título
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Tarea asignada:");
  
  // Cara detectada
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(cara_actual);
  
  // Cronómetro
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("Tiempo: ");
  display.print(tiempo_transcurrido);
  display.print(" seg");
  
  display.display();

  Serial.print(cara_actual);
  Serial.print(" | Tiempo: ");
  Serial.println(tiempo_transcurrido);

  delay(200);
}

float tansig(float n) {
  float a = exp(n);
  float b = exp(-n);
  return (a - b) / (a + b);
}

float dataNormalized(int inputData, int minData, int maxData) {
  return 2.0 * (inputData - minData) / (maxData - minData) - 1.0;
}