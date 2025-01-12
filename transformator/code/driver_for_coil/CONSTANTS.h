#ifndef CONSTANTS
#define CONSTANTS


#define SERIAL_BAUD_RATE (9600)
#define TIME_CLK A0            // Analog input for the total cycle duration
#define PULSE_POSITIVE A1      // Analog input for the positive pulse duration
#define N_DIRECTION 3          // Pin to control the negative direction
#define P_DIRECTION 5          // Pin to control the positive direction
#define LED_CONTROL 7          // Pin for the control LED

float DELAY_RELAY = 20;         //delay between relays 
float cycle_max = 1000;        // Maximum cycle duration in milliseconds

float time_positive;           // Duration of the positive signal (in microseconds)
float cycle_time;              // Total cycle duration (in milliseconds)

#endif





