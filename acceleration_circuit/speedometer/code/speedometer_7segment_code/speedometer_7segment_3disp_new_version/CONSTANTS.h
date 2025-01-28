#ifndef CONSTANTS
#define CONSTANTS

#define SERIAL_BAUD_RATE (9600) // Serial communication speed in bits per second
#define BCD_A (3) // Output pin for BCD_A (4511 decoder)
#define BCD_B (4) // Output pin for BCD_B (4511 decoder)
#define BCD_C (5) // Output pin for BCD_C (4511 decoder)
#define BCD_D (6) // Output pin for BCD_D (4511 decoder)

#define LE_CENTURIES (7) // Output pin for the leftmost (hundreds) digit (negative pulse, used for tens)
#define LE_DOZENS (8) // Output pin for the middle (tens) digit
#define LE_UNITS (9) // Output pin for the rightmost (units) digit (negative pulse)

#define SENSOR A2 // Analog input for sensor

const int16_t CALIBRATION = 100; // Calibration factor for measurements
const int32_t TIME_RESET = 10000; // Time interval for resetting sensor errors (in milliseconds)
const int16_t DELAY_BOUNCING = 50; // Debounce delay (in milliseconds)
const float meter = 3.14 *  1.14; // Circumference of the wheel (in meters)
const int ms_to_sec = 1000; // Conversion factor from milliseconds to seconds
const float ms_to_KmH = 3.6; // Conversion factor from meters per second to kilometers per hour
const float sec_to_min = 60; // Conversion factor from seconds to minutes
const int8_t VREF = 5; // Reference voltage (in volts)
const int8_t Number_OF_7SEG = 3; // Number of 7-segment displays
const int16_t BIT_RESOLUTION = 1023; // ADC bit resolution (10-bit)
const int8_t MIN_VOLT = 1; // Minimum voltage threshold for sensor
const int8_t MAX_VOLT = 3; // Maximum voltage threshold for sensor
const int8_t RIGH_DIGIT = 1; // Identifier for the rightmost digit
const int8_t MIDDLE_DIGIT = 2; // Identifier for the middle digit
const int8_t LEFT_DIGIT = 3; // Identifier for the leftmost digit

float timer = 0; // Timer variable (in milliseconds)
float velocity = 0; // Current velocity (in meters per second)
float km_per_h = 0; // Current speed (in kilometers per hour)
float delta_time_sec = 0; // Time difference between sensor readings (in seconds)
float rpm; // Rotations per minute of the wheel

bool check = HIGH; // Flag for sensor error detection
bool check_zero = HIGH; // Flag for zero velocity detection

int Digit_3_To_Display = 0; // Leftmost (hundreds) digit to display (used for tens)
int Digit_2_To_Display = 0; // Middle (tens) digit to display (used for units)
int Digit_1_To_Display = 0; // Rightmost (units) digit to display (not used in wind tunnel experiments)

#endif
