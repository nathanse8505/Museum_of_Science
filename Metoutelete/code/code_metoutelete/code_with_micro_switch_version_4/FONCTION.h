#ifndef FONCTION
#define FONCTION

#include "const.h"
#include <avr/sleep.h>
#include <avr/wdt.h> // Library for the Watchdog Timer


// Function to reset the Watchdog Timer
void resetWatchdog() {
  wdt_reset(); // Reset the Watchdog Timer to prevent system reset
}

void TURN_ON_MOTOR(){
  digitalWrite(MOTOR, HIGH); // Turn on the motor
  delay(MOTOR_DELAY);       // Wait for the defined motor delay
  resetWatchdog(); // Reset the Watchdog Timer after the delay
}


// Function to handle button state and debounce logic
bool PRESS_BUTTON() {
  // Check if the button is pressed
  if (digitalRead(BUTTON) == LOW && check == LOW) {
    check = HIGH;         // Mark that the button is being pressed
    delay(time_bouncing); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(BUTTON) == HIGH && check == HIGH) {
    check = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}



// Function to immediately stop the motor and put the system into low-power mode
void IEC() {//Imediately Engine Cutoff
  Serial.println("MOTOR is OFF");
  digitalWrite(MOTOR, LOW);// Turn off the motor
  delay(MOTOR_DELAY);
  wdt_disable();
  Serial.println("The Arduino is entering low power mode");
  delay(DELAY_BEFORE_SLEEP);
  sleep_mode();
}

#endif