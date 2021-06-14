char caracteres[80] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', ',', '?', '`', '!', '/', '(', ')', '&', ':', ';', '=', '-', '_', '"', '$', '@', ' '};

void setup() {
  Serial.begin(9600);
}

void loop() {
  for(int i = 0; i<80;i++){
    Serial.println(caracteres[i]+(String)i);
  }
  delay(2000);
}
