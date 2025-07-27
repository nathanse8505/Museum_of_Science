#include "Fonction.h"
#include "Const.h"

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  camSerial.begin(SERIAL_BAUDRATE);

  // === Configure input pins with pull-up ===
  pinMode(COLOR_SW, INPUT_PULLUP);
  pinMode(ZOOM_SW, INPUT_PULLUP);
  pinMode(CONTRAST_SW, INPUT_PULLUP);
  pinMode(BRIGHTNESS_SW, INPUT_PULLUP);

  pinMode(PLUS_BUTTON, INPUT_PULLUP);
  pinMode(MINUS_BUTTON, INPUT_PULLUP);

  pinMode(SAVE_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  pinMode(CALIBRATION_BUTTON, INPUT_PULLUP);

  // LED to indicate data reception
  pinMode(LED_RECEIVE_DATA, OUTPUT);

  // === Read current configuration from camera ===
  r_w_flag = 0x01; // Set to read mode
  data = DATA_READ;
  delay(3000);

  for (int i = 0; i < OPTION_NUM; i++) {
    class_command_addr = Setting_OPTION[i].class_addr;
    subclass_command_addr = Setting_OPTION[i].subclass_addr;
    SEND_AND_VALIDATE_COMMAND();        // Send read command and wait for valid response
    Setting_OPTION[i].data_current = data;             // Store value in current settings array
    data = DATA_READ;
  }

  #ifdef MODE_TEST
   for (int i = 0; i < OPTION_NUM; i++) { 
    Serial.println("Setting: " + String(Setting_OPTION[i].name) + " = " + String(Setting_OPTION[i].data_current));
   }
   Serial.println("Init complete");
  #endif
 
  // === Switch to write mode ===
  r_w_flag = 0x00;
  wdt_enable(WDTO_4S);
}

void loop() {
  wdt_reset();
  ////////////////// OPTION ////////////////////
  for (int i = 0; i < OPTION_NUM; i++){
    if (digitalRead(Setting_OPTION[i].PIN) == LOW) {
      // Restore previous value when first pressed
      if (Setting_OPTION[i].command) {
        data = Setting_OPTION[i].data_current;
        Setting_OPTION[i].command = false;
        #ifdef MODE_TEST
          Serial.println("Setting: " + String(Setting_OPTION[i].name) + " = " + String(data));
        #endif
      }
      // Set target address and limits for color
      class_command_addr = Setting_OPTION[i].class_addr;
      subclass_command_addr = Setting_OPTION[i].subclass_addr;
      Setting_OPTION[i].data_current = data;
      max_value_data = Setting_OPTION[i].max_val;
      min_value_data = Setting_OPTION[i].min_val;
    } else {
      Setting_OPTION[i].command = true;
    }
    // === Process +/- buttons for selected parameter ===
    PRESS_PLUS_MINUS(PLUS_BUTTON, MINUS_BUTTON, max_value_data, min_value_data);
    delay(5); // Small delay to reduce CPU usage
  }
  
     //////////////////  BUTTON OPTION ////////////////////
  for(int i = 0; i < BUTTON_OPTION_NUM; i++){
    if (PRESS_BUTTON(Setting_BUTTON_OPTION[i].PIN, &Setting_BUTTON_OPTION[i].flag)) {
      class_command_addr = Setting_BUTTON_OPTION[i].class_addr;
      subclass_command_addr = Setting_BUTTON_OPTION[i].subclass_addr;
      data =  Setting_BUTTON_OPTION[i].data_current;
      SEND_AND_VALIDATE_COMMAND();
    }
     delay(5); // Small delay to reduce CPU usage
  }

 
  
}


