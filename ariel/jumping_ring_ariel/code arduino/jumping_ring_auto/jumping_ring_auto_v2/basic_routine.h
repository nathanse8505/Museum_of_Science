#ifndef basic_routine
#define basic_routine

#include <Arduino.h>

/*
*=================Arduino Nano pinout==================
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *                  D2-|       |-A7  
 *                  D3-|       |-A6  
 *                  D4-|       |-A5 
 *              SSR D5-|       |-A4 
 *                  D6-|       |-A3
 *                  D7-|       |-A2  MODE_IO  
 *                  D8-|       |-A1   
 *                  D9-|       |-A0  BUTTON_IO 
 * LED_ACTIVATION  D10-|       |-Ref
 *         SSR_IO  D11-|       |-3.3V   
 *            WDI  D12-|       |-D13
 *                      --USB--        
 */
#define BAUDRATE (115200)
////////////  I/O   ////////////
#define BUTTON_IO       A0
#define LED_ACTIVATION  10
#define MODE_IO         A2  // Manual/AUTO system 
#define SSR_IO          11  // relay active contactor 
#define WDI             12  // pin to reset watchdog 

////////button mode ///////
const bool MANUAL = true;
// global vars
int32_t time_start = 0;
int32_t time_new_session = 0;
bool flag_new_session = true;

const uint32_t ACTIVATION_TIME = 15000;
const uint32_t TIME_RELAY_ON =2000;

/////////button ignition//////////
bool check_ignit = LOW;
bool buttonPressed;
bool flag_first_press = false;
const int16_t BOUNCE_TIME = 100;//ms 
//////////////////////////////////

void PULSE_WDT_RST(){
   digitalWrite(WDI, HIGH);        // the ring jump
   delay(1);
   digitalWrite(WDI, LOW);   // Turn off activation LED
   delay(1);
}


bool PRESS_BUTTON_IGNITION() {
  PULSE_WDT_RST();//reset the watchdog
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