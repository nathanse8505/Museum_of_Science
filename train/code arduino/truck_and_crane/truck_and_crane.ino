#include "FONCTION_AND_CONST.h"

void setup() {
  Serial.begin(BAUDERATE);      // Start serial communication at the specified baud rate
  pinMode(BUTTON_TRUCK, INPUT_PULLUP);
  pinMode(BUTTON_CRANE, INPUT_PULLUP);
  pinMode(MOTOR_TRUCK_R, OUTPUT);
  pinMode(MOTOR_TRUCK_L, OUTPUT);
  pinMode(MOTOR_CRANE_R, OUTPUT);
  pinMode(MOTOR_CRANE_L, OUTPUT);
  
  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
  timer_truck = millis();
  timer_crane = millis();
    
}

void loop() {

  TURN_ON(BUTTON_TRUCK,check_truck,MOTOR_TRUCK_R,MOTOR_TRUCK_L,timer_truck);
  TURN_ON(BUTTON_CRANE,check_crane,MOTOR_CRANE_R,MOTOR_CRANE_L,timer_crane);
  delay(50); // Petite pause pour éviter trop de rafraîchissements
  wdt_reset();
}


