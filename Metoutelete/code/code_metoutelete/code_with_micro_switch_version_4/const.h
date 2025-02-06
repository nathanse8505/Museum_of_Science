#ifndef const
#define const

#define LED_BUTTON 2
#define MOTOR 3
#define BUTTON 5
#define MICRO_SW 6

/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *      LED_BUTTON  D2-|       |-A7  
 *           MOTOR  D3-|       |-A6  
 *                  D4-|       |-A5  
 *          BUTTON  D5-|       |-A4  
 *        MICRO_SW  D6-|       |-A3
 *                  D7-|       |-A2  
 *                  D8-|       |-A1
 *                  D9-|       |-A0
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */

bool NC_MICRO_SWITCH = LOW;//when the microswitch in normally close its in the gnd
bool NO_MICRO_SWITCH = HIGH;//when the microswitch in normally open its in the VCC

const int8_t time_bouncing = 50;
const int BAUDERATE = 9600;
const unsigned long ACTIVATION_TIME =30000;//ms after pushing the button the motor stop until activation time
const unsigned long RESET_TIME_SECURE = 5000;//if the micoswitch dosent work stop the motor and the arduino
const int16_t MOTOR_DELAY = 1000;//ms delay motor to unpress the microswitch
const int16_t DELAY_BEFORE_SLEEP = 1000;//ms delay before to enter in sleep mode



bool check = LOW;//variable for pushing the button
unsigned long time_start = 0;//start time to disable the button until activation time
unsigned long time_to_secure = 0;//start time to disable the arduino until stop_time_safety
bool flag_first_press = HIGH;



#endif

