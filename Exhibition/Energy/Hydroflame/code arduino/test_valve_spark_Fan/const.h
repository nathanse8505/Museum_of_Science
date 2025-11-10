#ifndef const
#define const
#include <Arduino.h>

/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 * ignition button  D2-|       |-A7  current input pot(not used)
 *                  D3-|       |-A6  input pot (not used)
 *     SPARK output D4-|       |-A5 
 *  RELAY_VALVE_IO  D5-|       |-A4 
 *RELAY_TURN_ON_OFF D6-|       |-A3
 *          FAN_IO  D7-|       |-A2   SENSOR_FIRE_IO
 *  LED_ACTIVATION  D8-|       |-A1   SENSOR_WATER_IO
 *                  D9-|       |-A0   Current sensor ACS712T-05
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *         Buzzer  D12-|       |-D13
 *                      --USB--        
 */
#define BAUDRATE (115200)
////////////  I/O   ////////////
#define CURRENT_INPUT_IO  A0 // current sensor 
#define SENSOR_WATER_IO   A3 //finir setup
#define START_TIME_POT    A6
#define STOP_TIME_POT     A7
#define IGNITION_BUTTON_IO 2

#define RELAY_TURN_ON_OFF_IO 7
#define FAN_IO               8
#define RELAY_VALVE_WATER_IO 9
#define SPARK_IO             10
#define LED_ACTIVATION       11

#define THERMOD   A1   // SO (MISO)
#define THERMOCS  12   // CS
#define THERMOCLK A2   // SCK
// global vars
int32_t time_start = 0;
int32_t time_start_hydro = 0;
int32_t time_new_session = 0;
const int16_t HYGROGEN_TIME = 20000;
const int16_t SESSION_TIME =120000;
const uint32_t ACTIVATION_TIME = 120000;


//////SPARK //////
const int16_t SPARK_SPACE_TIME = 1000;//ms
const int16_t SPARK_TIME = 50; // ms - Spark time !!! keep as short as possible to avoid HV driver damage
const int16_t NUM_OF_SPARK = 3;//ms
const int16_t DELAY_AFTER_SPARK = 500; // ms - delay after spark before opening valve again
bool ready_flag_fire = false;

//////// FAN /////////
const int16_t DELAY_FAN_ON = 10;
const int16_t DELAY_FAN_OFF = 10;
bool flag_ready_fan = false;

//////////////  CURRENT //////////
const float READ_TO_CURRENT = 0.02639;  // A/bit, (5000mV/(1024*185m))       4mV/bit / 185mV/A
const int16_t ZERO_CURRENT_READ = 512; //2.5 Volt sensor zero current output
const int16_t CURRENT_SYSTEM = 2;
const int16_t ITERATION = 100;
int32_t total_offset_sensor = 0;
int16_t offset_sensor = 0;
int16_t current_sensor = 0;  // value read from sensor
float current_value = 0;        // calculated current [A]
float ZeroCurrentSensor = 0;
const int32_t MEASURE_INTERVAL_TIME = 50;  // ms measure interval 

//////////// WATER ///////////
const int16_t NO_DETECTION = 300;//in bit resolution
const int16_t DELAY_FILL_WATER = 1000;//in ms
const int16_t DELAY_AFTER_FILL_WATER = 100;//in ms

//////////// FIRE ///////////
uint8_t status_sensor;
float tempC;
long timer_read_temp;
const int DELAY_TEMP = 220;//in ms
const int TEMP_TRESHOLD = 100;//in Celsius
const int FIRE_TIME = 6000;

/////////button ignition//////////
bool check_ignit = LOW;
bool buttonPressed;
bool flag_new_session = true;
bool flag_first_press = false;
const int16_t BOUNCE_TIME = 100;//ms 
const int DELAY_AFTER_ON = 500;//ms
//////////////////////////////////




#endif