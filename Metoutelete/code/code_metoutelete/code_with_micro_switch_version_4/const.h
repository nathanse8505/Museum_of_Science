#ifndef const
#define const


#define MICRO_SW 6
#define BUTTON 5
#define MOTOR 3
#define LED_BUTTON 2

bool NC_MICRO_SWITCH = LOW;//when the microswitch in normally close its in the gnd
bool NO_MICRO_SWITCH = HIGH;//when the microswitch in normally open its in the VCC

const int8_t time_bouncing = 50;
const int BAUDERATE = 9600;
const int ACTIVATION_TIME =1000;//ms after pushing the button the motor stop until activation time
const float RESET_TIME_SECURE = 7000;//if the micoswitch dosent work stop the motor and the arduino
const int16_t MOTOR_DELAY = 500;
const int16_t DELAY_BEFORE_SLEEP = 1000;



bool check = LOW;//variable for pushing the button
float time_start = 0;//start time to disable the button until activation time
float time_to_secure = 0;//start time to disable the arduino until stop_time_safety
bool button_check = LOW;//button status



#endif

