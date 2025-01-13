#ifndef CONSTS_H
#define CONSTS_H
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
// #define CURRENT_INPUT_IO A6 // simulation potentiometer

#define BAUDRATE (115200)
//#define IGNITION_BUTTON_IO A2
#define IGNITION_BUTTON_IO 2
#define SPARK_IO 4
#define HYDROGEN_VALVE_IO 5
//#define LANG_BUTTON_IO A1
#define LANG_BUTTON_IO 3
#define SOUND 8

const float AUTO_EXPLOSION_THRESHOLD = 90; // absolute max charge for auto explosion

const float MIN_CHARGE = 20;// Coulomb, minimum charge enable ignition  
const float MAX_CHARGE = 40;// Coulomb, maximum charge have to ignite

const float CURRENT_THRESHOLD = 0.1;  // 0.1 Amp , minimum current to measure (against current noise)
const float READ_TO_CURRENT = 0.02639;  // A/bit, (5000mV/(1024*185))
const int16_t ZERO_CURRENT_READ = 512; //2.5 Volt sensor zero current output
const int16_t MOVING_AVG_LENGTH = 20;// number of samples used for moving/rolling average 
const int32_t MEASURE_INTERVAL_TIME = 50;  // ms measure interval 
const int16_t BOUNCE_TIME = 50;//ms 
const int16_t DELAY_BETWEEN_VALVE_AND_SPARK = 1000; // ms
const int16_t DELAY_AFTER_SPARK = 500; // ms - delay after spark before opening valve again
const int16_t NUM_OF_SPARK = 3;//ms
const int16_t SPARK_SPACE_TIME = 150;//ms
const int16_t SPARK_TIME = 50; // ms - Spark time !!! keep as short as possible to avoid HV driver damage
const int16_t DELAY_SWITCH_SOUND = 100; //ms delay for short press button to activate the sound
const int16_t DELAY_SOUND = 4000;//ms 2000ms to listen to the sound + 2000ms of silence
const int16_t TIME_TO_IGNITE = 1000;


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
int32_t time_sound = 0;   //ms mesuring time sound until the ignit
int16_t sensor_value = 0;  // value read from sensor 
int32_t avg_sensor_value = 0;        // (moving) average value  
float current_value = 0;        // calculated current [A]
float avg_current_value = 0;        // calculated average current [A]
float abs_avg_current_value = 0;        // absolute average current [A]
bool ignited_now = false;  // for serial msg


////////language///////////////
uint8_t lang = 0;  // 0 - hebrew, 1 - english, 2 - arabic
//bool last_lang_button_state = HIGH;  // used for checking if lang button was released
//////////////////////////////


/////////button ignition//////////88888

+
bool check = LOW;
//////////////////////////////

 #endif
