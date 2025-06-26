#ifndef basic_routine
#define basic_routine
#include "const.h"
#include <avr/wdt.h>
//#include <MovingAverage.h>  // see https://github.com/careyi3/MovingAverage
//MovingAverage filter(MOVING_AVG_LENGTH);  // define/use the movinge average object

void TURN_OFF_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, LOW);
}

void TURN_ON_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, HIGH);
}

bool check_current(){
  current_sensor_value = analogRead(CURRENT_INPUT_IO);
  current_value = map(current_sensor_value,MIN_CURRENT_SENSOR_VALUE,MAX_CURRENT_SENSOR_VALUE,MIN_CURRENT_VALUE,MAX_CURRENT_VALUE);
  if (current_value > CURRENT_SYSTEM){
    return true;
  }
  return false;
}


bool check_water_level(){
  if (digitalRead(SENSOR_WATER_IO) == HIGH){
    return true;
  }
  return false;
}

bool check_fire(){
  if (digitalRead(SENSOR_FIRE_IO) == HIGH){
    return true;
  }
  return false;
}

void FILL_WATER(){
  digitalWrite(RELAY_TURN_ON_OFF_IO, HIGH);
  delay(DELAY_FILL_WATER);
  digitalWrite(RELAY_TURN_ON_OFF_IO, LOW);
  delay(DELAY_AFTER_FILL_WATER);
}

bool PRESS_BUTTON_IGNITION() {
   wdt_reset();//reset the watchdog
  // Check if the button is pressed
  if (digitalRead(IGNITION_BUTTON_IO) == LOW && check_ignit == LOW) {
     //Serial.println("press :");
     check_ignit = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(IGNITION_BUTTON_IO) == HIGH && check_ignit == HIGH) {
    //Serial.println("unpress");
    check_ignit = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}



void SPARK_ON(){
  wdt_reset();//reset the watchdog
  for(int i = 0; i < NUM_OF_SPARK; i++){//ignite - turn on spark 3 times
    digitalWrite(SPARK_IO, HIGH);  // ignite - turn on spark
    delay(SPARK_TIME);
    digitalWrite(SPARK_IO, LOW);  // turn off spark
    delay(SPARK_SPACE_TIME);
    wdt_reset();
  }
  delay(DELAY_AFTER_SPARK);  // wait 
  wdt_reset();//reset the watchdog
}

void ACTIVE_FAN() {
  digitalWrite(RELAY_FAN_IO, HIGH);
  delay(DELAY_FAN_ON);
}

void DEACTIVE_FAN() {
  digitalWrite(RELAY_FAN_IO, LOW);
  delay(DELAY_FAN_OFF);

}


#endif