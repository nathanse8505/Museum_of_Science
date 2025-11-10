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


#define SENSOR A2


/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *                  D2-|       |-A7  
 *          BCD_A   D3-|       |-A6  
 *          BCD_B   D4-|       |-A5  
 *          BCD_C   D5-|       |-A4 
 *          BCD_D   D6-|       |-A3
 *    LE_CENTURIES  D7-|       |-A2  SENSOR
 *    LE_DOZENS     D8-|       |-A1,
 *    LE_UNITS      D9-|       |-A0, 
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */



const float meter = 3.14 * 0.08;

float timer_1 = 0.0;
float delta_time;
float rpm;
float velocity = 0;
float km_per_h = 0; 

int check = 0;
bool check_zero = HIGH;

int Digit_4_To_Display = 0;
int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel


#endif



