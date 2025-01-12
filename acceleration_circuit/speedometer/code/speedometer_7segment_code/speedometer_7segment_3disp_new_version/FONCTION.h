#ifndef FONCTION
#define FONCTION

#include "CONSTANTS.h"

void Init_Output(int IO_Pin){
  digitalWrite(IO_Pin, LOW);/* try avoid high output */
  pinMode(IO_Pin, OUTPUT);
  digitalWrite(IO_Pin, LOW);/* make sure low  output */
}


void Digits_from_Number(int in_number){
    Digit_3_To_Display = in_number / 100;// left digit to display 
    Digit_2_To_Display = (in_number - 100 * Digit_3_To_Display)/10;// mid digit to display 
    Digit_1_To_Display = in_number % 10 ;// right digit to display 

}
void Display_Digit(int digit_to_show) { 
    digitalWrite(BCD_A, LOW);// asume bit zero 
    if ((digit_to_show & 1) == 1) {digitalWrite(BCD_A, HIGH);}
    digitalWrite(BCD_B, LOW);// asume bit zero 
    if ((digit_to_show & 2) == 2) {digitalWrite(BCD_B, HIGH);}
    digitalWrite(BCD_C, LOW);// asume bit zero 
    if ((digit_to_show & 4) == 4) {digitalWrite(BCD_C, HIGH);}
    digitalWrite(BCD_D, LOW);// asume bit zero 
    if ((digit_to_show & 8) == 8) {digitalWrite(BCD_D, HIGH);}  
}


void enable(int digit_enable){
 switch(digit_enable) {
      case 1: //range 0-9  '00000110'
       digitalWrite(LE_UNITS, LOW);
       digitalWrite(LE_UNITS, HIGH); 
       break;
      case 2: //range 9-99 '000000101'
       digitalWrite(LE_DOZENS, LOW);
       digitalWrite(LE_DOZENS, HIGH); 
       break;
      case 3: //range 100-999 '00000011'
       digitalWrite(LE_CENTURIES, LOW);
       digitalWrite(LE_CENTURIES, HIGH); 
      
       break;
    }
}

void all_zero_digit(){
  digitalWrite(BCD_A, LOW);
  digitalWrite(BCD_B, LOW);
  digitalWrite(BCD_C, LOW);
  digitalWrite(BCD_D, LOW);
  for(int j=1; j <=  Number_OF_7SEG; j++){
         enable(j);
    }
}

void blank_Digit(int digit_enable){ //blank digit right to the left
  digitalWrite(BCD_A, HIGH);
  digitalWrite(BCD_B, HIGH);
  digitalWrite(BCD_C, HIGH);
  digitalWrite(BCD_D, HIGH);

  enable(digit_enable);

}

void blank_All_Digit(){
  for(int j=1;j <=  Number_OF_7SEG; j++){
         blank_Digit(j);
    }
}

bool SENSOR_OUT(){
  float volt_sensor=(float)(analogRead(SENSOR) * VREF)/BIT_RESOLUTION;
  if(volt_sensor < MIN_VOLT){
    return HIGH;
  }
  else if(volt_sensor > MAX_VOLT){
    return LOW;
  }
}

void Reset_Error_Sensor(){
  if((millis() - timer)/ms_to_sec > (1.8 * delta_time_sec) && check == LOW){
    check = HIGH;
  }
}








#endif