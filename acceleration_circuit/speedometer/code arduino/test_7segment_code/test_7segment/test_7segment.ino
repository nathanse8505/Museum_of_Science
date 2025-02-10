#include "CONSTANTS_test.h"

int Number_To_Display = 0;
int Digit_4_To_Display = 0;
int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel



void Init_Output(int IO_Pin){
  digitalWrite(IO_Pin, LOW);/* try avoid high output */
  pinMode(IO_Pin, OUTPUT);
  digitalWrite(IO_Pin, LOW);/* make sure low  output */
}
void Digits_from_Number(int in_number){
    Digit_4_To_Display =  in_number / 1000;
    Digit_3_To_Display = (in_number - 1000 *Digit_4_To_Display)/100;// left digit to disply 
    Digit_2_To_Display = (in_number - 1000 *Digit_4_To_Display - 100 * Digit_3_To_Display)/10;// mid digit to disply 
    Digit_1_To_Display = in_number % 10 ;// right digit to disply 
  //Serial.println(Digit_3_To_Display);
  //Serial.println(Digit_2_To_Display);
  //Serial.println(Digit_1_To_Display);

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

void blank_Digit(int digit_enable){ //blank digit right to the left
  digitalWrite(BCD_A, HIGH);
  digitalWrite(BCD_B, HIGH);
  digitalWrite(BCD_C, HIGH);
  digitalWrite(BCD_D, HIGH);

  enable(digit_enable);

}

void blank_All_Digit(){
  for(int j=1 ;j<=4;j++){
         blank_Digit(j);
    }
}

void all_zero_digit()
{
  digitalWrite(BCD_A, LOW);
  digitalWrite(BCD_B, LOW);
  digitalWrite(BCD_C, LOW);
  digitalWrite(BCD_D, LOW);
  for(int j=1 ;j<=4;j++){
         enable(j);
    }

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
       case 4:
       digitalWrite(LE_THOUSANDS, LOW);
       digitalWrite(LE_THOUSANDS, HIGH); 
       break;
      
      
    }
}


void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);
  
 
  Init_Output(LE_UNITS);
  digitalWrite(LE_UNITS, HIGH);// lock digit

  Init_Output(LE_DOZENS);
  digitalWrite(LE_DOZENS, HIGH);// lock digit

  Init_Output(LE_CENTURIES);
  digitalWrite(LE_CENTURIES, HIGH);// lock digit

  Init_Output(LE_THOUSANDS);
  digitalWrite(LE_THOUSANDS, HIGH);// lock digit

}

int i = 0;
void loop() {
    Digits_from_Number(i);
    Display_Digit(Digit_1_To_Display);
    enable(1); 
    Display_Digit(Digit_2_To_Display);
    enable(2);
    Display_Digit(Digit_3_To_Display); 
    enable(3);
    Display_Digit(Digit_4_To_Display);
    enable(4);
    
    
    
  Serial.println(i);
  Serial.print("unity ");
  Serial.println(Digit_1_To_Display);
  Serial.print( "Dozens ");
  Serial.println(Digit_2_To_Display);
  Serial.print("centurie ");
  Serial.println(Digit_3_To_Display);
  Serial.print("Thousand "); 
  Serial.println(Digit_4_To_Display);
  delay(100);
    i+=1;
    if(i == 1000){i=0;}
}

