#include "Arduino.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void initialize_HC12();
void setup_wifi();
void reconnect();

#define RX D2
#define TX D1

SoftwareSerial HC12(RX, TX);

#define TOKEN "xxxxxxxxxx"

const char* ssid = "NPD-2,4GHZ";
const char* password = "falecomoarata";

const char mqtt_server[] = "docker.webfrequencia.com.br";
const char publishTopic[] = "v1/devices/me/telemetry";

WiFiClient espClient;
PubSubClient client(espClient);

long lastData = 0;
//String payload;
//char attributes[1000];
boolean state = false;

void initialize_HC12() {
  
  pinMode(D3, OUTPUT);

  HC12.begin(9600);

  if (HC12.isListening()) {
    Serial.println("OK");
  }

  else {
    Serial.println("Not OK");
  }

  //Test HC-12
  digitalWrite(D3, LOW);
  HC12.write("AT+DEFAULT");
  delay(100);
  digitalWrite(D3, HIGH);

  Serial.println();
  Serial.println("Initialization done.");

}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.println("Connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {

  while (!client.connected()) {

    if (client.connect("deviceID", TOKEN, NULL)) {

      Serial.println("Connected to server");

    }

    else {

      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }


}

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  initialize_HC12();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  String payload;
  char attributes[1000];

  if (HC12.available() > 0) {

    while (HC12.available() > 0) {

      payload = HC12.readString();
      payload.toCharArray(attributes, 1000);
      Serial.print(attributes);
      client.publish(publishTopic, attributes);
    }

    /*payload = HC12.readString();
      payload.toCharArray(attributes, 1000);
      Serial.print(attributes);
      client.publish(publishTopic, attributes);
      delay(3000);*/
  }

  delay(3000);
  digitalWrite(LED_BUILTIN, state);
  state = not state;
}
