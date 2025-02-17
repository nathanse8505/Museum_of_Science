/* 
 *  based on 1.8 
 *  NOV 11 2024 add reading and display MPXV5010GP analog pressure sensor  see https://www.nxp.com/docs/en/data-sheet/MPX5010.pdf
 *  Test 7 segment 3X1.8"display designe for Energy Exhibit 2024
 *  see I:\Amir Design\PCB_Design\EASYEDA JLBPCB\projects\7_Segment_1.8X3
 *  based on G:\Bloomfield\פעילויות חינוך\מתקן אלקטרוליזה פיצוץ מימן חנוכה 201
 *   *
 */
#include "constants.h"
#include "FONCTION.h"


void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);
  Init_Output(LE_100);
  digitalWrite(LE_100, HIGH);// lock digit
  Init_Output(LE_10);
  digitalWrite(LE_10, HIGH);// lock digit
  Init_Output(LE_1);
  digitalWrite(LE_1, HIGH);// lock digit

  pinMode(LANG_BUTTON_IO,INPUT_PULLUP);
  // Test_7_segments() ;
}
void loop() {

  if (PRESS_BUTTON_LANG()) {  // pressed on language button
    lang = lang >= 2 ? 0 : (lang + 1);  // toggle language
  }
 
// read the value from the sensor:
  SensorValue = analogRead(Pressure_sensor_IO);
  // Serial.print("Original analog value: ");
  // Serial.println(SensorValue);
  Pressure_Value = map(SensorValue, Min_Pres_V, Max_Pres_V, Min_Pres_P, Max_Pres_P);
  Serial.println("Sensor value: " + String(SensorValue) + " Pressure_Value: " + String(Pressure_Value));
////////////////for 7seg/////////////
  Number_To_Display = Pressure_Value;// as only 3 digits
  Number_To_Display = map(Number_To_Display, 0, 1024, 0, 999);
  Number_To_Display = max(Number_To_Display, 0);
//////////////////////////////////////////////////////
  Display_full_Number(Number_To_Display);
  Serial.println(String(Pressure_Value) + " " + String(SensorValue) + " " + String(lang));
  delay (READ_SENSOR_RATE);
}
