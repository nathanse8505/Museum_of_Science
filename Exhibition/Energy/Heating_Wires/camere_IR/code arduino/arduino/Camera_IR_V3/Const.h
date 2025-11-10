
////////////////////////////////
/*Read_and_Write_FLAG
        CONTENT    STATUS
      --------------------------------------
      |  0x00   | HOST WRITE               |
      |  0x01   | HOST READ                |
      |  0x03   | CAMERA FEEDBACK CORRECT  |
      |  0x04   | CAMERA FEEDBACK ERROR    |
      --------------------------------------
*/ 

/*
DATA for class 0x78 and subclass 0x20 
  color:
  0x00: White heat
  0x01: Black heat
  0x02: Fusion 1
  0x03: Rainbow
  0x04: Fusion 2
  0x05: Iron red 1
  0x06: Iron red 2
  0x07: Dark brown
  0x08: Color 1
  0x09: Color 2
  0x0A: Ice fire
  0x0B: Rain
  0x0C: Green heat
  0x0D: Red heat
  0x0E: Dark blue
*/
/*
DATA for class 0x78 and subclass 0x02,0x03
  0x03 contrast:0-100 in decimal (50 default)
  0x02 Brightness:0-100 in decimal (50 default)
*/

/*DATA for class 0x70 and subclass 0x11 
0x00: No mirroring.
0x01: Central mirroring.
0x02: Left and right mirroring.
0x03: Up and down mirroring.
*/
/*DATA for class 0x70 and subclass 0x12 
0x00: Zoom X1.
0x01: Zoom X2.
0x02: Zoom X4.
0x03: Zoom X8.
*/
/*DATA for class 0x7C and subclass 0x04 
0x00: Automatic control off
0x01: Automatic switching, timing control
0x02: Automatic switching, temperature difference control
0x03: Full-automatic control (Default)

/*DATA for class 0x74 and subclass 0x10,0x0F (writing only)
subclass 0x10: --> Data -->0x00   to save param
subclass 0x0F: --> Data -->0x00   to reset param to the default
*/

/*
const int CLASS[5] = {0x78,0x74,0x70,0x7C,0x7D};
const int SUB_CLASS_0x74[9] = {0x10,0x02,0x03,0x04,0x05,0x06,0x0B,0x0C,0x0F};//0x10 Save the modified parameters of the module.
const int SUB_CLASS_0x78[7] = {0x20,0x02,0x03,0x10,0x15,0x16,0x1A};//0x20 color, 0x03 contrast, 0x02 Brightness,0x10 improve image detail
const int SUB_CLASS_0x70[2] = {0x11,0x12};//0x11 Mirorring,0x12 Zoom
const int SUB_CLASS_0x7C[4] = {0x02,0x03,0x0C,0x04};//0x02 Calibration image ,0x03 Calibration backgroung,0x0C Vignetting Correction 
*/

#ifndef Const
#define Const

#define SERIAL_BAUDRATE (115200)
#define MODE_TEST



#define LED_RECEIVE_DATA     2
#define COLOR_SW             3
#define ZOOM_SW              4
#define CONTRAST_SW          5
#define BRIGHTNESS_SW        6
#define SAVE_BUTTON          7
#define PLUS_BUTTON          8
#define MINUS_BUTTON         9
#define TX_CAM               11
#define RX_CAM               12
#define RESET_BUTTON         A0
#define CALIBRATION_BUTTON   A1

/*
/*
*=================== Arduino Nano pinout =====================
 *                         _______
 *                    TXD-|       |-Vin 
 *                    RXD-|       |-Gnd  
 *                    RST-|       |-RST
 *                    GND-|       |-+5V  
 * LED_RECEIVE_DATA    D2-|       |-A7   
 * COLOR_SW            D3-|       |-A6   
 * ZOOM_SW             D4-|       |-A5 
 * CONTRAST_SW         D5-|       |-A4 
 * BRIGHTNESS_SW       D6-|       |-A3  
 * MINUS_BUTTON        D7-|       |-A2 
 * PLUS_BUTTON         D8-|       |-A1   
 * SAVE_BUTTON         D9-|       |-A0 CALIBRATION_BUTTON 
 * RX_CAM             D10-|       |-Ref 
 * TX_CAM             D11-|       |-3.3V    
 * RESET_BUTTON       D12-|       |-D13   
 *                         --USB--        
 */

// === FRAME STRUCTURE (Protocol framing) ===
const byte BEGIN           = 0xF0;  // Start byte of frame
const byte SIZE            = 0x05;  // Number of bytes (excluding BEGIN, checksum, END)
const byte DEVICE_ADDRESS  = 0x36;  // Address of the camera module

byte class_command_addr    = 0x78;  // Class address to send
byte subclass_command_addr = 0x20;  // Subclass address to send
byte r_w_flag              = 0x00;  // 0x00 = Write, 0x01 = Read
byte data                  = 0x00;  // Data byte to write or buffer to store received value
byte checksum              = 0x00;  // Checksum of the frame
const byte END             = 0xFF;  // End byte of frame

// ====== Value range boundaries (used by +/- button handling) ======
uint8_t max_value_data;
uint8_t min_value_data;
// === CONSTANTS ===
const int BOUNCE_TIME               = 50; // Debounce time in milliseconds
const uint8_t BUFFER_SIZE_SEND      = 9;  // Total frame size (BEGIN + SIZE + ... + END)
const uint8_t NUMBER_OF_BLINK       = 3;  // Number of LED blinks on success
const uint8_t OPTION_NUM            = 4;  // Number of configurable parameters (color, contrast, brightness, zoom)
const uint8_t BUTTON_OPTION_NUM     = 3;  // Number of configurable parameters (save, reset, calibration)
const uint8_t NUMBER_OF_CALIBRATION = 3;
const byte DATA_READ                = 0;  // Value used to read current settings

//======== GLOBAL =========
byte data_current[OPTION_NUM];   // Array holding current values
bool valid              = false; // Flag indicating if the last command was confirmed
uint8_t buffer_size     = 0;     // Actual number of bytes received
uint8_t index           = 0;     // Current byte index while reading
unsigned long last_read = 0;     // Timestamp of last byte received (used for timeout detection)

const int SUB_CLASS_0x7C[BUTTON_OPTION_NUM] = {0x02, 0x03, 0x0C};        // Subclasses used for calibration sequence


typedef struct {
  int PIN;
  byte class_addr;
  byte subclass_addr;
  uint8_t min_val;
  uint8_t max_val;
  byte data_current;
  bool command;
  const char* name;
}CameraSetting;


typedef struct {
  int PIN;
  byte class_addr;
  byte subclass_addr;
  bool flag;
  byte data_current;
  const char* name;
}CameraSetting_Button;

/*
CameraSetting Color_setting      = {COLOR_SW,      0x78, 0x20, 0,  15,  0, true, "COLOR"};
CameraSetting Contrast_setting   = {CONTRAST_SW,   0x78, 0x03, 0, 100, 50, true, "CONTRAST"};
CameraSetting Brightness_setting = {BRIGHTNESS_SW, 0x78, 0x02, 0, 100, 50, true, "BRIGHTNESS"};
CameraSetting Zoom_setting       = {ZOOM_SW,       0x70, 0x12, 0,   4,  0, true, "ZOOM"};
*/
CameraSetting Setting_OPTION[OPTION_NUM] = {{COLOR_SW,      0x78, 0x20, 0,  15,  0, true, "COLOR"},
                                            {ZOOM_SW,       0x70, 0x12, 0,   4,  0, true, "ZOOM"},
                                            {CONTRAST_SW,   0x78, 0x03, 0, 100, 50, true, "CONTRAST"},
                                            {BRIGHTNESS_SW, 0x78, 0x02, 0, 100, 50, true, "BRIGHTNESS"}
                                           };

/*
CameraSetting_Button Save_Setting        = {SAVE_BUTTON,        0x74, 0x10, false, 0, "SAVE"};
CameraSetting_Button Reset_Setting       = {RESET_BUTTON,       0x74, 0x0F, false, 0, "RESET"};
CameraSetting_Button Calibration_Setting = {CALIBRATION_BUTTON, 0x7C, 0x02, false, 0, "CALIBRATION"};
*/
CameraSetting_Button Setting_BUTTON_OPTION[BUTTON_OPTION_NUM] = {{SAVE_BUTTON,        0x74, 0x10, false, 0, "SAVE"},
                                                                 {RESET_BUTTON,       0x74, 0x0F, false, 0, "RESET"},
                                                                 {CALIBRATION_BUTTON, 0x7C, 0x02, false, 0, "CALIBRATION"}
                                                                };


#endif