#include "FONCTION_and_CONST.h"

File logFile; // File object for logging events

void setup() {
    Serial.begin(BAUDERATE);
    while (!Serial);  // Wait for Serial Monitor to be ready

    // Initialize the SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(CHIPSELECT)) {
        Serial.println("SD card initialization failed!");
    } else {
        Serial.println("SD card successfully initialized.");
    }

    // Configure pins
    pinMode(MICRO_SW, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(MOTOR, OUTPUT);
    pinMode(LED_BUTTON, OUTPUT);

    // Ensure the motor is off at startup
    digitalWrite(MOTOR, LOW);
    
    Serial.println("Initialization complete.");
    logEvent("System initialized");
}

void loop() {
    // Log button presses even when they don't activate the motor
    if (PRESS_BUTTON()) {
        Serial.println("Button pressed (no action triggered)");
        logEvent("Button pressed (no action triggered)");
    }

    // Check if the activation time has elapsed
    if ((millis() - time_start) > ACTIVATION_TIME) { 
        digitalWrite(LED_BUTTON, HIGH);
        
        if (PRESS_BUTTON()) {
            Serial.println("Button pressed (within activation window)");
            logEvent("Button pressed (within activation window)");
        }

        // If the button is pressed and it's the first valid press
        if (PRESS_BUTTON() && flag_first_press) {
            Serial.println("Motor activated!");
            logEvent("Button pressed - Motor activated");
            TURN_ON_MOTOR();
            flag_first_press = LOW;

            // Safety check: Stop motor if micro switch is constantly pressed
            if (digitalRead(MICRO_SW) == !NO_MICRO_SWITCH) {
                Serial.println("Safety alert: Micro switch constantly pressed! Stopping motor.");
                logEvent("Safety alert: Micro switch constantly pressed - Motor stopped");
                IEC();
                BLINK_FLAG(500);
            }

            time_to_secure = millis();
        }

        // Keep checking while motor is running and micro switch is not triggered
        while (digitalRead(MICRO_SW) == NO_MICRO_SWITCH && digitalRead(MOTOR) == HIGH && flag_first_press == LOW) {
            digitalWrite(LED_BUTTON, LOW);

            if (PRESS_BUTTON()) {
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
            if (digitalRead(MICRO_SW) == !NO_MICRO_SWITCH) {
                digitalWrite(MOTOR, LOW);
                Serial.println("Motor stopped - Micro switch triggered");
                logEvent("Motor stopped - Micro switch triggered");
                time_start = millis();
                flag_first_press = HIGH;
            }
        }
    }
}
