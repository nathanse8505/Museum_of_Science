
#include <Arduino.h>

#define SR_st_pin 4 // 74HC565 shift register strob pin (12) - active high (low/hig/low pulse shift Sr vector to outputs)make sure set low during "push"data in
#define SR_clk_pin 13 // 74HC565 shift clock pin (11) - active high (low/high/low pulse shift data in) make sure initaly set to low 
#define SR_data_pin 11 // 74HC565 Data in pin (14)  
#define SR_en_pin 7 // 74HC565 out put enable pin (13) - active low 
#define encoder_pinA 2
#define encoder_pinB 3
#define encoder_sw 8

#define INTERNAL_LED 13
#define BAUDRATE 115200
 
const uint32_t max_pulse_time = 1000; // ms
const uint32_t min_pulse_time = 0;
const uint32_t max_space_time = 2000;
const uint32_t min_space_time = 0;
const byte max_cassettes = 32;
const byte boards_per_cassette = 4;
const byte valves_per_board = 16;
const uint32_t max_frequency = 30;
const uint32_t min_frequency = 0;
const uint32_t max_valve_on_time = 5000;
const uint32_t min_valve_on_time = 100;
const uint32_t pulse_encoder_inc = 10; // how much does the encoder increases the pulse time
const uint32_t space_encoder_inc = 100; // how much does the encoder increases the pulse time
const uint32_t valve_on_time_step = 100;

const int num_of_valves = max_cassettes*boards_per_cassette*valves_per_board; 
const int valves_per_cassette = boards_per_cassette*valves_per_board;

uint32_t pulse_time = 100;
byte cassette_num = 0;
byte board_num = 0;
byte valve_num = 0;
uint32_t valve_on_time = 4000;
//uint32_t frequency = 0;
uint32_t space_time = 500;
byte current_setting = 0; // 0-cassette , 1-board , 2-valve
const byte max_settings = 5;
const uint32_t confirm_time = 2200;
const uint32_t started_display_time = 2000;
bool on_button = false;
bool started = false;
bool cycle_flag = false;
uint32_t last_click = 0;
uint32_t last_start = 0;
byte old_encoder_read = 0;
byte new_encoder_read = 0;
signed int encoder_pos = 0;
const byte prescaler = 2;

float delay_tick = 1;

bool use_serial = true ; //  print to serial (set monitor to same boudrate)
bool use_display = false; // display on SH106 (make sure Display I2C address and DATA nad Clock conection)  
