#define BUTTON 5
#define CHIPSELECT 10 
#define LED_SD_CARD 4

#include <SPI.h>
#include <SD.h>
File logFile;
bool check = LOW;
void setup() {
    Serial.begin(115200);
    //while (!Serial);  // Wait for Serial Monitor to be ready

    // Initialize the SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(10)) {
        Serial.println("SD card initialization failed!");
        digitalWrite(LED_SD_CARD, HIGH);
    } else {
        Serial.println("SD card successfully initialized.");
        logEvent("SD card successfully initialized.");
        digitalWrite(LED_SD_CARD, LOW);
    }
    // Configure pins
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(LED_SD_CARD, OUTPUT);


    Serial.println("Init.");
    logEvent("Init");
}
bool PRESS_BUTTON() {
    if (digitalRead(BUTTON) == LOW && check == LOW) {
        check = HIGH;
        delay(50);
    }
    if (digitalRead(BUTTON) == HIGH && check == HIGH) {
        check = LOW;
        return HIGH;
    }
    return LOW;
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
        digitalWrite(LED_SD_CARD, HIGH);
    }
}

void loop() {

  if(PRESS_BUTTON()){
    Serial.println("Button pressed (no action triggered)");
    logEvent("Button pressed (no action triggered)");

  }
  // put your main code here, to run repeatedly:

}
