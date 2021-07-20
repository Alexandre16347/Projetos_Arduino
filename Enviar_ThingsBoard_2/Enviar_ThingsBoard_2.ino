#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SDL_Arduino_INA3221.h>
#include "uFire_SHT20.h"


uFire_SHT20 sht20;

SDL_Arduino_INA3221 ina3221(0x41);


#define Canal_1 1
#define Canal_2 2
#define Canal_3 3

// Configurações do Wifi
#define WIFI_AP "Grafica"
#define WIFI_PASSWORD "gr@fica1"

//Definicao do tempo de dormida/sleep
#define dorme 30e6 // 30 segundos // e6 = microsegundos

// Configurações do ThingsBoard
// O TOKEN do dispositivo é obtido na
// guia Devices -> ESP8266 Sala de Estar
#define TOKEN "YD7C25qcTjmiprrezEHE"


// Endereço do servidor ThingsBoard
char thingsboardServer[] = "alexandre.webfrequencia.com.br";

// Tópico onde serão publicados os dados do sensor
char topic_publish[] = "v1/devices/me/telemetry";


// Instancia objeto wifiClient
WiFiClient wifiClient;

// Instancia cliente MQTT 'client'
PubSubClient client(wifiClient);

// Status de conexão Wifi
int status = WL_IDLE_STATUS;
// Variável para controle do
// intervalo de tempo
unsigned long last_time;




// Inicia conexão Wifi
void InitWiFi() {
  Serial.println("Conectando-se ao AP ...");
  // Inicia conexão com Wifi
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
  }
  Serial.println("Conexão Wifi estabelecida");
}


// Inicia conexão ThingsBoard
void InitThingsBoard() {
  Serial.print("Conectando-se ao ThingsBoard ...");
  // Inicia conexão MQTT
  // Função connect(clientId, username, password)
  if ( client.connect("Estacao", TOKEN, NULL) ) {
    // Imprime resultado da conexão MQTT, indicando que foi estabelecida
    Serial.println( "[MQTT DONE]" );
    // Inscreve para receber as solicitações RPC
    //client.subscribe("v1/devices/me/rpc/request/+");

  } else {
    // Imprime resultado da conexão MQTT, indicando que não foi estabelecida
    Serial.print( "[MQTT FAILED] [ rc = " );
    Serial.print( client.state() );
    Serial.println( " : retrying in 5 seconds]" );
    // Aguarda 5 segundos antes de se reconectar
    delay( 5000 );
  }
}


// Reconecta cliente MQTT ao ThingsBoard
void reconnect() {
  // Se o cliente MQTT não estiver conectado, verifica conexão wifi e mqtt
  while (!client.connected()) {
    status = WiFi.status();

    // Caso não esteja conectado ao wifi, inicia conexão
    if ( status != WL_CONNECTED) {
      InitWiFi();
    }

    InitThingsBoard();
  }
}


// Coleta dados do sensor e envia ao Servidor
void sendTelemetry() {

  StaticJsonDocument <200> data;
  // Cria mensagem Json na variável data
  // Cria string payload

  String payload;


  //Dados do sensor de Temperatura e humidade;
  data["Temperatura"] = (String)sht20.temperature() + " °C";
  data["Umidade"] = (String)sht20.humidity() + " %";

   //Minha localização
  data["latitude"] = -4.2795657;
  data["longitude"] = -41.7782000;

  

  
  // Serializa a variável data para string payload
  serializeJson(data, payload);

  // Por fim, publica esta string no tópico especificado
  client.publish(topic_publish, payload.c_str());

  // Imprime o tópico e a mensagem enviados
  Serial.print("Publish in topic: ");
  Serial.print(topic_publish);
  Serial.print(" , message: ");
  Serial.print(payload.c_str());
  Serial.println();

  StaticJsonDocument <200> data2;
  String payload2;

  //Dados da Placa e Bateria
  data2["Tensão_Placa"] = ina3221.getBusVoltage_V (Canal_1);
  data2["Corrente_Placa"] = ((ina3221.getCurrent_mA (Canal_1)) / 1000);
  data2["Tensão_Stepdown"] = ina3221.getBusVoltage_V (Canal_2);
  data2["Corrente_Stepdown"] = ((ina3221.getCurrent_mA (Canal_2)) / 1000);
  data2["Tensão_Bateria"] = ina3221.getBusVoltage_V (Canal_3);
  data2["Corrente_Bateria"] = ((ina3221.getCurrent_mA (Canal_3)) / 1000);


  // Serializa a variável data para string payload
  serializeJson(data2, payload2);

  // Por fim, publica esta string no tópico especificado
  client.publish(topic_publish, payload2.c_str());

  // Imprime o tópico e a mensagem enviados
  Serial.print("Publish in topic: ");
  Serial.print(topic_publish);
  Serial.print(" , message: ");
  Serial.print(payload2.c_str());
  Serial.println();


  

}



void on_message(const char* topic, byte* payload, unsigned int length) {
  // Variável que receberá a mensagem da variavel payload
  char message_json[length + 1];
  // Copia os bytes da payload para a variavel message_json
  strncpy (message_json, (char*)payload, length);
  // Define o terminador da string com o caracter nulo '\0'
  // Identificando o fim da string.
  message_json[length] = '\0';

  // Cria a variável Json data de tamanho 200 bytes
  StaticJsonDocument <200> data;
  // Desserializando a mensagem da variável message_json
  DeserializationError error = deserializeJson(data, message_json);
  // Se houver erro ao desserializar, informa.
  if (error) {
    Serial.print("Erro ao desserializar Json");
    return;
  }

  // Status do gpio
  boolean status_gpio;
  // Recebe parâmetro "method"
  String methodName = String((const char*)data["method"]);
  // Recebe parâmetro "enable"
  boolean enabled = data["params"]["enabled"];
  // Recebe parâmetro "gpio"
  int pin = data["params"]["gpio"];

  // Verifica se o método recebido é "setValue"
  if (methodName.equals("setValue")) {
    // Se enable for true, então aciona relé
    // Note que o acionamento é com nível lógico 0
    if (enabled) {
      digitalWrite(pin, LOW);
      // Atualiza status do pino
      status_gpio = true;
    }
    // Caso contrário, desliga relé
    else {
      digitalWrite(pin, HIGH);
      status_gpio = false;
    }


    // Variável que recebe o tópico
    // Ex. tópico: v1/devices/me/rpc/request/$request_id
    String responseTopic = String(topic);
    // Substitui a palavra request por response, atualizando o tópico
    // Ex. tópico atualizado: v1/devices/me/rpc/response/$request_id
    responseTopic.replace("request", "response");
    // Cria variável Json send_data
    StaticJsonDocument <200> send_data;
    // Cria mensagem Json na variável send_data
    send_data["method"] = "getValue";
    JsonObject obj = send_data.createNestedObject("params");
    obj["enabled"] = status_gpio;
    obj["gpio"] = pin;
    /*
        Formato da mensagem criada:
        {
          "method" : "getValue",
          "params" : {
              "enabled" : true,
              "gpio"    : 2
          }
        }

    */
    String send_payload;
    // Serializa a variável send_data para string send_payload
    serializeJson(send_data, send_payload);
    // Por fim, publica no tópico atualizado e mensagem criada
    client.publish(responseTopic.c_str(), send_payload.c_str());
  }
}


void setup() {
  // Inicia porta serial
  Serial.begin(9600);
  Wire.begin();
  sht20.begin();

  Serial.println("\n\nligou!!!");


  // Função que inicia a conexão com o Access Point
  InitWiFi();

  // Em seguida, configura a conexão MQTT
  client.setServer( thingsboardServer, 1883 );
  // Define função on_message como retorno de chamada MQTT
  // Processa as mensagens recebidas do servidor
  client.setCallback(on_message);

  InitThingsBoard();


  // Se o cliente MQTT não tiver conectado, executa função reconnect()
  if ( !client.connected() ) {
    reconnect();
  }


  client.loop();


  // Enviar dados do sensor periodicamente
  // Se o intervalo de tempo for maior que 1s
  if ( millis() - last_time > 1000 ) {
    // Envia dados do sensor ao ThingsBoard
    sendTelemetry();
    last_time = millis();
  }

  delay(2000);

  Serial.print("Agora o ESP vai dormir por 30 segundos\n\n");
  //Tempo do deepSleep 60e6 = 60000000 microssegundos
  ESP.deepSleep(dorme);
}


void loop() {}
