#include "FONCTION.h"

const int Bauderate = 9600;
// Array of switch pins for easy iteration
const int SWITCH_PINS[] = {SW1, SW2, SW3, SW4};//add SW5
const int NUMBER_OF_SWITCH = 4;

// Timing constants (in milliseconds)
const int16_t DELAY_BLINK = 300;
const int16_t BOUNCE_TIME = 100;        // Debounce delay for switches
const unsigned long  TIME_LED_ON = 90000;       // Duration LED stays on
const unsigned long  TIME_LED_OFF = 50;       // Duration LED stays off
const int16_t DELAY_AFTER_SHIFT = 100;  // Delay after shifting LED position
unsigned long timer_on = 0;
unsigned long timer_off = 0;

// Global variables
int16_t out_data = 0;                   // Current state of shift register outputs
int index_switch = 0;                   // Current switch index in sequence
bool checkSW[NUMBER_OF_SWITCH];         // Array to track active switch in sequence
bool checkStates[NUMBER_OF_SWITCH] ;  // Array for switch debounce states

//log message
String logMessage;
