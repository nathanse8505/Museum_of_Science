#include "CONSTANTS.h"

#define TIME_RESTART 20000
const float meter = 3.14 * 1.14;

float timer_1 = 0.0;
float timer_2;
float delta_time;
float rpm;
float velocity = 0;
float km_per_h = 0; 

int check = 0;
bool check_zero = HIGH;

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
  for(int j=1 ;j<=4;j++){
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
  for(int j=1 ;j<=4;j++){
         blank_Digit(j);
    }
}



void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(SENSOR,INPUT_PULLUP);

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

  Serial.println("init");

}


void loop() {

    //Serial.println(check);
    //Serial.println(analogRead(SENSOR)); 
    float volt_sensor1=(float)(analogRead(SENSOR)*5)/1023;

      if (volt_sensor1 < 1 && check == 0){ 
        Serial.println("we are in a black to white zone start"); 
        timer_1 = millis();
        check = 1;
        
      }
      if(check == 1 && volt_sensor1 > 3){
        Serial.println("we are in a white to black zone"); 
        check = 2;
      }

      if(volt_sensor1 < 1 && check == 2){
         Serial.println("we are in a black to white zone stop");  

        timer_2 = millis();
        delta_time = (timer_2 - timer_1)/1000;
        check = 0;
        check_zero = HIGH;
        velocity = meter / delta_time;
        velocity = (velocity * 100);//for 3 display
        km_per_h = (int)velocity * 3.6;
        rpm = 60 / delta_time;

        Serial.print(velocity,3);
        Serial.println(" m/s");
         Serial.print(km_per_h,3);
        Serial.println(" km/h\n");

      }  
      
    if ((millis() - timer_1) > TIME_RESTART && check_zero){
        //all_zero_digit();
        velocity = 0;
        km_per_h = 0;
        rpm = 0;
        Serial.print(velocity,3);
        Serial.println(" m/s");
        Serial.print(rpm,3);
        Serial.println(" rpm\n"); 
        check_zero = LOW;
        //blank_All_Digit();
    }
    
        
            Digits_from_Number(km_per_h);
            Display_Digit(Digit_1_To_Display);
            enable(1);
            Display_Digit(Digit_2_To_Display);
            enable(2);
            Display_Digit(Digit_3_To_Display);
            enable(3);
          
    

}


      

