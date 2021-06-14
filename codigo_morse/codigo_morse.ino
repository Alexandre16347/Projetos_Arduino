String morse[26] = {"01", "1000", "1010", "100", "0", "0010", "110", "0000", "00", "0111", "101", "0100", "11", "10", "111", "0110", "1101", "010", "000", "1", "001", "0001", "011", "1001", "1011", "1100"};
String morseCarac[17] = {"010101", "110011", "001100", "011110", "101011", "10010", "10110", "101101", "01000", "111000", "101010", "10001", "100001", "001101", "010010", "0001001", "011010"};
String morseNum[10] = {"01111", "00111", "00011", "00001", "00000", "10000", "11000", "11100", "11110", "11111"};

int numero[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
char caract[17] = {'.', ',', '?', '`', '!', '/', '(', ')', '&', ':', ';', '=', '-', '_', '"', '$', '@'};
char alfabeto[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
char alfabetoMaiusculo[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

String nome;
String num;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  nome = coletaDados();
  Serial.println(nome);
  digitalWrite(LED_BUILTIN, HIGH);

  for (int i = 0;  i < nome.length(); i++) {
    for (int j = 0; j < 26; j ++) {
      if (nome[i] == alfabeto[j] || nome[i] == alfabetoMaiusculo[j]) {
        for (int k = 0; k < morse[j].length(); k++) {
          ledMorse(morse[j][k]);
        }
      }
    }
    for (int j = 0; j < 10; j ++) {
      if (nome[i] == numero[j]) {
        for (int k = 0; k < morseNum[j].length(); k++) {
          ledMorse(morseNum[j][k]);
        }
      }
    }

    for (int j = 0; j < 17; j ++) {
      if (nome[i] == caract[j]) {
        for (int k = 0; k < morseCarac[j].length(); k++) {
          ledMorse(morseCarac[j][k]);
        }
      }
    }
    delay(1000);
  }
}
String coletaDados() {
  String nome = "";
  while (Serial.available() != 0) {
    nome += (char)Serial.read();
  }

  return nome;
}

void ledMorse(char letraMorse) {
  if (letraMorse == '0') {
    digitalWrite(LED_BUILTIN, LOW);
    delay(700);

  } else if (letraMorse == '1') {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  delay(150);
}
