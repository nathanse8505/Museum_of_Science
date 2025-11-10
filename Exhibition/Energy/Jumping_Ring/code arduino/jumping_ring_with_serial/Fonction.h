#ifndef FONCTION
#define FONCTION

#include "Const_N_Global.h"
#include <avr/wdt.h>

//--------------------------Subroutines ----------------------------//
// initial IO to be output and set to zero - may not nead
void Init_Output(int IO_Pin){
  digitalWrite(IO_Pin, LOW);// try avoid high output 
  pinMode(IO_Pin, OUTPUT);
  digitalWrite(IO_Pin, LOW);//make sure low  output 
}
 
// digtal from number extrude the 3 digits to display (dlobal variables) 
void Digits_from_Number(int in_number){
    Digit_3_To_Display = 100*(in_number/100)/100;// left digit to disply 
    int Temp_10 = in_number - 100*Digit_3_To_Display;
    Digit_2_To_Display = 10*(Temp_10/10)/10;// mid digit to disply 
    Digit_1_To_Display = Temp_10-10*Digit_2_To_Display;// right digit to disply 
}
// send digit to 4511 
void Display_Digit(int digit_number, int digit_to_show) {
 if ((digit_number >=1)&&(digit_number <=3)&&(digit_to_show >=0)&&(digit_to_show <=9)) {
    digitalWrite(BCD_A, LOW);// asume bit zero 
    if ((digit_to_show & 1) == 1) {digitalWrite(BCD_A, HIGH);}
    digitalWrite(BCD_B, LOW);// asume bit zero 
    if ((digit_to_show & 2) == 2) {digitalWrite(BCD_B, HIGH);}
    digitalWrite(BCD_C, LOW);// asume bit zero 
    if ((digit_to_show & 4) == 4) {digitalWrite(BCD_C, HIGH);}
    digitalWrite(BCD_D, LOW);// asume bit zero 
    if ((digit_to_show & 8) == 8) {digitalWrite(BCD_D, HIGH);}
    switch(digit_number) {
      case 1:
       digitalWrite(LE_1, LOW);
       digitalWrite(LE_1, HIGH); 
       break;
      case 2:
       digitalWrite(LE_10, LOW);
       digitalWrite(LE_10, HIGH); 
       break;
      case 3:
       digitalWrite(LE_100, LOW);
       digitalWrite(LE_100, HIGH); 
       break;
    }
 }
}
//************************
//Blank Digit (send FF)
void Blank_Digit(int digit_number){
    digitalWrite(BCD_A, HIGH);// set bit one
    digitalWrite(BCD_B, HIGH);// set bit one
    digitalWrite(BCD_C, HIGH);// set bit one
    digitalWrite(BCD_D, HIGH);// set bit one
    switch(digit_number) {
      case 1:
       digitalWrite(LE_1, LOW);
       digitalWrite(LE_1, HIGH); 
       break;
      case 2:
       digitalWrite(LE_10, LOW);
       digitalWrite(LE_10, HIGH); 
       break;       
      case 3:
       digitalWrite(LE_100, LOW);
       digitalWrite(LE_100, HIGH); 
       break;
    }
}
//************************
// display full number 
void Display_full_Numnber(int Number_To_Display) {
  if (Number_To_Display > MAX_NUM_TO_DISPLAY){Number_To_Display = MAX_NUM_TO_DISPLAY;};// trim the number
  if (Number_To_Display < MIN_NUM_TO_DISPLAY){Number_To_Display = MIN_NUM_TO_DISPLAY;};
  Digits_from_Number(Number_To_Display);
//  Serial.print ("Now display");  
//  Serial.print ("\t"); //tab
//  Serial.println (Number_To_Display);  
   Display_Digit(1, Digit_1_To_Display);  // 
   Display_Digit(2, Digit_2_To_Display); // 
   Display_Digit(3, Digit_3_To_Display); // 
}
//********************
//Test 7 segments display 0-9 same digit on all 3 digits 
void Test_7_segments(){
for (int i = 0; i <= 9; i++){
  Digits_from_Number(Number_To_Display);
  /*Serial.print ("testing 7 segments");  
  Serial.print ("\t"); //tab
  Serial.println (i); 
  */ 
   Display_Digit(1, i);  
   Display_Digit(2, i);
   Display_Digit(3, i);    
   delay(DIGIT_TEST_RATE);
  }
}

//********************
//Test display 0 to 999  
void Test_Count_UP(){
   for (int K = 0; K<= 999; K++ ) {
    Display_full_Numnber(K);
    delay(DISPLAY_TEST_RATE);
    }
}
//************************
//Test display 999 doun to 0  
void Test_Count_DN(){
   for (int K = 999; K>= 0; K--) {
    Display_full_Numnber(K);
    delay(DISPLAY_TEST_RATE);
    }
}
//************************


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



#endif