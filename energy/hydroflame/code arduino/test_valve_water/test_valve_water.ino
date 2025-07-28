#include "const.h"
#include "basic_routine.h"

void setup() {
  // Configure pin modes
  
  pinMode(CURRENT_INPUT_IO, INPUT);                // Current sensor input
  pinMode(IGNITION_BUTTON_IO, INPUT_PULLUP);       // Ignition button (active LOW)
  pinMode(SPARK_IO, OUTPUT);                       // Spark output
  pinMode(SENSOR_WATER_IO, INPUT_PULLUP);          // Water level sensor
  pinMode(RELAY_TURN_ON_OFF_IO, OUTPUT);           // Power relay
  pinMode(FAN_IO, OUTPUT);                         // Fan relay
  pinMode(LED_ACTIVATION, OUTPUT);                 // Activation indicator LED
  pinMode(RELAY_VALVE_WATER_IO, OUTPUT);                 // Water valve control
  Serial.begin(115200);                          // Start serial communication
  delay(100);
 
}



void loop() {
  if(PRESS_BUTTON_IGNITION()){
    digitalWrite(LED_ACTIVATION, HIGH);  // Turn on activation LED
    FILL_WATER();
    SPARK_ON();
    ACTIVE_FAN();
    DEACTIVE_FAN();
    TURN_ON_CURRENT();
    TURN_OFF_CURRENT();
    digitalWrite(LED_ACTIVATION, LOW);  // Turn on activation LED

  }
  
 

}
