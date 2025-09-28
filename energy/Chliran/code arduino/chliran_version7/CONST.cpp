#include "FONCTION_and_CONST.h"

const int32_t Bauderate = 115200;
const int NUMBER_OF_SWITCH = 5;
// Array of switch pins for easy iteration
const int SWITCH_PINS[NUMBER_OF_SWITCH] = {SW1,SW2,SW3,SW4,SW5};//add SW5


// Timing constants (in milliseconds)
const int16_t BOUNCE_TIME = 100;        // Debounce delay for switches
const unsigned long  TIME_LED_ON = 3000;       // Duration LED stays on
const int16_t DELAY_AFTER_ON = 20;  // Delay after shifting LED position
const unsigned long SET_DAY = 3600000*5;
unsigned long timer_on = 0;
unsigned long timer_off = 0;
unsigned long time_log = 0;
const int16_t index_data_led_uv[NUMBER_OF_SWITCH] = {1,3,5,7,9};
const int16_t index_data_led_sw[NUMBER_OF_SWITCH] = {0,2,4,6,8};
// Global variables
int16_t out_data = 0;                   // Current state of shift register outputs
int index_switch = 0;                   // Current switch index in sequence
bool checkStates[NUMBER_OF_SWITCH];  // Array for switch debounce states


//log message
String logMessage;
