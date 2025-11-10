#include "CONST.h"
#include "FONCTION.h"

void setup() {
  Serial.begin(Bauderate);

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
}

void loop() { 
  wdt_reset();
  // Check if current switch is pressed and is active in the sequence
  if(PRESS_BUTTON(index_switch) && (checkSW[index_switch] == HIGH)){
    timer_on = millis();
    // Shift LED to next position 
    out_data = SHIFT_LEFT(out_data);
    while(millis() - timer_on < TIME_LED_ON){
      wdt_reset();
    }
    RESET_ALL(out_data);
    timer_off = millis();
    while(millis() - timer_off < TIME_LED_OFF){
        wdt_reset();
    }
    timer_on = 0;
    timer_off = 0;

    // If not the last switch, shift LED one more position
    if(index_switch < NUMBER_OF_SWITCH - 1){
      Serial.println("enter");
      out_data = SHIFT_LEFT(out_data);
    }

    // Update switch states for next sequence
    checkSW[index_switch] = LOW;      // Deactivate current switch
    checkSW[index_switch + 1] = HIGH; // Activate next switch
    index_switch++;                   // Move to next switch in sequence

    // Debug information
    Serial.println(out_data);
    for(int i = 0; i < NUMBER_OF_SWITCH; i++){
      wdt_reset();
      Serial.print("checkSW" + String(i) + ": ");
      Serial.println(checkSW[i]);
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
    Serial.println(out_data);
  }
  if(out_data == 65536 -1){
    RESET_ALL(out_data);
  }
}