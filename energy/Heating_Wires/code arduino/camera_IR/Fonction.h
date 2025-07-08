#ifndef FONCTION
#define FONCTION


#include "Const.h"

//#include <Adafruit_INA219.h>
#include <Wire.h>
#include <avr/wdt.h>


void ZOOM_IN(){
  long Zoom_in_X1 = 0xF0053670120000B8FF;
  long Zoom_in_X2 = 0xF0053670120001B9FF;
  long Zoom_in_X4 = 0xF0053670120002BAFF;
  long Zoom_in_X8 = 0xF0053670120003BAFF;
} 

byte CALCUL_CHK(byte data,byte class_addr,byte subclass_addr){
  int sum = DEVICE_ADDRESS + class_addr + subclass_addr +  R_W_FLAG + data;
  return sum & 0xFF;
}

void WRITE_COMMEND(byte data,byte class_addr,byte subclass_addr){

  byte checksum = CALCUL_CHK(data,class_addr,subclass_addr);

  Serial.write(BEGIN);
  Serial.write(SIZE);
  Serial.write(DEVICE_ADDRESS);
  Serial.write(class_addr);
  Serial.write(subclass_addr);
  Serial.write(R_W_FLAG);
  Serial.write(data);
  Serial.write(checksum);
  Serial.write(END);

  byte trame[9] = {BEGIN,SIZE,DEVICE_ADDRESS,class_addr,subclass_addr,R_W_FLAG,data,checksum,END};
  Serial.write(trame, sizeof(trame));

}

void READ_FEEDBACK_COMMEND(){

}


bool PRESS_BUTTON(int BUTTON_IO,bool flag_button) {

  // Check if the button is pressed
  if (digitalRead(BUTTON_IO) == LOW && flag_button == LOW) {
     //Serial.println("press :");
     flag_button = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(BUTTON_IO) == HIGH && flag_button == HIGH) {
    //Serial.println("unpress");
    flag_button = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}





#endif