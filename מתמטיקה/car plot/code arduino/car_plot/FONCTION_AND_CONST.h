#ifndef FONCTION_AND_CONST
#define FONCTION_AND_CONST

#include <Wire.h>
#include <VL53L1X.h>
#include <avr/wdt.h>

// Global sensor object declared here for use in read_average_distance()
VL53L1X sensor;

#define MIN_D  A0 // Entrée analogique
#define MAX_D  A1 // Entrée analogique

#define D1 2
#define D2 3
#define D3 4
#define D4 5
#define D5 6
#define D6 7
#define D7 8
#define D8 9

#define XSHUT_PIN 13 // to shutdown the sensor



/*
 * Arduino Nano pin mapping reference
 * (For reference only; not all pins are used here.)
 *                       ---------
 *                   TXD-|       |-Vin 
 *                   RXD-|       |-GND to sensor VL53L1X
 *                   RST-|       |-RST
 *                   GND-|       |-+5V to sensor VL53L1X
 *     PARALLEL_PORT  D2-|       |-A7
 *     PARALLEL_PORT  D3-|       |-A6
 *     PARALLEL_PORT  D4-|       |-A5 (SCL) VL53L1X
 *     PARALLEL_PORT  D5-|       |-A4 (SDA) VL53L1X
 *     PARALLEL_PORT  D6-|       |-A3
 *     PARALLEL_PORT  D7-|       |-A2
 *     PARALLEL_PORT  D8-|       |-A1
 *     PARALLEL_PORT  D9-|       |-A0
 *                   D10-|       |-Ref
 *                   D11-|       |-3.3V
 *                   D12-|       |-D13  (VL53L1X) XSHUT_PIN
 *                        --USB--
 */


const uint16_t BAUDERATE = 9600;
const uint8_t OUTPUT_PARALLEL_PORT = 8;
const int outputPins[OUTPUT_PARALLEL_PORT] = {D1, D2, D3, D4, D5, D6, D7, D8}; // Pins de sortie
const int MAX_DISTANCE = 500;
const int MIN_DISTANCE = 100;
const int ITERATION = 10;

float Current_distance;
int min_value;
int max_value;

void init_pinmode(){
  for (int i = 0; i < OUTPUT_PARALLEL_PORT; i++) {
      pinMode(outputPins[i], OUTPUT);
  }
  pinMode(XSHUT_PIN, OUTPUT);
}

void init_sensor(){

  Wire.begin(); // Initialize I2C communication
  Wire.setClock(400000); // Use 400 kHz I2C
  sensor.setTimeout(500); // Timeout for sensor operations (in ms)

  if (!sensor.init()){// Initialize the VL53L1X sensor
    Serial.println("the sensor is not connected");
    while (1); // Stay here if sensor initialization fails
  }
  
  sensor.setDistanceMode(VL53L1X::Medium);// Set distance measurement mode to Medium
  sensor.setROISize(16,16);//set minimum Range Of Interest 16x16 to 4x4
  //sensor.setROICenter(136);//Y=8,X=8--> = 1000 1000 = 136 to center the ROI in the middle
  sensor.setMeasurementTimingBudget(30000); // Set the timing budget (time per measurement in microseconds).
  sensor.startContinuous(30); // Start continuous measurements every 30 ms
  //read_ROI_XY_and_ROI_center();

}

int set_distance(int pin){
  int number = analogRead(pin);
  int mappedValue = map(number, 0, 1023, MIN_DISTANCE, MAX_DISTANCE); // Mapper de 0-1023 à 0-255
  return  mappedValue;
}


void num_to_parallel(int number , int min , int max){
    int mappedValue = map(number, min, max, 0, 255); // Mapper de 0-1023 à 0-255
    Serial.println(mappedValue);
    // Écrire la valeur binaire sur les pins de sortie
    for (int i = 0; i < 8; i++) {
        digitalWrite(outputPins[i], (mappedValue >> i) & 1); // Extraction du bit i
    }
    delay(100); // Petite pause pour éviter trop de rafraîchissements
}

/**
 * Reads the sensor multiple times and returns the average distance.
 * This helps smooth out noisy readings.
 *
 * @return The average distance measured by the sensor.
 */
float read_average_distance()
{
  float avg = 0;
  for(int i = 0; i < ITERATION; i++)
  {
    wdt_reset();               // Reset the watchdog timer for each reading
    float Current_distance = sensor.read();
    avg += Current_distance;
  }
  avg = avg / ITERATION;       // Compute the average distance
  return avg;
}

void read_ROI_XY_and_ROI_center(){
  uint8_t x = sensor.readReg(0x0080);
  uint8_t y = sensor.readReg(0x007F);
  Serial.println("read ROI coordinate: " + String(x) + " read center ROI: " + String(y));
}

void reset_sensor() {
  digitalWrite(XSHUT_PIN, LOW);  // Désactive the sensor
  delay(100);
}

#endif