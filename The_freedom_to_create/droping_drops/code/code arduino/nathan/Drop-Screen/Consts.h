#ifndef CONSTS_H
#define CONSTS_H
#include <Arduino.h>
#include <avr/pgmspace.h>

#define SR_st_pin 4  // shift register (74HC565) strobe pin
#define SR_clk_pin 13  // shift register clock pin
#define SR_data_pin 11 // shift register data in pin  
#define SR_en_pin 7  // shift register enable pin
#define red_led_pin 5
#define green_led_pin 6
#define blue_led_pin 9

#define INTERNAL_LED 13
//#define encoder_pinA 2  -- for encoder if wanted
//#define encoder_pinB 3  -- for encoder if wanted
//#define encoder_sw 8  -- for encoder if wanted
/*
 *  *==========Arduino Nano pinout======
 *                      _______
 *                 TXD-|       |-Vin
 *                 RXD-|       |-Gnd
 *                 RST-|       |-RST
 *                 GND-|       |-+5V
 *                  D2-|       |-A7  
 *                  D3-|       |-A6  
 *        SR_st_pin D4-|       |-A5,
 *      red_led_pin D5-|       |-A4,
 *    green_led_pin D6-|       |-A3, 
 *        SR_en_pin D7-|       |-A2, 
 *                  D8-|       |-A1, 
 *     blue_led_pin D9-|       |-A0,  
 *                 D10-|       |-Ref
 *    SR_data_pin  D11-|       |-3.3V
 *                 D12-|       |-D13 SR_clk_pin,INTERNAL_LED
 *                      --USB--
 */
#define START_KEY 's'  // start key for collecting image data from Raspberry PI
#define DROP_KEY 'd'  // drops the last image in the buffer
#define END_KEY 'e'  // end key for ending protocol
#define READY_KEY 'r'  // arduino ready for next image
#define GOOD_KEY 'g'  // for general positive response
#define BAUDRATE 115200

const byte max_cassettes = 32;
const byte boards_per_cassette = 4;
const byte valves_per_board = 16;
const uint16_t num_of_valves = max_cassettes*boards_per_cassette*valves_per_board; 
//const uint16_t valves_per_cassette = boards_per_cassette*valves_per_board;
const byte TIME_DELAY_ARDUINO = 10;  //ms
const uint32_t MAX_TIME_TO_COLLECT_DATA = 3000;  //ms

byte image_w = 64;  // width of image - DO NOT CHNAGE
byte image_h = 40;  // height of image, PI sends this
byte valve_on_time = 5;  // how much time (in ms) each row from image is opened on valves
uint16_t led_start = 350;  // how much time the lights are delayed from first row sent (relevant if "full light" is off)
uint16_t led_on_time = 550;  // how much time the lights are on (after waiting let_start millis)(relevant if "full light" is off)
byte drawing_depth = 1;  // how many layers (of cassettes) each image is viewed
byte color = 0;  // index for colors array
byte colors[8][3] = {{255,0,255},{255,255,0},{255,215,215}};  // default colors array - max 8 colors
byte colors_num = 3;  // size of colors array
byte led_power = 255;  // PWM for lights - not working for now

bool drawing_flag = false;  // true if an image is currenlty being dropped
bool valve_on_flag = false;  // true if the valves are on
bool led_start_flag = false;  // true if waiting for lights (relevant if "full light" is off)
bool led_on_flag = false;  // true if lights are on (relevant if "full light" is off)
bool full_light = true;  // true if lights are always on

uint32_t last_valve_on = 0;  // last time valves started
uint32_t last_led_start = 0;  // last time delay in lights started
uint32_t last_led_on = 0;  // last time led started

signed char row_in_drawing = image_h - 1; //which row in the drawing is currently dropped (from bottom to top)
byte cassette_drawing = 0; //in which cassette is the image being dropped 
byte image[320];  // buffer image - max of 64X40 pixels (divided by 8 for bytes)

byte param_index = 0;
bool got_param = false;  // got inital parameters
bool start_flag = false;  // got start key
bool got_cassette = false;   // got cassette number to drop drawing on
#define PARAM_NUMBER 3

#endif
