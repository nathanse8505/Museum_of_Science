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
 * 2/4/2025 - Modified to VL53L1X sensor (originally VL53L1X)
 */


 
#include <Arduino.h>
#include <HID-Project.h>

#define LOOP_MS					(20)
#define TOF_MAX_VALUE			(3000)
#define JOYSTICK_MIN_VALUE		(12000)
#define JOYSTICK_MAX_VALUE		(20000)

#include <Wire.h>
#include <VL53L1X.h>			// https://github.com/pololu/vl53l1x-arduino

VL53L1X tof;

int16_t dist = 0;

void setup()
{
	
	// initialize USB joystick HID
	Gamepad.begin();
	// initialize TOF sensor to long range readings
	Wire.begin();
	tof.setTimeout(500);
	while (!tof.init()){
    delay(1000);
    //Serial.println("error");
   }

  tof.setDistanceMode(VL53L1X::Long);
  tof.setMeasurementTimingBudget(50000);
	tof.startContinuous(50);
}

void loop()
{
  dist = tof.read();
	if (tof.timeoutOccurred())
		dist = 0;

	 // Mapper la distance en valeur de joystick
  int16_t joystickValue = map(dist, 0, TOF_MAX_VALUE, JOYSTICK_MIN_VALUE, JOYSTICK_MAX_VALUE);

  // Envoi au joystick
  Gamepad.xAxis(joystickValue);
  Gamepad.write();
	delay(LOOP_MS);
}
