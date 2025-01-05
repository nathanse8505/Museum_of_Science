#define MICRO_SW 6
#define BUTTON 5
#define MOTOR 3
#define LED_DELAY 2

bool  NC_MICRO_SWITCH = LOW;//when the microswitch in normally close is in the gnd
bool NO_MICRO_SWITCH = HIGH;//when the microswitch in normally close is in the VCC

const int time_bouncing = 20;
const int bauderate = 9600;
const float stop_time = 30000;//after pushing the button the motor stop until stop time
const float motor_delay = 300;
const float stop_time_safety = 15000;//if the micoswitch dosent work stop the motor

