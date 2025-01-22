#ifndef CONST
#define CONST

/*
*==================Arduino Nano pinout================ 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *        LatchPin  D2-|       |-A7  
 *        ClockPin  D3-|       |-A6  
 *     Data Serial  D4-|       |-A5 
 *   Output enable  D5-|       |-A4 
 *             SW1  D6-|       |-A3
 *             SW2  D7-|       |-A2
 *             SW3  D8-|       |-A1
 *             SW4  D9-|       |-A0  
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB-- 
 *===================================================       
 */

// Pin definitions for shift register
#define LatchPin 2        // ST_CP: Storage register clock input
#define ClockPin 3        // SH_CP: Shift register clock input
#define Data_Serial 4     // DS: Serial data input
#define Output_Enable 5   // OE: Output enable (active LOW)

// Pin definitions for switches
#define SW1 6
#define SW2 7
#define SW3 8
#define SW4 9
//#define SW5 10

// Array of switch pins for easy iteration
const int SWITCH_PINS[] = {SW1, SW2, SW3, SW4};//add SW5
const int NUMBER_OF_SWITCH = 4;

// Timing constants (in milliseconds)
const int16_t DELAY_BLINK = 300;
const int16_t BOUNCE_TIME = 100;        // Debounce delay for switches
const long int TIME_LED_ON = 30000;       // Duration LED stays on
const long int TIME_LED_OFF = 60000;       // Duration LED stays off
const int16_t DELAY_AFTER_SHIFT = 100;  // Delay after shifting LED position

// Global variables
int16_t out_data = 0;                   // Current state of shift register outputs
bool checkSW[NUMBER_OF_SWITCH];         // Array to track active switch in sequence
int index_switch = 0;                   // Current switch index in sequence
bool checkStates[NUMBER_OF_SWITCH] ;  // Array for switch debounce states

#endif