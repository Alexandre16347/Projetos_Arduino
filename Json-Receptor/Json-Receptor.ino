#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define rxPin 5
#define txPin 4
#define setPin 6

SoftwareSerial HC12(rxPin, txPin); // ke arah sensor node
void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;
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
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, HC12);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  int indeks;
  int sessionid = doc["sessionid"];
  boolean uplink = doc["uplink"];
  JsonArray idmeter = doc["idmeter"].as<JsonArray>();
  JsonArray standmeter = doc["standmeter"].as<JsonArray>();
  int count = idmeter.size();

  if (uplink == false) {
    uplink = true;
    for (int i = 0; i < count; i++) { //Iterate through results
      doc["idmeter"][i];  //Implicit cast
      if (doc["idmeter"][i] == 2) {
        indeks = i;
        doc["standmeter"][indeks] = 100;
      }
    }
  }

  delay(300);
  serializeJsonPretty(doc, HC12);
}
