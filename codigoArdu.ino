// --------------------------------------
// Include files
// --------------------------------------
#include <string.h>
#include <stdio.h>
#include <Wire.h>

// --------------------------------------
// Global Constants
// --------------------------------------
#define SLAVE_ADDR 0x8
#define MESSAGE_SIZE 9

//pines de sensores y actuadores
int ledAlarma = 7;
int lectorLuminosidad = 0;


//variables globales de control interno
bool request_received = false;
bool answer_requested = false;
char request[MESSAGE_SIZE+1];
char answer[MESSAGE_SIZE+1];
int estadoActualLedAlarma = 0;
int estadoDeseadoLedAlarma = 0;
int porcentajeLuminico = 0;
// --------------------------------------
// Encender Led Alarma
// --------------------------------------
void checkLedAlarma(){
  if(estadoActualLedAlarma!=estadoDeseadoLedAlarma){
    digitalWrite(ledAlarma, estadoDeseadoLedAlarma);
    estadoActualLedAlarma = estadoDeseadoLedAlarma;
  }
}
// --------------------------------------
// Leer nivel luminico
// --------------------------------------

void leerNivelLuminico(){
  int valor = analogRead(0);
  porcentajeLuminico = map(valor,0,1023,0,100);
}

// --------------------------------------
// Handler function: receiveEvent
// --------------------------------------
void receiveEvent(int num)
{
   char aux_str[MESSAGE_SIZE+1];
   int i=0;

   // read message char by char
   for (int j=0; j<num; j++) {
      char c = Wire.read();
      if (i < MESSAGE_SIZE) {
         aux_str[i] = c;
         i++;
      }
   }
   aux_str[i]='\0';

   // if message is correct, load it
   if ((num == MESSAGE_SIZE) && (!request_received)) {
      memcpy(request, aux_str, MESSAGE_SIZE+1);
      request_received = true;
   }
}

// --------------------------------------
// Handler function: requestEvent
// --------------------------------------
void requestEvent()
{
   // if there is an answer send it, else error
   if (answer_requested) {
      
      Wire.write(answer,MESSAGE_SIZE);
      memset(answer,'\0', MESSAGE_SIZE+1);
   } else {
      Wire.write("MSG:ERROR\n",MESSAGE_SIZE);
   }
  
   // set answer empty
   request_received = false;
   answer_requested = false;
   memset(request,'\0', MESSAGE_SIZE+1);
   memset(answer,'\0', MESSAGE_SIZE);
}

// --------------------------------------
// Function: servidor_comunicaciones con RaspBerry
// --------------------------------------
int servidorComunicaciones()
{
   // while there is enough data for a request
   if(request_received){
      if(strcmp("HOLA ARDU",request) == 0){
          sprintf(answer,"HOLA RASP");
          // set buffers and flags
          memset(request,'\0', MESSAGE_SIZE+1);
          request_received = false;
          answer_requested = true;
      }else if(strcmp("LEER:LUMI",request) == 0){
          sprintf(answer, "LUMI:%03d%%", porcentajeLuminico);
          // set buffers and flags
          Serial.println(porcentajeLuminico);
          memset(request,'\0', MESSAGE_SIZE+1);
          request_received = false;
          answer_requested = true;
      }else if(strcmp("ALARM:SET",request) == 0){
          estadoDeseadoLedAlarma = 1;
          sprintf(answer,"ALARM: OK");
          // set buffers and flags
          memset(request,'\0', MESSAGE_SIZE+1);
          request_received = false;
          answer_requested = true;
      }else if(strcmp("ALARM:CLR",request) == 0){
          estadoDeseadoLedAlarma = 0;
          sprintf(answer,"ALARM: OK");
          // set buffers and flags
          memset(request,'\0', MESSAGE_SIZE+1);
          request_received = false;
          answer_requested = true;
      }
      
      
   }

   
   return 0;
}


void setup() {
  pinMode(ledAlarma, OUTPUT);
  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);
  // Function to run when data requested from master
  Wire.onRequest(requestEvent);
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {  
  checkLedAlarma();
  servidorComunicaciones();
  leerNivelLuminico();
  delay(100);
}
