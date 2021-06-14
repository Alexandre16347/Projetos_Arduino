

int sensor = A0;      
//variável usada para ler o valor do sensor em tempo real.
int valorSensor = 0; 
 
//Método setup, executado uma vez ao ligar o Arduino.
void setup(){
  //Ativando o serial monitor que exibirá os 
  //valores lidos no sensor.
  Serial.begin(9600);
}
 
//Método loop, executado enquanto o Arduino estiver ligado.
void loop(){
   
  //Lendo o valor do sensor.
  int valorSensor = analogRead(sensor);
   
  //Exibindo o valor do sensor no serial monitor.
  Serial.println(valorSensor);
   
  delay(500); 
}
