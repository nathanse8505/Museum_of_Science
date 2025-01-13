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


bool PRESS_BUTTON(int BUTTON_IO) {
   reset_watchdog();
  // Check if the button is pressed
  if (digitalRead(BUTTON_IO) == LOW && check == LOW) {
     //Serial.println("press :");
     check = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(BUTTON_IO) == HIGH && check == HIGH) {
    //Serial.println("unpress");
    check = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
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
