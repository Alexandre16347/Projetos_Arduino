/*
   @autor Carlos Alexandre
   date 2022-05-05
*/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "seguranca.h"
#include "CTBot.h"

CTBot myBot;

//Variavel Telegram
int ADMINISTRADOR = 1323703301;  // ID DO BOT DO ADMINISTRADOR
#define BOT_TOKEN "1339424001:AAHFKsfLCj7NW3zLFZgxai5xcP0clp24Tik"  //chave Token Bot Telegram


//SoftwareSerial HC12(2, 3);  // Arduino
// Rx pino 2 (Ligar no Tx do outro)
// Tx pino 3 (Ligar no Rx do outro)

SoftwareSerial HC12(D3, D4); // Esp8266
// Rx pino D3 (Ligar no Tx do outro)
// Tx pino D4 (Ligar no Rx do outro)


// DECLARAÇÃO DE VARIÁVEIS GLOBAIS
double temperatura;
double umidade;
boolean fumacaD;
int fumacaA;
double tensaoPlaca, tensaoCircuito, correntePlaca, correnteCircuito, tensaoAHT, correnteAHT;


// DECLARAÇÃO DE VARIÁVEIS PARA WIFI
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* host = SECRET_HOST;

WiFiClient client;




// DECLARAÇÃO DE FUNÇÕES
void InitWiFi();
void InitConexaoBD();
void RecepcaoDeDados();
void ConexaoTelegram();


void setup() {
  Serial.begin(2400);
  // ESPERA O SERIAL ESTAR PRONTO
  while (!Serial);

  HC12.begin(2400);  // Inicializando hc12 // Velocidade da Porta Serial do HC12

  /*if (HC12.isListening()) {
    Serial.println("OK");
    } else {
    Serial.println("NOK");
    }*/

  // Função que inicia a conexão com o Access Point
  InitWiFi();
  ConexaoTelegram();

}

void loop() {

  // Se o cliente MQTT não tiver conectado, executa função reconnect()
  if (WiFi.status() != WL_CONNECTED) {
    //Serial.println("conectando dnv");
    InitWiFi();
    ConexaoTelegram();
  }

  TBMessage msg;



  // Enviar dados do sensor periodicamente
  // Se o intervalo de tempo for maior que 30s
  while (HC12.available()) {
    
    // Envia dados do sensor ao Banco de Dados
    RecepcaoDeDados();
    InitConexaoBD();

  }

  if (CTBotMessageText == myBot.getNewMessage(msg)) {

    String MensagemRecebida = msg.text;
    MensagemRecebida.toUpperCase();

    //Comandos publicos
    //=========================================================================================================================================================================================================
    if (MensagemRecebida == "/TEMPERATURA") {
      myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName + "\n\nTemperatura: " +
                        (String)temperatura + " °C");
    }
    else if (MensagemRecebida == "/UMIDADE") {

      myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName +  "\n\nUmidade: " +
                        (String)umidade + " RH%");
    }
    else if (MensagemRecebida == "/FUMACA" || MensagemRecebida == "/FOGO") {
      if (fumacaD == true) {
        myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName +  "\n\nFumaça não detectada");
      } else {
        myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName +  "\n\nFumaça detectada");
      }
    }
    else if (MensagemRecebida == "/PLACA") {

      myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName +  "\n\nTensao Placa: " +
                        (String)tensaoPlaca + " V" + "\nCorrente Placa:" + (String)correntePlaca + " mA");
    }
    else if (MensagemRecebida == "/CIRCUITO") {

      myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName +  "\n\nTensao Circuito: " +
                        (String)tensaoCircuito + " V" + "\nCorrente Circuito:" + (String)correnteCircuito + " mA");
    }
    else if (MensagemRecebida == "/STATUS") {
      if (fumacaD == true) {
        myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName
                          + "\n\nTemperatura: " + (String)temperatura + " °C\n" + "Umidade: " + (String)umidade + " RH%"
                          + "\n\nTensao Placa: " + (String)tensaoPlaca + " V" + "\nCorrente Placa:" + (String)correntePlaca + " mA"
                          + "\n\nTensao Circuito: " + (String)tensaoCircuito + " V" + "\nCorrente Circuito:" + (String)correnteCircuito + " mA"
                          + "\n\nFumaça não detectada");
      } else {
        myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName
                          + "\n\nTemperatura: " + (String)temperatura + " °C\n" + "Umidade: " + (String)umidade + " RH%"
                          + "\n\nTensao Placa: " + (String)tensaoPlaca + " V" + "\nCorrente Placa:" + (String)correntePlaca + " mA"
                          + "\n\nTensao Circuito: " + (String)tensaoCircuito + " V" + "\nCorrente Circuito:" + (String)correnteCircuito + " mA"
                          + "\n\nFumaça detectada");
      }
    }
    else {
      myBot.sendMessage(msg.sender.id, msg.sender.firstName + " " + msg.sender.lastName + "\n\nComando não identificado");
    }

  }
}


// Inicia conexão Wifi
void InitWiFi() {
  //  Serial.println("\n\nConectando-se ao AP ...");
  // Inicia conexão com Wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
  }
  //  Serial.println("Conexão Wifi estabelecida");
  //  // print out info about the connection:
  //  Serial.print("My IP address is: ");
  //  Serial.println(WiFi.localIP());
}


void InitConexaoBD() {



  if (temperatura != 0 && umidade != 0 && tensaoPlaca != 0 && tensaoCircuito != 0 && correnteCircuito != 0) {

    const int httpPort = 8181;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    client.print(String("GET http://10.107.4.2/conexaoEstacao.php?") +
                 ("&temperatura=") + temperatura +
                 ("&umidade=") + umidade +
                 ("&fumaca=") + fumacaD +
                 ("&fumacaA=") + fumacaA +
                 ("&tensaoPlaca=") + tensaoPlaca +
                 ("&correntePlaca=") + correntePlaca +
                 ("&tensaoCircuito=") + tensaoCircuito +
                 ("&correnteCircuito=") + correnteCircuito +
                 ("&tensaoAHT=") + tensaoAHT +
                 ("&correnteAHT=") + correnteAHT +
                 " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 1000 ) {
        //      Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    //  while (client.available()) {
    //    String line = client.readStringUntil('\r');
    //        Serial.println(line);
    //  }


    //  Serial.println("\nclosing connection");
  }
}

void ConexaoTelegram() {

  TBMessage msg;
  myBot.wifiConnect(ssid, password);
  myBot.setTelegramToken(BOT_TOKEN);

  if (myBot.testConnection()) {
    //Serial.println("Conexao Ok!");
    myBot.sendMessage(ADMINISTRADOR, "Estou ligado e pronto para transmitir!", "");
  }
}


void RecepcaoDeDados() {

  StaticJsonDocument <200> data;

  deserializeJson(data, HC12);
  serializeJson(data, Serial);
  // Cria mensagem Json na variável data
  // Cria string payload




  umidade =  data["umi"];
  temperatura = data["temp"];
  fumacaD = data["fumaD"];
  fumacaA = data["fumaA"];
  tensaoPlaca = data["tP"];
  correntePlaca = data["cP"];
  tensaoCircuito = data["tC"];
  correnteCircuito = data["cC"];
  tensaoAHT = data["tA"];
  correnteAHT = data["cA"];

  if (temperatura != 0 && umidade != 0 && tensaoPlaca != 0 && tensaoCircuito != 0 && correnteCircuito != 0) {

    if (tensaoCircuito <= 3.3) {
      myBot.sendMessage(ADMINISTRADOR,
                        "\n\nTensao da bateria baixa, estação retornará a se comunicar daqui a 1 hora");
    }
    if (fumacaD == false) {
      myBot.sendMessage(ADMINISTRADOR,
                        "\n\nATENÇÃO, FUMAÇA DETECTADA !!!!!!!!!!!!!!!!");
    }
  }

}
