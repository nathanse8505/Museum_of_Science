#ifndef CONST_N_GLOBAL_H
/*  IO map
 *  Encoder bits: BIT_0 - D2,   BIT_1 - D3 
 *  BCD bits: A-D3, B-D4, C-D5,  D-D6
 *  LE (
 *  Left digit (LE_100) LE (Negative Pulse) - D8; !! used as 10th digit
 *  Right digit (10th)LE (Negative Pulse) - D9 ; !! used as units 
 *  *==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *           SENS_2 D2-|       |-A7  pot 2/R2
 *            BCD_A D3-|       |-A6  pot 1/R1
 *            BCD_B D4-|       |-A5 ,SCL (to Display) 
 *            BCD_C D5-|       |-A4 ,SDA (to Display) 
 *            BCD_D D6-|       |-A3, SENS_3
 *           LE_100 D7-|       |-A2, SENS_2 Sense voltage 
 *            LE_10 D8-|       |-A1, SENS_1
 *             LE_1 D9-|       |-A0, SENS_0                                
 *          PWM_10 D10-|       |-Ref
 *          PWM_11 D11-|       |-3.3V   
 *           DP100 D12-|       |-D13 DP10
 *                      --USB--     
 */

// 7 segments parameters
const int MAX_NUM_TO_DISPLAY  = 999 ; //the maximum number to display
const int MIN_NUM_TO_DISPLAY  = 0 ; //the minimum number to display
/********************************/
//int const A_CONST = 1;// for ln correction 
//int const B_CONST = 27;// for ln correction 

int const map_table[ 3 ][ 4 ] = { {0,100,0,100},{ 101, 590,101 ,130}, { 591,1023,131,360 } };

int const Min_Read_V = 0; //  ) 10 bit A2D
int const Max_Read_V = 1023; //  10 bit A2D
int const Min_Show_V = 0; // 0  
int const Max_Show_V = 1023; // 

int SensorValue = 0;  // variable to store the value coming from the sensor
int Pressure_Value = 0;  // variable to store the calculated pressur (KP) 
int Number_To_Display = 0;// variable to store full number display
int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel

int const  READ_SENSOR_RATE = 100 ; //mSeconds
int const  DIGIT_TEST_RATE = 500 ; //mSeconds
int const  DISPLAY_TEST_RATE = 100 ; //mSeconds
int const  BLINK_ON = 800 ; //mSeconds
int const  BLINK_OFF = 200 ; //mSeconds
int const  BLINK_TIME = 5;// Seconds !!
int const  BOUNCE_TIME = 50; //ms


#define SERIAL_BAUD_RATE (115200)
#define Pressure_sensor_IO (A2) // pressure sensor input 
//#define POT_2 (A7) // pot input 
#define BCD_A (3) // Output to BCD_A (4511)
#define BCD_B (4) // Output to BCD_B (4511)
#define BCD_C (5) // Output to BCD_C (4511)
#define BCD_D (6) // Output to BCD_D (4511)

#define LE_100 (7) // Output Left (100th) digit LE (negative pulse) used as 100th
#define LE_10 (8) // Output center (10th) digit LE (negative pulse) used as 10th 
#define LE_1 (9) // Output right (1th) digit LE (negative pulse) used as units 


#endif /* CONST_N_GLOBAL_H */
