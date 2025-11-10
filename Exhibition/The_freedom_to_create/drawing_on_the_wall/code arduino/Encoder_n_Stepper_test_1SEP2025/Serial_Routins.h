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
void Print_Encoders_Count(){
  Serial.print("Encoder A Count: ");
  Serial.print(Current_Encoder_A_Count);
  Serial.print(", Encoder B Count: ");
  Serial.print(Current_Encoder_B_Count);
  Serial.print(", Switch A ");
  Serial.print (SW_A > 0 ? "OFF" : "ON");
  Serial.print(", Switch B ");
  Serial.println (SW_B > 0 ? "OFF" : "ON");
}
//---------------------------

//---------print steps-----------
void print_step()
{
  char buffer [100];
  sprintf(buffer, "Steps[XY pulses]: %d,%d," , Current_Steps[0] , Current_Steps[1]);// for XYZ
  Serial.println(buffer);  
}










#endif /* SERIAL_ROUTINS_H */
