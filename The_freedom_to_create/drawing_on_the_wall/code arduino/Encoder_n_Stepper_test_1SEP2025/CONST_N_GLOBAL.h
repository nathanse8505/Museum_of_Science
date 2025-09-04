#ifndef CONST_N_GLOBAL_H
#define CONST_N_GLOBAL_H
#include <Arduino.h>
//#define BAUDRATE    (115200)    //

bool Use_Serial = true ; ; //  print to serial (set monitor to same boudrate)
bool Use_Display = true ; // display on SH106 (make sure Display I2C address and DATA nad Clock conection)  

const uint32_t Update_Display_Time = 100;// ms - time between display update
const uint16_t Main_Loop_Delay_Time = 1;// ms - ! Critical value for encoder read pulling. May miss IO change if too small (and/or fast rotating) 

// 
/*==========Arduino Nano pinout====== 
 * IO map see J:\Amir Design\PCB_Design\EASYEDA JLBPCB\projects\SHIELD_V4\Shield_Amir_Shalev_V_0_0
 * Encoder A - Left, B-right 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd to Display  
 *                 RST-|       |-RST
 *  To Encoders    GND-|       |-+5V To Display
 *                  D2-|       |-A7
 *                  D3-|       |-A6
 *                  D4-|       |-A5 SCL (to Display) 
 *                  D5-|       |-A4 SDA (to Display)
 *                  D6-|       |-A3 (D17) Encoder B Switch 
 *                  D7-|       |-A2 (D16) Encoder A Switch
 *                  D8-|       |-A1 (D15) Encoder B bit 0
 *  Encoder A bit 1 D9-|       |-A0 (D14) Encoder A bit 0 , mark Abort 
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 * Encoder B bit 1 D12-|       |-D13
 *                      --USB--          
 *                     
 * ! Nano can use only D2, D3 as interupt -  encoder B must read by pulling                      
 * Mechanical encoder like https://www.aliexpress.com/item/1005005239756119.html 
 */ 

// servo motor 
#define SERVO_CONTROL_IO (13) // control (PWM) to left servo

uint16_t const  UP_SERVO = 0 ; // servo angle when pan up 
uint16_t const  DN_SERVO = 90 ; // servo angle when pan down 
 
// motors consts and globals 
// X is driver 0, Y is 1
const int8_t NUMBER_OF_MOTORS = 2;//X, Y !! make sure not 3 as Z step io used for UV led 
const int8_t NUMBER_OF_MOVES = 4;//X+, X-, Y+, Y-
//==================IO pin define =======================
const uint8_t DIR_PIN[NUMBER_OF_MOTORS] = {2, 3}; // original on V4 Shield used also on Amir Shields
const uint8_t STEP_PIN[NUMBER_OF_MOTORS] = {5, 6}; // original on V4 Shield used also on Amir Shields
#define STEPPER_EN_PIN 8 // invert - Hige is disable, low enable this is for both motors

#define SPEED_POT_INPUT_PIN A6 // analog read IO mark A6 on shield 
#define POT_INPUT_PIN A7 // TBD what will be used, maybe laser or motor power 
#define LED_OUT 13// also internal led

//timming/rate parameters
//const int8_t BOUNCE_TIME = 20;//ms
const uint16_t DIRECTION_CHANGE_WAIT_TIME = 5; //[ms] time wait/stop move between change direction to avoid loos steps
const uint16_t MIN_RATE = 3; //ms - minimum time between steps pulses - faster speed
const uint16_t MAX_RATE = 200; //ms - maximum, time between steps pulses - slower speed
const uint16_t HOMMING_RATE  = MIN_RATE ; //ms -
//----- variable------------------

//bool Is_destination_done = true;
bool Is_Run_Command = false ; // true if system running/moving acording to serial port command 
int16_t Current_Steps[NUMBER_OF_MOTORS] = {0, 0}; //X, Y only 
int16_t MIN_Steps [NUMBER_OF_MOTORS] = {-2000, -2000};// for testing later set to 0 
int16_t MAX_Steps [NUMBER_OF_MOTORS] = {2000, 2000};// TBD acording to system
int16_t HOMMING_Steps [NUMBER_OF_MOTORS] = {1000, 1000}; // TBD acording to system 825, 825 small laser engrave for the exhibit 

float mm_per_pulse [NUMBER_OF_MOTORS]= {80.0/820,80.0/820}; //small laser engrave for the exhibit 
//(115$  https://www.aliexpress.com/item/1005001708922115.html17/5/2016 last order 20 SEP 2023 Yossi) CD(?)  linear actuator 

uint16_t rate = MIN_RATE; // register hold rate (read from potentiometer)
bool is_PBs_Preased = false ; // for enable/disable motoers if no PB plreased
// array for each parameter one variable for each motor (usealy only 3)
bool motor_direction[NUMBER_OF_MOTORS] = {false, false}; // initialy asume arbitary direction - false count pulses down
uint16_t XY_rates [NUMBER_OF_MOVES] = {MIN_RATE, MIN_RATE, MIN_RATE, MIN_RATE}; // initial value same rates
uint16_t XY_homming_rates [NUMBER_OF_MOVES] = {HOMMING_RATE, HOMMING_RATE, HOMMING_RATE, HOMMING_RATE}; // initial value same rates

//Encoders consts and globals 
#define UV_LED (7) // output for uv led/laser (originaly Z axes motor step 
#define ENCODER_A_BIT_1 (9) // input IO for gray code bit 0 
#define ENCODER_A_BIT_0 (14) // A0 input IO for gray code bit 1
#define ENCODER_B_BIT_1 (12) // input IO for gray code bit 0
#define ENCODER_B_BIT_0 (15) // A1 input IO for gray code bit 1
// cross bits(0-1) for change counting direction (CW<>CCW)
#define ENCODER_A_SW (16) // A2  
#define ENCODER_B_SW (17) // A3  
//=====================
byte Old_Encoder_A_Read = 0; // old 2 bits read from first encoder 
byte New_Encoder_A_Read = 0; // new 2 bits read from first encoder 
int16_t Current_Encoder_A_Count = 0;// serial_printed and/or display 
int16_t Old_Encoder_A_Count = 0;// for later use
byte Old_Encoder_B_Read = 0; // old 2 bits read from second encoder 
byte New_Encoder_B_Read = 0; // new 2 bits read from second encoder 
int16_t Current_Encoder_B_Count = 0;// serial_printed and/or display 
int16_t Old_Encoder_B_Count = 0;// for later use
byte SW_A = 0; // current reading of switch A
byte SW_B = 0; // current reading of switch B
byte OLD_SW_A = 0; // last reading of switch A
byte OLD_SW_B = 0; // last reading of switch B
uint32_t Current_Time = 0   ;


#endif /* CONSTANTS_H */
