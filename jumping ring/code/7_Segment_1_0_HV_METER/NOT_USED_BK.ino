/*
 * 
 * 
#define ELECTROD_PWM (10) // Output to FET drive electrod ecurrent
#define PS_OUT (10) // Output to FET drive power supply 
#define SPARK_OUT (11) // Output to FET drive HV spark module

#define SW_IN (12) // input from pushbutton switch (nead pull up SW/HW)

#define SET_CURRENT_TRIMER_IN (A0) // analod input to set PWM

#define ENCODER_BIT_0 (2) // input IO for gray code bit 0, Gren wire in 400 and 100 ppr encoders 
#define ENCODER_BIT_1 (3) // input IO for gray code bit 1, white wire in 400 and 100 ppr encoders 
int const PRESCALAR = 4 ; //encoder coun per "click"

int const  MIN_ELECTROLYSIS_TIME = 5 ; //Seconds
int const  MAX_ELECTROLYSIS_TIME = 60 ; //Seconds
uint32_t const DEFAULT_ELECTROLYSIS_TIME = 30; //sec 


int const  MIN_PWM = 0 ; //PWM set electrolysis current
int const  MAX_PWM = 75; //PWM ser electrolysis current
int const  DEFAULT_PWM = 40 ; //asume 3Volt to set 2A current. based on Dec experiments with 50# stainless steel mesh

int const  SPARK_TIME = 500 ; //mSeconds

int const  WAIT_FOR_CALIBRATION = 4000 ; //mSeconds waiting during switch preased for calibration




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
int pwm  = DEFAULT_PWM ; // drive electrolysis cyrrent 

//uint32_t Idle_Time_Counter = 0; 
//uint32_t Current_Time = 0; 
//uint32_t Time_From_Start = 0; 


// Ignition - ignition time set in mS !!!
void Ignition(int ignition_time) {
  digitalWrite (SPARK_OUT, HIGH);
  delay (ignition_time);
  digitalWrite (SPARK_OUT, LOW);
  digitalWrite (SPARK_OUT, LOW);// make sure :) 
}
void setup() {
  TCCR1B = TCCR1B & B11111000 | B00000010;// to set fast PWM on D9, D10 to 3.9 Khz see https://etechnophiles.com/change-frequency-pwm-pins-arduino-uno/
  //TCCR1B = TCCR1B & B11111000 | B00000001;// to set fast PWM on D9, D10 to 30 Khz see https://etechnophiles.com/change-frequency-pwm-pins-arduino-uno/

// Count_Down - time to count in SEC (not mS)!!!
void Count_Down(uint32_t Time_To_Count) {
//int switch_state = HIGH ; 
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
  
  //Init_Output(ELECTROD_PWM);
  Init_Output(PS_OUT);
  Init_Output(SPARK_OUT);
 
  pinMode(ENCODER_BIT_0, INPUT_PULLUP);
  pinMode(ENCODER_BIT_1, INPUT_PULLUP);
  pinMode(SW_IN, INPUT_PULLUP);


*/
