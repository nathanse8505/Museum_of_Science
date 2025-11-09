#include <Arduino.h>

// --- A4988 Driver Pins ---
#define PIN_STEP  6   // STEP pin (sends step pulses to the driver)
#define PIN_DIR   7   // DIR pin (controls the rotation direction)
#define PIN_EN    8   // EN pin (enables/disables the driver)

// RST and SLEEP pins are permanently connected to 5V
// const uint8_t PIN_RST = 5V;
// const uint8_t PIN_SLEEP = 5V;

/*
*        ==========Arduino Nano pinout============== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V
 *                  D2-|       |-A7  
 *                  D3-|       |-A6  
 *                  D4-|       |-A5 
 *                  D5-|       |-A4 
 *        PIN_STEP  D6-|       |-A3
 *         PIN_DIR  D7-|       |-A2
 *          PIN_EN  D8-|       |-A1 BUTTON_COUNTERCLOCK
 *                  D9-|       |-A0 BUTTON_CLOCKWISE
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */


// --- Stepper Motor Parameters ---
const int STEPS_PER_REV = 200;  // Number of steps per revolution (in full step mode)
const int RPM           = 50;   // Target speed in revolutions per minute
const int MICROSTEPS    = 16;   // Microstepping setting: 1, 2, 4, 8, or 16

// --- Timing Parameters ---
const uint8_t STEP_PULSE_US  = 3; // STEP pulse width in microseconds (≥1 µs)
unsigned long stepIntervalUs = 0; // Time between two steps (in microseconds)
unsigned long lastStepUs     = 0; // Timestamp of the last step (in microseconds)
// --- timer left/right ---
const unsigned long TIME_ALL_DISTANCE = 18000;
unsigned long timer = 0;
bool flag_dir_L = HIGH;



// Calculates the time interval between two steps based on RPM and microstepping
void updateStepInterval() {
  // steps/s = RPM * (STEPS_PER_REV * MICROSTEPS) / 60
  // period (µs) = 1e6 / steps/s
  double stepsPerSecond = (double)RPM * (double)STEPS_PER_REV * (double)MICROSTEPS / 60.0;
  if (stepsPerSecond < 1.0) stepsPerSecond = 1.0;  // Avoid division by zero
  stepIntervalUs = (unsigned long)(1000000.0 / stepsPerSecond);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_STEP,  OUTPUT);
  pinMode(PIN_DIR,   OUTPUT);

  digitalWrite(PIN_STEP,  LOW);
  digitalWrite(PIN_DIR,   LOW);

  updateStepInterval();
  Serial.println("Stepper initialized");
  timer = millis();
}

// Generates one STEP pulse
void doOneStep() {
  digitalWrite(PIN_STEP, HIGH);
  delayMicroseconds(STEP_PULSE_US);
  digitalWrite(PIN_STEP, LOW);
}

void loop() {
  if (millis() - timer > TIME_ALL_DISTANCE) {
    timer = millis();
    flag_dir_L = !flag_dir_L;
    // Set rotation direction (invert HIGH/LOW if wiring reversed)
    digitalWrite(PIN_DIR,flag_dir_L); 
    //Serial.println("enter");
    //Serial.println(flag_dir_L);
  }
  // Generate steps at the correct frequency without blocking
  if (micros() - lastStepUs >= stepIntervalUs) {
      doOneStep();
      lastStepUs = micros();
    }
  

  // No delay() needed; step timing is controlled by micros()
}
