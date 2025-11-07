#include <Adafruit_SleepyDog.h>
#define RESETPIN PA1
#define OPCION1 PC4 // mas cercano al gnd
#define OPCION2 PC2 // mas lejano al gnd
#define MUESTRA_SERIE false 

/* 
PINOUT USADO PARA NUESTRO CH32V003J4M6 
--------------------------------------
PA1  1-  /--8 GND (PIN8 PUENTEADO A PIN2)
GND  2--/  -7 PC4 OPCION1
     3-    -6 PC2 OPCION2 
VCC  4-    -5                       
--------------------------------------
*/

unsigned long tiempos_ms[4] = {
  60000UL * 60 * 24, // 24 HORAS SIN JUMPERS A GND
  60000UL * 60 * 12, // 12 HORAS OPCION1
  60000UL * 60 * 6,  //  6 HORAS OPCION2   
  60000UL            //  1 MIN   OPCION1 + OPCION2
};

unsigned long tiempo_acumulado_ms = 0;
unsigned long tiempo_reseteo; // 43200000 12 horas

void miserie(String txt){
  Serial.begin(115200); 
  Serial.println(txt);
  delay(10); 
  Serial.end(); 
}

void setup() {
  int jumpers = 0;
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, HIGH); 
  pinMode(OPCION1, INPUT_PULLUP);
  pinMode(OPCION2, INPUT_PULLUP);
  if(digitalRead(OPCION1) == LOW){ jumpers = jumpers + 1;  }
  if(digitalRead(OPCION2) == LOW){ jumpers = jumpers + 2;  }
  tiempo_reseteo = tiempos_ms[jumpers];
  if(MUESTRA_SERIE){
    miserie("Velocidad de reloj: "+ String(F_CPU));
    miserie("Reseteo en " + String(tiempo_reseteo / 1000 / 60) + " Minutos");
  }
  //muestra_config(jumpers);
  
}

void muestra_config(int op){
  op = op +1;
  while(op>=1){
    op = op-1;
    digitalWrite(RESETPIN, HIGH); 
    delay(200); 
    digitalWrite(RESETPIN, LOW); 
    delay(200); 
  }
  delay(5000); 
  digitalWrite(RESETPIN, HIGH); 
}

void loop() {
  int sleepMS = Watchdog.sleep();
  tiempo_acumulado_ms += sleepMS; 

  if (tiempo_acumulado_ms >= tiempo_reseteo) {
    if(MUESTRA_SERIE){ miserie("--- ¡TIEMPO TRANSCURRIDO! Enviando pulso de reset ---"); }
    digitalWrite(RESETPIN, LOW); 
    delay(200); 
    digitalWrite(RESETPIN, HIGH); 
    tiempo_acumulado_ms = 0; 
  }

  if(MUESTRA_SERIE){
    String mensaje = "";
    mensaje += "Dormire por " + String(sleepMS/1000) + "s. ";
    mensaje += "Tiempo funcionando: " + String(tiempo_acumulado_ms/1000/60) + " mins. ";
    mensaje += "Faltan: " + String((tiempo_reseteo - tiempo_acumulado_ms) / 1000 / 60 ) + " mins para el reseteo";
    miserie(mensaje);
  }
}