#include <SoftwareSerial.h>
//SoftwareSerial HC12(D1,D2);      // RX pino D1 , TX pino D2 Esp8266
SoftwareSerial HC12(2,3);      // RX pino 2 , TX pino 3 Arduino
void setup() {
  Serial.begin(9600);          // Porta Serial do computador 
  HC12.begin(9600);            // Porta Serial do HC12
}
void loop() {  
  while (HC12.available()) {   // Enquanto HC12 receber dados
    Serial.write(HC12.read()); // Escreve os dados na Serial
  }
  while (Serial.available()) { // Enquanto a Serial receber dados
    HC12.write(Serial.read()); // Escreve os dados no HC12
    HC12.flush();              // Aguarda os dados, depois limpa a memória
    delay(2000);
  }
} 
