#include "Fonction.h"
#include "Const.h"

void setup() {
  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);
  Init_Output(LE_CENTURIES);
  Init_Output(LE_DOZENS);
  Init_Output(LE_UNITS);
  pinMode(LANG_BUTTON_IO, INPUT_PULLUP);

  Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {

  if (PRESS_BUTTON_LANG()) {            // pressed on language button
    lang = lang >= 2 ? 0 : (lang + 1);  // toggle language
  }
  Vout = analogRead(ThermistorPin);
  temperature_C = read_temperature(Vout);

  NUMBER_TO_DISPLAY(temperature_C * UNIT_CALIBRATION);

  Serial.println(String(temperature_C) + " " + String(Vout));
  delay(200);
}