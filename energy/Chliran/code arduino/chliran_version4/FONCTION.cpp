#include "FONCTION.h"

/**
 * Initialize shift register by setting Output Enable and Latch pins
 * OE is set LOW to enable outputs
 */
void init_shift_register(){
  digitalWrite(Output_Enable, LOW);  // Enable outputs
  delay(10);
  digitalWrite(LatchPin, LOW);       // Prepare for data input
}

/**
 * Initialize switch state array
 * Sets all switches to LOW except first one
 */
void init_switch_state(){
  for(int i = 0; i < NUMBER_OF_SWITCH; i++){
      checkSW[i] = LOW;
      checkStates[i] =  LOW;
  }
  checkSW[0] = HIGH;  // Activate first switch
}

/**
 * Handle button press with debouncing
 * @param switchNumber Index of switch to check (0 to NUMBER_OF_SWITCH-1)
 * @return HIGH if button was pressed and released, LOW otherwise
 */
bool PRESS_BUTTON(uint8_t switchNumber) {
    // Check if button is pressed
    if (digitalRead(SWITCH_PINS[switchNumber]) == LOW && checkStates[switchNumber] == LOW) {
        checkStates[switchNumber] = HIGH;
        delay(BOUNCE_TIME);
    }

    // Check if button is released
    if (digitalRead(SWITCH_PINS[switchNumber]) == HIGH && checkStates[switchNumber] == HIGH) {
        checkStates[switchNumber] = LOW;
        return HIGH;
    }
    return LOW;
}

/**
 * Reset all LEDs to OFF state
 * @param data Current LED state (will be set to 0)
 */
void RESET_ALL(int data){
   data = 0;
   // Send 16 bits of zeros (two 8-bit shifts)
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data >> 8); 
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data); 
   digitalWrite(LatchPin, HIGH);  // Latch data
   delay(DELAY_AFTER_SHIFT);
   digitalWrite(LatchPin, LOW);   // Prepare for next data
}

/**
 * Set first LED in sequence
 * @return 1 (binary 0000000000000001)
 */
int SET_FIRST_SW_LED(){
   RESET_ALL(out_data);          // Clear all LEDs first
   shiftOut(Data_Serial, ClockPin, MSBFIRST, 1);  // Set first LED
   digitalWrite(LatchPin, HIGH);  // Latch data
   delay(DELAY_AFTER_SHIFT);
   digitalWrite(LatchPin, LOW);   // Prepare for next data
   return 1;
}

/**
 * Shift LED pattern one position left
 * @param data Current LED pattern
 * @return New LED pattern after shift
 */
int SHIFT_LEFT(int data){ 
   data = data << 1;             // Shift bits left
   // Send 16 bits in two 8-bit chunks
   shiftOut(Data_Serial, ClockPin, MSBFIRST, (data >> 8)); 
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data); 
   digitalWrite(LatchPin, HIGH);  // Latch data
   delay(DELAY_AFTER_SHIFT);
   digitalWrite(LatchPin, LOW);   // Prepare for next data
   return data;
}

void BLINK(int data){
  shiftOut(Data_Serial, ClockPin, MSBFIRST, (data >> 8)); 
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data); 
   digitalWrite(LatchPin, HIGH);  // Latch data
   delay(DELAY_AFTER_SHIFT);
   digitalWrite(LatchPin, LOW);   // Prepare for next data
   delay(DELAY_BLINK);
   RESET_ALL(data);
   delay(DELAY_BLINK);
}

void logEvent(const char* message) {
    File logFile = SD.open("log.txt", FILE_WRITE);
    if (logFile) {
        logFile.print(millis());
        logFile.print(" ms ; ");
        logFile.println(message);
        logFile.close();
    } else {
        Serial.println("Erreur d'écriture sur la carte SD.");
    }
}

void log_press_button(){

  if(PRESS_BUTTON(0)) {
      logEvent("SW1 ;has been pressed");
      Serial.println("the button SW1 has been pressed");
  }
  if(PRESS_BUTTON(1)) {
      logEvent("SW2 ;has been pressed");
      Serial.println("the button SW2 has been pressed");
  }
  if(PRESS_BUTTON(2)) {
      logEvent("SW3 ;has been pressed");
      Serial.println("the button  SW3 has been pressed");
  }
  if(PRESS_BUTTON(3)) {
      logEvent("SW4 ;has been pressed");
      Serial.println("the button  SW4 has been pressed");
  }
}
