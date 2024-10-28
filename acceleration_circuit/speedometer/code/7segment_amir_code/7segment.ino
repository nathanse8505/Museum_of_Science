/* 
 *  Control Electrolysis demo  Dec 2019 
 *  see G:\Bloomfield\פעילויות חינוך\מתקן אלקטרוליזה פיצוץ מימן חנוכה 201
 *  
 *  7 segments for Wid tunel and Piston Robot Bike, Bicycle Exhebition 2017
 *  See Seven_Segments_Arduino_Main_Board_June_2017.sch
 *  library H:\Amir Design\PCB_Design\Bloomfield\Bicycle_2017\Seven_Segments_Arduino
 *  control 2 digit BCD 4511 
 *  read encoder -.
 *  Blink for ZZZ sec
 *
 *  IO map
 *  Encoder bits: BIT_0 - D2,   BIT_1 - D3 
 *  BCD bits: A-D4, B-D5, C-D6,  D-D7
 *  Left digit (100th) LE (Negative Pulse) - D8; !! used as 10th digit
 *  Right digit (10th)LE (Negative Pulse) - D9 ; !! used as units 
  *  
 */
#include "CONSTANTS.h"

//-- Parameters and global variable ---
// -- encoder reading parameters and vairables 

uint32_t electrolysis_time = DEFAULT_ELECTROLYSIS_TIME ; //Seconds

int prescalar_counter = 0 ; //encoder coun per "click"
int Move_Sum  = 0; //Counting total encoder movment 
int Eror_counter = 0;// to check if encoder reading is OK
byte Old_Read = 0; // old 2 bits read from encoder 
byte New_Read = 0; // new 2 bits read from encoder 
byte Check_Direction = 0; // 4 bits (old1, old0, new1, new0) 

int switch_state = HIGH ; 
int trimmer_read = 0 ;//
int pwm  = DEFAULT_PWM ; // drive electrolysis current 

//uint32_t Idle_Time_Counter = 0; 
//uint32_t Current_Time = 0; 
//uint32_t Time_From_Start = 0; 


// 7 segments parameters
//const int MAX_NUM_TO_DISPLAY  = 99 ; //the maximum number to display
//const int MIN_NUM_TO_DISPLAY  = 0 ; //the minimum number to display
int Number_To_Display = 0;
int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel

/********************************/

//--------------------------Subroutines ----------------------------//
// initial IO to be output and set to zero - may not nead
void Init_Output(int IO_Pin){
  digitalWrite(IO_Pin, LOW);/* try avoid high output */
  pinMode(IO_Pin, OUTPUT);
  digitalWrite(IO_Pin, LOW);/* make sure low  output */
}

/***********/ 
// Calibration - read trimer and set PWM only during SW preased !!!
void Calibration(){
  Ignition(SPARK_TIME);
  Display_full_Numnber(88) ;
  while  (switch_state == LOW) {
     analogWrite(ELECTROD_PWM, pwm);
     trimmer_read = analogRead(SET_CURRENT_TRIMER_IN);
     pwm = map(trimmer_read,0,1024, MIN_PWM, MAX_PWM);
     Display_full_Numnber(pwm);
     switch_state = digitalRead(SW_IN); // wait switch release 
    }
  digitalWrite (ELECTROD_PWM, LOW);
  digitalWrite (ELECTROD_PWM, LOW);// make sure
}

/***********/ 
// Ignition - ignition time set in mS !!!
void Ignition(int ignition_time) {
  digitalWrite (SPARK_OUT, HIGH);
  delay (ignition_time);
  digitalWrite (SPARK_OUT, LOW);
  digitalWrite (SPARK_OUT, LOW);// make sure :) 
}

/***********/ 
// Count_Down - time to count in SEC (not mS)!!!
void Count_Down(uint32_t Time_To_Count) {
int switch_state = HIGH ; 
uint32_t time_from_start = millis()  ;
uint32_t temp_pass_time = millis() - time_from_start; 
uint32_t temp_time_to_show = (Time_To_Count - temp_pass_time/1000) ;
//Display_full_Numnber (temp_time_to_show);
while (temp_pass_time <= Time_To_Count*1000) {
    temp_time_to_show = (Time_To_Count - temp_pass_time/1000) ;
    Display_full_Numnber (temp_time_to_show);
    if (temp_time_to_show <=BLINK_TIME){
      delay (BLINK_ON);    
      switch_state = digitalRead(SW_IN);
      if (switch_state == LOW) {break;} ; // exit count down if sw preased 
      Blank_Digit(3);
      Blank_Digit(2);
      delay (BLINK_OFF);
      }
  temp_pass_time = millis() - time_from_start;
  switch_state = digitalRead(SW_IN);
  if (switch_state == LOW) {break;} ; // exit count down if sw preased 
  }
}

/****************************************************/

// read encoder and returen total counting (serial print also errors..) use global variable 
int Read_Encoder(){
     Old_Read = New_Read ;
     int temp_return = 0 ;
 //  New_Read = PINB & 3;// only if ATmega 168/328 and encoder bits are 8,9 (port b 0, 1)
     New_Read = (2 * (digitalRead(ENCODER_BIT_1)) + (digitalRead(ENCODER_BIT_0))) ;
     Check_Direction  = 4*Old_Read + New_Read ; // x4 = 2 rotate left 
     
    //Serial.println (Check_Direction, DEC) ;
     switch (Check_Direction)
     {
     case 0: 
     break;
     case 5:
     break;
     case 10:
     break;
     case 15:
     break;
     case 1:
     temp_return = 1;
     break;
     case 7:
     temp_return = 1;
     break;
     case 14:
     temp_return = 1;
     break;
     case 8:
     temp_return = 1; 
     break;
     case 4:
     temp_return = -1;
     break;
     case 13:
     temp_return = -1;
     break;
     case 11:
     temp_return = -1;
     break;
     case 2:
     temp_return = -1;
     break;
     default:
     Eror_counter = Eror_counter+1;//for test only (or not...)
     break;
    }
  return temp_return;  
}

/***********/ 
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
      case 2:
       digitalWrite(LE_RIGHT, LOW);
       digitalWrite(LE_RIGHT, HIGH); 
       break;
      case 3:
       digitalWrite(LE_LEFT, LOW);
       digitalWrite(LE_LEFT, HIGH); 
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
      case 2:
       digitalWrite(LE_RIGHT, LOW);
       digitalWrite(LE_RIGHT, HIGH); 
       break;
      case 3:
       digitalWrite(LE_LEFT, LOW);
       digitalWrite(LE_LEFT, HIGH); 
       break;
    }
}
//************************

// display full number 
void Display_full_Numnber(int Number_To_Display) {
  Digits_from_Number(Number_To_Display);
  Serial.print ("Now display");  
  Serial.print ("\t"); //tab
  Serial.println (Number_To_Display);  
   Display_Digit(2, Digit_1_To_Display);  // for 3 digits use Display_Digit(1, Digit_1_To_Display)
   Display_Digit(3, Digit_2_To_Display); // for 3 digits use Display_Digit(2, Digit_2_To_Display)
   //Display_Digit(3, Digit_3_To_Display); // not used    
}
//********************

//Test 7 segments display 0-9 same digit on all 3 digits 
void Test_7_segments(){
for (int i = 0; i <= 9; i++){
  Digits_from_Number(Number_To_Display);
  Serial.print ("testing 7 segments");  
  Serial.print ("\t"); //tab
  Serial.println (i);  

   Display_Digit(1, i);  
   Display_Digit(2, i);
   Display_Digit(3, i);    
   delay(1000);
  }
}

//************************

void setup() {
  TCCR1B = TCCR1B & B11111000 | B00000010;// to set fast PWM on D9, D10 to 3.9 Khz see https://etechnophiles.com/change-frequency-pwm-pins-arduino-uno/
  //TCCR1B = TCCR1B & B11111000 | B00000001;// to set fast PWM on D9, D10 to 30 Khz see https://etechnophiles.com/change-frequency-pwm-pins-arduino-uno/
	Serial.begin(SERIAL_BAUD_RATE);

  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);
  Init_Output(LE_LEFT);
  digitalWrite(LE_LEFT, HIGH);// lock digit
  Init_Output(LE_RIGHT);
  digitalWrite(LE_RIGHT, HIGH);// lock digit
  //Init_Output(ELECTROD_PWM);
  Init_Output(PS_OUT);
  Init_Output(SPARK_OUT);
 
  pinMode(ENCODER_BIT_0, INPUT_PULLUP);
  pinMode(ENCODER_BIT_1, INPUT_PULLUP);
  pinMode(SW_IN, INPUT_PULLUP);

  
  Test_7_segments() ;
  for (int i = 1; i<= 3; i++ ) {Blank_Digit(i);}// blank all digits 
  switch_state = digitalRead(SW_IN);
  if (switch_state == LOW) { // check switch  
      delay (WAIT_FOR_CALIBRATION) ;  }//
  switch_state = digitalRead(SW_IN);
  if (switch_state == LOW) { // start count down 
      Calibration() ;  }//  if switch presed long enought - call calibrastion 
 trimmer_read = analogRead(SET_CURRENT_TRIMER_IN);
 pwm = map(trimmer_read,0,1024, MIN_PWM, MAX_PWM);   
 
 Display_full_Numnber (electrolysis_time);
 Read_Encoder();
}

void loop() {
//  int switch_state = HIGH ; 
  int temp_read = Read_Encoder();
  Display_full_Numnber (electrolysis_time);
  if (temp_read != 0) {
    prescalar_counter = prescalar_counter + temp_read;
     if (prescalar_counter >= PRESCALAR ) {
      electrolysis_time = electrolysis_time +  (prescalar_counter / PRESCALAR);
      prescalar_counter = 0 ;
      }
    if (prescalar_counter <=  (- PRESCALAR) ) {
      electrolysis_time = electrolysis_time +  (prescalar_counter / PRESCALAR);
      prescalar_counter = 0 ;
      }
      
      if (electrolysis_time <=  MIN_ELECTROLYSIS_TIME) { electrolysis_time =  MIN_ELECTROLYSIS_TIME;}
      if (electrolysis_time >=  MAX_ELECTROLYSIS_TIME) { electrolysis_time =  MAX_ELECTROLYSIS_TIME;}
      Serial.println (electrolysis_time);
     // Display_full_Numnber (electrolysis_time);
  }
switch_state = digitalRead(SW_IN);
  if (switch_state == LOW) { // start count down 
    delay (BOUNCE_TIME);
    while  (switch_state == LOW) {
    switch_state = digitalRead(SW_IN); // wait switch release 
     }
  // delay (BOUNCE_TIME);
     Ignition(SPARK_TIME);
     digitalWrite (PS_OUT, HIGH);
     //trimmer_read = analogRead(SET_CURRENT_TRIMER_IN);// to enable trimmer change between demo
     //pwm = map(trimmer_read,0,1024, MIN_PWM, MAX_PWM);
     //analogWrite(ELECTROD_PWM, pwm);
     Count_Down (electrolysis_time); // exit count down is sw preased 
     Ignition(SPARK_TIME);
     Display_full_Numnber(0) ; // just for case of igniting during count down 
     //analogWrite(ELECTROD_PWM, 0);
//     digitalWrite (ELECTROD_PWM, LOW);
//     digitalWrite (ELECTROD_PWM, LOW);// make sure
     digitalWrite (PS_OUT, LOW);
     digitalWrite (PS_OUT, LOW);// make sure
     delay (500); 
     delay (BOUNCE_TIME);
      while  (switch_state == LOW) {
      switch_state = digitalRead(SW_IN); // wait switch release  
      }
  }
}
