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
 *                  D6-|       |-A3
 *                  D7-|       |-A2
 *                  D8-|       |-A1 SW1
 *                  D9-|       |-A0 SW2 
 *                 D10-|       |-Ref
 *             SW3 D11-|       |-3.3V   
 *             SW4 D12-|       |-D13
 *                      --USB-- 
 *===================================================       
 */

// Pin definitions for shift register
#define LatchPin 4        // ST_CP: Storage register input
#define ClockPin 5        // SH_CP: Shift register clock input
#define Data_Serial 2     // DS: Serial data input
#define Output_Enable 3   // OE: Output enable (active LOW)

// Pin definitions for switches
#define SW1 6
#define SW2 7
#define SW3 8
#define SW4 9
//#define SW5 12

const int Bauderate = 9600;
// Array of switch pins for easy iteration
const int SWITCH_PINS[] = {SW1, SW2, SW3, SW4};//add SW5
const int NUMBER_OF_SWITCH = 4;

// Timing constants (in milliseconds)
const int16_t DELAY_BLINK = 300;
const int16_t BOUNCE_TIME = 100;        // Debounce delay for switches
const unsigned long  TIME_LED_ON = 900;       // Duration LED stays on
const unsigned long  TIME_LED_OFF = 0;       // Duration LED stays off
const int16_t DELAY_AFTER_SHIFT = 100;  // Delay after shifting LED position
unsigned long timer_on = 0;
unsigned long timer_off = 0;

// Global variables
int16_t out_data = 0;                   // Current state of shift register outputs
int index_switch = 0;                   // Current switch index in sequence
bool checkSW[NUMBER_OF_SWITCH];         // Array to track active switch in sequence
bool checkStates[NUMBER_OF_SWITCH] ;  // Array for switch debounce states

#endif