#ifndef FUNCTION_H
#define FUNCTION_H

// === INCLUDES ===
#include "Const.h"
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <IRremote.h>


// === SOFTWARE SERIAL FOR CAMERA ===
SoftwareSerial camSerial(RX_SER, TX_SER); // RX, TX

// === FUNCTION DECLARATIONS ===
void SEND_COMMAND();
void PRINT_BUFFER(byte buffer[], uint8_t frame_size);
bool READ_FEEDBACK_COMMAND();
bool CHECKSUM_verify(byte buffer[]);
void SEND_AND_VALIDATE_COMMAND();
void PRESS_PLUS_MINUS(int i , uint8_t max_val, uint8_t min_val,long code_IR);
bool IS_BUTTON_PRESSED(int BUTTON_IO, bool button_flag);
void PRINT_SETTING(char* name,byte data_c);


// === SEND COMMAND TO CAMERA ===
void SEND_COMMAND() {
  // Checksum calculation:
  // Sum of DEVICE_ADDRESS + CLASS + SUBCLASS + R/W FLAG + DATA
  checksum = (DEVICE_ADDRESS + class_command_addr + subclass_command_addr + r_w_flag + data) & 0xFF;

  // Construct the transmission frame
  byte send_buffer[BUFFER_SIZE_SEND] = {
    BEGIN, SIZE, DEVICE_ADDRESS, class_command_addr,
    subclass_command_addr, r_w_flag, data, checksum, END
  };

  // Send the frame to the camera
  Serial.write(send_buffer, sizeof(send_buffer));

  // Debug output
  camSerial.write(">> Transmitting buffer: ");
  PRINT_BUFFER(send_buffer, BUFFER_SIZE_SEND);
}

// === PRINT A BUFFER TO SERIAL IN HEX FORMAT ===
void PRINT_BUFFER(byte buffer[], uint8_t frame_size) {
  for (int i = 0; i < frame_size; i++) {
    if (buffer[i] < 0x10) camSerial.write("0");
    camSerial.write(buffer[i]);
    camSerial.write(" ");
  }
  camSerial.write("\n");
}

void PRINT_SETTING(char* name,byte data_c){
    camSerial.write("Setting: ");
    camSerial.write(name);
    camSerial.write(" = ");
    camSerial.write(data_c);
    camSerial.write("\n");
}

// === READ AND PARSE RESPONSE FRAME FROM CAMERA ===
bool READ_FEEDBACK_COMMAND() {
  static byte buffer[10]; // Persistent buffer
  last_read = millis();
  index = 0;
  buffer_size = 0;

  // Read available bytes
  while ( Serial.available()) {
    byte b =  Serial.read();
    buffer[index++] = b;
    last_read = millis(); // update timestamp
    buffer_size++;
    if (b == 0xFF) { // End of frame
      break;
    }
  }

  // Timeout if no data received for 200 ms
  if (index > 0 && millis() - last_read > 200 || index > 10) {
    index = 0;
    camSerial.write("⚠️ Incomplete data: timeout \n");
    return false;
  }

  // If complete frame received
  if (index == buffer_size) {
    camSerial.write(">> Complete frame received: ");
    PRINT_BUFFER(buffer, buffer_size);
    index = 0;

    if (!CHECKSUM_verify(buffer)) return false;

    if (r_w_flag == 0x01) {
      data = buffer[buffer_size - 3]; // Extract data field
    }

    return true;
  }

  return false;
}

// === VERIFY CHECKSUM OF RECEIVED FRAME ===
bool CHECKSUM_verify(byte buffer[]) {
  // Validate start and end markers
  if (buffer[0] != BEGIN || buffer[buffer_size - 1] != END) {
    camSerial.write("❌ Error: Invalid BEGIN or END marker.\n");
    return false;
  }

  // Checksum calculation (excluding BEGIN, SIZE, CHECKSUM, END)
  byte checksum_calc = 0;
  for (int i = 2; i < buffer_size - 2; i++) {
    checksum_calc += buffer[i];
  }
  checksum_calc &= 0xFF;

  byte checksum_received = buffer[buffer_size - 2];

  // Compare calculated and received checksum
  if (checksum_calc == checksum_received) {
    camSerial.write("✅ CHECKSUM OK\n");
    return true;
  } else {
    camSerial.write("❌ CHECKSUM Mismatch: expected ");
    camSerial.write(checksum_calc);
    camSerial.write(", received ");
    camSerial.write(checksum_received);
    camSerial.write("\n");
    return false;
  }
}

// === LOOP UNTIL VALID RESPONSE RECEIVED ===
void SEND_AND_VALIDATE_COMMAND() {
  while (!valid) {
    wdt_reset();
    SEND_COMMAND();
    delay(50);
    valid = READ_FEEDBACK_COMMAND();
    delay(50);
  }

  // Blink LED to indicate success
  for (int i = 0; i < NUMBER_OF_BLINK; i++) {
    digitalWrite(LED_RECEIVE_DATA, HIGH);
    delay(50);
    digitalWrite(LED_RECEIVE_DATA, LOW);
    delay(50);
  }
  valid = false;
}

// === CHECK FOR BUTTON PRESS/RELEASE WITH DEBOUNCE ===
bool PRESS_BUTTON(int BUTTON_IO, bool *button_flag) {
  if (digitalRead(BUTTON_IO) == LOW && *button_flag == LOW) {
    *button_flag = HIGH;
    delay(BOUNCE_TIME);
  }

  if (digitalRead(BUTTON_IO) == HIGH && *button_flag == HIGH) {
    *button_flag = LOW;
    return HIGH;
  }

  return LOW;
}

// === HANDLE +/- BUTTONS TO MODIFY DATA VALUE AND SEND ===
void PRESS_PLUS_MINUS(int BUTTON_IO_PLUS, int BUTTON_IO_MINUS, uint8_t max_val, uint8_t min_val) {
  if (digitalRead(BUTTON_IO_PLUS) == LOW) {
    // Increment with upper limit
    data = (data >= (max_val - 1)) ? (max_val - 1) : (data + 1);
    Serial.println(data);
    delay(30);
    SEND_AND_VALIDATE_COMMAND();
    //Serial.print("Setting: " + String(Setting_OPTION[i].name) + " = " + String(data));
  }

  if (digitalRead(BUTTON_IO_MINUS) == LOW) {
    // Decrement with lower limit
    Serial.println(data);
    data = (data <= min_val) ? min_val : (data - 1);
    Serial.println(data);
    delay(30);
    SEND_AND_VALIDATE_COMMAND();
  }
}
#endif
