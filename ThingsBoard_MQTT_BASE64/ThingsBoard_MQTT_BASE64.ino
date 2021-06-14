#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h> /* Header para uso das funcionalidades de wi-fi do ESP32 */
#include "Base64.h"
#include "esp_camera.h"

// Configurações do Wifi
#define WIFI_AP "Grafica"
#define WIFI_PASSWORD "gr@fica1"

/* Tópico MQTT para recepção de informações do broker MQTT para ESP32 */
#define TOPICO_SUBSCRIBE "ESP32_recebe_informacao"
/* Tópico MQTT para envio de informações do ESP32 para broker MQTT */
#define TOPICO_PUBLISH   "ESP32_envia_informacao"

// Configurações do ThingsBoard
// O TOKEN do dispositivo é obtido na
// guia Devices -> ESP8266 Sala de Estar
#define TOKEN "DDtmI8d3HJ0hYWQhxCS5"
// Endereço do servidor ThingsBoard
char thingsboardServer[] = "gpiot.webfrequencia.com.br";
// Tópico onde serão publicados os dados do sensor
char topic_publish[] = "v1/devices/me/telemetry";

//Definição dos pinos
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define flash 4


// Instancia objeto wifiClient
WiFiClient wifiClient;

// Instancia cliente MQTT 'client'
PubSubClient client(wifiClient);

// Status de conexão Wifi
int status = WL_IDLE_STATUS;
// Variável para controle do
// intervalo de tempo
unsigned long last_time;

void setup() {
  // Inicia porta serial
  //Serial.begin(9600);
  //  Serial.println("INA3221 Iniciando...")

  //define o máximo de milissegundos para aguardar os dados seriais.
  //Serial.setTimeout(2000);

  // Função que inicia a conexão com o Access Point
  InitWiFi();

  pinMode(flash,OUTPUT);

  //Configuração do pinos da câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;  
  config.jpeg_quality = 4;
  config.fb_count = 1;
   
  esp_err_t err = esp_camera_init(&config); //Inicialização da câmera
   
  if (err != ESP_OK) {
     
    Serial.printf("O início da câmera falhou com erro 0x%x", err);//Informa erro se a câmera não for iniciada corretamente
    delay(1000);
    ESP.restart();//Reinicia o ESP
     
  }
  
  // Em seguida, configura a conexão MQTT
  client.setServer( thingsboardServer, 1883 );
  // Define função on_message como retorno de chamada MQTT
  // Processa as mensagens recebidas do servidor
  client.setCallback(on_message);


  // Se o cliente MQTT não tiver conectado, executa função reconnect()
  if ( !client.connected() ) {
    reconnect();
  }

  
  client.loop();
  // Enviar dados do sensor periodicamente
  // Se o intervalo de tempo for maior que 1s
  if ( millis() - last_time > 1000 ) {
    // Envia dados do sensor ao ThingsBoard
    sendTemperatureAndHumidity();
    last_time = millis();
  }


  //  Serial.print("Tensão Placa = "); Serial.println(ina3221.getBusVoltage_V (Canal_1));
  //  Serial.print("Corrente Placa = "); Serial.println((ina3221.getCurrent_mA (Canal_1))/1000);
  //  Serial.println("");
  //
  //  //Imprimindo dados do DSN
  //
  //  Serial.print("Tensão DSN = "); Serial.println(ina3221.getBusVoltage_V (Canal_2));
  //  Serial.print("Corrente DSN = "); Serial.println((ina3221.getCurrent_mA (Canal_2))/1000);
  //  Serial.println("");
  //
  //  //Imprimindo dados do Cunsumo Circuito
  //
  //  Serial.print("Tensão Circuito = "); Serial.println(ina3221.getBusVoltage_V (Canal_3));
  //  Serial.print("Corrente Circuito = "); Serial.println((ina3221.getCurrent_mA (Canal_3))/1000);
  //  Serial.println("");

  delay(2000);

  //  Serial.print("Agora o ESP vai dormir por 1 minuto\n\n");
  //Tempo do deepSleep 60e6 = 60000000 microssegundos
  ESP.deepSleep(15e6);
}

void loop() {
  // put your main code here, to run repeatedly:

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
  StaticJsonDocument <100000> data;
  // Desserializando a mensagem da variável message_json
  DeserializationError error = deserializeJson(data, message_json);
  // Se houver erro ao desserializar, informa.
  if (error) {
    //    Serial.print("Erro ao desserializar Json");
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
    StaticJsonDocument <100000> send_data;
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

// Inicia conexão Wifi
void InitWiFi() {
  //  Serial.println("Conectando-se ao AP ...");
  // Inicia conexão com Wifi
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //    Serial.print(".");
//    digitalWrite(LED_BUILTIN, LOW);
//    delay(50);
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(50);
  }
  //  Serial.println("Conexão Wifi estabelecida");
}

// Reconecta cliente MQTT ao ThingsBoard
void reconnect() {
  // Se o cliente MQTT não estiver conectado, verifica conexão wifi e mqtt
  while (!client.connected()) {
    status = WiFi.status();
    // Caso não esteja conectado ao wifi, inicia conexão
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        //        Serial.print(".");
//        digitalWrite(LED_BUILTIN, LOW);
//        delay(50);
//        digitalWrite(LED_BUILTIN, HIGH);
//        delay(50);
      }
      //      Serial.println("Connected to AP");
    }
    //    Serial.print("Conectando-se ao ThingsBoard ...");
    // Inicia conexão MQTT
    // Função connect(clientId, username, password)
    if ( client.connect("Monitoramento-Placa", TOKEN, NULL) ) {
      // Imprime resultado da conexão MQTT, indicando que foi estabelecida
      //      Serial.println( "[MQTT DONE]" );
      // Inscreve para receber as solicitações RPC
      client.subscribe("v1/devices/me/rpc/request/+");

    } else {
      // Imprime resultado da conexão MQTT, indicando que não foi estabelecida
      //      Serial.print( "[MQTT FAILED] [ rc = " );
      //      Serial.print( client.state() );
      //      Serial.println( " : retrying in 5 seconds]" );
      //      // Aguarda 5 segundos antes de se reconectar
      delay( 5000 );
    }
  }
}

void sendTemperatureAndHumidity() {

  StaticJsonDocument <100000> data;
  // Cria mensagem Json na variável data
  // Cria string payload

  data["Imagem_Base64"] = "fui";//Captura_Imagem();
  

  String payload;
  // Serializa a variável data para string payload
  serializeJson(data, payload);
  // Por fim, publica esta string no tópico especificado
  client.publish(topic_publish, payload.c_str());
  // Imprime o tópico e a mensagem enviados
  //    Serial.print("Publish in topic: ");
  //    Serial.print(topic_publish);
  //    Serial.print(" , message: ");
  //    Serial.print(payload.c_str());
  //    Serial.println();

}

String Captura_Imagem() {
     
    Serial.println("Tirando foto!");
     
    camera_fb_t * fb = NULL;
     
    digitalWrite(flash,HIGH);
    delay(100);
     
    fb = esp_camera_fb_get(); //Função que captura imagem na ESP
     
    digitalWrite(flash,LOW);
    delay(100);
     
    if(!fb) {
       
      Serial.println("Falha ao capturar imagem!");
      delay(1000);
      ESP.restart();
      return "";
    }
     
    char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];
    String imageFile = "";
     
    for (int i=0;i<fb->len;i++) {
      base64_encode(output, (input++), 3);
      if (i%3==0) imageFile += urlencode(String(output));
    }
     
    esp_camera_fb_return(fb); 
    
    return imageFile;
}
 
 
String urlencode(String str)  //Função de codificação
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
     
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
      }
      yield();
    }
    return encodedString;
}
