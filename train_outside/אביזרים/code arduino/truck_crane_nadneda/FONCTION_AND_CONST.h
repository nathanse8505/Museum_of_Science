#ifndef FONCTION_AND_CONST
#define FONCTION_AND_CONST

#include <Wire.h>
#include <avr/wdt.h>
#include <Servo.h>

Servo servo_truck;
Servo servo_crane;

#define SERVO_CRANE   2
#define COIL_NADNEDA  3
#define SERVO_TRUCK   5

#define BUTTON_TRUCK   7 
#define BUTTON_CRANE   8 
#define BUTTON_NADNEDA 9 





/*
 * Arduino Nano pin mapping reference
 * (For reference only; not all pins are used here.)
 *                       ---------
 *                   TXD-|       |-Vin 
 *                   RXD-|       |-GND 
 *                   RST-|       |-RST
 *                   GND-|       |-+5V 
 *      SERVO_CRANE   D2-|       |-A7
 *      COIL_NADNEDA  D3-|       |-A6
 *                    D4-|       |-A5 
 *      SERVO_TRUCK   D5-|       |-A4 
 *                    D6-|       |-A3
 *    BUTTON_TRUCK    D7-|       |-A2
 *    BUTTON_CRANE    D8-|       |-A1
 *    BUTTON_NADNEDA  D9-|       |-A0
 *                   D10-|       |-Ref
 *                   D11-|       |-3.3V
 *                   D12-|       |-D13  
 *                        --USB--
 */

const uint16_t BOUNCE_TIME = 50;
const uint16_t BAUDERATE = 9600;
bool check_truck = LOW;
bool check_crane = LOW;
bool check_nadneda = LOW;
unsigned long now = 0;



// Variables for nadneda
bool active_nadneda = false;                // Flag indicating if nadneda is active
unsigned long last_pwm_update = 0;          // Timer to control PWM update frequency
int nadneda_counter = 0;                    // Counter for the number of complete PWM oscillation cycles
int pos_pwm = 0;                            // Current PWM value
int pwm_step = 1;                           // PWM increment or decrement step

// Constants for nadneda PWM control
const unsigned long PWM_INTERVAL = 7;       // Time in milliseconds between PWM updates
const int CYCLE_NADNEDA = 5;                // Number of desired oscillation cycles (not used directly in function)
const int MAX_PWM = 255;                    // Maximum PWM value
const int MIN_PWM = 0;                      // Minimum PWM value


// Variables for truck
bool active_truck = false;                  // Flag indicating if the truck is active
unsigned long last_truck_update = 0;        // Timer to control the update rate of the truck servo
int pos_servo_truck = 90;                   // Current angle position of the truck servo
int truck_step = 1;                         // Step increment or decrement for servo movement
int bounce_counter = 0;                     // Counter for the number of remaining bounce cycles
bool bounce_phase = false;                  // Flag indicating if the servo is in bounce mode (between 160° and 180°)
bool descending_to_min_angle = false;       // Flag indicating if the servo is descending toward 90° after bouncing

// Constants for truck servo control
const unsigned long TRUCK_TIME_INTERVAL = 20; // Time in milliseconds between servo updates
const int MAX_ANGLE_TRUCK = 180;              // Maximum angle the truck should reach
const int MIN_ANGLE_TRUCK = 90;               // Minimum angle the truck should return to
const int ANGLE_BOUNCE = 160;                 // Intermediate bounce angle during bouncing
const int NUM_OF_BOUNCE = 4;                  // Number of bounce cycles before descending




// Variables for crane
bool active_crane = false;                // Flag indicating if the crane is active
unsigned long last_crane_update = 0;      // Timer to control the update rate of the servo
int pos_servo_crane = 10;                 // Current angle position of the crane servo
int crane_step = 1;                       // Step increment or decrement for servo movement
int crane_counter = 0;                    // Counter to track how many full cycles the crane has completed
bool ascending_to_max_angle = true;       // Flag indicating if the crane is currently ascending
bool pausing_at_max_angle = false;        // Flag indicating if the crane is in pause state at max angle
unsigned long pause_start_time = 0;       // Timestamp when the pause at max angle started

// Constants for crane servo control
const int CYCLE_CRANE = 1;                         // Number of up-down cycles to perform (not used directly in CRANE function)
const unsigned long PAUSE_AT_MAX_ANGLE = 2000;     // Duration in milliseconds to pause at max angle (e.g. 2 seconds)
const int MAX_ANGLE_CRANE = 130;                   // Maximum angle the crane should reach
const int MIN_ANGLE_CRANE = 10;                    // Minimum angle the crane should reach
const unsigned long CRANE_SPEED_INTERVAL = 25;     // Time in milliseconds between servo updates (controls speed)





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


void NADNEDA() {
  // Handle the coil PWM
  if (now - last_pwm_update >= PWM_INTERVAL) {
    pos_pwm += pwm_step;              // Update PWM position
    last_pwm_update = now;
    analogWrite(COIL_NADNEDA, pos_pwm); // Send PWM signal to the coil

    if (pos_pwm >= MAX_PWM || pos_pwm <= MIN_PWM) {
      pwm_step = -pwm_step;           // Reverse direction at limits
    }

    if (pos_pwm == MIN_PWM) {
      nadneda_counter++;              // Count full oscillation cycles
    }
  }
}

void TRUCK() {
  // Handle truck servo movement
  if (now - last_truck_update >= TRUCK_TIME_INTERVAL) {
    last_truck_update = now;
    servo_truck.write(pos_servo_truck); // Update servo position

    if (bounce_phase) {
      // During bounce phase between ANGLE_BOUNCE° and MAX_ANGLE
      pos_servo_truck += truck_step;
      if (pos_servo_truck >= MAX_ANGLE_TRUCK) {
        truck_step = -1; // Start descending
      } 
      else if (pos_servo_truck <= ANGLE_BOUNCE) {
        bounce_counter--; // One bounce completed
        if (bounce_counter > 0) {
          truck_step = 1; // Go up again for another bounce
        } 
        else {
          bounce_phase = false;             // End of bouncing
          descending_to_min_angle = true;   // Start descending to 90°
          truck_step = -1;
        }
      }
    }
    else if (descending_to_min_angle) {
      // After bouncing, go down to min angle
      pos_servo_truck += truck_step;
      if (pos_servo_truck <= MIN_ANGLE_TRUCK) {
        descending_to_min_angle = false; // End of sequence
        truck_step = 1;
      }
    }
    else {
      // Regular upward motion from min to max angle
      pos_servo_truck += truck_step;
      if (pos_servo_truck >= MAX_ANGLE_TRUCK) {
        bounce_phase = true;            // Start bounce phase
        bounce_counter = NUM_OF_BOUNCE; // Set number of bounces
        truck_step = -1;
      }
    }
  }
}


void CRANE() {
  // Handle crane servo movement
  if (now - last_crane_update >= CRANE_SPEED_INTERVAL) {
    last_crane_update = now;

    if (pausing_at_max_angle) {
      // Waiting phase at MAX_ANGLE_CRANE
      if (now - pause_start_time >= PAUSE_AT_MAX_ANGLE) {
        // End of pause, start descending
        pausing_at_max_angle = false;
        ascending_to_max_angle = false;
        crane_step = -1;
      }
      return; // Do nothing else during pause
    }

    servo_crane.write(pos_servo_crane); // Update servo position

    if (ascending_to_max_angle) {
      // Ascend toward MAX_ANGLE_CRANE
      pos_servo_crane += crane_step;
      if (pos_servo_crane >= MAX_ANGLE_CRANE) {
        pos_servo_crane = MAX_ANGLE_CRANE; // Clamp to max angle
        servo_crane.write(pos_servo_crane); // Apply final position

        // Start pause at top
        pausing_at_max_angle = true;
        pause_start_time = now;
      }
    } 
    else {
      // Descend toward MIN_ANGLE_CRANE
      pos_servo_crane += crane_step;
      if (pos_servo_crane <= MIN_ANGLE_CRANE) {
        pos_servo_crane = MIN_ANGLE_CRANE; // Clamp to min angle
        servo_crane.write(pos_servo_crane); // Apply final position

        ascending_to_max_angle = true; // Prepare next cycle
        crane_step = 1;
        crane_counter++; // One full up-down cycle complete
      }
    }
  }
}




#endif