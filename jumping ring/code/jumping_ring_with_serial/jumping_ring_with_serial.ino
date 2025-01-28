/* 
 *  High voltage tester for jumping ring exhibit , 1"7 segnets (CBD27 segemtn based on 1.8" code 
 *  Dec 26 2024  
 *   see מצמד אופטי לטבעת קופצת.docx
 *   I:\Bloomfield\Exhibits\רעיונות לתערוכות חדשות\אנרגיה\פיתוח מוצגים מינואר 2024\טבעת קופצת\מצמד אופטי
 *   *
 */
#include "Const_N_Global.h"
#include "Fonction.h"

void setup(){
  Serial.begin(SERIAL_BAUD_RATE);
  delay (100);// wait to make sure serial begin
  /*Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  Serial.println("START");*/


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
  last_display_time = millis();  // reset display timer
}
void loop() {
  
  if (PRESS_BUTTON_LANG()) {  // pressed on language button
    lang = lang >= 2 ? 0 : (lang + 1);  // toggle language
  }
  
// read the value from the sensor:
  SensorValue = analogRead(Pressure_sensor_IO);  
  if(SensorValue >= map_table[2][0]){
  Pressure_Value = map(SensorValue, map_table[2][0], map_table[2][1], map_table[2][2], map_table[2][3]);
 } 
  else if(SensorValue >= map_table[1][0]){
  Pressure_Value = map(SensorValue, map_table[1][0], map_table[1][1], map_table[1][2], map_table[1][3]);
 }  
  else if(SensorValue >= map_table[0][0]){
  Pressure_Value = map(SensorValue, map_table[0][0], map_table[0][1], map_table[0][2], map_table[0][3]);
 }

 if(Pressure_Value == map_table[2][3]){
  reset_bar = true;
 }

 //Pressure_Value = map(SensorValue, Min_Read_V, Max_Read_V, Min_Show_V, Max_Show_V);
 // Pressure_Value = B_CONST*log(A_CONST*Pressure_Value);
  Number_To_Display = Pressure_Value;
//Number_To_Display = Pressure_Value/10;// as only 3 digits
//  Number_To_Display = analogRead(POT_2);  
//  Number_To_Display = map(Number_To_Display, 0, 1024, 0, 999); 

  //Display_full_Numnber(Number_To_Display);
  //delay (READ_SENSOR_RATE); 

   if (millis() - last_display_time >= DISPLAY_INTERVAL_TIME) {
    Display_full_Numnber(Number_To_Display);
    
    charge = CAPACITY * Pressure_Value;
    Serial.println(String(charge) + " " + String(Pressure_Value) + " " + reset_bar + " " + lang);
    last_display_time = millis(); //reset timer

    reset_bar = false;  // reset ignite flag (after sending it 1 time if there has been an ignition)
  }

}
