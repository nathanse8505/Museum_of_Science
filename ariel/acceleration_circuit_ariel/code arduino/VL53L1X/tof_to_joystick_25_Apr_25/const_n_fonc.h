#ifndef  const__n_fonc
#define  const_n_fonc
/*
 * tof_to_joystick - reads distance measurements from VL53L1X TimeOfLight sensor,
 * writes the results to the USB joystick (Pro Micro ATmega32U4).
 * 
 * Pinout:
 *  ┌─────────┬─────────┐
 *  │ Arduino │ VL53L1X │
 *  ├─────────┼─────────┤
 *  │ VCC     │ VCC     │
 *  │ GND     │ GND     │
 *  │ D2      │ SDA     │
 *  │ D3      │ SCL     │
 *  └─────────┴─────────┘
 * Arad Eizen 25/12/21.
 * 21/4/2025 - Modified to VL53L1X sensor (originally VL53L1X)
 *   *=========Arduino Leonardo pinout==========*
 *                     _________
 *                TXD |         | Vin
 *                RXD |         | GND
 *                RST |         | RST
 *     GND (ToF)  GND |         | +5V +5V (ToF)
 *     SDA (ToF)   D2 |         | A0
 *     SCL (ToF)   D3 |         | A1
 *                 D4 |         | A2
 *     XSHUT (ToF) D5 |         | A3
 *                 D6 |         | A4
 *                 D7 |         | A5
 *                 D8 |         | A6
 *                 D9 |         | A7
 *                D10 |         | A8
 *                D11 |         | A9
 *                D12 |         | A10
 *                D13 |         | A11
 *                    --- USB ---
 *             (ATmega32u4 with native USB)

 */


 
#include <Arduino.h>
#include <HID-Project.h>
#include <Wire.h>
#include <VL53L1X.h>			// https://github.com/pololu/vl53l1x-arduino
#include <avr/wdt.h>

#define XSHUT   5 

VL53L1X tof;

const int LOOP_MS =	20;
const int TOF_MAX_VALUE = 3000;
const int JOYSTICK_MIN_VALUE = 12000;
const int JOYSTICK_MAX_VALUE = 20000;
const int RST_WDT = 5000;

int16_t dist = 0;
int16_t sensor_to_joystick = 0;
int16_t test_x = 0;


void init_sensor(){

  Wire.begin();
  tof.setTimeout(500);

	while (!tof.init()){
    delay(1000);
   }

  tof.setDistanceMode(VL53L1X::Long);
  tof.setMeasurementTimingBudget(50000);
	tof.startContinuous(50);
}

void reset_sensor() {
  digitalWrite(XSHUT, LOW);  // Déactive the sensor
  delay(100);
}

void write_Joystick_X(int16_t joystickValue){
   Gamepad.xAxis(joystickValue);
   Gamepad.write();
	 delay(LOOP_MS);
}

#endif