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


float read_temperature(int data){
  //R2 = R1 * (1023.0 / (float)data - 1.0); //+5V --- Thermistance (R2) --- A3 --- R1 --- GND
  R2 = R1 * ((float)data / (1023.0 - (float)data)); //+5V --- R1 (fixe) --- A3 --- Thermistance (R2) --- GND

  float TK = (1.0 / (A + B*log(R2) + C*pow(log(R2),3)));
  float Tc = TK - 273.15;
  float Tf = (Tc * 9.0)/ 5.0 + 32.0; 
  return Tc;
}


bool PRESS_BUTTON_LANG() {
  // Check if the button is pressed
  if (digitalRead(LANG_BUTTON_IO) == LOW && check_lang == LOW) {
     //Serial.println("press :");
     check_lang = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(LANG_BUTTON_IO) == HIGH && check_lang == HIGH) {
    //Serial.println("unpress");
    check_lang = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}


float rolling_average(float newTemp) {
  // save the new measure
  temperatureBuffer[bufferIndex++] = newTemp;
  if (bufferIndex >= AVG_WINDOW_SIZE) {
    bufferIndex = 0;
    bufferFull = true;
  }

  float sum = 0;
  int count = bufferFull ? AVG_WINDOW_SIZE : bufferIndex;
  for (int i = 0; i < count; i++) {
    sum += temperatureBuffer[i];
  }

  return sum / count;
}



#endif