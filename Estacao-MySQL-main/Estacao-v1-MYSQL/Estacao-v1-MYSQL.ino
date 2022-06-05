#include <ESP8266WiFi.h>
#include <SDL_Arduino_INA3221.h>
#include <Adafruit_AHTX0.h>
#include "seguranca.h"
Adafruit_AHTX0 aht;

SDL_Arduino_INA3221 ina3221(0x41);


#define Canal_1 1
#define Canal_2 2
#define Canal_3 3

int pin_mq = D6;
int pin_on = D5;

// DECLARAÇÃO DE VARIÁVEIS GLOBAIS
double temperatura;
double umidade;
boolean fumacaD;
int fumacaA;
double tensaoPlaca, tensaoCircuito, correntePlaca, correnteCircuito;
int amostra = 20;
sensors_event_t humidity, temp;


// DECLARAÇÃO DE VARIÁVEIS PARA WIFI
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* host = SECRET_HOST;


// DECLARAÇÃO DE FUNÇÕES
void InitWiFi();
void InitConexao();
void ReadIna3221();
void MediaAHT10(int amostra);
void ReadMQ();



void setup() {
  //  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(pin_mq, INPUT);
  pinMode(pin_on, OUTPUT);

  digitalWrite(pin_on, HIGH);

  // ESPERA O SERIAL ESTAR PRONTO
  while (!Serial);

  //  Serial.println("INA3221 Iniciando...");
  ina3221.begin();

  aht.begin();

  InitWiFi();

  //  Serial.print("Connecting to: " );
  //  Serial.println(host);

  ReadIna3221();
  MediaAHT10(amostra);
  ReadMQ();

  delay(2000);

  InitConexao();

  delay(2000);

  //    Serial.print("a mimir por 10 segundos\n\n");
  //Tempo do deepSleep 60e6 = 60000000 microssegundos
  if (tensaoCircuito <= 3.3)
    ESP.deepSleep(dormeMais);
  else
    ESP.deepSleep(dormeMenos);

}

void loop() {}


// Inicia conexão Wifi
void InitWiFi() {
  //  Serial.println("\n\nConectando-se ao AP ...");
  // Inicia conexão com Wifi
  WiFi.begin(ssid, password);

  unsigned long limitConnetion = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);

    if (millis() - limitConnetion > 20000 ) {
      ESP.deepSleep(dormeMenos);
    }
  }
  //  Serial.println("Conexão Wifi estabelecida");
  //  // print out info about the connection:
  //  Serial.print("My IP address is: ");
  //  Serial.println(WiFi.localIP());
}


void InitConexao() {

  WiFiClient client;
  const int httpPort = 8080;
  if (!client.connect(host, httpPort)) {
    //    Serial.println("connection failed");
    return;
  }

  client.print(String("GET http://10.107.4.5/iot_mysql/conexaoEstacao.php?") +
               ("&temperatura=") + temperatura +
               ("&umidade=") + umidade +
               ("&fumaca=") + fumacaD +
               ("&tensaoPlaca=") + tensaoPlaca +
               ("&correntePlaca=") + correntePlaca +
               ("&tensaoCircuito=") + tensaoCircuito +
               ("&correnteCircuito=") + correnteCircuito +
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

void ReadIna3221() {
  tensaoPlaca = ina3221.getBusVoltage_V (Canal_1);
  correntePlaca = ((ina3221.getCurrent_mA (Canal_1)) / 1000);
  tensaoCircuito = ina3221.getBusVoltage_V (Canal_3);
  correnteCircuito = ((ina3221.getCurrent_mA (Canal_3)) / 1000);
}

void MediaAHT10(int amostra) {   //Função para calcular a media de 20 leituras de Temperatura
  double media = 0.0;
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  for (int i = 0 ; i < amostra; i++) {
    media +=  temp.temperature;
  }

  temperatura = media / amostra;
  media = 0.0;

  for (int i = 0 ; i < amostra; i++) {
    media +=  humidity.relative_humidity;
  }

  umidade = media / amostra;
}

void ReadMQ() {
  fumacaD = digitalRead(pin_mq);//Leemos el sensor
  fumacaA = analogRead(A0); //Lemos la salida analógica del MQ
}
