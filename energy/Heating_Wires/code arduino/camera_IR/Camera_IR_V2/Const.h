#ifndef Const
#define Const


#define SERIAL_BAUDRATE (115200)
#define LED_RECEIVE_DATA     2
#define COLOR_SW             3
#define ZOOM_SW              4
#define CONTRAST_SW          5
#define BRIGHTNESS_SW        6
#define MINUS_BUTTON         7
#define PLUS_BUTTON          8
#define SAVE_BUTTON          9
#define RX_CAM               10
#define TX_CAM               11
#define RESET_BUTTON         12
#define CALIBRATION_BUTTON   A0

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
const byte CLASS_TO_READ[4] = {0x78,0x78,0x78,0x70};
const byte SUBCLASS_TO_READ[4] = {0x20,0x03,0x02,0x12};

const int SUB_CLASS_0x7C[3] = {0x02,0x03,0x0C};
//////////trame sequence/////////
const byte BEGIN           = 0xF0;
const byte SIZE            = 0x05;//1 + 4 CONST_NUM_OF_BYTE is (Device Address,Class Address,Subclass Address,R/W Flag)
const byte DEVICE_ADDRESS  = 0x36;
byte class_commend_addr    = 0x78;
byte subclass_commend_addr = 0x20;
byte  r_w_flag             = 0x00; //read/write to the camera
byte  data                 = 0x00;
byte  checksum             = 0x00;
const byte END             = 0xFF;


///////////button/////////
bool flag_save        = false;
bool flag_reset       = false;
bool flag_plus        = false;
bool flag_minus       = false;
bool flag_calibration = false;
const int BOUNCE_TIME = 50;

///////////state/////////
const uint8_t OPTION_NUM = 4;
byte data_color_current      = 0;//(0 to 14)
byte data_zoom_current       = 0;//(0 to 3)
byte data_contrast_current   = 50;//(0 to 100)
byte data_brightness_current = 50;//(0 to 100) 50 default
byte data_current [OPTION_NUM];
bool color_command      = true;
bool zoom_command       = true;
bool contrast_command   = true;
bool brightness_command = true;


uint8_t max_value = 4;
uint8_t min_value = 0;

const byte DATA_SAVE        = 0;
const byte DATA_RESET       = 0;
const byte DATA_CALIBRATION = 0;
const byte DATA_READ        = 0;


///////////CONST//////////
const uint8_t NUMBER_OF_CALIBRATION = 2;
const uint8_t MAX_NUMBER_OF_COLOR   = 15;
const uint8_t MIN_NUMBER_OF_COLOR   = 0;
const uint8_t MAX_NUMBER_OF_ZOOM    = 4;
const uint8_t MIN_NUMBER_OF_ZOOM    = 0;
const uint8_t MAX_BRIGHTNESS        = 100;
const uint8_t MIN_BRIGHTNESS        = 0;
const uint8_t MAX_CONTRAST          = 100;
const uint8_t MIN_CONTRAST          = 0;



////// SEND AND CONFIRM DATA //////
bool valid   = false;
bool command = false;
const uint8_t TRAME_SIZE_SEND = 9;
const uint8_t NUMBER_OF_BLINK = 3;
uint8_t trame_size = 0;// trame is 9 byte
uint8_t index = 0;// trame is 9 byte

#endif