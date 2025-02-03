#ifndef Fonction
#define Fonction

#include "constants.h"
#include <Wire.h>
#include <VL53L1X.h>

// Declare sensor object
VL53L1X sensor;

float read_average_distance(){
  
  float avg = 0;
  for(int i = 0 ; i < ITERATION;i++){
    float Current_distance = sensor.read() / 1000.0;
    avg+=Current_distance;
}
avg = avg / ITERATION;
return avg;
}

bool PRESS_BUTTON_SPACE() {
  // Check if the button is pressed
  if (digitalRead(SPACE_BUTTON_IO) == LOW && check_space == LOW) {
     //Serial.println("press :");
     check_space = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(SPACE_BUTTON_IO) == HIGH && check_space == HIGH) {
    //Serial.println("unpress");
    check_space = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}

#endif