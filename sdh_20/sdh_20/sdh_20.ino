//#include <uFire_SHT20.h>
//#include <uFire_SHT20_JSON.h>
//#include <uFire_SHT20_MP.h>
#include "uFire_SHT20.h"
uFire_SHT20 sht20;


void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sht20.begin();
}

void loop()
{
  Serial.println("Temperatura: "+(String)sht20.temperature() + "°C");
  Serial.println("Umidade:    "+(String)sht20.humidity() + " %");
  Serial.println();
  delay(2000);
}
