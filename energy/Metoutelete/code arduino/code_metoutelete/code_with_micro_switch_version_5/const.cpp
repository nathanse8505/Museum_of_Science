#include "FONCTION_and_CONST.h"

bool NC_MICRO_SWITCH = LOW;  // Initialisation ici
bool NO_MICRO_SWITCH = HIGH;

int8_t time_bouncing = 50;
unsigned long BAUDERATE = 115200;
unsigned long ACTIVATION_TIME = 30000;
unsigned long RESET_TIME_SECURE = 5000;
int16_t MOTOR_DELAY = 1000;
int16_t DELAY_BEFORE_SLEEP = 1000;

bool check = LOW;
unsigned long time_start = 0;
unsigned long time_to_secure = 0;
bool flag_first_press = HIGH;
bool flag_led_on = HIGH;

