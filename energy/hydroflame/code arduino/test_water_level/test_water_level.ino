#include "const.h"
#include "basic_routine.h"


void setup() {
  // Configure pin modes
  pinMode(SENSOR_WATER_IO, INPUT_PULLUP);          // Water level sensor
  pinMode(RELAY_VALVE_WATER_IO, OUTPUT);                 // Water valve control
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(BAUDRATE);                          // Start serial communication  
}

void loop() {

  // Wait until the water tank is full
  while(check_water_level() == false){
    digitalWrite(LED_BUILTIN, HIGH);
    FILL_WATER();
  }
   digitalWrite(LED_BUILTIN, LOW);

}
