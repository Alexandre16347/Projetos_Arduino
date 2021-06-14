#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#define rxPin 5
#define txPin 4
#define setPin 6

SoftwareSerial HC12(rxPin, txPin); // ke arah sensor node

void setup() {
  Serial.begin(9600);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  HC12.begin(9600);
  
  if (HC12.isListening()) {
    Serial.println("OK");
  } else {
    Serial.println("NOK");
  }
}

void loop() {
  while (!Serial) continue;
  StaticJsonDocument<200> doc;

  doc["sessionid"] = 1;
  doc["uplink"] = false;

  JsonArray idmeter = doc.createNestedArray("idmeter");
  idmeter.add(1);
  idmeter.add(2);
  idmeter.add(3);
  JsonArray standmeter = doc.createNestedArray("standmeter");
  standmeter.add(0);
  standmeter.add(0);
  standmeter.add(0);

  serializeJsonPretty(doc, HC12);

  delay(300);
  void clear();
  delay(1000);

  StaticJsonDocument<200> dok;
  deserializeJson(dok, HC12);
  serializeJsonPretty(doc, Serial);
}
