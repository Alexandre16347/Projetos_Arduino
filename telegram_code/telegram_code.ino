#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

char ssid[] = "Grafica";               // nome do seu roteador WIFI (SSID)
char password[] = "gr@fica1";    // senha do roteador WIFI

#define BOT_TOKEN "1339424001:AAF62CsN7KidCcyhr48alknKpUUQx1VJBWI"  // sua chave Token Bot Telegram
// Para obter o Chat ID, acesse Telegram => usuario IDBot => comando /getid

// cliente SSL necessario para a Biblioteca
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

#define R1 D1    // GPIO_14 
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

//***************************************************************************************

void setup()
{
  Serial.begin(115200);

  // Inicializa as entradas e saidas

  pinMode(R1, OUTPUT);
  digitalWrite(R1, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //***************************************************************************************

  // Inicializa conexão Wifi

  WiFi.mode(WIFI_AP_STA);   // Configura o WIFI para modo estação e Acess point
  WiFi.disconnect();        // desconecta o WIFI
  delay(100);               // atraso de 100 milisegundos

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)    // aguardando a conexão WEB
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
  }

  // Conecta cliente com SSL

  client.setFingerprint("F2:AD:29:9C:34:48:DD:8D:F4:CF:52:32:F6:57:33:68:2E:81:C1:90");
  while (!client.connect("api.telegram.org", 443))
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }

}

void loop()
{
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }

  if (millis() > Bot_lasttime + Bot_mtbs) {

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {

      for (int i = 0; i < numNewMessages; i++) {

        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;


        if (text == "On" || text == "on" || text == "ON") {
          digitalWrite(R1, HIGH) ;
          bot.sendMessage(chat_id, from_name + " / Dispositivo / SAÍDA 1 LIGADA", "");
        }

        if (text == "Off" || text == "off" || text == "OFF") {
          digitalWrite(R1, LOW);
          bot.sendMessage(chat_id, from_name + " / Dispositivo / SAÍDA 1 DESLIGADA", "");
        }

      }

      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }


}
