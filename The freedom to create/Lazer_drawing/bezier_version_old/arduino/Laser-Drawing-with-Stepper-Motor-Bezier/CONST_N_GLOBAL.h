#ifndef CONST_N_GLOBAL_H
#define CONST_N_GLOBAL_H
#include <Arduino.h>

//=========motor and draw area parameters
// X is driver 0, Y is 1, Z is 2.
const int8_t NUMBER_OF_MOTORS = 2;//X, Y
const int8_t NUMBER_OF_MOVES = 4;//X+, X-, Y+, Y-
const float board_size[NUMBER_OF_MOTORS] = {83.0,83.0}; //mm
const float screen_size[2] = {295.0,165.0}; //mm

//=============start blink led parameters=================
const uint8_t START_LED_BLINK_NUMBER = 3;
const uint16_t START_LED_BLINK_ON = 100;//ms
const uint16_t START_LED_BLINK_OFF = 200;//ms

//==================IO pin define =======================
const uint8_t MOVE_PIN[NUMBER_OF_MOVES] = {9, 16, 10, 15}; //push buttons to move (X+, X-, Y+, Y-)
const uint8_t DIR_PIN[NUMBER_OF_MOTORS] = {2, 3}; // original on V4 Shield
const uint8_t STEP_PIN[NUMBER_OF_MOTORS] = {5, 6}; // original on V4 Shield

#define IS_EN_JUMPER 17 // )A3 - Mark CoolEN on V3 SHIELD - Hige is disable, low enable this is for all 3 drivers/motors
#define STEPPER_EN_PIN 8 // invert - Hige is disable, low enable this is for all 3 drivers/motors
#define SPEED_POT_INPUT_PIN A6 // analog read IO mark A6 on shield 
#define LED_OUT 13// also internal led
#define LASER_OUT 11 //pwm drive for laser
#define DC_MOTOR_OUT 10 //pwm drive for dc motor

const float MIN_POSITION[NUMBER_OF_MOTORS]={0,0};
const float MAX_POSITION[NUMBER_OF_MOTORS]={board_size[0],board_size[1]};
//timming/rate parameters
const uint16_t DIRECTION_CHANGE_WAIT_TIME = 5; //[ms] time wait/stop move between change direction to avoid loos steps
const uint16_t MIN_RATE = 6; //ms - minimum time between steps pulses - faster speed
const uint16_t MAX_RATE = 150; //ms - maximum, time between steps pulses - slower speed
const uint16_t HOMMING_RATE  = MIN_RATE ; //ms -
const float TOLERANCE = 0.1;// ratio to mm per pulse tolerance for not move 

//----- global variable------------------

// float mm_per_pulse[NUMBER_OF_MOTORS]= {83.0/843,83.0/843};
float mm_per_pulse[NUMBER_OF_MOTORS]= {2*80.0/800,2*80.0/800};
float mm_per_pixel[NUMBER_OF_MOTORS] = {295.0/1366,165.0/768};
float border[4] = {437.0,147.0,930.0,620}; //x,y, xMax,yMax - all in pixels
float screen_scale[2] = {board_size[0]/(border[2]-border[0]), board_size[1]/(border[3]-border[1])};   // units: mm/pixel

int param_index = 0;

bool Is_destination_done = true;
bool Is_Run_Command = false ; // true if system running/moving acording to serial port command 
int16_t Current_Steps[NUMBER_OF_MOTORS] = {0, 0}; //can changed acording to number of motors (XYZ is 3)
int16_t MIN_Steps [NUMBER_OF_MOTORS] = {0, 0};
int16_t MAX_Steps [NUMBER_OF_MOTORS] = {3500, 3500};
int16_t HOMMING_Steps [NUMBER_OF_MOTORS] = {board_size[0]/mm_per_pulse[0], board_size[1]/mm_per_pulse[1]};
int16_t STEPS_TO_CENTER = 270;

float Current_Position[NUMBER_OF_MOTORS]={0.0,0.0};
float destination[NUMBER_OF_MOTORS] ={0.0,0.0}; // nead to change name !!
float Delta[NUMBER_OF_MOTORS] = {0.0,0.0};// sqrt(X^2+Y^2+Z^2)

float max_delta [NUMBER_OF_MOTORS]= {TOLERANCE*abs(mm_per_pulse[0]),TOLERANCE*abs(mm_per_pulse[1])}; // max delat between current pos and destination  

byte laser_power = 255;
byte dc_motor_power = 255;
uint16_t rate = MIN_RATE; // register hold rate (read from potentiometer)

byte LASER_ON_POWER = 255;
byte CONTOUR_POWER = 255;
uint16_t LASER_OFF_RATE = MIN_RATE;
uint16_t LASER_ON_RATE = 80;
uint16_t CONTOUR_RATE = 80;
uint16_t MAX_DC_MOTOR_TIME = 1500;
const uint8_t PARAM_NUMBER = 10;

uint16_t TIME_DELAY_FOR_CONTOUR_CUTTING = 110; // delay in ms for letting the laser stay on a little longer for contour to be cut properly 

bool is_PBs_Preased = false ; // for enable/disable motoers if no PB plreased
// array for each parameter one variable for each motor (usealy only 3)
bool motor_direction[NUMBER_OF_MOTORS] = {false, false}; // initialy asume arbitary direction - false count pulses down
uint32_t last_pulse_time[NUMBER_OF_MOVES] = {0, 0, 0, 0}; // !! nead to update as state change to continu !
//uint16_t XYZ_rates [NUMBER_OF_MOVES] = {MAX_RATE,MAX_RATE,MAX_RATE,MAX_RATE}; // initial value same rates
uint16_t XYZ_prop_rates [NUMBER_OF_MOVES] = {MIN_RATE, MIN_RATE, MIN_RATE, MIN_RATE}; // proportinal rates
uint16_t XYZ_rates [NUMBER_OF_MOVES] = {MIN_RATE, MIN_RATE, MIN_RATE, MIN_RATE}; // initial value same rates
uint16_t XYZ_homming_rates [NUMBER_OF_MOVES] = {HOMMING_RATE, HOMMING_RATE, HOMMING_RATE, HOMMING_RATE}; // initial value same rates

uint32_t last_time_dc_motor = 0;

bool is_laser_on = false;
bool is_dc_motor_on = false;

const uint8_t squareX_step = 60/mm_per_pulse[0];
const uint8_t squareY_step = 60/mm_per_pulse[1];

//const int BUFFER_SIZE = 10;
//float BUFFER[2*BUFFER_SIZE] = {};

// const float start_point[NUMBER_OF_MOTORS] = {30.0,30.0}; //mm

//------------bezier related ---------------------
const int points_per_curve = 8; //4*(x,y) - 8 floats total, DO NOT CHANGE
const float tolerance_float = 0.1;
const float starting_key = -2;
const float finished_reading_key = -2.5;
const float next_curve_key = -3;
const float end_key = -4;
const int numPoints = 30;
const int numSteps = numPoints-1;
const int TIME_DELAY_ARDUINO = 10;
boolean drawing_curve = false;
boolean start_flag = false;
int num_of_curves = 0;
int num_of_contour = 0;
const int MAX_CURVES = 2; // can be 1 but 2 for safety
//float curve[point_per_curve] = {};
float curves[points_per_curve*MAX_CURVES] = {};
int curve_index = 0;
int step_index = 0;

float bezier_point[2] = {0,0};
float b[8] = {}; // for computation of bezier curve
float coeff[8] = {}; //polynomial coefficients from Bezier points
float pointX = 0;
float pointY = 0;
float firstFDX = 0;
float firstFDY = 0;
float secondFDX = 0;
float secondFDY = 0;
float thirdFDX = 0;
float thirdFDY = 0;


bool py_flag = false;
bool got_param = false;


#define BAUDRATE (115200)
String in_command ="";


#endif /* CONSTANTS_H */
