#include "FONCTION_and_CONST.h"

File logFile; // File object for logging events

void setup() {
    Serial.begin(BAUDERATE);
<<<<<<< HEAD
=======
    //while (!Serial);  // Wait for Serial Monitor to be ready
>>>>>>> 609987c6a1cb516f8a5f7e5a7f55e326c1839b6b

    // Initialize the SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(CHIPSELECT)) {
        Serial.println("SD card initialization failed!");
    } else {
        Serial.println("SD card successfully initialized.");
        logEvent("SD card successfully initialized.");
    }

    // Configure pins
    pinMode(MICRO_SW, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(MOTOR, OUTPUT);
    pinMode(LED_BUTTON, OUTPUT);

    // Ensure the motor is off at startup
    digitalWrite(MOTOR, LOW);

    Serial.println("Init.");
    logEvent("Init");
}

void loop() {
    bool buttonPressed = PRESS_BUTTON(); // Lire une seule fois l'état du bouton

    // Log button presses even when they don't activate the motor
    if ((millis() - time_start) < ACTIVATION_TIME) {
        flag_led = HIGH;
        if (buttonPressed) {
            Serial.println("Button pressed (no action triggered)");
            logEvent("Button pressed (no action triggered)");
            delay(20);
        }
        flag_led_on = HIGH;
        //Serial.println("Waiting Mode");
    }
    // Check if the activation time has elapsed
    else {
<<<<<<< HEAD
        if(flag_led){
           digitalWrite(LED_BUTTON, HIGH);
          flag_led = LOW;
        }
        //Serial.println("Waiting Mode");
 
=======
        if(flag_led_on){
          digitalWrite(LED_BUTTON, HIGH);
          flag_led_on = LOW;
        }
        
>>>>>>> 609987c6a1cb516f8a5f7e5a7f55e326c1839b6b

        // If the button is pressed and it's the first valid press
        if (buttonPressed && flag_first_press) {

            Serial.println("Motor activated!");
            logEvent("Button pressed - Motor activated");
            digitalWrite(LED_BUTTON, LOW);
            flag_first_press = LOW;
            TURN_ON_MOTOR();

            // Safety check: Stop motor if micro switch is constantly pressed
            if (digitalRead(MICRO_SW) != NO_MICRO_SWITCH) {
                Serial.println("Safety alert: Micro switch constantly pressed! Stopping motor.");
                logEvent("Safety alert: Micro switch constantly pressed - Motor stopped");
                IEC();
                BLINK_FLAG(500);
            }

            time_to_secure = millis();
        }
        //Serial.println("Activation Mode");

        // Keep checking while motor is running and micro switch is not triggered
<<<<<<< HEAD

        while (digitalRead(MOTOR) == HIGH && flag_first_press == LOW) {
            
            if (PRESS_BUTTON()){
=======
        while (digitalRead(MOTOR) == HIGH && flag_first_press == LOW) {
             
            if (PRESS_BUTTON()) {
>>>>>>> 609987c6a1cb516f8a5f7e5a7f55e326c1839b6b
                Serial.println("Button pressed (while motor running)");
                logEvent("Button pressed (while motor running)");
            }

            // Safety timeout: Stop motor if micro switch does not trigger within safety period
            if ((millis() - time_to_secure) > RESET_TIME_SECURE) {
                Serial.println("Safety timeout: Motor stopped.");
                logEvent("Safety timeout: Motor stopped");
                IEC();
                BLINK_FLAG(2000);
            }

            // Stop motor if the micro switch is triggered
            if (digitalRead(MICRO_SW) != NO_MICRO_SWITCH) {
                digitalWrite(MOTOR, LOW);
                Serial.println("Motor stopped - Micro switch triggered");
                logEvent("Motor stopped - Micro switch triggered");
                time_start = millis();
                flag_first_press = HIGH;
            }
        }
    }
}
