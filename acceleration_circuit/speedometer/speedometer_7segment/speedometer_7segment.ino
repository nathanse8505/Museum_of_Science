#include "CONSTANTS.h"

float timer_1 = 0.0;
float timer_2;
float delta_time;
float velocity=0; 
float meter = 0.067;
bool check = LOW;


int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel



void Init_Output(int IO_Pin){
  digitalWrite(IO_Pin, LOW);/* try avoid high output */
  pinMode(IO_Pin, OUTPUT);
  digitalWrite(IO_Pin, LOW);/* make sure low  output */
}
void Digits_from_Number(int in_number){
    Digit_3_To_Display = in_number/100;// left digit to disply 
    int Temp_10 = in_number - 100 * Digit_3_To_Display;
    Digit_2_To_Display = Temp_10/10;// mid digit to disply 
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

void enable(int digit_enable){
 switch(digit_enable) {
      case 1: //range 0-9  '00000110'
       digitalWrite(LE_RIGHT, LOW);
       digitalWrite(LE_RIGHT, HIGH); 
       break;
      case 2: //range 9-99 '000000101'
       digitalWrite(LE_MIDDLE, LOW);
       digitalWrite(LE_MIDDLE, HIGH); 
       break;
      
      case 3: //range 100-999 '00000011'
       digitalWrite(LE_LEFT, LOW);
       digitalWrite(LE_LEFT, HIGH); 
       break;
      
      
    }
}



void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(IN_FET,INPUT_PULLUP);
  pinMode(OUT_FET,INPUT_PULLUP);

  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);
  Init_Output(LE_LEFT);
  digitalWrite(LE_LEFT, HIGH);// lock digit
  Init_Output(LE_MIDDLE);
  digitalWrite(LE_MIDDLE, HIGH);// lock digit
  Init_Output(LE_RIGHT);
  digitalWrite(LE_RIGHT, HIGH);// lock digit

}

void loop() {
    bool OUT = digitalRead(OUT_FET);
    bool IN  = digitalRead(IN_FET);
    if (OUT == LOW && check == LOW)
    {
      timer_1 = millis();
      check = HIGH;
    }
    if (IN == LOW && OUT == HIGH && check == HIGH)
    {   
        timer_2 = millis();
        delta_time = (timer_2 - timer_1)/1000;
        check = LOW;
        velocity = meter / delta_time;

        Serial.print(velocity,3);
        Serial.println(" m/s\n");
       
        velocity = (int)(velocity * 10);
        Serial.print(velocity,3);
        Serial.println(" m/s\n");
        
        Digits_from_Number(velocity);
        Display_Digit(Digit_1_To_Display);
        enable(1);
        Display_Digit(Digit_2_To_Display);
        enable(2);
    }

      

        

}

