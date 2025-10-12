#include "FONCTION_and_CONST.h"

void setup() {
  Serial.begin(BAUDERATE);

  // Initialize output pins for shift register
  pinMode(Data_Serial, OUTPUT);
  pinMode(ClockPin, OUTPUT);
  pinMode(LatchPin, OUTPUT);
  pinMode(Output_Enable, OUTPUT);

  pinMode(LED_SD_CARD, OUTPUT);

  // Initialize input pins for switches with internal pull-up resistors
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);

  while (!Serial);  // Wait for Serial Monitor to be ready

  // Initialize the SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(CHIPSELECT)) {
      Serial.println("SD card initialization failed!");
      //digitalWrite(LED_SD_CARD, HIGH);
  } else {
      Serial.println("SD card successfully initialized.");
      //digitalWrite(LED_SD_CARD, LOW);
  }
  // Initialize shift register and switch states
  init_shift_register();
  init_switch_state();
  
  wdt_enable(WDTO_2S);
  Serial.println("init");
  logEvent("init");
  time_log = millis();
}

void loop() {

  wdt_reset();
  
  if(millis() - time_log > SET_DAY){
    logEvent("5 hours have passed");
    time_log = millis();
  } 

  // Check if current switch is pressed and is active in the sequence
  if(digitalRead(SWITCH_PINS[index_switch]) == LOW){

     /////////////////////LOG PRESS BUTTON//////////////////////
    if (checkStates[index_switch] == LOW) {
        checkStates[index_switch] = HIGH;
        logMessage = "SW" + String(index_switch + 1) + " ;has been pressed";
        logEvent(logMessage.c_str());
        Serial.println(logMessage.c_str());
    }
    ///////////////////////////////////////////////////////////
    
    // Shift LED to next position 
    if(index_switch < NUMBER_OF_SWITCH){
      out_data = LED_ON_UV(out_data,index_switch);
      //Serial.println("Led SW" + String(index_switch + 1) + " ;has activate");
    }
    //timer_on =  millis();
    timer_on_SW[index_switch] = millis();
        
    // Debug information
    //Serial.println("out data: " + String(out_data));
    //Serial.println("index_switch: " + String(index_switch));
  }else{
    ////////////////////LOG RELEASE BUTTON/////////////////////
    if (checkStates[index_switch] == HIGH) {
        checkStates[index_switch] = LOW;
        logMessage = "SW" + String(index_switch + 1) + " ;has been released";
        logEvent(logMessage.c_str());
        Serial.println(logMessage.c_str());     
    }
  ///////////////////////////////////////////////////////////
    if(millis() -timer_on_SW[index_switch] > TIME_LED_ON){
      out_data = LED_ON_SW(out_data,index_switch);
    }
  }
  index_switch=(index_switch + 1) % NUMBER_OF_SWITCH;

  if(out_data == 65536 -1){
    RESET_ALL(out_data);
  }

}