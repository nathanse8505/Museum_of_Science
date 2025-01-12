#ifndef BASIC_ROUTINES_H
#define BASIC_ROUTINES_H
#include "consts.h"
#include <MovingAverage.h>  // see https://github.com/careyi3/MovingAverage
#include <avr/wdt.h>

MovingAverage filter(MOVING_AVG_LENGTH);  // define/use the movinge average object

void reset_watchdog(){
  wdt_reset();//reset the watchdog
}

void fill_initial_rolling_average_value(){
  /*
   * fill rolling avg with sensor value
   */
   for (int i = 0; i <= MOVING_AVG_LENGTH; i++) {  // fill initial rolling average vector 
      sensor_value = analogRead(CURRENT_INPUT_IO);// read the analog in value (the current sensor output):
//      Serial.println(sensor_value);
      avg_sensor_value  = filter.addSample(sensor_value);
      reset_watchdog();
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


void ignite(){
  /*
   * ignite after ignition button was pressed
   */

digitalWrite(HYDROGEN_VALVE_IO, HIGH);  // close valve
delay(DELAY_BETWEEN_VALVE_AND_SPARK);  // wait
reset_charge();
reset_watchdog();
for(int i = 0; i < NUM_OF_SPARK; i++){//ignite - turn on spark 3 times
  digitalWrite(SPARK_IO, HIGH);  // ignite - turn on spark
  delay(SPARK_TIME);
  digitalWrite(SPARK_IO, LOW);  // turn off spark
  delay(SPARK_SPACE_TIME);
  reset_watchdog();
}
delay(DELAY_AFTER_SPARK);  // wait
digitalWrite(HYDROGEN_VALVE_IO, LOW);  // open valve
reset_watchdog();
}


bool check_charge() {
  /*
   * check is charge is between limits
   */
   return charge >= MIN_CHARGE;
}

void sound(){
  digitalWrite(SOUND,LOW);// active the sound with short pulse 
  delay(DELAY_SWITCH_SOUND );
  digitalWrite(SOUND,HIGH);
  time_sound = millis();
  while(millis() - time_sound <= DELAY_SOUND){ //Delay to listen the sound and reset the watchdog
  reset_watchdog();
  }
}

void init_sount(){
  digitalWrite(SOUND,HIGH);
  delay(100);
}

void IGNITE_WITH_SOUND(){
  sound();
  ignite();
}

#endif
