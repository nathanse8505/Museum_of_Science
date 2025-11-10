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
  timer = millis();
}

void loop() {

  Vout = analogRead(ThermistorPin);
  temperature_C = read_temperature(Vout);
  
  float avg_temp = rolling_average( temperature_C);

  
  if(millis() - timer > DATA_SER){
      NUMBER_TO_DISPLAY( avg_temp);
      Serial.println(String(avg_temp) + " " + String(Vout));
      timer = millis();
  }
  
}