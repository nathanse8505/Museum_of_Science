#ifndef SERIAL_ROUTINS_H
#define SERIAL_ROUTINS_H
//#include <Arduino.h>
#include "CONST_N_GLOBAL.h"
#define BAUDRATE    (115200)    // make sure set same in terminal(monitor) 

void Start_Serial(){
  Serial.begin(BAUDRATE);
  delay (500);// wait to make sure serial begin
  Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  Serial.println("START");
//  Serial.end();
}

//-------------------
void Print_Pulses_Parameters(){
  Serial.print("P2 = ");
  Serial.print(pot_2_value);
  Serial.print(", P3 = ");
  Serial.print(pot_3_value);
  Serial.print(", Pulse Time = ");
  Serial.print(pulse_time);
  Serial.print(", Space Time = ");
  Serial.print(space_time);
  Serial.print(", Number of sets = ");
  Serial.println(number_of_sets);

    
}
//---------------------------

//---------------------------






#endif /* SERIAL_ROUTINS_H */
