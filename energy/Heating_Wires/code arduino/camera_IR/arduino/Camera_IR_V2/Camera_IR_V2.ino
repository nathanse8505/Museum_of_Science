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

  for (int i = 0; i < OPTION_NUM; i++) {
    class_command_addr = CLASS_TO_READ[i];
    subclass_command_addr = SUBCLASS_TO_READ[i];
    SEND_AND_VALIDATE_COMMAND();        // Send read command and wait for valid response
    data_current[i] = data;             // Store value in current settings array
    data = DATA_READ;
  }

  // === Save individual current values for each parameter ===
  data_color_current      = data_current[0];
  data_contrast_current   = data_current[1];
  data_brightness_current = data_current[2];
  data_zoom_current       = data_current[3];

  // Debug: print initial values
  Serial.println(data_color_current, HEX);
  Serial.println(data_contrast_current, HEX);
  Serial.println(data_brightness_current, HEX);
  Serial.println(data_zoom_current, HEX);

  // === Switch to write mode ===
  r_w_flag = 0x00;

  Serial.println("Init complete");
}

void loop() {
  ////////////////// COLOR ////////////////////
  if (digitalRead(COLOR_SW) == LOW) {
    // Restore previous value when first pressed
    if (color_command) {
      data = data_color_current;
      color_command = false;
      Serial.println(data);
    }
    // Set target address and limits for color
    class_command_addr = 0x78;
    subclass_command_addr = 0x20;
    data_color_current = data;
    max_value_data = MAX_NUMBER_OF_COLOR;
    min_value_data = MIN_NUMBER_OF_COLOR;
  } else {
    color_command = true;
  }

  ////////////////// ZOOM ////////////////////
  if (digitalRead(ZOOM_SW) == LOW) {
    if (zoom_command) {
      data = data_zoom_current;
      zoom_command = false;
    }
    data_zoom_current = data;
    class_command_addr = 0x70;
    subclass_command_addr = 0x12;
    max_value_data = MAX_NUMBER_OF_ZOOM;
    min_value_data = MIN_NUMBER_OF_ZOOM;
  } else {
    zoom_command = true;
  }

  ////////////////// CONTRAST ////////////////////
  if (digitalRead(CONTRAST_SW) == LOW) {
    if (contrast_command) {
      data = data_contrast_current;
      contrast_command = false;
    }
    data_contrast_current = data;
    class_command_addr = 0x78;
    subclass_command_addr = 0x03;
    max_value_data = MAX_CONTRAST;
    min_value_data = MIN_CONTRAST;
  } else {
    contrast_command = true;
  }

  ////////////////// BRIGHTNESS ////////////////////
  if (digitalRead(BRIGHTNESS_SW) == LOW) {
    if (brightness_command) {
      data = data_brightness_current;
      brightness_command = false;
    }
    data_brightness_current = data;
    class_command_addr = 0x78;
    subclass_command_addr = 0x02;
    max_value_data = MAX_BRIGHTNESS;
    min_value_data = MIN_BRIGHTNESS;
  } else {
    brightness_command = true;
  }

  // === Process +/- buttons for selected parameter ===
  PRESS_PLUS_MINUS(PLUS_BUTTON, MINUS_BUTTON, max_value_data, min_value_data);

  ////////////////// CALIBRATION ////////////////////
  if (PRESS_BUTTON(CALIBRATION_BUTTON, &flag_calibration)) {
    class_command_addr = 0x7C;
    for (int i = 1; i < NUMBER_OF_CALIBRATION; i++) {
      subclass_command_addr = SUB_CLASS_0x7C[i];
      data = DATA_CALIBRATION;
      delay(100); // Let the camera process each calibration command
      SEND_AND_VALIDATE_COMMAND();
    }
  }

  ////////////////// SAVE ////////////////////
  if (PRESS_BUTTON(SAVE_BUTTON, &flag_save)) {
    class_command_addr = 0x74;
    subclass_command_addr = 0x10;
    data = DATA_SAVE;
    SEND_AND_VALIDATE_COMMAND();
  }

  ////////////////// RESET ////////////////////
  if (PRESS_BUTTON(RESET_BUTTON, &flag_reset)) {
    class_command_addr = 0x74;
    subclass_command_addr = 0x0F;
    data = DATA_RESET;
    SEND_AND_VALIDATE_COMMAND();
  }

  delay(1); // Small delay to reduce CPU usage
}
