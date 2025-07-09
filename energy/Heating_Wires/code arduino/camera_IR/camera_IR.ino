#include "Fonction.h"
#include "Const.h"


void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  camSerial.begin(SERIAL_BAUDRATE);

  pinMode(LED_START_END_BUTTON,OUTPUT);
  pinMode(START_END_BUTTON,INPUT_PULLUP);
  
  pinMode(COLOR_BUTTON,INPUT_PULLUP);
  pinMode(ZOOM_BUTTON,INPUT_PULLUP);
  pinMode(SAVE_BUTTON,INPUT_PULLUP);
  pinMode(RESET_BUTTON,INPUT_PULLUP);
  pinMode(CALIBRATION_BUTTON,INPUT_PULLUP);

  pinMode(LED_OPTION_BUTTON,OUTPUT);
  pinMode(OPTION_BUTTON,INPUT_PULLUP);
  pinMode(PLUS_BUTTON,INPUT_PULLUP);
  pinMode(MINUS_BUTTON,INPUT_PULLUP);

  Serial.println("init");
}
void loop() {

  if (PRESS_BUTTON(START_END_BUTTON,flag_start_end)){
    start_end_state = start_end_state == 0 ? 1 : 0;
    digitalWrite(LED_START_END_BUTTON,start_end_state);
  }

  if (start_end_state){
    ////////////////// COLOR ////////////////////
    if (PRESS_BUTTON(COLOR_BUTTON,flag_color)){
      data_color_state = data_color_state < NUMBER_OF_COLOR ? (data_color_state + 1) : 0;
      Serial.println(data_color_state,HEX);
      class_commend_addr = 0x78;
      subclass_commend_addr = 0x20;
      data = data_color_state;
      SEND_AND_CHECK_COMMAND(data);
    }
    ////////////////// CALIBRATION ////////////////////
    if (PRESS_BUTTON(CALIBRATION_BUTTON,flag_calibration)){
      class_commend_addr = 0x7C;
      for(int i = 0; i < NUMBER_OF_CALIBRATION; i++){
        subclass_commend_addr = SUB_CLASS_0x7C[i];
        data = DATA_CALIBRATION;
        SEND_AND_CHECK_COMMAND(data);
        delay(100);
      }
    
    }
    ////////////////// ZOOM ////////////////////
    if (PRESS_BUTTON(ZOOM_BUTTON,flag_zoom)){
      data_zoom_state = data_zoom_state < NUMBER_OF_ZOOM ?  (data_zoom_state + 1) : 0 ;
      Serial.println(data_zoom_state,HEX);
      class_commend_addr = 0x70;
      subclass_commend_addr = 0x12;
      data = data_zoom_state;
      SEND_AND_CHECK_COMMAND(data);
    }
    ////////////////// SAVE ////////////////////
    if (PRESS_BUTTON(SAVE_BUTTON,flag_save)){
      class_commend_addr = 0x74;
      subclass_commend_addr = 0x10;
      data = DATA_SAVE;
      SEND_AND_CHECK_COMMAND(data);
    }
    ////////////////// RESET ////////////////////
    if (PRESS_BUTTON(RESET_BUTTON,flag_reset)){
      class_commend_addr = 0x74;
      subclass_commend_addr = 0x0F;
      data = DATA_RESET;
      SEND_AND_CHECK_COMMAND(data);
    }
    ////////////////CONTRAST & BRIGHTNESS ///////////////
    if (PRESS_BUTTON(OPTION_BUTTON,flag_option)){
      option_state = option_state > 2 ? 0 : option_state + 1;
      digitalWrite(LED_OPTION_BUTTON,option_state);
    }

    if(option_state == CONTRAST){
      digitalWrite(LED_OPTION_BUTTON,HIGH);
      class_commend_addr = 0x78;
      subclass_commend_addr = 0x03;
      data_contrast_state = PRESS_MINUS_PLUS(data_contrast_state,MAX_CONTRAST,MIN_CONTRAST);
    }
    else if(option_state == BRITGHNESS){ 
      digitalWrite(LED_OPTION_BUTTON,HIGH);
      class_commend_addr = 0x78;
      subclass_commend_addr = 0x02;
      data_brightness_state = PRESS_MINUS_PLUS(data_brightness_state ,MAX_BRIGHTNESS,MIN_BRIGHTNESS);
    }
    else{
      digitalWrite(LED_OPTION_BUTTON,LOW);
    }  
    delay(1);
  }
 

}
  