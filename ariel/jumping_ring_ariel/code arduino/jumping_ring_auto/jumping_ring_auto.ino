#include "basic_routine.h"


void setup() {
  // Configure pin modes
  pinMode(BUTTON_IO, INPUT_PULLUP);  // Ignition button (active LOW)
  pinMode(MODE_IO, INPUT_PULLUP);  // Ignition button (active LOW)
  pinMode(LED_ACTIVATION, OUTPUT);   // Activation indicator LED
  pinMode(RELAY_IO, OUTPUT);         // relay control Contactor Coil

  Serial.begin(BAUDRATE);            // Start serial communication

  Serial.println("init");
  wdt_enable(WDTO_2S);  
}

void loop() {

  wdt_reset();

  // Check if ignition button is pressed
  if(digitalRead(MODE_IO) == MANUAL){
    buttonPressed = PRESS_BUTTON_IGNITION();
  }
  else{
    buttonPressed = true;
  }
  // After some time, move on to ignition phase
  if ((millis() - time_start) > ACTIVATION_TIME) {
    
    // If it's the first entry into this phase
    if(flag_new_session){

      digitalWrite(LED_ACTIVATION, HIGH);  // Turn on activation LED  
      flag_new_session = false;             // new Session ready
      Serial.println("we can push the button");
      Serial.println("led activated");
      Serial.println();
    }

    // On first button press (ignition)
    if (buttonPressed && flag_first_press == false) {
      digitalWrite(LED_ACTIVATION, LOW);   // Turn off activation LED
      digitalWrite(RELAY_IO, HIGH);        // the ring jump
      flag_first_press = true;            // Mark ignition as pressed
      time_new_session = millis();
      Serial.println("button has been pressed");
      Serial.println("the ring jumping");
    }
     /*else if (buttonPressed && flag_first_press) {
      Serial.println("button has been pressed");
      Serial.println("no action");
    }*/
    
    if (millis() - time_new_session > TIME_RELAY_ON && flag_first_press){
      digitalWrite(RELAY_IO, LOW);
      reset_session();
    }
    delay(1);
  }
  /*else if(buttonPressed) {
      Serial.println("button has been pressed");
      Serial.println("no action");
    }*/

}
