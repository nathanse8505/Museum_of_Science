#include "Fonction.h"
#include "Const.h"

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  camSerial.begin(SERIAL_BAUDRATE);

  
  // LED to indicate data reception
  pinMode(LED_RECEIVE_DATA, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // === Read current configuration from camera ===
  r_w_flag = 0x01; // Set to read mode
  data = DATA_READ;
  
  for (int i = 0; i < OPTION_NUM; i++) {
    class_command_addr = Setting_OPTION[i].class_addr;
    subclass_command_addr = Setting_OPTION[i].subclass_addr;
    SEND_AND_VALIDATE_COMMAND();        // Send read command and wait for valid response
    Setting_OPTION[i].data_current = data;             // Store value in current settings array
    data = DATA_READ;
  } 
  for (int i = 0; i < OPTION_NUM; i++) {
    class_command_addr =  Setting_BUTTON_OPTION[i].class_addr;
    subclass_command_addr =  Setting_BUTTON_OPTION[i].subclass_addr;
    SEND_AND_VALIDATE_COMMAND();        // Send read command and wait for valid response
    Setting_BUTTON_OPTION[i].data_current = data;             // Store value in current settings array
    data = DATA_READ;
  }

  // === Switch to write mode ===
  r_w_flag = 0x00;

  for (int i = 0; i < OPTION_NUM; i++) {
     PRINT_SETTING(Setting_OPTION[i].name,Setting_OPTION[i].data_current); 
  }
  for (int i = 0; i < OPTION_NUM; i++) {
    PRINT_SETTING(Setting_BUTTON_OPTION[i].name,Setting_BUTTON_OPTION[i].data_current);
  }

  // === Configure input pins IR ===
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  camSerial.write("Init complete\n");
}

void loop() {
  ////////////////// OPTION ////////////////////
  byte code_IR = RECEIVE_IR_CODE();  // Une seule fois par boucle
  //byte code_IR = -1;
  if (code_IR != REPEAT && code_IR != -1) {
    camSerial.write("Code IR: ");
    camSerial.write(code_IR);
    camSerial.write("\n");
  }
  if (code_IR == -1) return;  // Rien reçu, ne fait rien

  for (int i = 0; i < OPTION_NUM; i++) {

    if (code_IR == REPEAT) {
      // Appui long détecté après un vrai code connu
      if ((last_valid_code == Setting_OPTION[i].CODE_IR_P || 
          last_valid_code == Setting_OPTION[i].CODE_IR_M)) {

        camSerial.write("Appui long détecté pour le code : 0x");
        camSerial.write(last_valid_code);
        camSerial.write("\n");
        
        PRESS_PLUS_MINUS(i, max_value_data, min_value_data, last_valid_code);
        Setting_OPTION[i].data_current = data;
        delay(10);
      }

    } else {
      // Appui court ou début d’un appui long
      if (code_IR == Setting_OPTION[i].CODE_IR_P || code_IR == Setting_OPTION[i].CODE_IR_M) {
        last_valid_code = code_IR;
        time_first_press = millis();

         // Restaurer valeur initiale au premier appui
        if (Setting_OPTION[i].command) {
          data = Setting_OPTION[i].data_current;
          Setting_OPTION[i].command = false;

          PRINT_SETTING(Setting_OPTION[i].name,data); 
        }

        // Paramètres de la commande
        class_command_addr = Setting_OPTION[i].class_addr;
        subclass_command_addr = Setting_OPTION[i].subclass_addr;
        max_value_data = Setting_OPTION[i].max_val;
        min_value_data = Setting_OPTION[i].min_val;

        // Appui court → comportement ici si souhaité
        PRESS_PLUS_MINUS(i, max_value_data, min_value_data, code_IR);

        camSerial.write("Appui court détecté pour le code : 0x");
        camSerial.write(code_IR);
        camSerial.write("\n");
      }
    }

    Setting_OPTION[i].command = true;
    delay(5);
  }

  
     //////////////////  BUTTON OPTION ////////////////////
  for(int i = 0; i < BUTTON_OPTION_NUM; i++){
    if (Setting_BUTTON_OPTION[i].CODE_IR == code_IR) {
      // Restore previous value when first pressed
      if (Setting_BUTTON_OPTION[i].command) {
        data = Setting_BUTTON_OPTION[i].data_current;
        Setting_BUTTON_OPTION[i].command = false;

        PRINT_SETTING(Setting_BUTTON_OPTION[i].name,data); 
      }
      // Set target address and limits for color
      class_command_addr = Setting_BUTTON_OPTION[i].class_addr;
      subclass_command_addr = Setting_BUTTON_OPTION[i].subclass_addr;
      max_value_data = Setting_BUTTON_OPTION[i].max_val;
      min_value_data = Setting_BUTTON_OPTION[i].min_val;
      data = (data >= (max_value_data - 1)) ? min_value_data : (data + 1);
      Setting_BUTTON_OPTION[i].data_current = data;

      SEND_AND_VALIDATE_COMMAND();
    } else {
      Setting_BUTTON_OPTION[i].command = true;
    }
     delay(5); // Small delay to reduce CPU usage
  }
  
}


