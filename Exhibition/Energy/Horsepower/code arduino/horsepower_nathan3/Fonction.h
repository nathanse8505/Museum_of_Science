#ifndef Fonction
#define Fonction

#include "constants.h"
#include <Wire.h>
#include <VL53L1X.h>
#include <avr/wdt.h>

// Global sensor object declared here for use in read_average_distance()
VL53L1X sensor;

/**
 * Reads the sensor multiple times and returns the average distance.
 * This helps smooth out noisy readings.
 *
 * @return The average distance measured by the sensor.
 */
float read_average_distance(){
  float avg = 0;
  for(int i = 0; i < ITERATION; i++)
  {
    wdt_reset();               // Reset the watchdog timer for each reading
    float Current_distance = sensor.read();
    avg += Current_distance;
  }
  avg = avg / ITERATION;       // Compute the average distance
  return avg;
}

void calcul_min_distance(){
  initialDistance = read_average_distance();// Read and set the initial distance (this will be our baseline)
  minDistance = initialDistance - THRESHOLD;// Calculate the minimum distance threshold to start the mesurment
  Serial.println(minDistance);
}

/**
 * Checks if the language button has been pressed and released.
 *
 * @return true if the button was fully pressed and released; false otherwise.
 */
bool PRESS_BUTTON_LANG(){
  // Detect button press (active LOW) if previously unpressed
  if (digitalRead(LANG_BUTTON_IO) == LOW && check_lang == LOW){
    check_lang = HIGH;          // Mark the button as pressed
    delay(BOUNCE_TIME);         // Debounce delay
  }

  // Detect button release (goes HIGH) if it was previously pressed
  if (digitalRead(LANG_BUTTON_IO) == HIGH && check_lang == HIGH){
    check_lang = LOW;           // Reset the state for the next press
    return true;                // Indicate a valid press-release cycle
  }
  return false;                 // Otherwise, no valid press-release detected
}

void read_ROI_XY_and_ROI_center(){
  uint8_t x = sensor.readReg(0x0080);
  uint8_t y = sensor.readReg(0x007F);
  Serial.println("read ROI coordinate: " + String(x) + " read center ROI: " + String(y));
}

void reset_sensor() {
  digitalWrite(XSHUT_PIN, LOW);  // Déactive the sensor
  delay(100);
}
#endif
