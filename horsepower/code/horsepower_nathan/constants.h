#ifndef CONSTANTS_H
#define CONSTANTS_H

#define LANG_BUTTON_IO  2 // Pin connected to the button


/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  gnd to sensor
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  5v to sensor
 * SPACE_BUTTON_IO  D2-|       |-A7  
 *                  D3-|       |-A6  
 *                  D4-|       |-A5  SCL
 *                  D5-|       |-A4  SDA
 *                  D6-|       |-A3  
 *                  D7-|       |-A2  
 *                  D8-|       |-A1
 *                  D9-|       |-A0  
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */

/////////////button space///////////
const int BOUNCE_TIME = 50;
bool check_lang = false;
int8_t lang =0;
///////////////////////////////////

// Physics-related constants
const float WEIGHT_KG_BALL = 7.5;         // Weight in kilograms
const float WEIGHT_KG_POWER_HORSE = 75;
const float GRAVITY = 9.81;         // Acceleration due to gravity in m/s^2
const float HORSEPOWER_CONVERSION = 745.7; // Conversion factor Watt to horsepower
//const float Force = WEIGHT_KG * GRAVITY;

// Sensor-related constants
const float THRESHOLD = 100;       // Threshold to detect lift movement (meters)



float smoothedDistance = 0.0; // Smoothed sensor reading
const int ITERATION = 5; // Number of readings for smoothing
const int32_t BAUDERATE = 115200;
float initialDistance = 0.0;   // Starting distance (baseline)

bool Lift_in_motion = false;    // Tracks if the lift is in motion
bool first_try = true;

unsigned long startTime = 0;  // Time when the lift starts moving
unsigned long deltaTime = 0;    // Time when the lift reaches the topmost point

float minDistance = 0.0;      // Tracks the minimum distance to the groune during the lift in meter
const float maxDistance = 1000;         // max distance to the ground in meter
float horsepower = 0;

unsigned long timer_rst = 0;
const unsigned int RST_DISP= 20000;
const unsigned int RST_BOUNCING_BALL= 4000;





#endif
