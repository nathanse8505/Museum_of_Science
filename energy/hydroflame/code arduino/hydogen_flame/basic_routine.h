#ifndef basic_routine
#define basic_routine
#include "const.h"
#include <avr/wdt.h>
#include "MAX6675.h"

MAX6675 thermocouple(THERMOCS, THERMOD , THERMOCLK);
//#include <MovingAverage.h>  // see https://github.com/careyi3/MovingAverage
//MovingAverage filter(MOVING_AVG_LENGTH);  // define/use the movinge average object

void TURN_OFF_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, LOW);
}

void TURN_ON_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, HIGH);
   delay(DELAY_AFTER_ON);
}

void setZeroCurent(){   
    for (int i = 0; i < ITERATION; i++){
      offset_sensor = analogRead(CURRENT_INPUT_IO);
      if(offset_sensor <= 10 || offset_sensor >= 1000){
          Serial.println("error reading");
          i--;
          continue;
      }
      total_offset_sensor += offset_sensor;
    }

    ZeroCurrentSensor = (float) total_offset_sensor / ITERATION;
    Serial.print("the zero current offset in bit is:" + String(ZeroCurrentSensor));
}

float getcurrent(){
  current_sensor = analogRead(CURRENT_INPUT_IO);
  current_value = (current_sensor - ZeroCurrentSensor) * READ_TO_CURRENT;
  delay(MEASURE_INTERVAL_TIME);
  return current_value;
}

bool check_current(){
  
  if (getcurrent() > CURRENT_SYSTEM){
    return true;
  }
  return false;
}


bool check_water_level(){
  if (analogRead(SENSOR_WATER_IO) <= NO_DETECTION){
    return false;
  }
  return true;
}

float read_temperature(){
   if(millis() - timer_read_temp > DELAY_TEMP){
    timer_read_temp = millis();
    status_sensor = thermocouple.read();

    if (status_sensor == STATUS_OK) {
      tempC = thermocouple.getCelsius();
      Serial.println("Température : " + String(tempC) + " °C");
      return tempC;
    }
    Serial.print("Erreur capteur : code "  + String(status_sensor));
    return 0;
  }
  
}

bool check_fire(){
  // Lecture obligatoire pour mettre à jour la température
  if(read_temperature() > TEMP_TRESHOLD){
    Serial.println("true");
    return true;
  }
  Serial.println("false");
  return false;
}

bool r_check_fire(){
   if(millis() - timer_read_temp > DELAY_TEMP){
    timer_read_temp = millis();
    status_sensor = thermocouple.read();

    if (status_sensor == STATUS_OK) {
      tempC = thermocouple.getCelsius();
      Serial.println("Température : " + String(tempC) + " °C");
      
      if(tempC > TEMP_TRESHOLD){
        Serial.println("true");
        return true;
      }
    }
    Serial.println("Erreur capteur : code "  + String(status_sensor));
    return false;
  }
 
}

bool check_fire_test(){
  // Lecture obligatoire pour mettre à jour la température
  float temp1 = map(analogRead(A6),0,1024,0,130);
  if(temp1 > TEMP_TRESHOLD){
    Serial.println(temp1);
    return true;
  }
  Serial.println("false");
  return false;
}



void FILL_WATER(){
  digitalWrite(RELAY_VALVE_WATER_IO, HIGH);
  delay(DELAY_FILL_WATER);
  digitalWrite(RELAY_VALVE_WATER_IO, LOW);
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
  digitalWrite(FAN_IO, HIGH);
  delay(DELAY_FAN_ON);
}

void DEACTIVE_FAN() {
  digitalWrite(FAN_IO, LOW);
  delay(DELAY_FAN_OFF);

}

void reset_session() {
  TURN_OFF_CURRENT();
  flag_first_press = false;
  flag_new_session = true;
  time_start = millis();
}



#endif