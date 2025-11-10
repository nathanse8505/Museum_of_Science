#include "constants.h"
#include "Fonction.h"

/**
 * Arduino setup() function.
 * Initializes serial communication, configures pins, sets up the sensor,
 * and prepares initial distance values.
 */

void setup() {
  Serial.begin(BAUDERATE);
  pinMode(LANG_BUTTON_IO, INPUT_PULLUP);
  pinMode(XSHUT_PIN, OUTPUT);
  digitalWrite(XSHUT_PIN, HIGH);      // activate sensor power/shutdown pin

  Wire.begin();
  Wire.setClock(400000);              // 400 kHz I2C
  sensor.setTimeout(500);             // sensor timeout (ms)

  Serial.println(String(0.5) + " " + String(deltaTime) + " " + lang); // boot marker

  // Initialize the VL53L1X sensor
  if (!sensor.init()) {
    while (1) { /* stay here on init failure */ }
  }

  // ---- keep your original sensor timings/modes here (as you had before) ----
  // (Intentionally not inserting the alternative config block you asked to exclude)
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setROISize(8, 8);
  // sensor.setROICenter(136);
  sensor.setMeasurementTimingBudget(30000);
  sensor.startContinuous(30);
  // -------------------------------------------------------------------------

  // compute initial thresholds
  baseline     = INITIAL_DISTANCE;             // starting "rest" distance
  minDistance  = baseline - THRESHOLD;         // entry threshold
  smoothedDistance = 0.0f;

  //Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);

  wdt_enable(WDTO_4S);                          // enable watchdog (4 seconds)
  Tmr_rst_com = millis();
}

/**
 * Arduino loop() function.
 * Continuously reads the sensor, manages the lift motion logic, checks for
 * timeouts (watchdog, measurement resets), and handles language button presses.
 */
void loop() {
  wdt_reset();                                  // feed watchdog

  // --- robust distance reading ---
  smoothedDistance = getFilteredDistance();
  //Serial.println(smoothedDistance);

  // language button handling
  if (PRESS_BUTTON_LANG()) {
    lang = (lang >= 2) ? 0 : (lang + 1);
    Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
  }

  // --- adaptive baseline when idle (tracks slow drifts) ---
  if (!Lift_in_motion && start_try) {
    // only update baseline if we're close to it (no person/object present)
    if (abs(smoothedDistance - baseline) < 80.0f) {
      baseline = (1.0f - BETA) * baseline + BETA * smoothedDistance;
      minDistance = baseline - THRESHOLD;
    }
  }

  // --- hysteresis thresholds ---
  const float enterThresh = minDistance - HYST_MM;  // stricter to enter motion
  const float exitThresh  = minDistance + HYST_MM;  // easier to exit cooldown

  /*
   * ENTER condition (debounced with consecutive samples):
   * start timing only if K consecutive samples are below enterThresh.
   */
  if (!Lift_in_motion && start_try) {
    if (smoothedDistance < enterThresh) {
      belowCnt++;
    } else {
      belowCnt = 0;
    }

    if (belowCnt >= ENTER_COUNT) {
      Lift_in_motion = true;
      startTime = millis();
      meas_Tmr_rst = millis();
      belowCnt = 0;

      //Serial.println("enter begin");
      //Serial.println(smoothedDistance);
    }
  }

  /*
   * While the lift is in motion, keep reading and check:
   * - If traveled more than DELTA_MAX_DISTANCE -> compute horsepower
   * - If measurement timeout exceeded -> reset measurement
   */
  while (Lift_in_motion) {
    wdt_reset();
    smoothedDistance = getFilteredDistance();

    // reached required travel?
    if ((minDistance - smoothedDistance) > DELTA_MAX_DISTANCE) {
      deltaTime = (millis() - startTime); // total time
      P_Watt = (WEIGHT_KG_BALL * GRAVITY * DELTA_MAX_DISTANCE) / (float)deltaTime;
      horsepower = P_Watt / HORSEPOWER_CONVERSION;

      Lift_in_motion = false;  // end motion
      start_try = false;       // enter cooldown
      bouncingBallTimer = millis();

      //Serial.println("enter lift motion");
      Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
    }

    // took too long? (no max distance reached)
    if ((millis() - meas_Tmr_rst) > MEAS_RST_MS) {
      Lift_in_motion = false;
      start_try = true;        // full reset
      horsepower = 0;
      startTime = 0;
      meas_Tmr_rst = 0;
      bouncingBallTimer = 0;
      deltaTime = 0;

      Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
    }
  }

  /*
   * EXIT cooldown when:
   * - smoothedDistance is above exitThresh for M consecutive samples
   * - and cooldown time has elapsed (RST_BOUNCING_BALL)
   */
  if (!start_try) {
    if (smoothedDistance > exitThresh) {
      aboveCnt++;
    } else {
      aboveCnt = 0;
    }

    if (aboveCnt >= EXIT_COUNT && ((millis() - bouncingBallTimer) > RST_BOUNCING_BALL)) {
      start_try = true;
      horsepower = 0;
      startTime = 0;
      meas_Tmr_rst = 0;
      bouncingBallTimer = 0;
      deltaTime = 0;
      aboveCnt = 0;

      Serial.println(String(horsepower) + " " + String(deltaTime) + " " + lang);
    }
  }

  // ---------------- safety & recovery paths ----------------

  // if sensor kept a nonzero horsepower but nothing progressed => reset sensor
  if (((millis() - bouncingBallTimer) > SENSOR_FAULTY) && horsepower != 0) {
    reset_sensor();
    delay(RST_WDT);
  }

  // sensor timeout or zero reading => try to recover
  if (sensor.timeoutOccurred() || smoothedDistance == 0) {
    reset_sensor();
    delay(RST_WDT);
  }

  // periodic communication reset (long watchdog)
  if (millis() - Tmr_rst_com > RST_COMMUNICATION) {
    reset_sensor();
    delay(RST_WDT);
  }
}

