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
#define SW1 A2 
#define SW2 11
#define SW3 12
#define SW4 A0
#define SW5 A1

//SD card
#define CHIPSELECT 10

extern const int32_t Bauderate;
// Array of switch pins for easy iteration
extern const int SWITCH_PINS[];//add SW5
extern const int NUMBER_OF_SWITCH;

// Timing constants (in milliseconds)
extern const int16_t BOUNCE_TIME;        // Debounce delay for switches
extern const unsigned long  TIME_LED_ON;       // Duration LED stays on
extern const int16_t DELAY_AFTER_ON;  // Delay after shifting LED position
extern const unsigned long SET_DAY;
extern unsigned long timer_on;
extern unsigned long time_log;
extern const int16_t index_data_led_uv[];
extern const int16_t index_data_led_sw[];
// Global variables
extern int16_t out_data;                   // Current state of shift register outputs
extern int index_switch;                   // Current switch index in sequence
extern bool checkStates[] ;  // Array for switch debounce states
extern String logMessage ;



void init_shift_register();
void init_switch_state();
bool PRESS_BUTTON(uint8_t switchNumber);
void RESET_ALL(int data);
int LED_ON_UV(int data,int i);
int LED_ON_SW(int data,int i);
void logEvent(const char* message);
void LOG_PRESS_BUTTON(uint8_t switchNumber);





#endif