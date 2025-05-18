#ifndef CONSTANTS_H
#define CONSTANTS_H

// Pin assignment for the language selection button
#define LANG_BUTTON_IO  2

/*
 * Arduino Nano pin mapping reference
 * (For reference only; not all pins are used here.)
 *                    ---------
 *                TXD-|       |-Vin 
 *                RXD-|       |-GND to sensor
 *                RST-|       |-RST
 *                GND-|       |-+5V to sensor
 *  LANG_BUTTON_IO D2-|       |-A7
 *                 D3-|       |-A6
 *                 D4-|       |-A5 (SCL) to sensor 
 *                 D5-|       |-A4 (SDA) to sensor
 *                 D6-|       |-A3
 *                 D7-|       |-A2
 *                 D8-|       |-A1
 *                 D9-|       |-A0
 *                D10-|       |-Ref
 *                D11-|       |-3.3V
 *                D12-|       |-D13
 *                     --USB--
 */

//////////// BUTTON DEBOUNCE ////////////
const int BOUNCE_TIME = 50;      // Debounce duration in milliseconds
bool check_lang = false;         // Internal flag for button press state
int8_t lang = 0;                 // Current language index (0, 1, 2, etc.)
/////////////////////////////////////////

// Threshold-related constants
const float THRESHOLD = 100; //in mm This distance threshold helps detect the start of lift movement

// Smoothing sensor readings
float smoothedDistance = 0.0;   // Holds the last smoothed sensor reading
const int ITERATION = 10;        // Number of samples for averaging
const int32_t BAUDERATE = 115200;  // Serial baud rate
float initialDistance = 0.0;    // Baseline distance (starting point)

// Lift state tracking
bool Lift_in_motion = false;    // True if the lift is actively moving
bool first_try = true;          // Used as a cooldown indicator after movement

unsigned long startTime = 0;    // Timestamp when the lift starts moving
unsigned long deltaTime = 0;    // Time difference from start to finish of the lift
const unsigned long RST_DELTA_TIME = 10000;
const unsigned long SENSOR_DISCONNECT = 200;

// Distance constraints
float minDistance = 0.0;        // Threshold for detecting movement start
const float maxDistance = 1000; //in mm The maximum distance used for computing horsepower

// Timers for resets and cooldowns
unsigned long bouncingBallTimer = 0;  // Timer for cooldown once max distance is reached
unsigned long meas_Tmr_rst = 0;       // Timer for measurement reset if max distance is not reached
const unsigned long MEAS_RST_MS = 5000;      // If lift takes longer than 20s, reset
const unsigned long RST_BOUNCING_BALL = 4000; // 4s cooldown after reaching max distance

#endif
