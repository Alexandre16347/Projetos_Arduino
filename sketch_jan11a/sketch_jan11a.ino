#include <SDL_Arduino_INA3221.h>

SDL_Arduino_INA3221 ina3221(0x40);

#define Canal_1 1
#define Canal_2 2
#define Canal_3 3

void setup() {
  // put your Ysetup code here, to run once:
  // Inicia porta serial
  Serial.begin(9600);
  Serial.println("INA3221 Iniciando...");
  ina3221.begin();

  //define o máximo de milissegundos para aguardar os dados seriais.
  Serial.setTimeout(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  float Tensao_Placa = ina3221.getBusVoltage_V (Canal_1);
  float Corrente_Placa = ((ina3221.getCurrent_mA (Canal_1)) / 1000);
  float Tensao_DSN = ina3221.getBusVoltage_V (Canal_2);
  float Corrente_DSN = ((ina3221.getCurrent_mA (Canal_2)) / 1000);
  float Tensao_Circuito = ina3221.getBusVoltage_V (Canal_3);
  float Corrente_Circuito = ((ina3221.getCurrent_mA (Canal_3)) / 1000);

  Serial.println(Tensao_Placa);
  Serial.println(Corrente_Placa);
  Serial.println(Tensao_DSN);
  Serial.println(Corrente_DSN);
  Serial.println(Tensao_Circuito);
  Serial.println(Corrente_Circuito);
  Serial.println();
  Serial.println();
  delay(1000);
}
