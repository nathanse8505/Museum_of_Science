#ifndef basic_routine
#define basic_routine

#include <Arduino.h>
#include <avr/wdt.h>

/*
*=================Arduino Nano pinout==================
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *  ignition button D2-|       |-A7  
 *                  D3-|       |-A6  
 *   LED_ACTIVATION D4-|       |-A5 
 *                  D5-|       |-A4 
 *                  D6-|       |-A3
 * mode(auto/manale)D7-|       |-A2   
 *                  D8-|       |-A1   
 *                  D9-|       |-A0   
 *                 D10-|       |-Ref
 *  RELAY/SSR IO   D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */
#define BAUDRATE (115200)
// system stats (for states_flag byte)
const int8_t WAIT_FOR_BUTTON_PRESSED_STATE = 0; // after SSR/contactor switch off for minimu time. time limit, LED ON
const int8_t SSR_ON_STATE  = 1;  // during SSR active time, limit to TIME_RELAY_ON
const int8_t SSR_MIN_OFF_STATE = 2; // after SSR active, to make sure minimum capasitor chrging. limit to TIME_RELAY_ON, LED off
////////button mode ///////
const bool MANUAL = false;//


// timing constants
const int32_t SSR_ON_TIME = 2000 ;// 2 sec SSR on 
const int32_t SSR_MIN_OFF_TIME = 5000 ;// 5 sec SSR on 
const int32_t AUTO_CYCLE_TIME = 30000 ;// 30 sec 
// led blink constant
const int32_t LED_BLINK_SLOW = 1000 ;// 0.5Hz blinking led during 
const int32_t LED_BLINK_MID = 500 ;// 1Hz blinking led during 
const int32_t LED_BLINK_FAST = 100 ;// 5Hz blinking led during 

////////////  I/O   ////////////
#define BUTTON_IO       2
#define LED_ACTIVATION  4
#define MODE_IO         7  // relay active contactor 
#define RELAY_IO        11  // relay active contactor 
#define INTERNAL_LED    13  // 

// global vars
bool auto_sw_state = HIGH;// asumming not auto led off
bool button_sw_state = HIGH;// asumming not pressed
bool internal_led_state = LOW;// led off
int8_t current_state = 0;// see system stats

int32_t led_blink_timer = 0;
int32_t state_timer = 0;
int32_t time_start = 0;



void watchdogSetup(void)
{
cli(); // disable all interrupts
wdt_reset(); // reset the WDT timer
/*
WDTCSR configuration:
WDIE = 1: Interrupt Enable
WDE = 1 :Reset Enable
WDP3 = 0 :For 2000ms Time-out
WDP2 = 1 :For 2000ms Time-out
WDP1 = 1 :For 2000ms Time-out
WDP0 = 1 :For 2000ms Time-out
*/
// Enter Watchdog Configuration mode:
WDTCSR |= (1<<WDCE) | (1<<WDE);
// Set Watchdog settings:
//WDTCSR = (1<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);//original bote interapt and reset
WDTCSR = (0<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);// reset only 
sei();
}



#endif
