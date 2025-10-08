#ifndef basic_routine
#define basic_routine
#include "const.h"
#include <avr/wdt.h>
#include <INA226_WE.h>

INA226_WE ina226 = INA226_WE(I2C_ADDRESS);

//MAX6675 thermocouple(THERMOCS, THERMOD , THERMOCLK);
//#include <MovingAverage.h>  // see https://github.com/careyi3/MovingAverage
//MovingAverage filter(MOVING_AVG_LENGTH);  // define/use the movinge average object
bool init_current_sensor(){
  Wire.begin();
  if(!ina226.init()){
     Serial.println("No communication with the current sensor");
     return false;
  }
  ina226.setResistorRange(R_SHUNT_OHMS,I_RANGE_A); // choose resistor 5 mOhm and gain range up to 10 A
  return true;
}

void TURN_OFF_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, LOW);
}

void TURN_ON_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, HIGH);
   delay(DELAY_AFTER_ON);
}


int current_valid(){
  ina226.readAndClearFlags();
  float current = ina226.getCurrent_A();

  if (ina226.getI2cErrorCode() != 0) {
      return 0;
      delay(200);
    }
  read++;
  //Serial.println("read : " + String(read));
  //Serial.println("first_time_current : " + String(first_time_current));
  //average_current += current;
  //Serial.println("current : " + String(current));
  
  if(first_time_current && read == NUM_OF_READ){
    //average_current = average_current/NUM_OF_READ;
    first_time_current = false;
    Serial.println("\nCurrent : " + String(current) + "A");
    Serial.println("Voltage : " + String(ina226.getBusVoltage_V()) + "V\n");
  }
  
  if (current < CURRENT_SYSTEM){
    return 1;
  }
  return 2;
}


bool check_water_level(){
  if (analogRead(SENSOR_WATER_IO) <= NO_DETECTION){
    return false;
  }
  return true;
}

bool check_water_level_new(){
  //Serial.println(analogRead(SENSOR_WATER_IO));
  if (analogRead(SENSOR_WATER_IO) >= NO_DETECTION){
    return false;
  }
  return true;
}



// --- Fonction pour lire la température depuis un NTC ---
float readTemperature() {
  int analogValue = analogRead(NTC_PIN);
  
  // Conversion analogique -> résistance
  float resistance = R_SERIE / ((1023.0 / analogValue) - 1.0);
  
  // Formule Beta pour obtenir la température en Kelvin
  float temperatureK = 1.0 / ( (1.0 / T0) + (1.0 / BETA) * log(resistance / R0) );
  
  // Conversion en Celsius
  float temperatureC = temperatureK - 273.15;
  if (first_time_temp){
    first_time_temp = false;
    Serial.print("First Température : ");
    Serial.println(temperatureC);
  }
  return temperatureC;
}

// --- Fonction pour détecter baisse de température ---
bool detect_drop_temp() {
  float tempNow = readTemperature();
  // Première mesure : on l'accepte toujours comme "false"
  if (lastTemp == FIRST_SESSION_TEMP || first_temp) {
    lastTemp = tempNow;
    first_temp = false;
    return false;
  }
  if (first_time_delta){
    first_time_delta = false;
    Serial.print("Last Température : ");
    Serial.println(tempNow);
    Serial.print("\ndelta Température : ");
    Serial.println(tempNow - lastTemp);
  }
  // Si la température baisse par rapport à la précédente -> true
  if (tempNow <= lastTemp) {
    lastTemp = tempNow;
    return true;
  }
  
  // Sinon (stable ou hausse) -> true
  //lastTemp = tempNow;
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
  static bool first_time = true;
  digitalWrite(FAN_IO, HIGH);
  delay(DELAY_FAN_ON);
  if (first_time){
    first_time = false;
    Serial.println("THE FAN IS ON");
  }
}

void DEACTIVE_FAN() {
  digitalWrite(FAN_IO, LOW);
  delay(DELAY_FAN_OFF);

}

void reset_session() {
  TURN_OFF_CURRENT();
  read = 0;
  average_current = 0;
  first_time_current = true;
  first_time_temp = true;
  flag_first_press = false;
  flag_new_session = true;
  time_start = millis();
}


#endif