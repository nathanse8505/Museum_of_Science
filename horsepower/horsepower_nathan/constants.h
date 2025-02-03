#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SPACE_BUTTON_IO  A0 // Pin connected to the button


/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  gnd to sensor
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  5v to sensor
 *                  D2-|       |-A7  
 *                  D3-|       |-A6  
 *                  D4-|       |-A5  SCL
 *                  D5-|       |-A4  SDA
 *                  D6-|       |-A3  
 *                  D7-|       |-A2  
 *                  D8-|       |-A1
 *                  D9-|       |-A0  SPACE_BUTTON_IO
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */

/////////////button space///////////
const int BOUNCE_TIME = 50;
bool check_space = false;
///////////////////////////////////

// Physics-related constants
const float WEIGHT_KG = 7.5;         // Weight in kilograms
const float WEIGHT_KG_POWER_HORSE = 75;
const float GRAVITY = 9.81;         // Acceleration due to gravity in m/s^2
const float HORSEPOWER_CONVERSION = 745.7; // Conversion factor Watt to horsepower

// Sensor-related constants
const float THRESHOLD = 0.1;       // Threshold to detect lift movement (meters)



float smoothedDistance = 0.0; // Smoothed sensor reading
const int ITERATION = 5; // Number of readings for smoothing
const int32_t BAUDERATE = 115200;
float initialDistance = 0.0;   // Starting distance (baseline)

bool Lift_in_motion = false;    // Tracks if the lift is in motion
bool first_try = true;

float startTime = 0;  // Time when the lift starts moving
float deltaTime = 0;    // Time when the lift reaches the topmost point

float minDistance = 0.0;      // Tracks the minimum distance to the groune during the lift in meter
const float maxDistance = 0.5;         // max distance to the ground in meter



#endif
