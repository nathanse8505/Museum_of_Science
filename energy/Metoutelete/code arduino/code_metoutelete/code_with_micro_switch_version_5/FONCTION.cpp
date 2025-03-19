#include "FONCTION_and_CONST.h"

void TURN_ON_MOTOR() {
    digitalWrite(MOTOR, HIGH);
    delay(MOTOR_DELAY);
}

bool PRESS_BUTTON() {
    if (digitalRead(BUTTON) == LOW && check == LOW) {
        check = HIGH;
        delay(time_bouncing);
    }
    if (digitalRead(BUTTON) == HIGH && check == HIGH) {
        check = LOW;
        return HIGH;
    }
    return LOW;
}

void IEC() {
    digitalWrite(MOTOR, LOW);
    delay(MOTOR_DELAY);
}

void printMotorOffInfo() {
    Serial.println("The motor is OFF");
    Serial.println("The micro switch is activated");
    Serial.print("Time before micro switch: ");
    Serial.println(millis() - time_to_secure);
    Serial.println(" ");
}

void BLINK_FLAG(unsigned int DELAY_BLINK) {
    while(1) {
        digitalWrite(LED_BUTTON, HIGH);
        delay(DELAY_BLINK);
        digitalWrite(LED_BUTTON, LOW);
        delay(DELAY_BLINK);
    }
}

void logEvent(const char* message) {
    File logFile = SD.open("log.txt", FILE_WRITE);
    if (logFile) {
        logFile.print(millis());
        logFile.print(" ms ; ");
        logFile.println(message);
        logFile.close();
    } else {
        Serial.println("Erreur d'écriture sur la carte SD.");
    }
}
