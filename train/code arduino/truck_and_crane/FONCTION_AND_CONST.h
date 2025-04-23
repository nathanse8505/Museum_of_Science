#ifndef FONCTION_AND_CONST
#define FONCTION_AND_CONST

#include <Wire.h>
#include <avr/wdt.h>
#include <Servo.h>

Servo servo_truck;


#define BUTTON_TRUCK   8 // Entrée analogique
#define BUTTON_CRANE   9 // Entrée analogique
#define BUTTON_NADNEDA 10 // Entrée analogique


#define COIL_NADNEDA  3
#define MOTOR_TRUCK   5
//#define MOTOR_TRUCK_R 4
//#define MOTOR_TRUCK_L 5
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
bool check_nadneda = LOW;
unsigned long now = 0;

// Variables pour le truck
unsigned long start_truck = 0;
bool active_truck = false;

int pos_servo = 90;
int servo_step = 1;
unsigned long last_servo_update = 0;
const unsigned long servo_interval = 20; // ms

int bounce_counter = 0;
bool bounce_phase = false;
bool descending_to_90 = false;

// Variables pour la nadneda
unsigned long start_nadneda = 0;
bool active_nadneda = false;

int pos_pwm = 0;
int pwm_step = 1;
unsigned long last_pwm_update = 0;
const unsigned long pwm_interval = 8; // ms
int nadneda_counter = 0;
const int CYCLE_NADNEDA = 10;



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


void NADNEDA(){
  // Gestion de la bobine
  if (now - last_pwm_update >= pwm_interval) {
    pos_pwm += pwm_step;
    last_pwm_update = now;
    analogWrite(COIL_NADNEDA, pos_pwm);

    if (pos_pwm >= 255 || pos_pwm <= 0) {
      pwm_step = -pwm_step; // Inverse la direction
    }
    if(pos_pwm == 0){
      nadneda_counter++;
    }
    
    
  }

}


void TRUCK(){
  if (now - last_servo_update >= servo_interval) {
    last_servo_update = now;
    servo_truck.write(pos_servo);

    if (bounce_phase) {
      pos_servo += servo_step;
      if (pos_servo >= 180) {
        servo_step = -1;
      } 
      else if (pos_servo <= 160) {
        bounce_counter--;
        if (bounce_counter > 0) {
          servo_step = 1;
        } 
        else {
          bounce_phase = false;
          descending_to_90 = true;
          servo_step = -1;
        }
      }
    }
    else if (descending_to_90) {
      pos_servo += servo_step;
      if (pos_servo <= 90) {
        descending_to_90 = false;
        servo_step = 1;
      }
    }
    else {
      pos_servo += servo_step;
      if (pos_servo >= 180) {
        bounce_phase = true;
        bounce_counter = 4;
        servo_step = -1;
      }
    }
  }

  
}
#endif