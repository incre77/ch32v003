#include <Adafruit_SleepyDog.h>

#define RESETPIN PA1
#define PIN_MEDICION PC4 
#define PIN_POWER PC2    

#define MIN 60000UL
#define MUESTRA_SERIE false 

/* 
PINOUT USADO PARA NUESTRO CH32V003J4M6 
--------------------------------------
PA1  1-  /--8 GND ---------R2--\  Resistencia entre gnd y pc4 que define el tiempo de reinicio. Ej 5k = 5000/100 = 50 mins
GND  2--/  -7 PC4 --R1--\------/  Resistencia de 10k entre pc4 y pc2 a modo de referencia para la formula
     3-    -6 PC2 ------/ 
VCC  4-    -5                       
--------------------------------------
Si R2 es un puente el reinicio es 1 min
Si R2 es inexistente el reincio es 24h
*/


unsigned long tiempo_reseteo; // Se definirá en el setup
unsigned long tiempo_acumulado_ms = 0; 

void miserie(String txt){
  Serial.begin(115200); 
  Serial.println(txt);
  Serial.flush(); // Asegura que se envíe todo
  Serial.end(); 
}

long leerResistencia() {
  pinMode(PIN_POWER, OUTPUT);
  digitalWrite(PIN_POWER, HIGH);
  delay(100); 

  int lectura = analogRead(PIN_MEDICION);
  digitalWrite(PIN_POWER, LOW);

  // Si la lectura es muy baja, hay un corto o puente a GND
  if (lectura <= 10) return 0;  
  // Si la lectura es muy alta, no hay resistencia (abierto)
  if (lectura >= 1010) return -1; 

  // Fórmula para: PIN_POWER -> 10k -> PIN_MEDICION -> R_desconocida -> GND
  return (10000UL * lectura) / (1023 - lectura);
}

void setup() {
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, HIGH);   

  // 1. Leemos la resistencia para configurar el tiempo
  long r = leerResistencia();

  // 2. Lógica de asignación de tiempo
  if (r == -1) {
    // Si no hay resistencia, ponemos un tiempo por defecto (ej. 24h)
    tiempo_reseteo = MIN*60*24; 
  } else if (r == 0) {
    // Si está puenteado, tiempo mínimo (ej. 1 min)
    tiempo_reseteo = MIN;
  } else {
    // Ejemplo: El tiempo es proporcional a la resistencia
    // Si quieres que r/100 sean minutos:
    unsigned long minutos = r / 100;
    if (minutos < 1) minutos = 1; // Mínimo 1 minuto
    tiempo_reseteo = minutos * 60000UL;
  }

  if(MUESTRA_SERIE){
    miserie("Configurado: " + String(tiempo_reseteo / 60000) + " Minutos\n");
  }
}

void loop() {
  int sleepMS = Watchdog.sleep(10000);
  tiempo_acumulado_ms += sleepMS; 

  if (tiempo_acumulado_ms >= tiempo_reseteo) {
    digitalWrite(RESETPIN, LOW); 
    delay(2000); 
    digitalWrite(RESETPIN, HIGH); 
    tiempo_acumulado_ms = 0; 
    
    if(MUESTRA_SERIE) miserie("--- RESET ENVIADO ---\n");
  }

  if(MUESTRA_SERIE){
    String msj;
    if((tiempo_reseteo - tiempo_acumulado_ms) > MIN){
      msj = "Faltan: " + String((tiempo_reseteo - tiempo_acumulado_ms) / 1000 / 60) + " mins.\n";
    }else{
      msj = "Faltan: " + String((tiempo_reseteo - tiempo_acumulado_ms) / 1000) + " secs.\n";
    }
    miserie(msj);
  }
}