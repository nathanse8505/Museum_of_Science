#include "FONCTION_and_CONST.h"
File logFile;

void setup() {
  Serial.begin(Bauderate);
   while (!Serial);  // Wait for Serial Monitor to be ready

    // Initialize the SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(CHIPSELECT)) {
        Serial.println("SD card initialization failed!");
    } else {
        Serial.println("SD card successfully initialized.");
    }

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
  //pinMode(SW5, INPUT_PULLUP);

  // Initialize shift register and switch states
  init_shift_register();
  init_switch_state();
  
  // Set the first LED on power up
  out_data = SET_FIRST_SW_LED();
  wdt_enable(WDTO_2S);
  Serial.println("init");
  logEvent("init");
  time_log = millis();
}

void loop() {
  
  if(millis() - time_log > SET_DAY){
    logEvent("5 hours have passed");
    time_log = millis();
  } 

  //button = read_press_button();
  wdt_reset();
  // Check if current switch is pressed and is active in the sequence
  if(PRESS_BUTTON(index_switch) && (checkSW[index_switch] == HIGH)){

    logMessage = "SW" + String(index_switch + 1) + " ;has been pressed and the led UV has activate";
    logEvent(logMessage.c_str());
    Serial.println("SW" + String(index_switch + 1) + " ;has been pressed and the led UV has activate");
    timer_on = millis();
    // Shift LED to next position 
    out_data = SHIFT_LEFT(out_data);
    while(millis() - timer_on < TIME_LED_ON){
      //Serial.println("enter to the delay mode ON");
      log_press_button();
      wdt_reset();
    }
    RESET_ALL(out_data);
    timer_off = millis();
    while(millis() - timer_off < TIME_LED_OFF){
      //Serial.println("enter to the delay mode OFF");
        log_press_button();
        wdt_reset();
    }
    timer_on = 0;
    timer_off = 0;

    // If not the last switch, shift LED one more position
    if(index_switch < NUMBER_OF_SWITCH - 1){
      out_data = SHIFT_LEFT(out_data);
      Serial.println("Led SW" + String(index_switch + 2) + " ;has activate");
    }

    // Update switch states for next sequence
    checkSW[index_switch] = LOW;      // Deactivate current switch
    checkSW[(index_switch + 1) % NUMBER_OF_SWITCH] = HIGH; // Activate next switch
    index_switch++;                   // Move to next switch in sequence

    // Debug information
    Serial.println("out data: " + String(out_data));
    Serial.println("index_switch: " + String(index_switch));
    for(int i = 0; i < NUMBER_OF_SWITCH; i++){
      Serial.print("checkSW" + String(i + 1) + ": ");
      Serial.println(checkSW[i]);
    }
    
    
  }
  else {
    for (int i = 1; i <= 3; i++) {
      int swIndex = (index_switch + i) % NUMBER_OF_SWITCH;
      if (PRESS_BUTTON(swIndex)) {
        logMessage = "SW" + String(swIndex + 1) + " ;has been pressed";
        logEvent(logMessage.c_str());
        Serial.println(logMessage);
      }
    }
  }
  
  
  //BLINK(out_data);

  // Reset sequence when reaching the last switch
  if(index_switch == NUMBER_OF_SWITCH){
    Serial.println("enter to the reset");
    wdt_reset();
    RESET_ALL(out_data);             // Turn off all LEDs
    checkSW[index_switch] = LOW;     // Deactivate last switch
    index_switch = 0;                // Reset to first switch
    checkSW[index_switch] = HIGH;    // Activate first switch
    out_data = SET_FIRST_SW_LED();   // Turn on first LED
    Serial.println("out data: " + String(out_data));
  }
  if(out_data == 65536 -1){
    RESET_ALL(out_data);
  }

}