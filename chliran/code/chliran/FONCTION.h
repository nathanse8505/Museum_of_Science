#ifndef FONCTION
#define FONCTION

#include "CONST.h"


void init_shift_register(){
  digitalWrite(Output_Enable,LOW);
  delay(10);
  digitalWrite(LatchPin,LOW);
}


// Dans FONCTION.h
bool PRESS_BUTTON(uint8_t switchNumber) {  // switchNumber 0 to 3
    
    // Check if the button is pressed
    if (digitalRead(SWITCH_PINS[switchNumber]) == LOW && checkStates[switchNumber] == LOW) {
        checkStates[switchNumber] = HIGH;
        delay(BOUNCE_TIME);
    }

    // Check if the button is released
    if (digitalRead(SWITCH_PINS[switchNumber]) == HIGH && checkStates[switchNumber] == HIGH) {
        checkStates[switchNumber] = LOW;
        return HIGH;
    }
    return LOW;
}

byte SET_FIRST_SW_LED(){
    
   shiftOut(Data_Serial, ClockPin, MSBFIRST, B00000001); 
   digitalWrite(LatchPin, HIGH); 
   delay(DELAY_AFTER_SHIFT);
   digitalWrite(LatchPin, LOW); 
   return B00000001;

}

byte SHIFT_LEFT(byte data){ 

   data =  data << 1;
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data); 
   digitalWrite(LatchPin, HIGH); 
   delay(DELAY_AFTER_SHIFT);
   digitalWrite(LatchPin, LOW);
   return data;
}

void RESET_ALL(){
  shiftOut(Data_Serial, ClockPin, MSBFIRST, B00000000); 
   digitalWrite(LatchPin, HIGH); 
   delay(DELAY_AFTER_SHIFT);
   digitalWrite(LatchPin, LOW);
}


#endif