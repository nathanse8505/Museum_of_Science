#ifndef Fonction
#define Fonction

#include <Wire.h>
#include <VL53L1X.h>
#include <avr/wdt.h>


// Pin assignment for the language selection button
#define LANG_BUTTON_IO  2
#define XSHUT_PIN 3 // to shutdown the sensor



/*
 * Arduino Nano pin mapping reference
 * (For reference only; not all pins are used here.)
 *                       ---------
 *                   TXD-|       |-Vin 
 *                   RXD-|       |-GND to sensor VL53L1X
 *                   RST-|       |-RST
 *                   GND-|       |-+5V to sensor VL53L1X
 *     LANG_BUTTON_IO D2-|       |-A7
 *(VL53L1X) XSHUT_PIN D3-|       |-A6
 *                    D4-|       |-A5 (SCL) VL53L1X
 *                    D5-|       |-A4 (SDA) VL53L1X
 *                    D6-|       |-A3
 *                    D7-|       |-A2
 *                    D8-|       |-A1
 *                    D9-|       |-A0
 *                   D10-|       |-Ref
 *                   D11-|       |-3.3V
 *                   D12-|       |-D13
 *                        --USB--
 */

// Threshold-related constants
const float THRESHOLD = 100; //in mm This distance threshold helps detect the start of lift movement

// Smoothing sensor readings
float smoothedDistance = 0.0;   // Holds the last smoothed sensor reading
const int32_t BAUDERATE = 115200;  // Serial baud rate
const int32_t DELTA_MAX_DISTANCE = 700; //in mm The maximum distance used for computing horsepower
const int32_t INITIAL_DISTANCE = 1400;



// Global sensor object declared here for use in read_average_distance()
VL53L1X sensor;

void read_ROI_XY_and_ROI_center(){
  uint8_t x = sensor.readReg(0x0080);
  uint8_t y = sensor.readReg(0x007F);
  Serial.println("read ROI coordinate: " + String(x) + " read center ROI: " + String(y));
}

void reset_sensor() {
  digitalWrite(XSHUT_PIN, LOW);  // Déactive the sensor
  delay(100);
}
#endif
