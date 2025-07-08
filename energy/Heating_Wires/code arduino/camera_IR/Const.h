#ifndef Const
#define Const


#define SERIAL_BAUDRATE (115200)

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
 *           BCD D  D6-|       |-A3  voltmeter after wire
 *    LE_CENTURIES  D7-|       |-A2  
 *       LE_DOZENS  D8-|       |-A1  voltmeter 24V
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


int N = 1;//number of DATA Byte
const int CONST_NUM_OF_BYTE = 4;//Device Address,Class Address,Subclass Address,R/W Flag
const int CLASS[5] = {0x78,0x74,0x70,0x7C,0x7D};
const int SUB_CLASS_0x74[9] = {0x10,0x2,0x3,0x4,0x5,0x6,0xB,0xC,0x0F};//0x10 Save the modified parameters of the module.
const int SUB_CLASS_0x78[7] = {0x20,0x2,0x3,0x10,0x15,0x16,0x1A};//0x20 color, 0x03 contrast, 0x02 Brightness
const int SUB_CLASS_0x70[1] = {0x11};//0x11 Mirorring

const byte BEGIN = 0xF0;
const byte SIZE = N + CONST_NUM_OF_BYTE;
const byte DEVICE_ADDRESS = 0x36;
byte CLASS_COMMEND_ADDRESS = 0x78;
byte SUBCLASS_COMMEND_ADDRESS = 0x20;
const byte  R_W_FLAG = 0x00; //write to the camera
byte  DATA = 0x00;
byte  CHK = 0x00;
const byte END = 0xFF;







#endif