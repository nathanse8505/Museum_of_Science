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
   pinMode(LED_CONTROL_IO,OUTPUT);
  last_display_time = millis();  // reset display timer
  wdt_enable(WDTO_2S);
  delay(1000);
}
void loop() {
  
  wdt_reset();
  if (PRESS_BUTTON_LANG()) {  // pressed on language button
    lang = lang >= 2 ? 0 : (lang + 1);  // toggle language
    Serial.println(String(Pressure_Value) + " " + String(SensorValue) + " " + lang);
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
  if(Pressure_Value > VOLTAGE_LED_ON){
    digitalWrite(LED_CONTROL_IO,HIGH);
  }
  else{
    digitalWrite(LED_CONTROL_IO,LOW);
  }

   if (millis() - last_display_time >= DISPLAY_INTERVAL_TIME) {
    //Display_full_Numnber(Pressure_Value);
    Serial.println(String(Pressure_Value) + " " + String(SensorValue)+ " " + lang);
    last_display_time = millis(); //reset timer
   }

}
