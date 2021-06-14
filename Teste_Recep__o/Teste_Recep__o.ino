#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//SoftwareSerial HC12(2, 3);  // Arduino
// Rx pino 2 (Ligar no Tx do outro)
// Tx pino 3 (Ligar no Rx do outro)

SoftwareSerial HC12(D1, D2); // Esp8266
// Rx pino D1 (Ligar no Tx do outro)
// Tx pino D2 (Ligar no Rx do outro)

int var = 0;    // valor a ser enviado pelo HC12
//char cha = 'ç'; // caractere a ser enviado pelo HC12

String texto;

void setup(){
  Serial.begin(9600); // Velocidade da Porta Serial do Computador

//  pinMode(2, OUTPUT);
//  pinMode(3, INPUT);
  
  HC12.begin(9600);   // Velocidade da Porta Serial do HC12
}

void loop(){
  StaticJsonDocument<200> doc;
  deserializeJson(doc, HC12);

  delay(2000);
}
