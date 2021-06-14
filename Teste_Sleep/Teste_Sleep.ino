#include <GDBStub.h>
#include <ESP8266WiFi.h>
#include <SDL_Arduino_INA3221.h>

float placaT, placaA, redutorT, redutorA, bateriaT, bateriaA;

SDL_Arduino_INA3221 ina3221(0x40);

#define Canal_1 1
#define Canal_2 2 
#define Canal_3 3

// Configurações do Wifi
char ssid[] = "Grafica";         // nome do seu roteador WIFI (SSID)
char password[] = "gr@fica1";    // senha do roteador WIFI

void setup() {
  
  Serial.begin(9600);
  Serial.println("INA3221 Iniciando...");
  ina3221.begin();
  
  //define o máximo de milissegundos para aguardar os dados seriais.
  Serial.setTimeout(2000);
  pinMode(D2, OUTPUT);

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

  Serial.println("WIFI Conectado");

  //***************************************************************************************
  while(!Serial){}

  Serial.print("Tensão Placa = "); Serial.println(ina3221.getBusVoltage_V (Canal_1));
  Serial.print("Corrente Placa = "); Serial.println((ina3221.getCurrent_mA (Canal_1))/1000);
  Serial.println("");

  //Imprimindo dados do DSN

  Serial.print("Tensão DSN = "); Serial.println(ina3221.getBusVoltage_V (Canal_2));
  Serial.print("Corrente DSN = "); Serial.println((ina3221.getCurrent_mA (Canal_2))/1000);
  Serial.println("");

  //Imprimindo dados do Cunsumo Circuito

  Serial.print("Tensão Circuito = "); Serial.println(ina3221.getBusVoltage_V (Canal_3));
  Serial.print("Corrente Circuito = "); Serial.println((ina3221.getCurrent_mA (Canal_3))/1000);
  Serial.println("");







  Serial.print("Agora o ESP vai dormir por 1 minuto\n\n");
  //Tempo do deepSleep 10e6 = 10000000 microssegundos 
  ESP.deepSleep(10e6);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
