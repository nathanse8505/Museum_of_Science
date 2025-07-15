#include "Fonction.h"
#include "Const.h"

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  camSerial.begin(SERIAL_BAUDRATE);

  // === Configure input pins IR ===
  irrecv.enableIRIn(); // Démarre le récepteur IR
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

  for (int i = 0; i < OPTION_NUM; i++) {
    Serial.println("Setting: " + String(Setting_OPTION[i].name) + " = " + String(Setting_OPTION[i].data_current));
  }
  for (int i = 0; i < OPTION_NUM; i++) {
    Serial.println("Setting: " + String(Setting_BUTTON_OPTION[i].name) + " = " + String(Setting_BUTTON_OPTION[i].data_current));
  }

  // === Switch to write mode ===
  r_w_flag = 0x00;
  Serial.println("Init complete");
}

void loop() {
  ////////////////// OPTION ////////////////////
  long code_IR = RECEIVE_IR_CODE();  // Une seule fois par boucle
  if (code_IR != REPEAT && code_IR != -1) {
    Serial.print("Code IR: ");
    Serial.println(code_IR, HEX);
  }

  if (code_IR == -1) return;  // Rien reçu, ne fait rien

  for (int i = 0; i < OPTION_NUM; i++) {

    if (code_IR == REPEAT) {
      // Appui long détecté après un vrai code connu
      if ((last_valid_code == Setting_OPTION[i].CODE_IR_P || 
          last_valid_code == Setting_OPTION[i].CODE_IR_M)) {

        Serial.print("Appui long détecté pour le code : 0x");
        Serial.println(last_valid_code, HEX);

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
          Serial.println("Setting: " + String(Setting_OPTION[i].name) + " = " + String(data));
        }

        // Paramètres de la commande
        class_command_addr = Setting_OPTION[i].class_addr;
        subclass_command_addr = Setting_OPTION[i].subclass_addr;
        max_value_data = Setting_OPTION[i].max_val;
        min_value_data = Setting_OPTION[i].min_val;

        // Appui court → comportement ici si souhaité
        PRESS_PLUS_MINUS(i, max_value_data, min_value_data, code_IR);

        Serial.print("Appui court détecté pour le code : 0x");
        Serial.println(code_IR, HEX);
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
        Serial.println("Setting: " + String(Setting_BUTTON_OPTION[i].name) + " = " + String(data));
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


