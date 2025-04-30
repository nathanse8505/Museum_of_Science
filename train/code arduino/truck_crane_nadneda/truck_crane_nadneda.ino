#include "FONCTION_AND_CONST.h"

void setup() {
  Serial.begin(BAUDERATE);      // Start serial communication at the specified baud rate
  pinMode(BUTTON_TRUCK, INPUT_PULLUP);
  pinMode(BUTTON_CRANE, INPUT_PULLUP);
  pinMode(BUTTON_NADNEDA, INPUT_PULLUP);

  
  //pinMode(MOTOR_TRUCK_L, OUTPUT);
  servo_truck.attach(SERVO_TRUCK); // pin de signal
  pinMode(COIL_NADNEDA, OUTPUT);
  servo_crane.attach(SERVO_CRANE);

  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
  Serial.println("init");
    
}

void loop() {
  
  now = millis();

  ////////////////////////////NADNEDA//////////////////////////
    if (!active_nadneda){
    if (PRESS_BUTTON(BUTTON_NADNEDA, check_nadneda)) {
       Serial.println("enter to the press button nadneda");
      active_nadneda = true;
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
  

  ////////////////////////////TRUCK///////////////////////////////
    if (!active_truck){
    if (PRESS_BUTTON(BUTTON_TRUCK, check_truck)) {
       Serial.println("enter to the press button truck");
      active_truck = true;
    }
  }

  if (active_truck) {
    //Serial.println("enter to the active motor");
    TRUCK();
    if (pos_servo_truck == 90) {
      active_truck = false;
      servo_truck.write(pos_servo_truck); // Stop position (optional)
     
    }
  }

  /////////////////////////////CRANE//////////////////////////////

   if (!active_crane){
    if (PRESS_BUTTON(BUTTON_CRANE, check_crane)) {
      Serial.println("enter to the press button");
      active_crane = true;
    }
  }

  if (active_crane) {
    //Serial.println("enter to the active motor");
    CRANE();
    //analogWrite(MOTOR_CRANE_ENA,128);
    if (crane_counter >=  CYCLE_CRANE) {
      Serial.println("enter to the STOP");
      active_crane = false;
      crane_counter = 0;
      servo_crane.write(pos_servo_crane);
      
      
     
    }
    
  }
 
  wdt_reset();
}


