#include "constants.h"
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

  Wire.begin();                           // Initialize I2C communication

  Wire.setClock(400000);                 // Use 400 kHz I2C
  sensor.setTimeout(500);                // Timeout for sensor operations (in ms)

  Serial.println(String(0.5) + " " + String(deltaTime) + " " + lang);//sensor disconnect

  // Initialize the VL53L1X sensor
  if (!sensor.init())
  {
    while (1); // Stay here if sensor initialization fails
  }

  
  sensor.setDistanceMode(VL53L1X::Medium);// Set distance measurement mode to Medium
  sensor.setROISize(4,4);//set minimum Range Of Interest 16x16 to 4x4
  //sensor.setROICenter(136);//Y=8,X=8--> = 1000 1000 = 136 to center the ROI in the middle
  sensor.setMeasurementTimingBudget(30000); // Set the timing budget (time per measurement in microseconds).
  sensor.startContinuous(30); // Start continuous measurements every 30 ms
  //read_ROI_XY_and_ROI_center();

  // Read and set the initial distance (this will be our baseline)
  initialDistance = read_average_distance();
  
  // Calculate the minimum distance threshold by subtracting THRESHOLD
  minDistance = initialDistance - THRESHOLD;

  // Print the initial horsepower and current language index to Serial
  Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
   
  // Enable the watchdog timer with a 4-second timeout
  wdt_enable(WDTO_4S);
}

/**
 * Arduino loop() function.
 * Continuously reads the sensor, manages the lift motion logic, checks for
 * timeouts (watchdog, measurement resets), and handles language button presses.
 */
void loop()
{
  // Reset the watchdog timer to prevent a system reset
  wdt_reset();

  // Check if the language button has been pressed and released
  if (PRESS_BUTTON_LANG())
  {
    // Cycle the language index: 0 -> 1 -> 2 -> 0 ...
    lang = (lang >= 2) ? 0 : (lang + 1);

    // Print updated horsepower and language index
     Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
  }

  // Read the current distance from the sensor
  smoothedDistance = sensor.read();

  /**
   * If the current distance is less than the minDistance (lift is moving upward),
   * and the lift is NOT already in motion, and it's not the first try (i.e., cooldown ended),
   * we start timing the lift movement.
   */
  if (smoothedDistance < minDistance && Lift_in_motion == false && first_try == false)
  {
    Lift_in_motion = true;
    startTime = millis();      // Mark the start time of this motion
    meas_Tmr_rst = millis();   // Also mark for measurement reset check
  }

  /**
   * While the lift is in motion, keep reading the sensor and
   * check if we've reached the max distance or the measurement timeout.
   */
  while (Lift_in_motion)
  {
    // Reset watchdog to avoid system reset
    wdt_reset();

    // Read the sensor distance again
    smoothedDistance = sensor.read();

    // If the lift has traveled more than `maxDistance`, compute horsepower
    if ((minDistance - smoothedDistance) > maxDistance)
    {
      deltaTime = (millis() - startTime); // Calculate total time taken
      // Compute horsepower using a simplified formula
      horsepower = ((WEIGHT_KG_BALL / WEIGHT_KG_POWER_HORSE) * maxDistance) / (deltaTime);

      // End the lift motion, start cooldown
      Lift_in_motion = false;
      first_try = true;               // Indicates we're now in cooldown
      bouncingBallTimer = millis();   // Record when cooldown started

      // Print the computed horsepower and current language index
      Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
    }

    // If it takes too long (exceeding MEAS_RST_MS), reset the measurement
    if ((millis() - meas_Tmr_rst) > MEAS_RST_MS)
    {
      Lift_in_motion = false;   // Stop the lift
      first_try = false;        // Indicate a full reset
      horsepower = 0;
      startTime = 0;
      meas_Tmr_rst = 0;
      bouncingBallTimer = 0;
      deltaTime = 0;
      Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
    }
  }

  /**
   * If the distance is now greater than minDistance (meaning the ball is back down),
   * and we are in the cooldown phase (`first_try == true`),
   * and we have waited long enough (`RST_BOUNCING_BALL`),
   * then end the cooldown and reset for the next lift attempt.
   */
  if (smoothedDistance > minDistance && first_try && ((millis() - bouncingBallTimer) > RST_BOUNCING_BALL))
  {
    first_try = false;    // Ready for next measurement
    horsepower = 0;       // Reset horsepower to 0
    startTime = 0;
    meas_Tmr_rst = 0;
    bouncingBallTimer = 0;
    deltaTime = 0;           // reset to 0
    Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
  }
}
