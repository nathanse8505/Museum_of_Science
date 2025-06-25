#ifndef CONST_N_GLOBAL_H
#define CONST_N_GLOBAL_H
#include <Arduino.h>
//#define BAUDRATE    (115200)    //

bool Use_Serial = false ; //  true/false print to serial (set monitor to same baudrate)
bool Use_Display = true ; // true/false  display on SH106 (make sure Display I2C address and DATA nad Clock conection)  

const int32_t MIN_PULSE_TIME = 0;//ms minimum pulse time 
const int32_t MAX_PULSE_TIME = 5;//ms maximum pulse time 
const int32_t MIN_SPACE_TIME = 0;//ms minimum space(between pulses)rime
//const int32_t MAX_SPACE_TIME = 300;//ms maximum space(between pulses)rime
//const int8_t  NUMBER_OF_3PULSES = 1; // number of pulses set (3 pulses each set) 
const int32_t  MIN_NUMBER_OF_SETS = 1; // minimun number of pulses set (4 pulses each set) 
const int32_t  MAX_NUMBER_OF_SETS = 100; // maximu, number of pulses set (4 pulses each set) 

const int32_t MIN_POT_VALUE = 0;// expected minimum A2D read from potentiometer, may be larger then 0 
const int32_t MAX_POT_VALUE = 1023;// expected maximum A2D (10 bits) read from potentiometer, may be smaler then 1023 

const uint32_t Update_Display_Time = 100;// ms - time between display update
const uint16_t Main_Loop_Delay_Time = 10;// ms - ! Critical value for encoder read pooling. May miss IO change if too small (and/or fast rotating) 

const uint16_t DeBounce_Time = 5;// ms, debouncing time 
// 
/*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd to Display  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V To Display
 *                  D2-|       |-A7 POT_1
 *                  D3-|       |-A6 POT_2
 *                  D4-|       |-A5 SCL (to Display) 
 *                  D5-|       |-A4 SDA (to Display)
 *                  D6-|       |-A3
 *                  D7-|       |-A2
 *      OUT_COIL_1  D8-|       |-A1
 *      OUT_COIL_2  D9-|       |-A0 pushbutton input (puuled up) 
 *      OUT_COIL_3 D10-|       |-Ref
 *      OUT_COIL_4 D11-|       |-3.3V   
 *                 D12-|       |-D13 internal led
 *                      --USB--          
 *                     
 */ 
#define PUSHBUTTON (A0) // potentiometer P2 set pulse time 
#define POT_2 (A6) // potentiometer P2 set pulse time 
#define POT_3 (A7) // potentiometer P3 set pulse space 
#define OUT_COIL_1 (8)// to drive coil 1 
#define OUT_COIL_2 (9)// to drive coil 2 
#define OUT_COIL_3 (10)// to drive coil 3
#define OUT_COIL_4 (11)// to drive coil 4
#define INTERNAL_LED (13)

int32_t pot_2_value = MIN_POT_VALUE;//value reads from pot 2
int32_t pot_3_value = MIN_POT_VALUE;//value reads from pot 3

int32_t pulse_time = MIN_PULSE_TIME;// [ms] current pulse time 
int32_t space_time = MIN_SPACE_TIME;// [ms] current pulse time 

int32_t number_of_sets = MIN_NUMBER_OF_SETS;// number of sets  


uint32_t Current_Time = 0   ;

byte buttonState = HIGH;// asume PB not preased

#endif /* CONSTANTS_H */
