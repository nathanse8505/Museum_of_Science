
#ifndef BASIC_ROUTINES_H
#define BASIC_ROUTINES_H
#include "consts.h"
#include <MovingAverage.h>  // see https://github.com/careyi3/MovingAverage

MovingAverage filter(MOVING_AVG_LENGTH);  // define/use the movinge average object

void fill_initial_rolling_average_value(){
  /*
   * fill rolling avg with sensor value
   */
   for (int i = 0; i <= MOVING_AVG_LENGTH; i++) {  // fill initial rolling average vector 
      sensor_value = analogRead(CURRENT_INPUT_IO);// read the analog in value (the current semso output):
//      Serial.println(sensor_value);
      avg_sensor_value  = filter.addSample(sensor_value);
  }
}


void reset_charge(){
  /*
   * reset charge to 0
   */
  charge = 0;
  //fill_initial_rolling_average_value() ;// clear moving average vector 
}


bool check_ignition_button(){
  /*
   * check if pressed on ignite button (with bounce check)
   */
  bool temp_return = false;  // asume button not pressed 
  int button_state = digitalRead(IGNITION_BUTTON_IO); // check button pressed 
   
  if (button_state == LOW) {  // button pressed 
    delay(BOUNCE_TIME);  // wait antibounce time to make sure 
    button_state = digitalRead(IGNITION_BUTTON_IO);  // check button again  
    
    if (button_state == LOW) { // button really pressed
      temp_return = true;
    }
  }
  return temp_return;
}


bool check_lang_button() {
  /*
   * check if pressed on language button
   */
  int button_state = digitalRead(LANG_BUTTON_IO); // check button pressed 
  
  if (button_state == LOW) {
    last_lang_button_state = button_state;
  }
  if (button_state == HIGH && last_lang_button_state == LOW) {
    last_lang_button_state = HIGH;
    return true;
  }
  return false;
}


void ignite() {
  /*
   * ignite after ignition button was pressed
   */

   digitalWrite(HYDROGEN_VALVE_IO, HIGH);  // close valve
   delay(DELAY_BETWEEN_VALVE_AND_SPARK);  // wait
   reset_charge();
   digitalWrite(SPARK_IO, HIGH);  // ignite - turn on spark
   delay(SPARK_TIME);
   digitalWrite(SPARK_IO, LOW);  // turn off spark
   delay(DELAY_AFTER_SPARK);  // wait
   digitalWrite(HYDROGEN_VALVE_IO, LOW);  // open valve
}


bool check_charge() {
  /*
   * check is charge is between limits
   */
   return charge >= MIN_CHARGE;
}

#endif
