#ifndef FONCTION_H
#define FONCTION_H

#include <SPI.h>
#include <SD.h>

#define LED_BUTTON 2
#define MOTOR 3
#define LED_SD_CARD 4
#define BUTTON 5
#define MICRO_SW 6
#define CHIPSELECT 10  // Chip Select pin for the SD card module

/*
 *========== Arduino Nano Pinout ====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *      LED_BUTTON  D2-|       |-A7  
 *           MOTOR  D3-|       |-A6  
 *     LED_SD_CARD  D4-|       |-A5  
 *          BUTTON  D5-|       |-A4  
 *        MICRO_SW  D6-|       |-A3
 *                  D7-|       |-A2  
 *                  D8-|       |-A1
 *                  D9-|       |-A0
 *      CHIPSELECT D10-|       |-Ref
 *            MOSI D11-|       |-3.3V to SDcard  
 *            MISO D12-|       |-D13  SCK
 *                      --USB--        
 */

// Micro switch states
extern bool NC_MICRO_SWITCH;  // Normally closed (NC) micro switch → Connects to GND when unpressed
extern bool NO_MICRO_SWITCH;  // Normally open (NO) micro switch → Connects to VCC when unpressed

// Timing and communication settings
extern int8_t time_bouncing;           // Button debounce delay (ms)
extern unsigned long BAUDERATE;                   // Serial communication baud rate
extern unsigned long ACTIVATION_TIME;    // Time before the button can be pressed again (ms)
extern unsigned long RESET_TIME_SECURE;  // Safety timeout: stops motor if micro switch is stuck (ms)
extern int16_t MOTOR_DELAY;              // Delay to allow micro switch to release after motor stops (ms)
extern int16_t DELAY_BEFORE_SLEEP;       // Delay before entering low-power mode (ms)

// Variables for button press handling
extern bool check;               // Flag to detect button press
extern unsigned long time_start;  // Timer to disable button activation after use
extern unsigned long time_to_secure; // Timer for motor emergency stop if needed
extern bool flag_first_press;     // Flag to track the first button press after activation
extern bool flag_led_on;



void TURN_ON_MOTOR();// Function to turn on the motor
bool PRESS_BUTTON();// Function to detect and debounce button press
void IEC();// Function for Immediate Engine Cutoff (IEC) - Stops the motor immediately
void printMotorOffInfo();// Function to print motor shutdown information to the serial monitor
void BLINK_FLAG(unsigned int DELAY_BLINK);// Function to blink the LED as an indicator (e.g., for errors or warnings)
void logEvent(const char* message);// Function to log events to the SD card

#endif
