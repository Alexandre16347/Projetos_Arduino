//Programa: comunicação MQTT com ESP32
//Autor: Pedro Bertoleti

/* Headers */
#include <WiFi.h> /* Header para uso das funcionalidades de wi-fi do ESP32 */
#include <PubSubClient.h>  /*  Header para uso da biblioteca PubSubClient */
#include "Base64.h"
#include "esp_camera.h"


/* Defines do MQTT */
/* IMPORTANTE: recomendamos fortemente alterar os nomes
               desses tópicos. Caso contrário, há grandes
               chances de você enviar e receber mensagens de um ESP32
               de outra pessoa.
*/
/* Tópico MQTT para recepção de informações do broker MQTT para ESP32 */
//#define TOPICO_SUBSCRIBE "ESP32_recebe_informacao"
/* Tópico MQTT para envio de informações do ESP32 para broker MQTT */
#define TOPICO_PUBLISH   "ESP32_envia_informacao"
/* id mqtt (para identificação de sessão) */
/* IMPORTANTE: este deve ser único no broker (ou seja,
               se um client MQTT tentar entrar com o mesmo
               id de outro já conectado ao broker, o broker
               irá fechar a conexão de um deles).
*/
#define ID_MQTT  "ESP_32"
/*  Variáveis e constantes globais */
/* SSID / nome da rede WI-FI que deseja se conectar */
const char* SSID = "Grafica";
/*  Senha da rede WI-FI que deseja se conectar */
const char* PASSWORD = "gr@fica1";

/* URL do broker MQTT que deseja utilizar */
const char* BROKER_MQTT = "sistemas.amsolution.com.br";
/* Porta do Broker MQTT */
int BROKER_PORT = 1883;


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


/* Variáveis e objetos globais */
WiFiClient espClient;
PubSubClient MQTT(espClient);

//Prototypes
void init_serial(void);
void init_wifi(void);
void init_mqtt(void);
void reconnect_wifi(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verifica_conexoes_wifi_mqtt(void);

/*
    Implementações das funções
*/
void setup()
{
//  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  init_serial();
  pinMode(flash,OUTPUT);
  init_wifi();
  init_mqtt();

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

  //envio da imagem

  String Imagem = Captura_Imagem(); //Captura e envia imagem
  delay(1000);

  Serial.println(Imagem.length());
  Serial.println(Imagem);
  
  
  /* garante funcionamento das conexões WiFi e ao broker MQTT */
  verifica_conexoes_wifi_mqtt();
  /* Envia frase ao broker MQTT */
  
  Enviar_MQTT(Imagem, Imagem.length());
  

  /* keep-alive da comunicação com broker MQTT */
  MQTT.loop();
  delay(60000);

  //Tempo do deepSleep 60e6 = 60000000 microssegundos
  ESP.deepSleep(10e6);
  
}

/* Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial
           o que está acontecendo.
  Parâmetros: nenhum
  Retorno: nenhum
*/
void init_serial()
{
  Serial.begin(115200);
}

/* Função: inicializa e conecta-se na rede WI-FI desejada
   Parâmetros: nenhum
   Retorno: nenhum
*/
void init_wifi(void)
{
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");
  reconnect_wifi();
}

/* Função: inicializa parâmetros de conexão MQTT(endereço do
           broker, porta e seta função de callback)
   Parâmetros: nenhum
   Retorno: nenhum
*/
void init_mqtt(void)
{
  /* informa a qual broker e porta deve ser conectado */
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  Serial.println("Conectado ao MQTT!");
  /* atribui função de callback (função chamada quando qualquer informação do
    tópico subescrito chega) */
  //MQTT.setCallback(mqtt_callback);
}

/* Função: função de callback
            esta função é chamada toda vez que uma informação de
            um dos tópicos subescritos chega)
   Parâmetros: nenhum
   Retorno: nenhum
 * */
//void mqtt_callback(char* topic, byte* payload, unsigned int length)
//{
//  String msg;
//
//  //obtem a string do payload recebido
//  for (int i = 0; i < length; i++)
//  {
//    char c = (char)payload[i];
//    msg += c;
//  }
//  Serial.print("[MQTT] Mensagem recebida: ");
//  Serial.println(msg);
//}


/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
            em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
   Parâmetros: nenhum
   Retorno: nenhum
*/
void reconnect_mqtt(void)
{
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      //MQTT.subscribe(TOPICO_SUBSCRIBE);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
      init_wifi();
      init_mqtt();
    }  
  }
}

/* Função: reconecta-se ao WiFi
   Parâmetros: nenhum
   Retorno: nenhum
*/
void reconnect_wifi()
{
  /* se já está conectado a rede WI-FI, nada é feito.
     Caso contrário, são efetuadas tentativas de conexão */
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

/* Função: verifica o estado das conexões WiFI e ao broker MQTT.
           Em caso de desconexão (qualquer uma das duas), a conexão
           é refeita.
   Parâmetros: nenhum
   Retorno: nenhum
*/
void verifica_conexoes_wifi_mqtt(void)
{
  // se não há conexão com o WiFI, a conexão é refeita
  reconnect_wifi();
  // se não há conexão com o Broker, a conexão é refeita
  if (!MQTT.connected())
    reconnect_mqtt();
}

/* programa principal */
void loop()
{
  
  /* Agurda 1 minuto para próximo envio */
  //delay(1000);
}

void Enviar_MQTT(String Imagem, int tam){
  int cont=0;
  MQTT.publish(TOPICO_PUBLISH, "INICIO");
  for(int i=0; i<=tam; i+=100){
    if(i+99 > tam){
      MQTT.publish(TOPICO_PUBLISH, Imagem.substring(i).c_str());
      delay(100);
      Serial.println("Enviado parte ");
      Serial.println(cont);
    }else{
      MQTT.publish(TOPICO_PUBLISH, Imagem.substring(i,i+99).c_str());
      delay(100);
      Serial.println("Enviado parte ");
      Serial.println(cont);
    }
    cont++;
  } 
  MQTT.publish(TOPICO_PUBLISH, "FIM");
}

String Captura_Imagem(void) {
     
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
