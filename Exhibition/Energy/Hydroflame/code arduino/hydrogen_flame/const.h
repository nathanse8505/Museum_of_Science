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
#define SENSOR_WATER_IO      A3  // 
#define NTC_PIN              A2  // Pin du NTC 
#define STOP_TIME_POT        A7

#define IGNITION_BUTTON_IO   2
#define LED_ACTIVATION       3
#define RELAY_TURN_ON_OFF_IO 7
#define FAN_IO               8
#define RELAY_VALVE_WATER_IO 9
#define SPARK_IO             10


//#define THERMOD   A1   // SO (MISO)
//#define THERMOCS  12   // CS
//#define THERMOCLK A2   // SCK
#define CURRENT_INPUT_IO  A0  // current sensor 
// global vars
int32_t time_start = 0;
int32_t time_start_hydro = 0;
int32_t time_new_session = 0;
const uint32_t HYGROGEN_TIME = 10000;
const uint32_t SESSION_TIME =50000;
const uint32_t ACTIVATION_TIME = 15000;


//////SPARK //////
const int16_t SPARK_SPACE_TIME = 1000;//ms
const int16_t SPARK_TIME = 50; // ms - Spark time !!! keep as short as possible to avoid HV driver damage
const int16_t NUM_OF_SPARK = 2;//ms
const int16_t DELAY_AFTER_SPARK = 500; // ms - delay after spark before opening valve again
bool ready_flag_fire = false;

//////// FAN /////////
const int16_t DELAY_FAN_ON = 200;
const int16_t DELAY_FAN_OFF = 10;
bool flag_ready_fan = false;

//////////// WATER ///////////
const int16_t NO_DETECTION = 300;//in bit resolution
const int16_t DELAY_FILL_WATER = 1000;//in ms
const int16_t DELAY_AFTER_FILL_WATER = 100;//in ms

//////////// FIRE ///////////
const int FIRE_TIME = 5000;
const float BETA = 6224;            // Constante Beta du NTC (à adapter à ton capteur)
const float R_SERIE = 9218.0;      // Résistance fixe en série (10k par exemple)
const int  DELAY_TEMP = 1000;
const float T0 = 298.15;            // Température de référence (25°C en Kelvin)
const float R0 = 96400.0;           // Résistance du NTC à 25°C
float lastTemp = -1000.0;           // Valeur initiale impossible pour forcer la première lecture de la temperature
const float FIRST_SESSION_TEMP = -1000.0;
long lastReadTemp = 0;
bool first_temp =true;

/////////button ignition//////////
bool check_ignit = LOW;
bool buttonPressed;
bool flag_new_session = true;
bool flag_first_press = false;
const int16_t BOUNCE_TIME = 100;//ms 
const int DELAY_AFTER_ON = 500;//ms
//////////////////////////////////


////////current INA////////
#define I2C_ADDRESS 0x40
const int16_t CURRENT_SYSTEM = 5;
const int32_t CURRENT_INTERVAL_TIME = 1000;  // ms measure interval 
long time_read_current = 0;
#endif