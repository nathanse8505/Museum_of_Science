/*
 * Amir BS 21 Mars 2-25 
 * Joystick emulation tester
 * for test pro-micro for linear accelerator exhibit Ariel
 * Based on "tof_to_joystick" code, but only (sequentially) "move/change" joystick position without using/connection to TOF
 * ============================
 * Original from "tof_to_joystick"
 * tof_to_joystick - reads distance measurements from VL53L0X TimeOfLight sensor,
 * writes the results to the USB joystick (Pro Micro ATmega32U4).
 * 
 * Pinout:
 *  ┌─────────┬─────────┐
 *  │ Arduino │ VL53L0X │
 *  ├─────────┼─────────┤
 *  │ VCC     │ VCC     │
 *  │ GND     │ GND     │
 *  │ D2      │ SDA     │
 *  │ D3      │ SCL     │
 *  └─────────┴─────────┘
 * Arad Eizen 25/12/21.
 */


#include <Arduino.h>
#include <HID-Project.h> // https://github.com/NicoHood/HID

#define LOOP_MS					(20)
#define TOF_MAX_VALUE			(3000)
#define JOYSTICK_MAX_VALUE		(16000)

/*
#include <Wire.h>
#include <VL53L0X.h>			// https://github.com/pololu/vl53l0x-arduino

VL53L0X tof;
*/

int16_t temp_x = 0;
// gampepad x position see https://github.com/NicoHood/HID/blob/master/src/HID-APIs/GamepadAPI.hpp
// void GamepadAPI::xAxis(int16_t a){

void setup()
{
	// initialize USB joystick HID
	Gamepad.begin();
/*
	// initialize TOF sensor to long range readings
	Wire.begin();
	tof.setTimeout(500);
	while (!tof.init())
		delay(1000);

	tof.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
	tof.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
	tof.startContinuous();
*/
}

void loop()
{
/*
	uint16_t dist = tof.readRangeContinuousMillimeters();
	if (tof.timeoutOccurred())
		dist = 0;

	Gamepad.xAxis(map(dist, 0, TOF_MAX_VALUE, 0, JOYSTICK_MAX_VALUE));
*/
  Gamepad.xAxis(temp_x);
	Gamepad.write();
	delay(LOOP_MS);
  temp_x = (temp_x+100)%JOYSTICK_MAX_VALUE;
}
