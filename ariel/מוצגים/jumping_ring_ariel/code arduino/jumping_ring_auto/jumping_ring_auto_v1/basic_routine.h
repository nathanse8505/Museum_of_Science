#ifndef basic_routine
#define basic_routine

#include <Arduino.h>
#include <avr/wdt.h>
#define WDT_CHIP

/*
*=================Arduino Nano pinout==================
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *  ignition button D2-|       |-A7  
 *   LED_ACTIVATION D3-|       |-A6  
 *                  D4-|       |-A5 
 *            RELAY D5-|       |-A4 
 *                  D6-|       |-A3
 *                  D7-|       |-A2   
 *                  D8-|       |-A1   
 *                  D9-|       |-A0   
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */
#define BAUDRATE (115200)
////////////  I/O   ////////////
#define BUTTON_IO       2
#define LED_ACTIVATION  4
#define MODE_IO         7  // Manual/AUTO 
#define RELAY_IO        11  // relay active contactor 
#define WDI             12  // pin to reset watchdog 

void PULSE_WDT_RST(){
   digitalWrite(WDI, HIGH);        // the ring jump
   delay(1);
   digitalWrite(WDI, LOW);   // Turn off activation LED
   delay(1);
}


////////button mode ///////
const bool MANUAL = true;
// global vars
int32_t time_start = 0;
int32_t time_new_session = 0;
bool flag_new_session = true;

const uint32_t ACTIVATION_TIME = 5000;
const uint32_t TIME_RELAY_ON =2000;

/////////button ignition//////////
bool check_ignit = LOW;
bool buttonPressed;
bool flag_first_press = false;
const int16_t BOUNCE_TIME = 100;//ms 
//////////////////////////////////



bool PRESS_BUTTON_IGNITION() {
  // Check if the button is pressed
  if (digitalRead(BUTTON_IO) == LOW && check_ignit == LOW) {
     //Serial.println("press :");
     check_ignit = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(BUTTON_IO) == HIGH && check_ignit == HIGH) {
    //Serial.println("unpress");
    check_ignit = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}

void reset_session() {
  flag_first_press = false;
  flag_new_session = true;
  time_start = millis();
}



#endif