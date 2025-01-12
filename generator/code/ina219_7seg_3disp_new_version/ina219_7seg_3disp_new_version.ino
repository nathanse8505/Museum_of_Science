#include "CONSTANTS.h"
#include "FONCTION.h"


void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  while (!Serial) {
    // will pause Zero, Leonardo, etc until serial console opens
    delay(1);
  }
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);

  Init_Output(LE_UNITS);
  digitalWrite(LE_UNITS, HIGH);// lock digit

  Init_Output(LE_DOZENS);
  digitalWrite(LE_DOZENS, HIGH);// lock digit

  Init_Output(LE_CENTURIES);
  digitalWrite(LE_CENTURIES, HIGH);// lock digit

  Serial.println("init");
  Serial.println("Measuring voltage and current with INA219 ...");
  wdt_enable(WDTO_2S);

}


void loop() {
  
  //serial_disp();
  wdt_reset();
  float average_power = avg_power();
  average_power = average_power * UNIT_CALIBRATION;//for unit will be to the middle

  Digits_from_Number(average_power);
  Display_Digit(Digit_1_To_Display);
  enable(RIGH_DIGIT);
  Display_Digit(Digit_2_To_Display);
  enable(MIDDLE_DIGIT);

  if(average_power < UNIT_POWER_MAX * UNIT_CALIBRATION){
    blank_Digit(LEFT_DIGIT);
  }
  else{
    Display_Digit(Digit_3_To_Display);
    enable(LEFT_DIGIT);
  }
    
}

