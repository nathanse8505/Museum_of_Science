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

  Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {
  
  //voltmeter(VOLTMETER_PIN);
  voltmeter_map(VOLTMETER_PIN);
  Serial.println(Vin);
  NUMBER_TO_DISPLAY(Vin*UNIT_CALIBRATION);
  Serial.println("Vin: " + String(Vin));
  delay(100);
      
  }
  