#include "Fonction.h"

/**
 * Arduino setup() function.
 * Initializes serial communication, configures pins, sets up the sensor,
 * and prepares initial distance values.
 */
void setup()
{
  Serial.begin(BAUDERATE);            // Start serial communication at the specified baud rate
  pinMode(LANG_BUTTON_IO, INPUT_PULLUP); // Configure the language button pin as an input with pull-up
  pinMode(XSHUT_PIN, OUTPUT);
  digitalWrite(XSHUT_PIN, HIGH); // active the sensor
  
  Wire.begin();                           // Initialize I2C communication

  Wire.setClock(400000);                 // Use 400 kHz I2C
  sensor.setTimeout(500);                // Timeout for sensor operations (in ms)

  Serial.println(String(0.4) + " " + String(0) + " " + String(0));//sensor disconnect

  // Initialize the VL53L1X sensor
  if (!sensor.init()){
    while (1); // Stay here if sensor initialization fails
  }

  
  sensor.setDistanceMode(VL53L1X::Medium);// Set distance measurement mode to Medium
  sensor.setROISize(8,8);//set minimum Range Of Interest 16x16 to 4x4
  //sensor.setROICenter(136);//Y=8,X=8--> = 1000 1000 = 136 to center the ROI in the middle
  sensor.setMeasurementTimingBudget(30000); // Set the timing budget (time per measurement in microseconds).
  sensor.startContinuous(30); // Start continuous measurements every 30 ms
  //read_ROI_XY_and_ROI_center();
  
  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
}

/**
 * Arduino loop() function.
 * Continuously reads the sensor, manages the lift motion logic, checks for
 * timeouts (watchdog, measurement resets), and handles language button presses.
 */
void loop()
{
  
  wdt_reset();// Reset the watchdog timer to prevent a system reset
  smoothedDistance = sensor.read();// Read the current distance from the sensor
  Serial.println(smoothedDistance);// Print the initial horsepower and current language index to Serial
  
}
