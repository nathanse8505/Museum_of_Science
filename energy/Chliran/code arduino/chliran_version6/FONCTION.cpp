#include "FONCTION_and_CONST.h"

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
 * Reset all LEDs to OFF state
 * @param data Current LED state (will be set to 0)
 */
void RESET_ALL(int data){
   data = 0;
   // Send 16 bits of zeros (two 8-bit shifts)
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data >> 8); 
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data); 
   digitalWrite(LatchPin, HIGH);  // Latch data
   delay(DELAY_AFTER_ON);
   digitalWrite(LatchPin, LOW);   // Prepare for next data
}


/**
 * Shift LED pattern one position left
 * @param data Current LED pattern
 * @return New LED pattern after shift
 */
int LED_ON_UV(int data,int i){ 
   //data = data << 1; 
   data |= (1 << index_data_led_uv[i] );
   data &= ~(1 << index_data_led_sw[i]);
   // Send 16 bits in two 8-bit chunks
   shiftOut(Data_Serial, ClockPin, MSBFIRST, (data >> 8)); 
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data); 
   digitalWrite(LatchPin, HIGH);  // Latch data
   delay(DELAY_AFTER_ON);
   digitalWrite(LatchPin, LOW);   // Prepare for next data
   return data;
}
int LED_ON_SW(int data,int i){ 
   data |= (1 << index_data_led_sw[i] );
   data &= ~(1 << index_data_led_uv[i]);
   // Send 16 bits in two 8-bit chunks
   shiftOut(Data_Serial, ClockPin, MSBFIRST, (data >> 8)); 
   shiftOut(Data_Serial, ClockPin, MSBFIRST, data); 
   digitalWrite(LatchPin, HIGH);  // Latch data
   delay(DELAY_AFTER_ON);
   digitalWrite(LatchPin, LOW);   // Prepare for next data
   return data;
}
