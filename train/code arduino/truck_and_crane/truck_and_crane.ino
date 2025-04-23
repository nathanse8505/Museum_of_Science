#include "FONCTION_AND_CONST.h"

void setup() {
  Serial.begin(BAUDERATE);      // Start serial communication at the specified baud rate
  pinMode(BUTTON_TRUCK, INPUT_PULLUP);
  pinMode(BUTTON_CRANE, INPUT_PULLUP);
  pinMode(BUTTON_NADNEDA, INPUT_PULLUP);

  
  //pinMode(MOTOR_TRUCK_L, OUTPUT);
  servo_truck.attach(MOTOR_TRUCK); // pin de signal
  pinMode(COIL_NADNEDA, OUTPUT);

  //pinMode(MOTOR_TRUCK_R, OUTPUT);
  pinMode(MOTOR_CRANE_R, OUTPUT);
  pinMode(MOTOR_CRANE_L, OUTPUT);
  
  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
  Serial.println("init");
    
}

void loop() {
  
  now = millis();
  
  //if (!active_nadneda && (now - timer_nadneda > 7000)) {
    if (!active_nadneda){
    if (PRESS_BUTTON(BUTTON_NADNEDA, check_nadneda)) {
       Serial.println("enter to the press button nadneda");
      active_nadneda = true;
      start_nadneda = now;
    }
  }

  if (active_nadneda) {
    NADNEDA();
    //Serial.println("enter to the press button nadneda");
    if (nadneda_counter >=  CYCLE_NADNEDA) {
      active_nadneda = false;
      analogWrite(COIL_NADNEDA, 0);
      nadneda_counter = 0;
    }
  }
  

  //if (!active_truck && (now - timer_truck > 7000)) {
    if (!active_truck){
    if (PRESS_BUTTON(BUTTON_TRUCK, check_truck)) {
       Serial.println("enter to the press button");
      active_truck = true;
      start_truck = now;
    }
  }

  if (active_truck) {
    //Serial.println("enter to the active motor");
    TRUCK();
    if (pos_servo == 90) {
      active_truck = false;
      servo_truck.write(pos_servo); // Stop position (optional)
     
    }
  }
 
  wdt_reset();
}


