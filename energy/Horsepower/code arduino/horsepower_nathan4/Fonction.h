#ifndef Fonction
#define Fonction

#include "constants.h"
#include <Wire.h>
#include <VL53L1X.h>
#include <avr/wdt.h>

// Global sensor object declared here for use in read_average_distance()
VL53L1X sensor;


/**
 * Checks if the language button has been pressed and released.
 *
 * @return true if the button was fully pressed and released; false otherwise.
 */
bool PRESS_BUTTON_LANG(){
  // Detect button press (active LOW) if previously unpressed
  if (digitalRead(LANG_BUTTON_IO) == LOW && check_lang == LOW){
    check_lang = HIGH;          // Mark the button as pressed
    delay(BOUNCE_TIME);         // Debounce delay
  }

  // Detect button release (goes HIGH) if it was previously pressed
  if (digitalRead(LANG_BUTTON_IO) == HIGH && check_lang == HIGH){
    check_lang = LOW;           // Reset the state for the next press
    return true;                // Indicate a valid press-release cycle
  }
  return false;                 // Otherwise, no valid press-release detected
}

void read_ROI_XY_and_ROI_center(){
  uint8_t x = sensor.readReg(0x0080);
  uint8_t y = sensor.readReg(0x007F);
  Serial.println("read ROI coordinate: " + String(x) + " read center ROI: " + String(y));
}

void reset_sensor() {
  digitalWrite(XSHUT_PIN, LOW);  // Déactive the sensor
  delay(100);
}



// returns median of 5 values (simple bubble sort; tiny and sufficient here)
static uint16_t median5(uint16_t *w) {
  uint16_t t[MEDIAN_WIN];
  for (int i = 0; i < MEDIAN_WIN; i++) t[i] = w[i];

  for (int i = 0; i < MEDIAN_WIN - 1; i++) {
    for (int j = 0; j < MEDIAN_WIN - 1 - i; j++) {
      if (t[j] > t[j + 1]) {
        uint16_t tmp = t[j];
        t[j] = t[j + 1];
        t[j + 1] = tmp;
      }
    }
  }
  return t[MEDIAN_WIN / 2];
}

// read one raw sample and validate it
static bool readValid(uint16_t &mm) {
  // NOTE: if your VL53L1X library exposes dataReady()/range_status, use them here.
  uint16_t d = sensor.read();
  if (sensor.timeoutOccurred() || d == 0) return false;
  // Optional (library-dependent):
  // if (sensor.ranging_data.range_status != 0) return false;

  mm = d;
  return true;
}

// produce a robust, filtered distance in mm using median + step limiter + mild IIR
static float getFilteredDistance() {
  uint16_t raw;
  if (!readValid(raw)) {
    // keep last good value if current read is invalid
    return smoothedDistance;
  }

  ring[ringIdx++] = raw;
  if (ringIdx >= MEDIAN_WIN) {
    ringIdx = 0;
    ringFilled = true;
  }

  uint16_t med = ringFilled ? median5(ring) : raw;

  // limit the step between consecutive filtered outputs
  // (prevents single spikes from jumping too far in one loop)
 
  float prev = smoothedDistance;
  float candidate = (prev == 0.0f)
                      ? (float)med
                      : (abs((int)med - (int)prev) > MAX_STEP
                           ? prev + ((med > prev) ? MAX_STEP : -MAX_STEP)
                           : (float)med);

  // mild IIR smoothing (ALPHA close to 1.0 = less smoothing)
  return (prev == 0.0f) ? candidate : (IIR_ALPHA * candidate + (1.0f - IIR_ALPHA) * prev);
}

#endif
