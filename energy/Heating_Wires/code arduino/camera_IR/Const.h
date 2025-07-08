#ifndef Const
#define Const


#define SERIAL_BAUDRATE (115200)
#define LED_START_END_BUTTON 3
#define START_END_BUTTON 4
#define COLOR_BUTTON 5
#define ZOOM_BUTTON 6
#define BRIGHTNESS_POT A0
#define CONTRAST_POT A1
#define SAVE_BUTTON 9
#define RESET_BUTTON 10

/*
/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *                  D2-|       |-A7  
 *           BCD A  D3-|       |-A6  
 *           BCD B  D4-|       |-A5  
 *           BCD C  D5-|       |-A4  
 *           BCD D  D6-|       |-A3  
 *    LE_CENTURIES  D7-|       |-A2  
 *       LE_DOZENS  D8-|       |-A1  
 *        LE_UNITS  D9-|       |-A0
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
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
/*DATA for class 0x74 and subclass 0x10,0x0F (writing only)
subclass 0x10: --> Data -->0x00   to save param
subclass 0x0F: --> Data -->0x00   to reset param to the default
*/

int N = 1;//number of DATA Byte to send
//const int CONST_NUM_OF_BYTE = 4;//Device Address,Class Address,Subclass Address,R/W Flag
const int CLASS[5] = {0x78,0x74,0x70,0x7C,0x7D};
const int SUB_CLASS_0x74[9] = {0x10,0x02,0x03,0x04,0x05,0x06,0x0B,0x0C,0x0F};//0x10 Save the modified parameters of the module.
const int SUB_CLASS_0x78[7] = {0x20,0x02,0x03,0x10,0x15,0x16,0x1A};//0x20 color, 0x03 contrast, 0x02 Brightness
const int SUB_CLASS_0x70[1] = {0x11};//0x11 Mirorring

const byte BEGIN = 0xF0;
const byte SIZE = N + 4;//4 CONST_NUM_OF_BYTE is (Device Address,Class Address,Subclass Address,R/W Flag)
const byte DEVICE_ADDRESS = 0x36;
byte CLASS_COMMEND_ADDRESS = 0x78;
byte SUBCLASS_COMMEND_ADDRESS = 0x20;
const byte  R_W_FLAG = 0x00; //write to the camera
byte  DATA = 0x00;
byte  CHK = 0x00;
const byte END = 0xFF;



///////////button//////
bool flag_start = false;
bool flag_color = false;
bool flag_zoom = false;
bool flag_save = false;
bool flag_reset = false;
const int BOUNCE_TIME = 50;


#endif