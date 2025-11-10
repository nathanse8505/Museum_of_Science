#ifndef const
#define const
#include <Arduino.h>

/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 * ignition button  D2-|       |-A7  current input pot(not used)
 *                  D3-|       |-A6  input pot (not used)
 *     SPARK output D4-|       |-A5 
 *  RELAY_VALVE_IO  D5-|       |-A4 
 *RELAY_TURN_ON_OFF D6-|       |-A3
 *          FAN_IO  D7-|       |-A2   SENSOR_FIRE_IO
 *  LED_ACTIVATION  D8-|       |-A1   SENSOR_WATER_IO
 *                  D9-|       |-A0   Current sensor ACS712T-05
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *         Buzzer  D12-|       |-D13
 *                      --USB--        
 */
#define BAUDRATE (115200)
////////////  I/O   ////////////
//#define CURRENT_INPUT_IO A7 // simulation potentiometer
#define SENSOR_WATER_IO A3
#define RELAY_VALVE_WATER_IO 9


//////////// WATER ///////////
const int16_t DELAY_FILL_WATER = 500;
const int16_t DELAY_AFTER_FILL_WATER = 1000;
const bool NO_DETECTION = false;


#endif