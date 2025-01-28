
#ifndef CONST_N_GLOBAL_H
#define CONST_N_GLOBAL_H
#include <Arduino.h>
#include <HardwareSerial.h>

/*
 *==========Arduino Nano pinout======
 *                      _______
 *                 TXD-|       |-Vin
 *                 RXD-|       |-Gnd To driver board
 *                 RST-|       |-RST
 *                 GND-|       |-+5V To driver board
 *         To Data, D2-|       |-A7
 *          To EN,  D3-|       |-A6
 *       To Strobe, D4-|       |-A5
 *          To Clk, D5-|       |-A4
 *                  D6-|       |-A3
 *                  D7-|       |-A2(D16) To button 1
 *                  D8-|       |-A1(D15) To button 5
 *                  D9-|       |-A0(D14) To button 4
 *                 D10-|       |-Ref
 *    To Button 2, D11-|       |-3.3V
 *    To Button 3, D12-|       |-D13 
 *                      --USB--
 */

// const and global
const int LED_NUM = 16;

// IO MAPPING
#define UV_St_Pin (4)    // 74HC565 shift register strobe pin - active high (low/hig/low pulse shift Sr vector to outputs)make sure set low during "push"data in
#define UV_Clk_Pin (5)   // 74HC565 shift clock pin - active high (low/hig/low pulse shift data in) make sure initaly set to low
#define UV_Data_Pin (2)  // 74HC565 Data in pin
#define UV_En_Pin (3)    // 74HC565 out put enable pin - active low

#define BUTTON1 (16)
#define BUTTON2 (11)
#define BUTTON3 (12)
#define BUTTON4 (14)
#define BUTTON5 (15)

#define DEBOUNCE_DELAY 10

#define PRINT_LOG true

int b1_state = 0;
int b2_state = 0;
int b3_state = 0;
int b4_state = 0;
int b5_state = 0;

bool led1_toggle = false;
bool led2_toggle = false;
bool led3_toggle = false;
bool led4_toggle = false;
bool led5_toggle = false;

int last_b1_state = HIGH;
int last_b2_state = HIGH;
int last_b3_state = HIGH;
int last_b4_state = HIGH;
int last_b5_state = HIGH;

unsigned long led1_on_time = 0;
unsigned long led2_on_time = 0;
unsigned long led3_on_time = 0;
unsigned long led4_on_time = 0;
unsigned long led5_on_time = 0;

const unsigned long TURN_OFF_DELAY = 45000;  // Auto turn-off delay in milliseconds

const int NUM_GROUPS = 5;
bool ledStates[NUM_GROUPS] = { false, false, false, false, false };  // Array to store each group's on/off state

unsigned long LAST_BUTTON_PRESS_TIME = 0;        // Track last button press time
const unsigned long IDLE_DELAY = 60000;          // 1 minute (in milliseconds) idle delay
const unsigned long RANDOM_ON_DURATION = 45000;  // 45 seconds for random LED group
bool IDLE_MODE = false;                          // Flag for idle mode
int RANDOM_GROUP = -1;                           // To track the random group turned on
int LAST_GROUP = -1;

#endif /* CONSTANTS_H */