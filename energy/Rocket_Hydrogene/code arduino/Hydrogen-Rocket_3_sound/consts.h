#ifndef consts
#define consts
#include <Arduino.h>

/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 * ignition button, D2-|       |-A7  pot 2 (not used)
 * language button  D3-|       |-A6  current input pot
 *     spark output D4-|       |-A5 ,SCL (to Display) 
 *        valve IO  D5-|       |-A4 ,SDA (to Display) 
 *                  D6-|       |-A3
 *                  D7-|       |-A2
 *            SOUND D8-|       |-A1,
 *                  D9-|       |-A0,  Current sensor ACS712T-05
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *         Buzzer  D12-|       |-D13
 *                      --USB--        
 */

#define CURRENT_INPUT_IO A0 // current sensor 
//#define CURRENT_INPUT_IO A7 // simulation potentiometer
  
#define BAUDRATE (115200)
//#define IGNITION_BUTTON_IO A2
#define IGNITION_BUTTON_IO 2
#define LANG_BUTTON_IO 3
#define SPARK_IO 4
#define HYDROGEN_VALVE_IO 5
//#define LANG_BUTTON_IO A1


const float AUTO_EXPLOSION_THRESHOLD = 80; // absolute max charge for auto explosion

const float MIN_CHARGE = 0;// Coulomb, minimum charge enable ignition  
const float MAX_CHARGE = 40;// Coulomb, maximum charge have to ignite

const float CURRENT_THRESHOLD = 0.1;  // 0.1 Amp , minimum current to measure (against current noise)
const float READ_TO_CURRENT = 0.02639;  // A/bit, (5000mV/(1024*185))
const int16_t ZERO_CURRENT_READ = 512; //2.5 Volt sensor zero current output
const int16_t MOVING_AVG_LENGTH = 20;// number of samples used for moving/rolling average 
const int32_t MEASURE_INTERVAL_TIME = 50;  // ms measure interval 
const int16_t BOUNCE_TIME = 100;//ms 
const int16_t DELAY_BETWEEN_VALVE_AND_SPARK = 1000; // ms
const int16_t DELAY_AFTER_SPARK = 500; // ms - delay after spark before opening valve again
const int16_t NUM_OF_SPARK = 3;//ms
const int16_t SPARK_SPACE_TIME = 150;//ms
const int16_t SPARK_TIME = 50; // ms - Spark time !!! keep as short as possible to avoid HV driver damage
const int16_t TIME_TO_IGNITE = 1000;
const uint16_t TIME_LOST_COM = 15000;


// display consts
const int32_t DISPLAY_INTERVAL_TIME = 200;  // ms show/display interval
char* DISPLAY_TEXTS[] = {"low", "ok", "max"};


#define LOW_CHARGE_TEXT 0
#define CAN_IGNITE_TEXT 1
#define MAX_CHARGE_TEXT 2
byte text = 0;

// global vars
float charge = 0;
int32_t last_measure_time = 0;  // ms last measuring time 
int32_t last_display_time = 0;  // ms last time data was display
int32_t timer_lost_com = 0;
bool flag_lost_com = false;

int32_t time_sound = 0;   //ms mesuring time sound until the ignit
bool ready_sound = false; 
int16_t sensor_value = 0;  // value read from sensor 
int32_t avg_sensor_value = 0;        // (moving) average value  
float current_value = 0;        // calculated current [A]
float avg_current_value = 0;        // calculated average current [A]
float abs_avg_current_value = 0;        // absolute average current [A]

/////////////language/////////
uint8_t lang = 0;  // 0 - hebrew, 1 - english, 2 - arabic
bool check_lang = LOW;// used for checking if lang button was released
////////////////////////////////////

/////////button ignition//////////
bool check_ignit = LOW;
//////////////////////////////////

#endif