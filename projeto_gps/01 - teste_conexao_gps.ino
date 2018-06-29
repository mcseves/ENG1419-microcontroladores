#include <SoftwareSerial.h>

SoftwareSerial serial1(6, 7); // RX, TX

void setup() {
   serial1.begin(9600);
   Serial.begin(9600);
   Serial.println("Esperando por Dados do Modulo...");
}

void loop() {
  serial1.listen();
  while (serial1.available()) {
     char cIn = serial1.read();
     Serial.print(cIn);  
  }
}
