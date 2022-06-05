/*
   @autor Carlos Alexandre
   date 2022-05-28
*/

//Importacao de bibliotecas para uso dos sensores, módulos e rádio
#include <ArduinoJson.h> // Converter em Json
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <SDL_Arduino_INA3221.h> // Para uso do INA3221
#include <Adafruit_AHTX0.h> // Para uso do AHT10

// Instancia de Objetos
Adafruit_AHTX0 aht; // Instancia do obeto do sensor AHT10
SDL_Arduino_INA3221 ina3221(0x41); // Instancia do obeto do sensor AHT10
sensors_event_t humidity, temp; // Variaveis utilizadas pelo AHT10

//Instancias do HC12
//SoftwareSerial HC12(2, 3);  // Arduino
// Rx pino 2 (Ligar no Tx do outro)
// Tx pino 3 (Ligar no Rx do outro)

SoftwareSerial HC12(D3, D4); // Esp8266
// Rx pino D3 (Ligar no Tx do hc12)
// Tx pino D4 (Ligar no Rx do hc12)


//Definicao do tempo de dormida/sleep
//Não esquecer que assim q carregar o codigo
//ligar as portas D0(GPIO16) ao RES do esp e reiniciar
#define dormeMenos 5*6e7 // 2 * 60 segundos = 120 segundos = 2 minutos // 1e6 = 1000000 microsegundos = 1 segundo
#define dormeMais 6*6e8 // 6 * 10 minutos = 1 hora // 1e6 = 1000000 microsegundos = 1 segundo

#define Canal_1 1
#define Canal_2 2
#define Canal_3 3

int pin_mq = D6; // Porta usada para dados digitais do MQ-7
int pin_on = D5; // Porta para ligar sensores
int pin_hc = D7; // Porta para ligar hc12
//int pin_on_mq = D8; // Porta para ligar MQ-7

// DECLARAÇÃO DE VARIÁVEIS GLOBAIS
double temperatura, umidade;
boolean fumacaD;
double tensaoPlaca, tensaoCircuito, correntePlaca, correnteCircuito, tensaoAHT, correnteAHT;
int amostra = 100;
int fumacaA;


// DECLARAÇÃO DE FUNÇÕES
void ReadIna3221();
void MediaAHT10(int amostra);
void ReadMQ();
void EnviarDados();


void setup() {

  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10);

  Serial.begin(2400);
  // ESPERA O SERIAL ESTAR PRONTO
  while (!Serial);

  pinMode(pin_mq, INPUT);
  pinMode(pin_on, OUTPUT);
  pinMode(pin_hc, OUTPUT);
  // pinMode(pin_on_mq, OUTPUT);


  digitalWrite(pin_on, HIGH); // Ativa sensores
  digitalWrite(pin_hc, LOW);  // Desativa HC12
  delay(100);
  //digitalWrite(pin_on_mq, LOW);

  //  Serial.println("INA3221 Iniciando...");
  aht.begin(); //Inicialzando AHT10
  ina3221.begin(); // Inicializando INA3221
  HC12.begin(2400);  // Inicializando hc12 // Velocidade da Porta Serial do HC12

  /*if (HC12.isListening()) {
    Serial.println("OK");
    } else {
    Serial.println("NOK");
    }*/

  MediaAHT10(amostra);
  ReadMQ();
  ReadIna3221();


  delay(1000);

  digitalWrite(pin_on, LOW);  // Desativa Sensores
  digitalWrite(pin_hc, HIGH); // Ativa HC12

  delay(2000);

  EnviarDados();

  delay(2000);

  //Serial.print("a mimir por 10 segundos\n\n");
  //Tempo do deepSleep 60e6 = 60000000 microssegundos
  /*
    if (tensaoCircuito <= 3.3)
    ESP.deepSleep(dormeMais);
    else
    ESP.deepSleep(dormeMenos);
  */

  system_deep_sleep_set_option(4); // The chip won’t turn on RF after waking up from Deep-sleep. Power
  // consumption is the lowest, same as in Modem-sleep.

  // The system goes into Deep-sleep mode via the following interface.
  // The chip will be automatically woken up at regular intervals (unit: μs).
  if (tensaoCircuito <= 3.3)
    system_deep_sleep_instant(dormeMais);
  else
    system_deep_sleep_instant(dormeMenos);

}

void loop() {}

void ReadIna3221() {
  tensaoPlaca = ina3221.getBusVoltage_V (Canal_1);
  correntePlaca = ((ina3221.getCurrent_mA (Canal_1)) / 1000);
  tensaoCircuito = ina3221.getBusVoltage_V (Canal_3);
  correnteCircuito = ((ina3221.getCurrent_mA (Canal_3)) / 1000);
  tensaoAHT = ina3221.getBusVoltage_V (Canal_2);
  correnteAHT = ((ina3221.getCurrent_mA (Canal_2)) / 1000);
}

void MediaAHT10(int amostra) {   //Função para calcular a media de 20 leituras de Temperatura
  double mediaTemp = 0, mediaUmi = 0;
  sensors_event_t humidity, temp;

  for (int i = 0 ; i < amostra; i++) {
    aht.getEvent(&humidity, &temp);
    mediaTemp +=  temp.temperature;
    mediaUmi +=  humidity.relative_humidity;
  }

  temperatura = mediaTemp / amostra;
  umidade = mediaUmi / amostra;
}

void ReadMQ() {
  fumacaD = digitalRead(pin_mq);//Leemos el sensor
  fumacaA = analogRead(A0); //Lemos la salida analógica del MQ
}



void EnviarDados() {
  //criando arquivo Json para armazenar as variaveis antes do envio pelo HC12
  StaticJsonDocument<200> data;

  // Coleta dados do AHT10

  data["temp"] = temperatura;
  data["umi"] = umidade;

  // Coleta de dados do MQ-7

  data["fumaD"] = fumacaD;//leitura digital
  data["fumaA"] = fumacaA; //leitura Analogica

  // Coleta de dados do INA3221

  data["tP"] = tensaoPlaca;
  data["cP"] = correntePlaca;
  data["tC"] = tensaoCircuito;
  data["cC"] = correnteCircuito;

  data["cA"] = correnteAHT;
  data["tA"] = tensaoAHT;

  //serializeJson(data, Serial);

  //Comando usado para enviar o arquivo por HC12
  serializeJson(data, HC12);
}
