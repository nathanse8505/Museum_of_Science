#include "FONCTION_AND_CONST.h"

void setup() {
  Serial.begin(BAUDERATE);      // Start serial communication at the specified baud rate
  init_pinmode();
  digitalWrite(XSHUT_PIN, HIGH); // active the sensor
  init_sensor();
  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
    
}

void loop() {

    min_value = set_distance(MIN_D);
    max_value = set_distance(MAX_D);
    Current_distance = sensor.read();
    Serial.println(Current_distance);
    num_to_parallel(Current_distance , min_value , max_value);
    
}


