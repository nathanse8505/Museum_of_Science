#include "CONSTANTS.h"
#define Number_OF_7SEG 4

float timer_1 = 0.0;
float timer_2;
float delta_time;
float velocity=0; 
float meter = 0.067;
bool check = LOW;


int BCD[4] = {BCD_A, BCD_B, BCD_C, BCD_D};
int Digit_To_Display[Number_OF_7SEG]; //{Digit_THOUSANDS, Digit_CENTURIES, Digit_DOZENS, Digit_UNITS}
int LE[Number_OF_7SEG] = {LE_THOUSANDS, LE_CENTURIES, LE_DOZENS, LE_UNITS};

void Init_Output(int IO_Pin){
  digitalWrite(IO_Pin, LOW);/* try avoid high output */
  pinMode(IO_Pin, OUTPUT);
  digitalWrite(IO_Pin, LOW);/* make sure low  output */
}
void Digits_from_Number(int in_number){

    int temp = pow(10, Number_OF_7SEG - 1);
    for(int i = 0; i < Number_OF_7SEG; i++){
      Digit_To_Display[i] = in_number / temp;
      in_number -= temp * Digit_To_Display[i]; 
      temp = temp /10;
    }
}
void Display_Digit(int digit_to_show) { 
     for(int i = 0; i < 4; i++){
      digitalWrite(BCD[i], LOW);// asume bit zero 
      if ((digit_to_show & (int)pow(2,i))) {digitalWrite(BCD[i], HIGH);}
    }
}


void enable(int digit_enable){
 switch(digit_enable) {
      case 0://range 1000-9999
        digitalWrite(LE_THOUSANDS, LOW);
        digitalWrite(LE_THOUSANDS, HIGH); 
        break;
      case 1: //range 100-999
        digitalWrite(LE_CENTURIES, LOW);
        digitalWrite(LE_CENTURIES, HIGH); 
        break;
      case 2: //range 9-99 
        digitalWrite(LE_DOZENS, LOW);
        digitalWrite(LE_DOZENS, HIGH); 
        break;
      case 3: //range 0-9 
        digitalWrite(LE_UNITS, LOW);
        digitalWrite(LE_UNITS, HIGH); 
        break;
    }
}

void all_zero_digit(){
  for(int j = 0; j < Number_OF_7SEG; j++){
    for(int i = 0 ;i < 4; i++){
      digitalWrite(BCD[i], LOW);
    }
     enable(j);
  }
}

void blank_Digit(int digit_enable){ 
    for(int i = 0 ;i < 4; i++){
        digitalWrite(BCD[i], HIGH);
    }
    enable(digit_enable);
}

void blank_All_Digit(){ //blank all digit from left to right
    for(int i=0 ; i < 4; i++){
         blank_Digit(i);
    }
}



void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    pinMode(IN_FET,INPUT_PULLUP);
    pinMode(OUT_FET,INPUT_PULLUP);

    for(int i = 0; i < 4; i++){
      Init_Output(BCD[i]);
    }

    for(int i = 0; i < Number_OF_7SEG; i++){
        Init_Output(LE[i]);
        digitalWrite(LE[i], HIGH);// lock digit
    }
  
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
       
        velocity = (int)(velocity * 1000);

        Digits_from_Number(velocity);
        for(int i = 0; i < Number_OF_7SEG; i++){
            Display_Digit(Digit_To_Display[i]);
            enable(i);
        }
            
    }

    if ((millis() - timer_2) > 10000){
        all_zero_digit();
        //velocity = 0;
        //blank_All_Digit();
    }


      

        

}


