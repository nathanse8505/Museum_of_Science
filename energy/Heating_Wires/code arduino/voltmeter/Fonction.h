#ifndef FONCTION
#define FONCTION


#include "Const.h"

//#include <Adafruit_INA219.h>
#include <Wire.h>
#include <avr/wdt.h>


//////////////////////////////////////////////////////////////////////////////////
///////////////////////////// only 7 segment void///////////////////////////////
////////////////////////////////////////////////////////////////////////////////// 
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
      case RIGHT_DIGIT: //range 0-9  '00000110'
       digitalWrite(LE_UNITS, LOW);
       digitalWrite(LE_UNITS, HIGH); 
       break;
      case MIDDLE_DIGIT: //range 9-99 '000000101'
       digitalWrite(LE_DOZENS, LOW);
       digitalWrite(LE_DOZENS, HIGH); 
       break;
      case LEFT_DIGIT: //range 100-999 '00000011'
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


void NUMBER_TO_DISPLAY(float variable){
  Digits_from_Number(variable);
  Display_Digit(Digit_1_To_Display);
  enable(RIGHT_DIGIT);
  
  if(variable < 10 ){
    blank_Digit(MIDDLE_DIGIT);
  }
  else{
    Display_Digit(Digit_2_To_Display);
    enable(MIDDLE_DIGIT);
  }
  
  if(variable < 100 ){
    blank_Digit(LEFT_DIGIT);
  }
  else{
    Display_Digit(Digit_3_To_Display);
    enable(LEFT_DIGIT);
  }
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void voltmeter(int pin){
  Vout = (float) analogRead(pin);
  //Serial.println(Vout);
  Vout = (Vout*Vref)/BIT_RESOLUTION;
  Serial.println(Vout);
  Vin = (Vout*(R1 + R2))/R1;
}

void voltmeter_after_wire(int pin){
  Vout = (float) analogRead(pin);
  Serial.println(Vout);
  Vout = (Vout*Vref)/BIT_RESOLUTION;
  Vin = Vout;
}



#endif