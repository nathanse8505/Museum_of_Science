#include "Fonction.h"
#include "Const.h"

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  pinMode(LED_START_END_BUTTON,OUTPUT);
  pinMode(START_END_BUTTON,INPUT_PULLUP);
  pinMode(COLOR_BUTTON,INPUT_PULLUP);
  pinMode(ZOOM_BUTTON,INPUT_PULLUP);
  pinMode(BRIGHTNESS_POT,INPUT);
  pinMode(CONTRAST_POT,INPUT);
  pinMode(SAVE_BUTTON,INPUT_PULLUP);
  pinMode(RESET_BUTTON,INPUT_PULLUP);
}

void loop() {
  }
  