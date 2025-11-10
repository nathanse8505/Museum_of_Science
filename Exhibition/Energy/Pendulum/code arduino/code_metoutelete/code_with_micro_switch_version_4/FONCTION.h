#ifndef FONCTION
#define FONCTION

#include "const.h"
//#include <avr/sleep.h>
//#include <avr/wdt.h> // Library for the Watchdog Timer

/*
// Function to reset the Watchdog Timer
void resetWatchdog() {
  wdt_reset(); // Reset the Watchdog Timer to prevent system reset
}*/

void TURN_ON_MOTOR(){
  digitalWrite(MOTOR, HIGH); // Turn on the motor
  delay(MOTOR_DELAY);       // Wait for unpress the microswitch
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
  delay(MOTOR_DELAY);
  //wdt_disable();
  /*Serial.println("The Arduino is entering low power mode");
  delay(DELAY_BEFORE_SLEEP);
  sleep_mode();*/
}

void printMotorOffInfo(){
  Serial.println("The motor is OFF");
  Serial.println("The micro switch is activated");
  Serial.print("time before micro switch: ");
  Serial.println(millis() - time_to_secure);
  Serial.println(" ");
}

void BLINK_FLAG(unsigned int DELAY_BLINK){
  while(1){
    digitalWrite(LED_BUTTON, HIGH);
    delay(DELAY_BLINK);
    digitalWrite(LED_BUTTON, LOW);
    delay(DELAY_BLINK);

  }
}

#endif