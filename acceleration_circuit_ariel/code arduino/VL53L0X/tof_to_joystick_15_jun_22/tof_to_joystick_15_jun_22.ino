/*
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
#include <HID-Project.h>

#define LOOP_MS					(50)
#define TOF_MAX_VALUE			(3000)
#define JOYSTICK_MIN_VALUE		(12000)
#define JOYSTICK_MAX_VALUE		(20000)

#include <Wire.h>
#include <VL53L0X.h>			// https://github.com/pololu/vl53l0x-arduino

VL53L0X tof;

void setup()
{
	// initialize USB joystick HID
	Gamepad.begin();

	// initialize TOF sensor to long range readings
	Wire.begin();
	tof.setTimeout(500);
	while (!tof.init())
		delay(1000);

	tof.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
	tof.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
	tof.startContinuous();
}

void loop()
{
	uint16_t dist = tof.readRangeContinuousMillimeters();
	if (tof.timeoutOccurred())
		dist = 0;

	Gamepad.xAxis(map(dist, 0, TOF_MAX_VALUE, JOYSTICK_MIN_VALUE, JOYSTICK_MAX_VALUE));
	Gamepad.write();
	delay(LOOP_MS);
}
