#include <Arduino.h>
#include <avr/wdt.h>

/*
*=================Arduino Nano pinout==================
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *  ignition button D2-|       |-A7  
 *                  D3-|       |-A6  
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
#define SMOKE_IO        11  // relay active contactor 


// global vars
int32_t time_start = 0;
int32_t time_new_session = 0;
const uint32_t ACTIVATION_TIME = 5000;
const uint16_t DELAY_LED = 2000;
const uint16_t DELAY_SMOKE = 100;
const uint16_t TIME_END_SESSION = 30000;
/////////button ignition//////////
bool check_ignit = LOW;
bool buttonPressed;
bool flag_first_press = false;
const int16_t BOUNCE_TIME = 100;//ms 
//////////////////////////////////



bool PRESS_BUTTON_IGNITION() {
   wdt_reset();//reset the watchdog
  // Check if the button is pressed
  if (digitalRead(BUTTON_IO) == LOW && check_ignit == LOW) {
     check_ignit = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }
  // Check if the button is released
  if (digitalRead(BUTTON_IO) == HIGH && check_ignit == HIGH) {
    check_ignit = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}

void reset_session() {
  flag_first_press = false;
  time_start = millis();
}

void SMOKE(int delay_mode,int iteration){
  for(int i =0;i<iteration;i++){
        // Activer → OUTPUT + LOW
        pinMode(SMOKE_IO, OUTPUT);
        digitalWrite(SMOKE_IO, LOW);
        delay(delay_mode);

        // Désactiver → INPUT (High-Z)
        pinMode(SMOKE_IO, INPUT);
        delay(500);
    }
}



void setup() {
  // Configure pin modes
  pinMode(BUTTON_IO, INPUT_PULLUP);  // Ignition button (active LOW)

  // Désactiver SMOKE_IO par défaut (High-Z)
  pinMode(SMOKE_IO, INPUT);

  Serial.begin(BAUDRATE);            // Start serial communication
  Serial.println("init");

  //wdt_enable(WDTO_8S);  
}

void loop() {

  //wdt_reset();

  // Check if ignition button is pressed
  buttonPressed = PRESS_BUTTON_IGNITION();

  // After some time, move on to ignition phase
  if ((millis() - time_start) > ACTIVATION_TIME) {

    // On first button press (ignition)
    if (buttonPressed && flag_first_press == false) {
      SMOKE(DELAY_LED,2);
      SMOKE(DELAY_SMOKE,1);

      flag_first_press = true;            // Mark ignition as pressed
      time_new_session = millis();
      Serial.println("button has been pressed");
      Serial.println("smoke!!");
    }


    if (millis() - time_new_session > TIME_END_SESSION && flag_first_press){
      Serial.println("enter");
      SMOKE(DELAY_LED,1);
      SMOKE(DELAY_SMOKE,2);
      reset_session();
    }
  }

  
  
   

  //ACTIVE_SMOKE(DELAY_LED);
  delay(1);
}
