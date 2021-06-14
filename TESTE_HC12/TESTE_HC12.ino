#include <SoftwareSerial.h>
SoftwareSerial HC12(2,3);      // RX pino 2 , TX pino 3
bool msg = false;
void setup(){
    Serial.begin(2400);
    HC12.begin(2400);
}

void loop(){
    Serial.print("Recebido: ");
    while (HC12.available()){
      Serial.print((char)HC12.read());
      msg = true;
    }
    if (msg){
        HC12.write("Bom site!");
        msg = false;
    }else{
      Serial.println("nada");
    }
    delay(2000);
}
