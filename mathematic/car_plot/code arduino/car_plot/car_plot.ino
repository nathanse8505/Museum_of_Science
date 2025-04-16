#include "FONCTION_AND_CONST.h"

void setup() {
  //Serial.begin(BAUDERATE);      // Start serial communication at the specified baud rate
  init_pinmode();
  digitalWrite(XSHUT_PIN, HIGH); // active the sensor
  init_sensor();
  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
    
}

void loop() {

    min_value = MIN_DISTANCE;
    max_value = MAX_DISTANCE;
    Current_distance = sensor.read();
    int num = num_to_parallel(Current_distance , min_value , max_value);
    //Serial.println("Current_distance: " + String(Current_distance) + " integer: " + String(num));
    delay(70); // Petite pause pour éviter trop de rafraîchissements
    wdt_reset();
}


