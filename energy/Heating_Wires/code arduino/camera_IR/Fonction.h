#ifndef FONCTION
#define FONCTION


#include "Const.h"

//#include <Adafruit_INA219.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <string.h>
SoftwareSerial camSerial(RX_CAM, TX_CAM); // RX, TX

byte CALCUL_CHK(byte data,byte class_addr,byte subclass_addr){
  int sum = DEVICE_ADDRESS + class_addr + subclass_addr +  R_W_FLAG + data;
  return sum & 0xFF;
}

void WRITE_COMMEND(byte data,byte class_addr,byte subclass_addr){

  checksum = CALCUL_CHK(data,class_addr,subclass_addr);
  byte trame[TRAME_SIZE] = {BEGIN,SIZE,DEVICE_ADDRESS,class_addr,subclass_addr,R_W_FLAG,data,checksum,END};
  camSerial.write(trame, sizeof(trame));//send to camera
  Serial.print(">> transmiting trame :");
  for (int i = 0; i < TRAME_SIZE; i++) {
      if (trame[i] < 0x10) Serial.print("0");
      Serial.print(trame[i], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
}

bool CHECKSUM_receive(byte buffer[TRAME_SIZE]) {
 
  // Vérifier que les octets de début et de fin sont corrects
  if (buffer[0] != BEGIN || buffer[TRAME_SIZE - 1] != END) {
    Serial.println("❌ Erreur : BEGIN ou END invalide.");
    return false;
  }

  // Calcul du checksum attendu
  byte chk_calc = 0;
  for (int i = 2; i < TRAME_SIZE - 2; i++) { // from DEVICE to DATA include
    chk_calc += buffer[i];
  }
  chk_calc &= 0xFF;
  byte chk_received = buffer[TRAME_SIZE - 2];// Checksum reçeive

  if (chk_calc == chk_received) {// Comparaison
    Serial.println("✅ CHECKSUM OK");
    return true;
  } 
  else {
    Serial.print("❌ CHECKSUM invalide : attendu ");
    Serial.print(chk_calc, HEX);
    Serial.print(", reçu ");
    Serial.println(chk_received, HEX);
    return false;
  }
}


bool READ_FEEDBACK_COMMEND(){

  static byte buffer[TRAME_SIZE];
  static byte index = 0;
  unsigned long last_read = millis();

  while (camSerial.available()) {
    byte b = camSerial.read();
    if (index < TRAME_SIZE) {
      buffer[index++] = b;
      last_read = millis(); // mise à jour
    }
  }

  // Timeout : si pas de données depuis 100 ms
  if (index > 0 && millis() - last_read > 200) {
    index = 0; // reset pour ne pas rester bloqué
    return false;
  }

  // Quand la trame est complète
  if (index == TRAME_SIZE) {
    Serial.print(">> Trame complète receive :");
    for (int i = 0; i < TRAME_SIZE; i++) {
      if (buffer[i] < 0x10) Serial.print("0");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    index = 0; // Réinitialiser pour la trame suivante
    if (CHECKSUM_receive(buffer) == false) {return false;}
    return true;
  }

  return false;
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

void SEND_AND_CHECK_COMMAND(byte data){
  while(!valid){
      WRITE_COMMEND(data,class_commend_addr,subclass_commend_addr);
      delay(50);
      valid = READ_FEEDBACK_COMMEND();
      delay(50);
    }
    valid = false;
}

byte PRESS_MINUS_PLUS(byte data_option_state,uint8_t MAX_VALUE,uint8_t MIN_VALUE){
  //if (PRESS_BUTTON(PLUS_BUTTON,flag_plus)){
    if (digitalRead(PLUS_BUTTON) == LOW){
    data_option_state = data_option_state == MAX_VALUE ?  MAX_VALUE : (data_option_state + 1);
    data = data_option_state;
    delay(30);
    SEND_AND_CHECK_COMMAND(data);
    
  }
  //if (PRESS_BUTTON(MINUS_BUTTON,flag_minus)){
  if (digitalRead(MINUS_BUTTON) == LOW){
    data_option_state = data_option_state == MIN_VALUE ? MIN_VALUE : (data_option_state - 1);
    data = data_option_state;
    delay(30);
    SEND_AND_CHECK_COMMAND(data);
  }
  return data_option_state;

}




#endif