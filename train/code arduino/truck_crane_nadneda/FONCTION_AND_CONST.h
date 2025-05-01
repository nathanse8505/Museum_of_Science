#ifndef FONCTION_AND_CONST
#define FONCTION_AND_CONST

#include <Wire.h>
#include <avr/wdt.h>
#include <Servo.h>

Servo servo_truck;
Servo servo_crane;


#define BUTTON_TRUCK   7 // Entrée analogique
#define BUTTON_NADNEDA 9 // Entrée analogique
#define BUTTON_CRANE   8 // Entrée analogique


#define COIL_NADNEDA  3
#define SERVO_TRUCK   5
#define SERVO_CRANE  2




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
bool active_truck = false;
unsigned long last_truck_update = 0;
const unsigned long TRUCK_TIME_INTERVAL = 20; // ms
int pos_servo_truck = 90;
int truck_step = 1;
int bounce_counter = 0;
bool bounce_phase = false;
bool descending_to_90 = false;

// Variables pour la nadneda
bool active_nadneda = false;
unsigned long last_pwm_update = 0;
const unsigned long PWM_INTERVAL = 7; // ms
const int CYCLE_NADNEDA = 5;
int nadneda_counter = 0;
int pos_pwm = 0;
int pwm_step = 1;




// Variables pour le crane
bool active_crane = false;
unsigned long last_crane_update = 0;
const unsigned long CRANE_SPEED_INTERVAL = 30; // ms
const int CYCLE_CRANE = 1;
int crane_counter = 0;

//////servo crane//////
int pos_servo_crane = 10;
int crane_step = 1;
bool ascending_to_max_angle = true;
bool pausing_at_max_angle = false;
unsigned long pause_start_time = 0; // Temps où on arrive à 180°
const unsigned long PAUSE_AT_MAX_ANGLE = 2000; // Temps de pause en ms sur max angle (ex: 2 secondes)
const int MAX_ANGLE = 130;
const int MIN_ANGLE = 10;




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
  if (now - last_pwm_update >= PWM_INTERVAL) {
    pos_pwm += pwm_step;
    last_pwm_update = now;
    analogWrite(COIL_NADNEDA, pos_pwm);

    if (pos_pwm >= 200 || pos_pwm <= 0) {
      pwm_step = -pwm_step; // Inverse la direction
    }
    if(pos_pwm == 0){
      nadneda_counter++;
    }
    
    
  }

}


void TRUCK(){
  if (now - last_truck_update >= TRUCK_TIME_INTERVAL) {
    last_truck_update = now;
    servo_truck.write(pos_servo_truck);
    Serial.println(pos_servo_truck);

    if (bounce_phase) {
      pos_servo_truck += truck_step;
      if (pos_servo_truck >= 180) {
        truck_step = -1;
      } 
      else if (pos_servo_truck <= 160) {
        bounce_counter--;
        if (bounce_counter > 0) {
          truck_step = 1;
        } 
        else {
          bounce_phase = false;
          descending_to_90 = true;
          truck_step = -1;
        }
      }
    }
    else if (descending_to_90) {
      pos_servo_truck += truck_step;
      if (pos_servo_truck <= 90) {
        descending_to_90 = false;
        truck_step = 1;
      }
    }
    else {
      pos_servo_truck += truck_step;
      if (pos_servo_truck >= 180) {
        bounce_phase = true;
        bounce_counter = 4;
        truck_step = -1;
      }
    }
  }

  
}

void CRANE() {
  if (now - last_crane_update >= CRANE_SPEED_INTERVAL) {
    last_crane_update = now;

    if (pausing_at_max_angle) {
      // Phase d'attente à 180°
      if (now - pause_start_time >= PAUSE_AT_MAX_ANGLE) {
        // Fin de la pause, commencer à descendre
        pausing_at_max_angle = false;
        ascending_to_max_angle = false;
        crane_step = -1;
      }
      return; // Pendant la pause, ne rien faire d'autre
    }

    servo_crane.write(pos_servo_crane); // Mise à jour de la position actuelle
    Serial.println(pos_servo_crane);

    if (ascending_to_max_angle) {
      // Monter vers 180°
      pos_servo_crane += crane_step;
      if (pos_servo_crane >= MAX_ANGLE) {
        pos_servo_crane = MAX_ANGLE; // Fixer précisément à 180°
        servo_crane.write(pos_servo_crane);

        // Démarrer la pause
        pausing_at_max_angle = true;
        pause_start_time = now;
      }
    } 
    else {
      // Descendre vers 90°
      pos_servo_crane += crane_step;
      if (pos_servo_crane <= MIN_ANGLE) {
        pos_servo_crane = MIN_ANGLE; // Fixer précisément à 90°
        servo_crane.write(pos_servo_crane);

        ascending_to_max_angle = true; // Préparer la montée pour le prochain cycle
        crane_step = 1;
        crane_counter++; // Cycle complet terminé
        Serial.print("Crane Cycle Counter: ");
        Serial.println(crane_counter);
      }
    }
  }
}



#endif