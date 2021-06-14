/********************************************************
 * CANAL INTERNET E COISAS
 * ESP8266 - Utilizacao do SPIFFS
 * 03/2018 - Andre Michelon
 */

// Bibliotecas
#include <FS.h>

void setup(){
  Serial.begin(115200);

  // --- Inicializando SPIFSS ---
  Serial.println("\nInicializando SPIFSS ---------------------");
  if (SPIFFS.begin()) {
    Serial.println("Ok");
  } else {
    Serial.println("Falha");
    while(true);
  }
  
  // --- Informacoes sobre SPIFSS ---
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  Serial.println("\nInformações ------------------------------");
  Serial.printf("totalBytes: %u\nusedBytes: %u\nfreeBytes: %u\nblockSize: %u\n"
                "pageSize: %u\nmaxOpenFiles: %u\nmaxPathLength: %u\n",
                  fs_info.totalBytes,
                  fs_info.usedBytes,
                  fs_info.totalBytes - fs_info.usedBytes,
                  fs_info.blockSize,
                  fs_info.pageSize,
                  fs_info.maxOpenFiles,
                  fs_info.maxPathLength);

  // --- Listagem de arquivos ---
  Serial.println("\nArquivos ---------------------------------");

  if (!SPIFFS.exists("/imagem.jpg")) {
    Serial.println("Arquivo '/imagem.jpg' inexistente");
  }

  if (!SPIFFS.exists("/Imagem.jpg")) {
    Serial.println("Arquivo '/Imagem.jpg' inexistente");
  }

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.printf(" %s - %u bytes\n", dir.fileName().c_str(), dir.fileSize());
  }

  // --- Gravando arquivo ---
  File file;

  file = SPIFFS.open("/Log/Log.txt", "a");

  if (file) {
    file.println("Inicializando...");
    file.printf("Registro %u\r\n", millis());
    file.close();
  }

  // --- Lendo arquivo ---
  Serial.println("\nLendo Arquivo ----------------------------");
  file = SPIFFS.open("/Log/Log.txt", "r");

  if (file) {
    Serial.printf("Nome: %s - %u bytes\n", file.name(), file.size());
    while (file.available()){
      Serial.println(file.readStringUntil('\n'));
    }
    file.close();
  }

  // --- Excluindo arquivo ---
  Serial.println("\nExcluindo Arquivo ----------------------------");
  if (SPIFFS.remove("/Log/Log.txt")) {
    Serial.println("Arquivo '/Log/Log.txt' excluído");
  } else {
    Serial.println("Exclusão '/Log/Log.txt' falhou");
  }
}

void loop() {
}

