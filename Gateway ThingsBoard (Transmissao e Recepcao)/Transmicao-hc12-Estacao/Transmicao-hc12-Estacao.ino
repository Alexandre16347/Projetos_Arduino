/*
 * @autor Carlos Alexandre 
 * date 2021-07-20
 */

#include <ArduinoJson.h>
#include <SDL_Arduino_INA3221.h>
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

//Voce deve soldar o ponto vs do ina3221 para ele receber 
//a referencia 0x41
SDL_Arduino_INA3221 ina3221(0x41);


#define Canal_1 1
#define Canal_2 2
#define Canal_3 3


//Definicao do tempo de dormida/sleep
//Não esquecer que assim q carregar o codigo 
//ligar as portas D0 ao RES do esp e reiniciar
#define dorme 30e6 // 30 segundos // e6 = microsegundos


//SoftwareSerial HC12(2, 3);  // Arduino
// Rx pino 2 (Ligar no Tx do outro)
// Tx pino 3 (Ligar no Rx do outro)

SoftwareSerial HC12(D3, D4); // Esp8266
// Rx pino D1 (Ligar no Tx do outro)
// Tx pino D2 (Ligar no Rx do outro)




void LeituraDeSensor() {
  //criando arquivo Json para armazenar as variaveis antes do envio pelo HC12
  StaticJsonDocument<200> data;

  //Dados do sensor de Temperatura e humidade;
  data["Temperatura"] = (String)sht20.temperature() + " °C";
  data["Umidade"] = (String)sht20.humidity() + " %";

   //Minha localização
  data["latitude"] = -4.2795657;
  data["longitude"] = -41.7782000;

  //Dados da Placa e Bateria
  data["Tensão_Placa"] = ina3221.getBusVoltage_V (Canal_1);
  data["Corrente_Placa"] = ((ina3221.getCurrent_mA (Canal_1)) / 1000);
//  data["Tensão_Stepdown"] = ina3221.getBusVoltage_V (Canal_2);
//  data["Corrente_Stepdown"] = ((ina3221.getCurrent_mA (Canal_2)) / 1000);
  data["Tensão_Bateria"] = ina3221.getBusVoltage_V (Canal_3);
  data["Corrente_Bateria"] = ((ina3221.getCurrent_mA (Canal_3)) / 1000);

  //Comando usado para enviar o arquivo por HC12
  serializeJson(data, HC12);
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

  Serial.print("Agora o ESP vai dormir por 30 segundos\n\n");
  //Tempo do deepSleep 10e6 = 10000000 microssegundos
  //Se for usar Arduino recomendo pesquisar o metodo de DeepSleep do tal

  //Comando DeepSleep usado no esp8266
  ESP.deepSleep(dorme);
  //Não esquecer que assim q carregar o codigo ligar as portas D0 ao RES do esp e reiniciar

}

//Como o esp estará dormindo e acordando não é necessario o loop
void loop() {}
