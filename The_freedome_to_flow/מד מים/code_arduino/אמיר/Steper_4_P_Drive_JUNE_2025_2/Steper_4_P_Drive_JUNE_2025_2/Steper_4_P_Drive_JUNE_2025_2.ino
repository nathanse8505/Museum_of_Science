/*
 * drive 28BYJ gear 4 phase stepper motor https://vi.aliexpress.com/item/1005002332358417.html
 * 4 sequential pulses 
 * pulses and space time define with 2 potentiometer 
 * JUNE 2025, Amirb@mada.org.il.  
 * Nano, ATmega328T oldbootloader (or not see board..) 
 * pin out - see CONST_N_Global.h
 * serial print and/or (acording to flags - see/set in const_N_Global) display (OLED SH106) encoders counting 
 * For Arad Dalia water meters outdoor display 
 * Summer 2025 
 * 
 */

#include "CONST_N_GLOBAL.h"
#include "Serial_Routins.h"
#include "Display_Routins.h"
//--------------------------
void Read_Pot(){
  pot_2_value = analogRead(POT_2);
  pot_3_value = analogRead(POT_3);
  pulse_time = map(pot_2_value, MIN_POT_VALUE, MAX_POT_VALUE, MIN_PULSE_TIME, MAX_PULSE_TIME);
//  space_time = map(pot_3_value, MIN_POT_VALUE, MAX_POT_VALUE, MIN_SPACE_TIME, MAX_SPACE_TIME);
  number_of_sets = map(pot_3_value, MIN_POT_VALUE, MAX_POT_VALUE, MIN_NUMBER_OF_SETS, MAX_NUMBER_OF_SETS);
  
}
//--------------------------
void all_coils_off(){
  digitalWrite(OUT_COIL_4, LOW);// make sure no output      
  digitalWrite(OUT_COIL_3, LOW);// make sure no output 
  digitalWrite(OUT_COIL_2, LOW);// make sure no output 
  digitalWrite(OUT_COIL_1, LOW);// make sure no output 
}
  

//
void Drive_3_Coils(byte number_of_sets){
  all_coils_off(); // make suree all coils off 
  digitalWrite(INTERNAL_LED, HIGH);// indicate start pulses
  for (int32_t i = 0; i < number_of_sets; i++) {
     digitalWrite(OUT_COIL_1, HIGH);// pulse coil 1 
     delay(pulse_time); 
     digitalWrite(OUT_COIL_4, LOW);// stop pulse coil 4 
     delay(pulse_time); 
     digitalWrite(OUT_COIL_2, HIGH);// pulse coil 2 
     delay(pulse_time); 
     digitalWrite(OUT_COIL_1, LOW);// stop pulse coil 1
     delay(pulse_time); 
     digitalWrite(OUT_COIL_3, HIGH);// pulse coil 3      
     delay(pulse_time); 
     digitalWrite(OUT_COIL_2, LOW);// stop pulse coil 2
     delay(pulse_time); 
     digitalWrite(OUT_COIL_4, HIGH);// pulse coil 4      
     delay(pulse_time); 
     digitalWrite(OUT_COIL_3, LOW);// stop pulse coil 3
     delay(pulse_time); 
//     delay(space_time); 
     }
  all_coils_off(); // make suree all coils off 
  digitalWrite(INTERNAL_LED, LOW);// indicate stop pulses   
}
//--------------------------

//==================================-
void setup() {
  // make encoder IO pulsedup inputs:
  pinMode(PUSHBUTTON, INPUT_PULLUP);  
  pinMode(OUT_COIL_1, OUTPUT);
  pinMode(OUT_COIL_2, OUTPUT);
  pinMode(OUT_COIL_3, OUTPUT);
  pinMode(OUT_COIL_4, OUTPUT);
  pinMode(INTERNAL_LED, OUTPUT);
  all_coils_off(); // make suree all coils off 
  digitalWrite(OUT_COIL_4, LOW);// make sure no output      
  
  Current_Time = millis();

  if(Use_Serial) {Start_Serial();}
  if(Use_Display) {Start_Display();}

}
//--------------------

void loop() {
  buttonState = digitalRead(PUSHBUTTON);
  if (buttonState == LOW){
    delay(DeBounce_Time);// filter noise
    buttonState = digitalRead(PUSHBUTTON);// read PB again
    if (buttonState == LOW){
      Drive_3_Coils(number_of_sets);//imidiatly drive motor 
    }
    while (buttonState == LOW){
      buttonState = digitalRead(PUSHBUTTON);// wait untill button is release 
    }    
    delay(DeBounce_Time);// wait to avoid bounce during PB release 
  }

  Read_Pot();// read potenciometers and update puls and space time 
  if (millis() >= Current_Time+Update_Display_Time) {
  if(Use_Serial) {
    Print_Pulses_Parameters();
    }
  if(Use_Display) {
    Display_Pulses_Parameters();
    }  
  Current_Time = millis();     
  }

  delay(Main_Loop_Delay_Time);
}
