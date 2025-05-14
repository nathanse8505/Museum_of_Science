#include "FONCTION_AND_CONST.h"

void setup() {
  Serial.begin(BAUDERATE);      // Start serial communication at the specified baud rate
  init_pinmode();
  digitalWrite(XSHUT_PIN, HIGH); // active the sensor
  init_sensor();
  wdt_enable(WDTO_4S);// Enable the watchdog timer with a 4-second timeout
    
}

void loop() {

    Current_distance = sensor.read();
    int num = map_distance_to_python(Current_distance);
    Serial.println(num);
    
    //int num = num_to_parallel(Current_distance , MIN_DISTANCE , MAX_DISTANCE);
    //Serial.println("Current_distance: " + String(Current_distance) + " integer: " + String(num));
    
    delay(170); // Petite pause pour éviter trop de rafraîchissements
    wdt_reset();
}


