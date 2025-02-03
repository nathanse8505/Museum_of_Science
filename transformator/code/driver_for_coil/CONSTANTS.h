#ifndef CONSTANTS
#define CONSTANTS


#define SERIAL_BAUD_RATE (9600)
#define TIME_CLK A0            // Analog input for the total cycle duration
#define PULSE_POSITIVE A1      // Analog input for the positive pulse duration

#define N_DIRECTION 3          // Pin to control the negative direction
#define P_DIRECTION 5          // Pin to control the positive direction
#define LED_CONTROL 7          // Pin for the control LED


/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *                  D2-|       |-A7  
 *     N_DIRECTION  D3-|       |-A6  
 *                  D4-|       |-A5  
 *     P_DIRECTION  D5-|       |-A4  
 *                  D6-|       |-A3
 *     LED_CONTROL  D7-|       |-A2  
 *                  D8-|       |-A1  PULSE_POSITIVE (pot 1k)
 *                  D9-|       |-A0  TIME_CLK (pot 1k)
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */

float DELAY_RELAY = 20;         //delay between relays 
float cycle_max = 1000;        // Maximum cycle duration in milliseconds

float time_positive;           // Duration of the positive signal (in microseconds)
float cycle_time;              // Total cycle duration (in milliseconds)

#endif





