#ifndef const
#define const
#include <Arduino.h>

/*
*==========Arduino Nano pinout====== 
 *                            _______
 *                       TXD-|       |-Vin 
 *                       RXD-|       |-Gnd  
 *                       RST-|       |-RST
 *                       GND-|       |-+5V  
 *    IGNITION_BUTTON_IO  D2-|       |-A7  
 *         LED_BUTTON_IO  D3-|       |-A6  
 *                        D4-|       |-A5   SCL current sensor (INA226)
 *                        D5-|       |-A4   SDA current sensor (INA226)
 *                        D6-|       |-A3   SENSOR_WATER_IO
 *TURN_ON_OFF_CURRENT_IO  D7-|       |-A2   NTC_PIN (100kohm) connected to 10kohm PULLUP
 *                FAN_IO  D8-|       |-A1   
 *        VALVE_WATER_IO  D9-|       |-A0   
 *              SPARK_IO D10-|       |-Ref
 *                       D11-|       |-3.3V   
 *                       D12-|       |-D13
 *                            --USB--        
 */
#define BAUDRATE (115200)
////////////  I/O   ////////////
#define SENSOR_PRESSURE_HYDRO_IO  A1  // 
#define NTC_PIN                   A2  //+5V --- R_SERIE (fixe) --- A2 --- NTC --- GND
#define SENSOR_WATER_IO           A3  //  
//CURRENT_SENSOR                  A4,A5 for SCL SDA

#define IGNITION_BUTTON_IO       2  
#define LED_BUTTON_IO            3
#define TURN_ON_OFF_CURRENT_IO   7  //The current is switched by a transistor that drives a 220 V SSR, which turns on the 12V power supply.”
#define FAN_IO                   8  //The FAN is switched by a transistor
#define VALVE_WATER_IO           9  //The VALVE is switched by a transistor
#define SPARK_IO                 10 //The SPARK is switched by a transistor

// global vars
int32_t time_start = 0;
int32_t time_start_hydro = 0;
int32_t time_new_session = 0;

const uint32_t ACTIVATION_TIME       = 15000; //time between end-new session (fan is active)
const uint32_t CURRENT_INTERVAL_TIME = 1000;  // after this time we check the current 
const uint32_t HYGROGEN_TIME         = 15000; //after this time we active the spark
const uint32_t FIRE_TEMP_TIME        = 5000; //after this time we calculate the temperare of the flame
const uint32_t SESSION_TIME          = 60000;//time between spark and end of the session

//////SPARK //////
const int16_t SPARK_SPACE_TIME = 1000;//ms
const int16_t SPARK_TIME = 50; // ms - Spark time !!! keep as short as possible to avoid HV driver damage
const int16_t NUM_OF_SPARK = 2;//ms
const int16_t DELAY_AFTER_SPARK = 500; // ms - delay after spark before opening valve again
bool flag_spark_ready = false;

//////// FAN /////////
const int16_t DELAY_FAN_ON = 100;
const int16_t DELAY_FAN_OFF = 2000;
bool flag_ready_fan = false;

//////////// WATER VALVE and SENSOR///////////
const uint16_t NO_DETECTION = 900;//in bit resolution
const uint16_t DELAY_FILL_WATER = 500;//in ms
const uint16_t DELAY_AFTER_FILL_WATER = 1000;//in ms
const uint8_t MAX_NUMBER_FILL_WATER = 10;//
int16_t counter_fill_water;//
const int16_t ERROR_WATER_DELAY = 500;

////////////NTC 100kΩ FIRE ///////////
const float BETA = 6224;            // Constante Beta du NTC (à adapter à ton capteur)
const float R_SERIE = 9218.0;      // Résistance fixe en série (10k par exemple)
const int  DELAY_TEMP = 1000;
const float T0 = 298.15;            // Température de référence (25°C en Kelvin)
const float R0 = 96400.0;           // Résistance du NTC à 25°C
float lastTemp = -1000.0;           // Valeur initiale impossible pour forcer la première lecture de la temperature
const float FIRST_SESSION_TEMP = -1000.0;
long lastReadTemp = 0;
bool first_temp =true;
bool first_time_delta = false;
bool first_time_temp = true;

/////////BUTTON IGNITION//////////
bool check_ignit = LOW;
bool buttonPressed;
bool flag_new_session = true;
bool flag_first_press = false;
const int16_t BOUNCE_TIME = 100;//ms 


////////current INA////////
#define I2C_ADDRESS 0x40
const int ERROR_CURRENT_DELAY = 2000;
const float R_SHUNT_OHMS = 0.00215f;    // ex: 2 mΩ
const float I_RANGE_A    = 20.0f;      // ex: tu veux mesurer jusqu’à ~10 A
const float CURRENT_SYSTEM = 3.5;
long time_read_current = 0;
bool first_time_current = true;
const int NUM_OF_READ = 1;
int read = 0;
float average_current = 0;
int current_flag;


/////SENSOR PRESSURE HYDRO//////
const uint16_t HIGH_PRESSURE = 100;
const int DELAY_AFTER_ON = 500;//ms
#endif