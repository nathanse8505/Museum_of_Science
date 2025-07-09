#ifndef Const
#define Const


#define SERIAL_BAUDRATE (115200)
#define LED_OPTION_BUTTON    2
#define LED_START_END_BUTTON 3
#define START_END_BUTTON     4
#define COLOR_BUTTON         5
#define ZOOM_BUTTON          6
#define MINUS_BUTTON         7
#define PLUS_BUTTON          8
#define SAVE_BUTTON          9
#define RX_CAM               10
#define TX_CAM               11
#define RESET_BUTTON         12
#define OPTION_BUTTON        A0
#define CALIBRATION_BUTTON   A1

/*
/*
*==========Arduino Nano pinout====== 
 *                         _______
 *                    TXD-|       |-Vin 
 *                    RXD-|       |-Gnd  
 *                    RST-|       |-RST
 *                    GND-|       |-+5V  
 *   LED_OPTION_BUTTON D2-|       |-A7  
 *LED_START_END_BUTTON D3-|       |-A6  
 *    START_END_BUTTON D4-|       |-A5  
 *        COLOR_BUTTON D5-|       |-A4  
 *         ZOOM_BUTTON D6-|       |-A3  
 *        MINUS_BUTTON D7-|       |-A2  
 *         PLUS_BUTTON D8-|       |-A1 CALIBRATION_BUTTON 
 *         SAVE_BUTTON D9-|       |-A0 OPTION_BUTTON
 *             RX_CAM D10-|       |-Ref
 *             TX_CAM D11-|       |-3.3V   
 *       RESET_BUTTON D12-|       |-D13
 *                         --USB--        
 */

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
const int SUB_CLASS_0x7C[3] = {0x02,0x03,0x0C};
//////////trame sequence/////////
const byte BEGIN           = 0xF0;
const byte SIZE            = 0x05;//1 + 4 CONST_NUM_OF_BYTE is (Device Address,Class Address,Subclass Address,R/W Flag)
const byte DEVICE_ADDRESS  = 0x36;
byte class_commend_addr    = 0x78;
byte subclass_commend_addr = 0x20;
const byte  R_W_FLAG       = 0x00; //write to the camera
byte  data                 = 0x00;
byte  checksum             = 0x00;
const byte END             = 0xFF;



///////////button/////////
bool flag_start_end   = false;
bool flag_color       = false;
bool flag_zoom        = false;
bool flag_save        = false;
bool flag_reset       = false;
bool flag_option      = false;
bool flag_plus        = false;
bool flag_minus       = false;
bool flag_calibration = false;
const int BOUNCE_TIME = 50;

///////////state/////////
uint8_t start_end_state     = 0; //0 or 1 0 = END 1 = start
uint8_t option_state        = 0; //(0 or 1) 0 = CONTRAST , 1 = BRIGHTNESS
byte data_color_state       = 0;//(0 to 14)
byte data_zoom_state        = 0;//(0 to 3)
byte data_contrast_state    = 50;//(0 to 100)
byte data_brightness_state  = 50;//(0 to 100)
const byte DATA_SAVE        = 0;
const byte DATA_RESET       = 0;
const byte DATA_CALIBRATION = 0;



///////////CONST//////////
const uint8_t NUMBER_OF_CALIBRATION = 2;
const uint8_t NUMBER_OF_COLOR = 14;
const uint8_t NUMBER_OF_ZOOM  = 3;
const uint8_t MAX_BRIGHTNESS  = 100;
const uint8_t MIN_BRIGHTNESS  = 0;
const uint8_t MAX_CONTRAST    = 100;
const uint8_t MIN_CONTRAST    = 0;
const uint8_t TRAME_SIZE      = 9;// trame is 9 byte
const uint8_t BRITGHNESS      = 2;
const uint8_t CONTRAST        = 1;

////// SEND AND CONFIRM DATA //////
bool valid = false;

#endif