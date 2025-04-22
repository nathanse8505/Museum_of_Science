#ifndef FONCTION_AND_CONST
#define FONCTION_AND_CONST

#include <Wire.h>
#include <avr/wdt.h>

#define BUTTON_TRUCK  2 // Entrée analogique
#define BUTTON_CRANE  3 // Entrée analogique

#define MOTOR_TRUCK_R 4
#define MOTOR_TRUCK_L 5
#define MOTOR_CRANE_R 6
#define MOTOR_CRANE_L 7



/*
 * Arduino Nano pin mapping reference
 * (For reference only; not all pins are used here.)
 *                       ---------
 *                   TXD-|       |-Vin 
 *                   RXD-|       |-GND 
 *                   RST-|       |-RST
 *                   GND-|       |-+5V 
 *     BUTTON_TRUCK   D2-|       |-A7
 *     MOTOR_TRUCK_1  D3-|       |-A6
 *     BUTTON_CRANE   D4-|       |-A5 
 *     MOTOR_TRUCK_2  D5-|       |-A4 
 *   MOTOR CRANE_1    D6-|       |-A3
 *   MOTOR CRANE_2    D7-|       |-A2
 *                    D8-|       |-A1
 *                    D9-|       |-A0
 *                   D10-|       |-Ref
 *                   D11-|       |-3.3V
 *                   D12-|       |-D13  
 *                        --USB--
 */

const uint16_t BOUNCE_TIME = 50;
const uint16_t BAUDERATE = 9600;
const int ITERATION = 10;
bool check_truck = LOW;
bool check_crane = LOW;
unsigned long timer_truck;
unsigned long timer_crane;



bool PRESS_BUTTON(int IO , bool check) {
  // Check if the button is pressed
  if (digitalRead(IO) == LOW && check == LOW) {
     //Serial.println("press :");
     check = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(IO) == HIGH && check == HIGH) {
    //Serial.println("unpress");
    check = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}


void TURN_ON(int BUTTON,bool check,int MOTOR_R,int MOTOR_L,unsigned long timer){
  if(millis() - timer > 7000){

    if (PRESS_BUTTON(BUTTON,check)) {  // pressed on language button

        analogWrite(MOTOR_R,20);
        digitalWrite(MOTOR_L,LOW);
        delay(2000);

        digitalWrite(MOTOR_R,LOW);
        delay(3000);

        analogWrite(MOTOR_L,20);
        delay(2000);

        digitalWrite(MOTOR_L,LOW);
        timer = millis();

    }
  }

}



#endif