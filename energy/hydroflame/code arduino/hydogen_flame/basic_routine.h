#ifndef basic_routine
#define basic_routine
#include "const.h"
#include <avr/wdt.h>
#include <INA226_WE.h>

INA226_WE ina226 = INA226_WE(I2C_ADDRESS);

//MAX6675 thermocouple(THERMOCS, THERMOD , THERMOCLK);
//#include <MovingAverage.h>  // see https://github.com/careyi3/MovingAverage
//MovingAverage filter(MOVING_AVG_LENGTH);  // define/use the movinge average object

void TURN_OFF_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, LOW);
}

void TURN_ON_CURRENT(){
   digitalWrite(RELAY_TURN_ON_OFF_IO, HIGH);
   delay(DELAY_AFTER_ON);
}


bool current_valid(){
  ina226.readAndClearFlags();
  float current = ina226.getCurrent_A();
  Serial.println("\nCurrent : " + String(current) + "A");
  Serial.println("Voltage : " + String(ina226.getBusVoltage_V()) + "V\n");
  if (current < CURRENT_SYSTEM){
    return false;
  }
  return true;
}


bool check_water_level(){
  if (analogRead(SENSOR_WATER_IO) <= NO_DETECTION){
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
  Serial.print("Température : ");
  Serial.println(temperatureC);
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
  Serial.print("\ndelta Température : ");
  Serial.println(tempNow - lastTemp);

  // Si la température baisse par rapport à la précédente -> true
  if (tempNow < lastTemp) {
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