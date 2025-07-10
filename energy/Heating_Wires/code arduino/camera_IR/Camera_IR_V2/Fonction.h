#ifndef FONCTION
#define FONCTION


#include "Const.h"

//#include <Adafruit_INA219.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>

SoftwareSerial camSerial(RX_CAM, TX_CAM); // RX, TX


void WRITE_COMMEND(){ 
  checksum = (DEVICE_ADDRESS + class_commend_addr +subclass_commend_addr + r_w_flag + data) & 0xFF;

  byte trame[TRAME_SIZE_SEND] = {BEGIN,SIZE,DEVICE_ADDRESS,class_commend_addr,subclass_commend_addr,r_w_flag,data,checksum,END};
  camSerial.write(trame, sizeof(trame));//send to camera
  Serial.print(">> transmiting trame :");
  for (int i = 0; i < TRAME_SIZE_SEND; i++) {
      if (trame[i] < 0x10) Serial.print("0");
      Serial.print(trame[i], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
}


bool CHECKSUM_receive(byte buffer[]) {
 
  // Vérifier que les octets de début et de fin sont corrects
  if (buffer[0] != BEGIN || buffer[trame_size - 1] != END) {
    Serial.println("❌ Erreur : BEGIN ou END invalide.");
    return false;
  }

  // Calcul du checksum attendu
  byte chk_calc = 0;
  for (int i = 2; i < trame_size - 2; i++) { // from DEVICE to DATA include
    chk_calc += buffer[i];
  }
  chk_calc &= 0xFF;
  byte chk_received = buffer[trame_size - 2];// Checksum reçeive

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

  static byte buffer[10];
  unsigned long last_read = millis();
  index = 0;
  trame_size = 0;// trame is 9 byte
 

  while (camSerial.available()) {
    byte b = camSerial.read();
    buffer[index] = b;
    last_read = millis(); // mise à jour
    trame_size++;
    index++;
    if (b == 0xFF) {
      break;
    }
    
  }


  // Timeout : si pas de données depuis 100 ms
  if (index > 0 && millis() - last_read > 200) {
    index = 0; // reset pour ne pas rester bloqué
    Serial.println("data not complete");
    return false;
  }

  // Quand la trame est complète
  if (index == trame_size) {
    Serial.print(">> Trame complète receive :");
    for (int i = 0; i < trame_size; i++) {
      if (buffer[i] < 0x10) Serial.print("0");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    index = 0; // Réinitialiser pour la trame suivante
    if (CHECKSUM_receive(buffer) == false) {return false;}
    if(r_w_flag == 0x01){
      data = buffer[trame_size-3];
    }
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

void SEND_AND_CHECK_COMMAND(){
  while(!valid){
      WRITE_COMMEND();
      delay(50);
      valid = READ_FEEDBACK_COMMEND();
      delay(50);
    }
    for(int i = 0;i < NUMBER_OF_BLINK; i++){
        digitalWrite(LED_RECEIVE_DATA,HIGH);
        delay(50);
        digitalWrite(LED_RECEIVE_DATA,LOW);
        delay(50);
    } 
    valid = false;
}

void PRESS_MINUS_PLUS(uint8_t max_val,uint8_t min_val){
  //if (PRESS_BUTTON(PLUS_BUTTON,flag_plus)){
  if (digitalRead(PLUS_BUTTON) == LOW){
    data = (data == (max_val - 1)) ?  (max_val - 1) : (data + 1);
    Serial.println(data);
    delay(30);
    SEND_AND_CHECK_COMMAND();
  } 
  //if (PRESS_BUTTON(MINUS_BUTTON,flag_minus)){
  if (digitalRead(MINUS_BUTTON) == LOW){
     Serial.println(data);
    data = data == min_val ? min_val : (data - 1);
    Serial.println(data);
    delay(30);
    SEND_AND_CHECK_COMMAND();
  }
}

void PRESS_CONTINUTELY_MINUS_PLUS(uint8_t max_val,uint8_t min_val){
  //if (PRESS_BUTTON(PLUS_BUTTON,flag_plus)){
  if (digitalRead(PLUS_BUTTON) == LOW){
    data = (data == (max_val - 1)) ?  (max_val - 1) : (data + 1);
    Serial.println(data);
    delay(30);
    SEND_AND_CHECK_COMMAND();
  } 
  //if (PRESS_BUTTON(MINUS_BUTTON,flag_minus)){
  if (digitalRead(MINUS_BUTTON) == LOW){
     Serial.println(data);
    data = data == min_val ? min_val : (data - 1);
    Serial.println(data);
    delay(30);
    SEND_AND_CHECK_COMMAND();
  }
}




#endif