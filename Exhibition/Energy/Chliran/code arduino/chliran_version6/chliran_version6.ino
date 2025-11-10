#include "FONCTION_and_CONST.h"

void setup() {
  Serial.begin(Bauderate);
   while (!Serial);  // Wait for Serial Monitor to be ready

  // Initialize output pins for shift register
  pinMode(Data_Serial, OUTPUT);
  pinMode(ClockPin, OUTPUT);
  pinMode(LatchPin, OUTPUT);
  pinMode(Output_Enable, OUTPUT);

  // Initialize input pins for switches with internal pull-up resistors
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);

  // Initialize shift register and switch states
  init_shift_register();
  
  wdt_enable(WDTO_2S);
  Serial.println("init");
}

void loop() {

  wdt_reset();
  // Check if current switch is pressed and is active in the sequence
  if(digitalRead(SWITCH_PINS[index_switch]) == LOW){
    // Shift LED to next position 
    if(index_switch < NUMBER_OF_SWITCH){
      out_data = LED_ON_UV(out_data,index_switch);
      Serial.println("Led SW" + String(index_switch + 1) + " ;has activate");
    }
    timer_on = millis();    
    // Debug information
    //Serial.println("out data: " + String(out_data));
    //Serial.println("index_switch: " + String(index_switch));
  }
  
  if(millis() - timer_on > TIME_LED_ON && digitalRead(SWITCH_PINS[index_switch])){
    out_data = LED_ON_SW(out_data,index_switch);
  }

  index_switch=(index_switch + 1) % NUMBER_OF_SWITCH;

  if(out_data == 65536 -1){
    RESET_ALL(out_data);
  }

}