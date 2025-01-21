#include "const.h"
#include "FONCTION.h"

void setup() {
  
  Serial.begin(BAUDERATE); // Initialize serial communication with the defined baud rate
  
  // Configure pin modes
  pinMode(MICRO_SW, INPUT_PULLUP); // Set the micro switch pin as input with pull-up resistor
  pinMode(BUTTON, INPUT_PULLUP);   // Set the button pin as input with pull-up resistor
  pinMode(MOTOR, OUTPUT);          // Set the motor control pin as output
  pinMode(LED_BUTTON, OUTPUT);     // Set the button LED pin as output

  // Ensure the motor is turned off initially
  digitalWrite(MOTOR, LOW);

  // Initialize the Watchdog Timer with a timeout period of 2 seconds
  //wdt_enable(WDTO_4S);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
 
  Serial.println("init"); // Print initialization message to the serial monitor
}


void loop() {

  //resetWatchdog(); // Reset the Watchdog Timer to ensure the system does not reset

  // Check if the activation time has passed
  if ((millis() - time_start) > ACTIVATION_TIME) {
    
    digitalWrite(LED_BUTTON, HIGH); // Turn on the button LED
    //resetWatchdog(); // Reset the Watchdog Timer after this condition

    // If the button was successfully pressed and released
    if (PRESS_BUTTON()) {

      Serial.println("The motor is ON\n");
      TURN_ON_MOTOR();

      if(digitalRead(MICRO_SW)  == !NO_MICRO_SWITCH){

        Serial.print("the micro switch is faulty: ");
        IEC(); // Trigger Immediate Engine Cutoff
      }
      time_to_secure = millis();   // Record the start time for safety monitoring
      //resetWatchdog(); // Reset the Watchdog Timer after the delay
    }
      Serial.println("we are after the push");


    // Keep the motor running until the micro switch is activated
    while (digitalRead(MICRO_SW) ==  NO_MICRO_SWITCH && digitalRead(MOTOR) == HIGH) {
       Serial.println("we are in the while mode");

      digitalWrite(LED_BUTTON, LOW); // Turn off the button LED
      //resetWatchdog(); // Reset the Watchdog Timer during the loop

      // Check if the safety time has been exceeded
      if ((millis() - time_to_secure) > RESET_TIME_SECURE) {

        Serial.print("time after micro switch: ");
        Serial.println(millis() - time_to_secure);
        IEC(); // Trigger Immediate Engine Cutoff
      }

      // If the micro switch is activated
      if (digitalRead(MICRO_SW) == !NO_MICRO_SWITCH) {

        Serial.println("The motor is OFF");
        Serial.println("The micro switch is activated");
        Serial.print("time before micro switch: ");
        Serial.println(millis() - time_to_secure);
        Serial.println(" ");
        
        digitalWrite(MOTOR, LOW); // Turn off the motor
        //delay(MOTOR_DELAY);       // Wait for the defined motor delay
        button_check = LOW;       // Reset the button state
        time_start = millis();    // Reset the activation timer
        break;                    // Exit the loop
      }

    }
  }
}
