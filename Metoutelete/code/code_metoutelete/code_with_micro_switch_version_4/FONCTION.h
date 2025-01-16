#ifndef FONCTION
#define FONCTION

#include "const.h"
#include <LowPower.h>
#include <avr/wdt.h> // Library for the Watchdog Timer


// Function to reset the Watchdog Timer
void resetWatchdog() {
  wdt_reset(); // Reset the Watchdog Timer to prevent system reset
}

void TURN_ON_MOTOR(){
  digitalWrite(MOTOR, HIGH); // Turn on the motor
  delay(motor_delay);       // Wait for the defined motor delay
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
  
  digitalWrite(MOTOR, LOW);// Turn off the motor
  wdt_disable();
  Serial.println("The Arduino is entering low power mode");
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); // Enter low-power mode indefinitely until reset
}

#endif