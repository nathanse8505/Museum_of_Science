#ifndef FONCTION
#define FONCTION

#include "Arduino.h"
#include "stdint.h"
#include <math.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <SD.h>

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
 *      CHIPSELECT D10-|       |-Ref
 *            MOSI D11-|       |-3.3V to SDcard  
 *            MISO D12-|       |-D13  SCK
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
//#define SW5 A0

//SD card
#define CHIPSELECT 10

extern const int Bauderate;
// Array of switch pins for easy iteration
extern const int SWITCH_PINS[];//add SW5
extern const int NUMBER_OF_SWITCH;

// Timing constants (in milliseconds)
extern const int16_t DELAY_BLINK;
extern const int16_t BOUNCE_TIME;        // Debounce delay for switches
extern const unsigned long  TIME_LED_ON;       // Duration LED stays on
extern const unsigned long  TIME_LED_OFF;       // Duration LED stays off
extern const int16_t DELAY_AFTER_SHIFT;  // Delay after shifting LED position
extern unsigned long timer_on;
extern unsigned long timer_off;

// Global variables
extern int16_t out_data;                   // Current state of shift register outputs
extern int index_switch;                   // Current switch index in sequence
extern bool checkSW[];         // Array to track active switch in sequence
extern bool checkStates[] ;  // Array for switch debounce states
extern String logMessage ;


void init_shift_register();
void init_switch_state();
bool PRESS_BUTTON(uint8_t switchNumber);
void RESET_ALL(int data);
int SET_FIRST_SW_LED();
int SHIFT_LEFT(int data);
void BLINK(int data);
void logEvent(const char* message);
void log_press_button();




#endif