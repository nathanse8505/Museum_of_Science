#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

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

#define BUTTON_PIN 12  // Bouton sur GPIO 14

int photoCount = 0;
const int BOUNCE_TIME = 50;
bool check = LOW; 

void startCamera() {
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

  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Erreur init caméra: 0x%x", err);
    return;
  }
}

bool PRESS_BUTTON(){
  // Detect button press (active LOW) if previously unpressed
  if (digitalRead(BUTTON_PIN) == LOW && check == LOW){
    check = HIGH;          // Mark the button as pressed
    delay(BOUNCE_TIME);         // Debounce delay
  }
  // Detect button release (goes HIGH) if it was previously pressed
  if (digitalRead(BUTTON_PIN) == HIGH && check == HIGH){
    check = LOW;           // Reset the state for the next press
    return true;                // Indicate a valid press-release cycle
  }
  return false;                 // Otherwise, no valid press-release detected
}


void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  startCamera();

  if (!SD_MMC.begin()) {
    Serial.println("Erreur init SD_MMC !");
    return;
  }

  Serial.println("SD_MMC OK !");
}

void takePhoto() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Erreur capture photo");
    return;
  }

  String path = "/photo_" + String(photoCount++) + ".jpg";
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Erreur ouverture fichier");
    esp_camera_fb_return(fb);
    return;
  }

  file.write(fb->buf, fb->len);
  file.close();
  esp_camera_fb_return(fb);

  Serial.printf("Photo sauvegardée : %s\n", path.c_str());
}

void loop() {
  if (PRESS_BUTTON()) {
    Serial.println("Bouton pressé !");
    takePhoto();
  }

}
