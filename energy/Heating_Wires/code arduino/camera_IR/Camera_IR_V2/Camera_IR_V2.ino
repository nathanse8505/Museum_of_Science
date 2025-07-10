#include "Fonction.h"
#include "Const.h"


void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  camSerial.begin(SERIAL_BAUDRATE);

 
  pinMode(COLOR_SW,INPUT_PULLUP);
  pinMode(ZOOM_SW,INPUT_PULLUP);
  pinMode(CONTRAST_SW,INPUT_PULLUP);
  pinMode(BRIGHTNESS_SW,INPUT_PULLUP);

  pinMode(PLUS_BUTTON,INPUT_PULLUP);
  pinMode(MINUS_BUTTON,INPUT_PULLUP);

  pinMode(SAVE_BUTTON,INPUT_PULLUP);
  pinMode(RESET_BUTTON,INPUT_PULLUP);

  pinMode(CALIBRATION_BUTTON,INPUT_PULLUP);

  pinMode(LED_RECEIVE_DATA,OUTPUT);

  r_w_flag = 0x01;//read the camera
  data =  DATA_READ;
  
  for(int i = 0 ;i < OPTION_NUM;i++){
   class_commend_addr = CLASS_TO_READ[i];
   subclass_commend_addr = SUBCLASS_TO_READ[i];
   SEND_AND_CHECK_COMMAND();
   data_current[i] = data;
   data =  DATA_READ;
  }

  data_color_current      = data_current[0];
  data_contrast_current   = data_current[1];
  data_brightness_current = data_current[2];
  data_zoom_current       = data_current[3];
  Serial.println(data_color_current,HEX);
  Serial.println(data_contrast_current,HEX);
  Serial.println(data_brightness_current,HEX);
  Serial.println(data_zoom_current,HEX);



  r_w_flag = 0x00;//write to the camera

  Serial.println("init");
}
void loop() {
  
    ////////////////// COLOR ////////////////////
    if (digitalRead(COLOR_SW) == LOW){
      //Serial.println(data,HEX);
      if(color_command){
        data = data_color_current;
        color_command = false;
         Serial.println(data);
      }
      class_commend_addr = 0x78;
      subclass_commend_addr = 0x20;
      data_color_current = data;
      max_value = MAX_NUMBER_OF_COLOR;
      min_value = MIN_NUMBER_OF_COLOR;
      
    }
    else{
      color_command = true;  
    }
    
    ////////////////// ZOOM ////////////////////
    if (digitalRead(ZOOM_SW) == LOW){
      if(zoom_command){
        data = data_zoom_current;
        zoom_command = false;
      }
      data_zoom_current = data;
      class_commend_addr = 0x70;
      subclass_commend_addr = 0x12;
      max_value = MAX_NUMBER_OF_ZOOM;
      min_value = MIN_NUMBER_OF_ZOOM;
    }
    else{
      zoom_command = true;  
    }
   
    ////////////////CONTRAST /////////////////////

    if(digitalRead(CONTRAST_SW) == LOW){
      if(contrast_command){
        data = data_contrast_current;
        contrast_command = false;
      }
      data_contrast_current = data;
      class_commend_addr = 0x78;
      subclass_commend_addr = 0x03;
      max_value = MAX_CONTRAST;
      min_value = MIN_CONTRAST;
    }
    else{
      contrast_command = true;  
    }

    ///////////////////BRIGHTNESS ///////////////
    if(digitalRead(BRIGHTNESS_SW) == LOW){ 
      if(brightness_command){
        data = data_brightness_current;
        brightness_command = false;
      }
      data_brightness_current = data;
      class_commend_addr = 0x78;
      subclass_commend_addr = 0x02;
      max_value = MAX_BRIGHTNESS;
      min_value = MIN_BRIGHTNESS;
      command = true;
    }
    else{
      brightness_command = true;  
    }

    PRESS_MINUS_PLUS(max_value,min_value);
    

    ////////////////// CALIBRATION ////////////////////
    if (PRESS_BUTTON(CALIBRATION_BUTTON,flag_calibration)){
      class_commend_addr = 0x7C;
      for(int i = 1; i < NUMBER_OF_CALIBRATION; i++){
        subclass_commend_addr = SUB_CLASS_0x7C[i];
        data = DATA_CALIBRATION;
        delay(100);
        SEND_AND_CHECK_COMMAND();
      }
    
    }
     ////////////////// SAVE ////////////////////
    if (PRESS_BUTTON(SAVE_BUTTON,flag_save)){
      class_commend_addr = 0x74;
      subclass_commend_addr = 0x10;
      data = DATA_SAVE;
      SEND_AND_CHECK_COMMAND();
    }
    ////////////////// RESET ////////////////////
    if (PRESS_BUTTON(RESET_BUTTON,flag_reset)){
      class_commend_addr = 0x74;
      subclass_commend_addr = 0x0F;
      data = DATA_RESET;
      SEND_AND_CHECK_COMMAND();
    }
    delay(1);
 

}
  