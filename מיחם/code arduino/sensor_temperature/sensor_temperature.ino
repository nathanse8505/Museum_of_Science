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

  Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {

  Vout1 = analogRead(ThermistorPin1);
  Vout2 = analogRead(ThermistorPin2);
  Vout3 = analogRead(ThermistorPin3);

  temperature_C_1 = read_temperature(Vout1);
  temperature_C_2 = read_temperature(Vout2);
  temperature_C_3 = read_temperature(Vout3);

  

  NUMBER_TO_DISPLAY(temperature_C_3 * UNIT_CALIBRATION);

  Serial.println(String(temperature_C_3) + " " + lang);   

  delay(100);
}