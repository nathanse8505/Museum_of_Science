#include "FONCTION_AND_CONST.h"

void setup() {
  Serial.begin(BAUDERATE);      // Start serial communication at the specified baud rate
  pinMode(BUTTON_TRUCK, INPUT_PULLUP);
  pinMode(BUTTON_CRANE, INPUT_PULLUP);
  pinMode(MOTOR_TRUCK_1, OUTPUT);
  pinMode(MOTOR_TRUCK_2, OUTPUT);
  pinMode(MOTOR_CRANE_1, OUTPUT);
  pinMode(MOTOR_CRANE_2, OUTPUT);
  
  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
  timer_truck = millis();
  timer_crane = millis();
    
}

void loop() {

  TURN_ON(BUTTON_TRUCK,check_truck,MOTOR_TRUCK_1,MOTOR_TRUCK_2,timer_truck);
  TURN_ON(BUTTON_CRANE,check_crane,MOTOR_CRANE_1,MOTOR_CRANE_2,timer_crane);
  delay(50); // Petite pause pour éviter trop de rafraîchissements
  wdt_reset();
}


