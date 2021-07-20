#include <SDL_Arduino_INA3221.h>

SDL_Arduino_INA3221 ina3221(0x40);

#define Canal_1 1
#define Canal_2 2
#define Canal_3 3

void setup() {
  Serial.begin(9600);
  Serial.println("INA3221 Iniciando...");
  ina3221.begin();

}

void loop() {
  Serial.print("Tensão Placa = "); Serial.println(ina3221.getBusVoltage_V (Canal_1));
  Serial.print("Corrente Placa = "); Serial.println((ina3221.getCurrent_mA (Canal_1)) / 1000);
  Serial.println("");

  //Imprimindo dados do DSN

  Serial.print("Tensão DSN = "); Serial.println(ina3221.getBusVoltage_V (Canal_2));
  Serial.print("Corrente DSN = "); Serial.println((ina3221.getCurrent_mA (Canal_2)) / 1000);
  Serial.println("");

  //Imprimindo dados do Cunsumo Circuito

  Serial.print("Tensão Circuito = "); Serial.println(ina3221.getBusVoltage_V (Canal_3));
  Serial.print("Corrente Circuito = "); Serial.println((ina3221.getCurrent_mA (Canal_3)) / 1000);
  Serial.println("\n\n\n\n\n");

  delay(4000);

}
