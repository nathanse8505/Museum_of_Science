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

#pragma once

#define USE_POLAR_MODE true

#define ENABLE_SOFT_LIMIT true

#define DEBUG_MODE false
#define PRODUCTION_MODE true // disables buttons
#define ENCODER_DEBUG false

// STEPPERS CONFIGURATIONS
#define STEP_1_PIN 5
#define STEP_2_PIN 6
#define DIR_1_PIN 2
#define DIR_2_PIN 3
#define EN_PIN 8 // common enable pin for both motors.

// ENCODERS CONFIGURATIONS
#define ENCODER_A_BIT_1 (A0) // input IO for gray code bit 0 
#define ENCODER_A_BIT_0 (9) // input IO for gray code bit 1
#define ENCODER_B_BIT_1 (A1) // input IO for gray code bit 0
#define ENCODER_B_BIT_0 (12) // input IO for gray code bit 1
#define ENCODER_A_BUTTON (A2)
#define ENCODER_B_BUTTON (A3)
#define ENCODER_READ_INTERVAL 200 //microseconds

//UV LED CONFIGURATIONS
#define UV_PIN 7
#define UV_AUTO_TURN_OFF_TIME 3000 // ms
#define GO_TO_START_OF_MAZE 60000 // ms

struct Point { int x, y; };
