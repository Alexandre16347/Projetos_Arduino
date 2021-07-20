#include <SoftwareSerial.h>

SoftwareSerial HC12(D3, D4); // Esp8266
// Rx pino D1 (Ligar no Tx do outro)
// Tx pino D2 (Ligar no Rx do outro)

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
  }
}
