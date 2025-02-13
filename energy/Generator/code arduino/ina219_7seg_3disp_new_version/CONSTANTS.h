#ifndef CONSTANTS
#define CONSTANTS

#define SERIAL_BAUD_RATE (9600)
#define BCD_A (3) // Output to BCD_A (4511)
#define BCD_B (4) // Output to BCD_B (4511)
#define BCD_C (5) // Output to BCD_C (4511)
#define BCD_D (6) // Output to BCD_D (4511)

#define LE_CENTURIES (7) // Output Left (100th) digit LE (negative pulse) used as 10th
#define LE_DOZENS (8)
#define LE_UNITS (9) // Output right (10th) digit LE (negative pulse) used as units 


/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  to GND ina219
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  to 5V ina219
 *                  D2-|       |-A7  
 *           BCD A  D3-|       |-A6  
 *           BCD B  D4-|       |-A5  SCL ina219
 *           BCD C  D5-|       |-A4  SDA ina219
 *           BCD D  D6-|       |-A3
 *    LE_CENTURIES  D7-|       |-A2  
 *       LE_DOZENS  D8-|       |-A1
 *        LE_UNITS  D9-|       |-A0
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */


///////////////general variable////////////////
const int8_t VREF = 5;
const int16_t BIT_RESOLUTION = 1023;
///////////////////////////////////////////////

//////////////variable for 7seg///////////////
const int8_t  Number_OF_7SEG = 3;
const int8_t RIGH_DIGIT = 1;
const int8_t MIDDLE_DIGIT = 2;
const int8_t LEFT_DIGIT = 3;
int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel
///////////////////////////////////////////////

const int16_t ITERATION = 100;
const int16_t DELAY_AVG = 10;
const int16_t UNIT_CALIBRATION = 10;
const int32_t UNIT_POWER_MAX = 10;
const int32_t mW_to_W = 1000;

float power_W;
float avg;

#endif


