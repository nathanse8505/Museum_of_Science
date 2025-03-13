
//#include "const.h"
//#include "FONCTION.h"

#include <SPI.h>
#include <SD.h>

#define LED_BUTTON 2
#define MOTOR 3
#define BUTTON 5
#define MICRO_SW 6
#define CHIPSELECT 10  // Pin CS pour le module SD (modifiable selon le module)

/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *      LED_BUTTON  D2-|       |-A7  
 *           MOTOR  D3-|       |-A6  
 *                  D4-|       |-A5  
 *          BUTTON  D5-|       |-A4  
 *        MICRO_SW  D6-|       |-A3
 *                  D7-|       |-A2  
 *                  D8-|       |-A1
 *                  D9-|       |-A0
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */

bool NC_MICRO_SWITCH = LOW;//when the microswitch in normally close its in the gnd
bool NO_MICRO_SWITCH = HIGH;//when the microswitch in normally open its in the VCC

const int8_t time_bouncing = 50;
const int BAUDERATE = 9600;
const unsigned long ACTIVATION_TIME =5000;//ms after pushing the button the motor stop until activation time
const unsigned long RESET_TIME_SECURE = 5000;//if the micoswitch dosent work stop the motor and the arduino
const int16_t MOTOR_DELAY = 1000;//ms delay motor to unpress the microswitch
const int16_t DELAY_BEFORE_SLEEP = 1000;//ms delay before to enter in sleep mode



bool check = LOW;//variable for pushing the button
unsigned long time_start = 0;//start time to disable the button until activation time
unsigned long time_to_secure = 0;//start time to disable the arduino until stop_time_safety
bool flag_first_press = HIGH;

//#include <avr/sleep.h>
//#include <avr/wdt.h> // Library for the Watchdog Timer

/*
// Function to reset the Watchdog Timer
void resetWatchdog() {
  wdt_reset(); // Reset the Watchdog Timer to prevent system reset
}*/

void TURN_ON_MOTOR(){
  digitalWrite(MOTOR, HIGH); // Turn on the motor
  delay(MOTOR_DELAY);       // Wait for unpress the microswitch
}


// Function to handle button state and debounce logic
bool PRESS_BUTTON() {
  // Check if the button is pressed
  if (digitalRead(BUTTON) == LOW && check == LOW) {
    check = HIGH;         // Mark that the button is being pressed
    delay(time_bouncing); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(BUTTON) == HIGH && check == HIGH) {
    check = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}



// Function to immediately stop the motor and put the system into low-power mode
void IEC() {//Imediately Engine Cutoff
  digitalWrite(MOTOR, LOW);// Turn off the motor
  delay(MOTOR_DELAY);
  //wdt_disable();
  /*Serial.println("The Arduino is entering low power mode");
  delay(DELAY_BEFORE_SLEEP);
  sleep_mode();*/
}

void printMotorOffInfo(){
  Serial.println("The motor is OFF");
  Serial.println("The micro switch is activated");
  Serial.print("time before micro switch: ");
  Serial.println(millis() - time_to_secure);
  Serial.println(" ");
}

void BLINK_FLAG(unsigned int DELAY_BLINK){
  while(1){
    digitalWrite(LED_BUTTON, HIGH);
    delay(DELAY_BLINK);
    digitalWrite(LED_BUTTON, LOW);
    delay(DELAY_BLINK);

  }
}


File logFile;

void setup() {
    Serial.begin(BAUDERATE);
    while (!Serial);
    // Initialisation de la carte SD
    Serial.println("Initialisation de la carte SD...");
    if (!SD.begin(CHIPSELECT)) {
        Serial.println("Échec de l'initialisation de la carte SD !");
    } else {
        Serial.println("Carte SD initialisée avec succès.");
    }

    // Configuration des broches
    pinMode(MICRO_SW, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(MOTOR, OUTPUT);
    pinMode(LED_BUTTON, OUTPUT);

    // Assurez-vous que le moteur est éteint
    digitalWrite(MOTOR, LOW);
    
    Serial.println("Init terminé.");
    logEvent("Système initialisé");
}


void loop() {

    if (PRESS_BUTTON()){
      Serial.println("THE BUTTON IS PRESSED");
      logEvent("THE BUTTON IS PRESSED");
    }
    if ((millis() - time_start) > ACTIVATION_TIME) { 
        digitalWrite(LED_BUTTON, HIGH);
        
      if (PRESS_BUTTON()){
          Serial.println("THE BUTTON IS PRESSED");
          logEvent("THE BUTTON IS PRESSED");
      }

        if (PRESS_BUTTON() && flag_first_press) {
            Serial.println("Le moteur est ON");
            logEvent("THE BUTTON IS PRESSED AND THE MOTEUR IS ON");
            TURN_ON_MOTOR();
            flag_first_press = LOW;

            if (digitalRead(MICRO_SW) == !NO_MICRO_SWITCH) {
                Serial.println("Le micro switch est pressé en permanence, arrêt moteur !");
                logEvent("MICRO SWITCH pressé, arrêt moteur");
                IEC();
                BLINK_FLAG(500);
            }

            time_to_secure = millis();
        }

        while (digitalRead(MICRO_SW) == NO_MICRO_SWITCH && digitalRead(MOTOR) == HIGH && flag_first_press == LOW) {
            digitalWrite(LED_BUTTON, LOW);
            if (PRESS_BUTTON()){
                  Serial.println("THE BUTTON IS PRESSED");
                  logEvent("THE BUTTON IS PRESSED");
            }

            if ((millis() - time_to_secure) > RESET_TIME_SECURE) {
                Serial.println("Sécurité activée : arrêt moteur");
                logEvent("Sécurité RESET : arrêt moteur");
                IEC();
                BLINK_FLAG(2000);
            }

            if (digitalRead(MICRO_SW) == !NO_MICRO_SWITCH) {
                digitalWrite(MOTOR, LOW);
                Serial.println("MOTEUR OFF - Micro switch activé");
                //logEvent("MOTEUR OFF - Micro switch activé");
                time_start = millis();
                flag_first_press = HIGH;
            }
        }
    }
}


// Fonction pour enregistrer les événements sur la carte SD
void logEvent(const char* message) {  // ✅ Accepte "MOTEUR ON"
    logFile = SD.open("log.txt", FILE_WRITE);
    if (logFile) {
        logFile.print(millis());
        logFile.print(" ms ; ");
        logFile.println(message);
        logFile.close();
    } else {
        Serial.println("Erreur d'écriture sur la carte SD.");
    }
}



