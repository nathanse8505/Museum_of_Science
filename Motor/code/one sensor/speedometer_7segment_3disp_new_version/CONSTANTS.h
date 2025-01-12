#ifndef CONSTANTS
#define CONSTANTS

#define SERIAL_BAUD_RATE (9600)
#define BCD_A (3) // Output to BCD_A (4511)
#define BCD_B (4) // Output to BCD_B (4511)
#define BCD_C (5) // Output to BCD_C (4511)
#define BCD_D (6) // Output to BCD_D (4511)

#define LE_CENTURIES (7) // Output Left (100th) digit LE (negative pulse) used as 10th
#define LE_DOZENS (8)
#define LE_UNITS (9) // Output right (10th) digit LE (negative pulse) used as units 


#define SENSOR A2 //



const int32_t TIME_RESET = 2000;
const float meter = 2 * 3.14 * 0.076;
const int ms_to_sec = 1000;
const int8_t VREF = 5;
const int8_t  Number_OF_7SEG = 3;
const int16_t BIT_RESOLUTION = 1023;
const int8_t MIN_VOLT = 1;
const int8_t MAX_VOLT = 3;
const int8_t RIGH_DIGIT = 1;
const int8_t MIDDLE_DIGIT = 2;
const int8_t LEFT_DIGIT = 3;

float timer = 0;
float velocity = 0; 
float delta_time_sec;
float rpm;

bool check = HIGH;
bool check_zero = HIGH;


int Digit_3_To_Display = 0;// Left (100th) digit to disply - used as 10th
int Digit_2_To_Display = 0;// Mid right (10th) digit to disply  - used as units 
int Digit_1_To_Display = 0;// rigth (external unit) digit to disply - not used for winf tunnel



#endif


