#ifndef Const
#define Const


#define SERIAL_BAUD_RATE (115200)
#define BCD_A (3) // Output to BCD_A (4511)
#define BCD_B (4) // Output to BCD_B (4511)
#define BCD_C (5) // Output to BCD_C (4511)
#define BCD_D (6) // Output to BCD_D (4511)

#define LE_CENTURIES (7) // Output Left (100th) digit LE (negative pulse) used as 10th
#define LE_DOZENS (8)
#define LE_UNITS (9) // Output right (10th) digit LE (negative pulse) used as units 

#define ThermistorPin A3
#define LANG_BUTTON_IO 2
/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *  LANG_BUTTON_IO  D2-|       |-A7  
 *           BCD A  D3-|       |-A6  
 *           BCD B  D4-|       |-A5  
 *           BCD C  D5-|       |-A4  
 *           BCD D  D6-|       |-A3  ThermistorPin
 *    LE_CENTURIES  D7-|       |-A2  
 *       LE_DOZENS  D8-|       |-A1
 *        LE_UNITS  D9-|       |-A0
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */

//////////////variable for 7seg///////////////
const int8_t  Number_OF_7SEG = 3;
const int8_t RIGHT_DIGIT = 1;
const int8_t MIDDLE_DIGIT = 2;
const int8_t LEFT_DIGIT = 3;
int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel
///////////////////////////////////////////////

/////////////average//////////////////
const int AVG_WINDOW_SIZE  = 20;
float temperatureBuffer[AVG_WINDOW_SIZE];
int bufferIndex = 0;
bool bufferFull = false;


////////////language/////////
int const  BOUNCE_TIME = 100; //ms
uint8_t lang = 0;  // 0 - hebrew, 1 - english, 2 - arabic
bool check_lang = LOW;// used for checking if lang button was released
////////////////////////////////////

const float  R1 = 10000;
//const float A = 0.6097111451e-03, B = 3.133225362e-04, C = -1.811070168e-07; //
const float A = -2.425181458e-03, B =8.101790945e-04, C = -21.03109194e-07; //

int Vout;
float R2;
float temperature_C;

/////////serial com////////
unsigned long timer;
const unsigned long DATA_SER = 170;

#endif