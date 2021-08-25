/*
 * Exemplo: Acionamento de Rele ESP8266 - Adafruit IO
 * Por: Fábio Souza
 */

/************************ Configurações do Adafruit IO *******************************/
//Insira seu nome de usuário e chave. Você encontra essas informações acessando
//sua conta no Adafruit IO
#define IO_USERNAME  "Alexandre16347"
#define IO_KEY       "aio_IqTn39ZuuIu5gJKYiqj9fvjntwUc"
/********************* Configuração do WIFI **************************************/

//Insira o SSID e Senha da rede WIFI a qual você irá conectar
#define WIFI_SSID "Grafica"
#define WIFI_PASS "gr@fica1"


/***************Configurações do comunicaçaão************/
#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/************************ Mapeamento de IO *******************************/
#define RELE D2   //pino de saida para acionamento da Lampada L1


/************************ Configuração dos tópicos *******************************/

// configura o tópico "fs_embarcados/feeds/rele_1"
AdafruitIO_Feed *feedRele1 = io.feed("Campainha");


/************************ Função setup *******************************/

void setup() {

  //configura pino da Lampada como saída
  pinMode(RELE,OUTPUT);

  // configura comunicação serial
  Serial.begin(115200);

  // Aguarda serial monitor
  while(! Serial);

  conectaBroker(); //função para conectar ao broker


}

/************************ Função loop *******************************/

void loop() {

  // processa as mensagens e mantêm a conexão ativa
  byte state = io.run();

  //verifica se está conectado
  if(state == AIO_NET_DISCONNECTED | state == AIO_DISCONNECTED){
    conectaBroker(); //função para conectar ao broker
  }
 
}

/****** Função de tratamento dos dados recebidos em L1***************/

void handleRele1(AdafruitIO_Data *data) {

  // Mensagem
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if(data->isTrue())
    digitalWrite(RELE, HIGH);
  else
    digitalWrite(RELE, LOW);
}


/****** Função para conectar ao WIFI e Broker***************/

void conectaBroker(){
 
  //mensagem inicial
  Serial.print("Conectando ao Adafruit IO");

  // chama função de conexão io.adafruit.com
  io.connect();

   // instancia um novo handler para recepção da mensagem do feed Rele
  feedRele1->onMessage(handleRele1);

  // Aguarda conexação ser estabelecida
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Conectado
  Serial.println();
  Serial.println(io.statusText());

  // certifique-se de que todos os feeds obtenham seus valores atuais imediatamente
  feedRele1->get();
}
