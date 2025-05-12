#include "const_n_fonc.h"

void setup()
{
  pinMode(XSHUT, OUTPUT);
  digitalWrite(XSHUT, HIGH); // active the sensor

	Gamepad.begin();// initialize USB joystick HID	
  init_sensor();// initialize TOF sensor to long range readings
  wdt_enable(WDTO_4S);
}

void loop()
{
  dist = tof.read();
  sensor_to_joystick = map(dist, 0, TOF_MAX_VALUE, JOYSTICK_MIN_VALUE, JOYSTICK_MAX_VALUE);
  //test_x = (test_x+100)%JOYSTICK_MAX_VALUE; //test

  write_Joystick_X(sensor_to_joystick);

  if (tof.timeoutOccurred()){
    reset_sensor();
    delay(RST_WDT);
  }
 
}
