/* 
 _____________________________________
|  Curso Arduino 3 - Botão de pulso   |
|             OnDuino          |
|  http://www.onduino.blogspot.com.br |
|_____________________________________|

LED só fica aceso enquanto o botão estiver pressionado.

*/

int led = 2;            //LED conectado no pino 2
int botao = 7;          //Botão conectado no pino 7
int estadobotao = 0;    //Variável para ler o estado do botão

void setup(){
  pinMode(led, OUTPUT);   //Pino 2 (led) selecionado como saída
  pinMode(botao, INPUT);    //Pino 7 (botão) selecionado como entrada
}

void loop()
{
  estadobotao = digitalRead(botao);   
  /* A variável "estadobotao" será igual ao valor do pino 10 (botão)
  Se o botão estiver presisionado a variável terá valor 1 (5V no pino 7)
  Se o botão não estiver presisionado a variável terá valor 0 (0V no pino 7)*/
  
  if(estadobotao == HIGH){    //Se o botão estiver pressionado...
    digitalWrite(led, HIGH);    //... LED ligado
  }else{              //Se o botão não estiver pressionado...
    digitalWrite(led, LOW);     //... LED desligado
  }
}
