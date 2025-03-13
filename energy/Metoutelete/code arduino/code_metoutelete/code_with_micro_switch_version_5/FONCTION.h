#ifndef FONCTION_H
#define FONCTION_H

#include <SPI.h>
#include <SD.h>
#include "const.h"

// Function to turn on the motor
void TURN_ON_MOTOR();

// Function to detect and debounce button press
bool PRESS_BUTTON();

// Function for Immediate Engine Cutoff (IEC) - Stops the motor immediately
void IEC();

// Function to print motor shutdown information to the serial monitor
void printMotorOffInfo();

// Function to blink the LED as an indicator (e.g., for errors or warnings)
void BLINK_FLAG(unsigned int DELAY_BLINK);

// Function to log events to the SD card
void logEvent(const char* message);

#endif
