#ifndef FONCTION_AND_CONST
#define FONCTION_AND_CONST

#include <Wire.h>
#include <avr/wdt.h>
#include <Servo.h>

Servo servo_truck;
Servo servo_crane;


#define BUTTON_TRUCK   8 // Entrée analogique
#define BUTTON_NADNEDA 7 // Entrée analogique
#define BUTTON_CRANE   6 // Entrée analogique


#define COIL_NADNEDA  3
#define MOTOR_TRUCK   5

///////servo/////////
//#define MOTOR_CRANE  11

////////pwm/////////
#define MOTOR_CRANE_L 9
#define MOTOR_CRANE_R 10
#define MOTOR_CRANE_ENA 11




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
int pos_servo_truck = 90;
int truck_step = 1;
unsigned long last_truck_update = 0;
const unsigned long truck_interval = 20; // ms
int bounce_counter = 0;
bool bounce_phase = false;
bool descending_to_90 = false;

// Variables pour la nadneda
bool active_nadneda = false;
int pos_pwm = 0;
int pwm_step = 1;
unsigned long last_pwm_update = 0;
const unsigned long pwm_interval = 8; // ms
int nadneda_counter = 0;
const int CYCLE_NADNEDA = 10;


// Variables pour le crane
unsigned long last_crane_update = 0;
const unsigned long crane_interval = 40; // ms
int crane_counter = 0;
const int CYCLE_CRANE = 1;
bool active_crane = false;

//////servo crane//////
int pos_servo_crane = 90;
int crane_step = 1;
const unsigned long crane_pause_at_180 = 2000; // Temps de pause en ms sur 180° (ex: 2 secondes)
unsigned long pause_start_time = 0; // Temps où on arrive à 180°
bool ascending_to_180 = true;
bool pausing_at_180 = false;
////////pwm crane///////
bool  crane_direction_right = true;
bool crane_phase_left_done = false;


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
  if (now - last_truck_update >= truck_interval) {
    last_truck_update = now;
    servo_truck.write(pos_servo_truck);

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
/*
void CRANE() {
  if (now - last_crane_update >= crane_interval) {
    last_crane_update = now;

    if (pausing_at_180) {
      // Phase d'attente à 180°
      if (now - pause_start_time >= crane_pause_at_180) {
        // Fin de la pause, commencer à descendre
        pausing_at_180 = false;
        ascending_to_180 = false;
        crane_step = -1;
      }
      return; // Pendant la pause, ne rien faire d'autre
    }

    servo_crane.write(pos_servo_crane); // Mise à jour de la position actuelle
    Serial.println(pos_servo_crane);

    if (ascending_to_180) {
      // Monter vers 180°
      pos_servo_crane += crane_step;
      if (pos_servo_crane >= 180) {
        pos_servo_crane = 180; // Fixer précisément à 180°
        servo_crane.write(pos_servo_crane);

        // Démarrer la pause
        pausing_at_180 = true;
        pause_start_time = now;
      }
    } 
    else {
      // Descendre vers 90°
      pos_servo_crane += crane_step;
      if (pos_servo_crane <= 90) {
        pos_servo_crane = 90; // Fixer précisément à 90°
        servo_crane.write(pos_servo_crane);

        ascending_to_180 = true; // Préparer la montée pour le prochain cycle
        crane_step = 1;
        crane_counter++; // Cycle complet terminé
        Serial.print("Crane Cycle Counter: ");
        Serial.println(crane_counter);
      }
    }
  }
}

*/

void CRANE(){
  if (now - last_crane_update >= crane_interval) {
  
    last_crane_update = now;
    if (crane_direction_right) {
          // Phase moteur droit
      Serial.println("Turning RIGHT");
      digitalWrite(MOTOR_CRANE_R, HIGH);
      digitalWrite(MOTOR_CRANE_L, LOW);

      crane_direction_right = false; // Prochaine fois aller à gauche
    } 
    else {
      if (!crane_phase_left_done) {
          // Phase moteur gauche
        Serial.println("Turning LEFT");
        digitalWrite(MOTOR_CRANE_R, LOW);
        digitalWrite(MOTOR_CRANE_L, HIGH);
        crane_phase_left_done = true; // La phase gauche vient de commencer
      }
      else {
        crane_counter++;
        Serial.print("Crane Counter: ");
        Serial.println(crane_counter);
        // Remettre tout pour recommencer un cycle
        crane_direction_right = true;
        crane_phase_left_done = false;
        }
    }
  }
}

void setPwmFrequencyTimer2(int prescaler_setting) {
    TCCR2B = (TCCR2B & 0b11111000) | prescaler_setting;
}

#endif