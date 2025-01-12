#include "CONSTANTS.h"
#include "fonction.h"
#include <avr/wdt.h>

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);          // Initialize serial communication
  // Configure pins as output
  pinMode(N_DIRECTION, OUTPUT);
  pinMode(P_DIRECTION, OUTPUT);
  pinMode(LED_CONTROL, OUTPUT);
  Serial.println("init");
  wdt_enable(WDTO_2S);
}


void loop() {
  // Condition: TIME_CLK signal must be greater than PULSE_POSITIVE
  while (analogRead(TIME_CLK) > analogRead(PULSE_POSITIVE)) {
    wdt_reset();
    digitalWrite(LED_CONTROL, HIGH); // Turn on the control LED

    // Read and convert analog values
    time_positive = (float)analogRead(PULSE_POSITIVE) / 1023 * cycle_max;
    cycle_time = (float)analogRead(TIME_CLK) / 1023 * cycle_max;

    Serial_disp();
    POSITIVE_CURRENT();
    wdt_reset();
    NEGATIVE_CURRENT();
    wdt_reset();


  }
  
  // Turn off the control LED when the condition is not met
  digitalWrite(LED_CONTROL, LOW);
  wdt_reset();
}

