#include <Adafruit_SleepyDog.h>
#define RESETPIN PC1
#define OPCION1 PA1 
#define OPCION2 PA2 
#define MUESTRA_SERIE false 

/* 
PINOUT USADO PARA NUESTRO CH32V003J4M6 
--------------------------------------
PA1  1-----8 
GND  2-----7
PA2  3-----6
VCC  4-----5  PC1                      
--------------------------------------
*/

unsigned long tiempos_ms[4] = {
  60000UL * 60 * 24, // 24 HORAS SIN JUMPERS A GND
  60000UL * 60 * 12, // 12 HORAS PA1
  60000UL * 60 * 6,  //  6 HORAS PA2       
  60000UL            //  1 MIN   PA1 + PA2   
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
  if(MUESTRA_SERIE){miserie("Reseteo en " + String(tiempo_reseteo / 1000 / 60) + " Minutos");}
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