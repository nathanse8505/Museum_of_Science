#ifndef CONST
#define CONST


/*
*==================Arduino Nano pinout================ 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *      LatchPin  , D2-|       |-A7  
 *         clokPin  D3-|       |-A6  
 *      Data Serial D4-|       |-A5 
 *   Output enable  D5-|       |-A4 
 *              SW1 D6-|       |-A3
 *              SW2 D7-|       |-A2
 *              SW3 D8-|       |-A1
 *             SW4  D9-|       |-A0  
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *         Buzzer  D12-|       |-D13
 *                      --USB-- 
 *===================================================       
 */

// Pins
#define LatchPin 2  // ST_CP
#define ClockPin 3 // SH_CP
#define Data_Serial 4  // DS
#define Output_Enable 5 //Output Enable


#define SW1 6
#define SW2 7
#define SW3 8
#define SW4 9




const int16_t BOUNCE_TIME = 100;
const int16_t TIME_LED_ON = 2000;
const int16_t TIME_LED_OFF = 500;
const int16_t DELAY_AFTER_SHIFT = 100;
byte out_data = B00000000;

bool checkSW1 = LOW;
bool checkSW2 = LOW;
bool checkSW3 = LOW;
bool checkSW4 = LOW;


// Dans CONST.h
// Définir un tableau des pins des boutons
const int SWITCH_PINS[] = {SW1, SW2, SW3, SW4};
// Tableau des variables de contrôle
bool checkStates[] = {false, false, false, false};  // Pour remplacer checkS1, checkS2, etc.


#endif