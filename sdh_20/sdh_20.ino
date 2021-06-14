#include "uFire_SHT20.h"
uFire_SHT20 sht20;
#include "esp_camera.h"

////Definição dos pinos
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26  //sda - tx
#define SIOC_GPIO_NUM     27  //scl - rx
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define flash 4

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sht20.begin();

//Configuração do pinos da câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;  
  config.jpeg_quality = 4;
  config.fb_count = 1;
   
  esp_err_t err = esp_camera_init(&config); //Inicialização da câmera
   
  if (err != ESP_OK) {
     
    Serial.printf("O início da câmera falhou com erro 0x%x", err);//Informa erro se a câmera não for iniciada corretamente
    delay(1000);
    ESP.restart();//Reinicia o ESP
     
  }
}

void loop()
{
  Serial.println((String)sht20.temperature() + "°C");
  Serial.println((String)sht20.humidity() + " %");
  Serial.println();
  delay(5000);
}
