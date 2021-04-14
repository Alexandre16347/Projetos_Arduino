#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "uFire_SHT20.h"

/*
 * O Esp8266 tem alguns problemas em usar a biblioteca uFire_SHT20
 * por isso antes de executar o codigo vc deve ir para
 * "Documents\Arduino\libraries\uFire_SHT20\src"
 * e editar os aquivos "uFire_SHT20_JSON" e "uFire_SHT20_MP"
 * apagando a primeira linha inteira onde tem  escrito ""#if __has_include" 
 * e a ultima linha onde tem escrito "#endif"
 * apos salve e compile seu codigo ".ino"
 */



uFire_SHT20 sht20;



//SoftwareSerial HC12(2, 3);  // Arduino
// Rx pino 2 (Ligar no Tx do outro)
// Tx pino 3 (Ligar no Rx do outro)

SoftwareSerial HC12(D1, D2); // Esp8266
// Rx pino D1 (Ligar no Tx do outro)
// Tx pino D2 (Ligar no Rx do outro)




void LeituraDeSensor() {
  //criando arquivo Json para armazenar as variaveis antes do envio pelo HC12
  StaticJsonDocument<200> doc;

  //doc["Nome da variavel"] = valor ou dado do sensor
  //coletando dados do DHT20
  doc["Temperatura"] = (String)sht20.temperature() + " °C";
  doc["Umidade"] = (String)sht20.humidity() + " %";

  //Comando usado para enviar o arquivo por HC12
  serializeJson(doc, HC12);
}





void setup() {

  Serial.begin(9600);
  HC12.begin(9600);   // Velocidade da Porta Serial do HC12
  sht20.begin();      //inicializa DHT20

  if (HC12.isListening()) {
    Serial.println("OK");
  } else {
    Serial.println("NOK");
  }

  //Funcao de leitura do sensor e envio pelo HC12
  LeituraDeSensor();

  delay(2000);

  Serial.print("Agora o ESP vai dormir por 10 segundos\n\n");
  //Tempo do deepSleep 10e6 = 10000000 microssegundos
  //Se for usar Arduino recomendo pesquisar o metodo de DeepSleep do tal

  ESP.deepSleep(10e6);
  //Não esquecer que assim q carregar o codigo ligar as portas D0 ao RES do esp e reiniciar

}

//Como o esp estará dormindo e acordando não é necessario o loop
void loop() {}
