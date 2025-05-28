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
  pinMode(VOLTMETER_PIN, INPUT);

  //Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {
  Vout = (float) analogRead(VOLTMETER_PIN);
  //Serial.println("Vout: " + String(Vout));

  //analog_to_voltmeter();calcul the voltage between 0 to Vref
  //voltmeter();calcul the voltage between 0 to VINMAX
  NUMBER_TO_DISPLAY(Vin);//display the numner to the 7seg
  //Serial.println("Vin: " + String(Vin/10));
  delay(100);
      
  }
  