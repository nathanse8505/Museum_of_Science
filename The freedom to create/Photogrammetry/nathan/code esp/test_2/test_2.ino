#include "const_n_global.h"
#include "fonction.h"

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //pinMode(FLASH_PIN, OUTPUT);    // GPIO du flash

  startCamera();

  if (!SD_MMC.begin()) {
    Serial.println("Erreur init SD_MMC !");
    return;
  }
  
  Serial.println("SD_MMC OK !");
}

void loop() {
  
  if (PRESS_BUTTON()) {
    Serial.println("Bouton pressé !");
    takePhoto();
  }

}
