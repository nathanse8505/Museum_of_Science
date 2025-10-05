#include "const.h"
#include "basic_routine.h"
#define TEST

void setup() {
  // Configure pin modes
  pinMode(SENSOR_WATER_IO, INPUT_PULLUP);          // Water level sensor
  pinMode(RELAY_VALVE_WATER_IO, OUTPUT);                 // Water valve control
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(BAUDRATE);                          // Start serial communication  
  Serial.println("init");
}

void loop() {

  // Wait until the water tank is full
  while(check_water_level() == NO_DETECTION){
    digitalWrite(LED_BUILTIN, HIGH);
    FILL_WATER();
    #ifdef TEST
      Serial.println(analogRead(SENSOR_WATER_IO));    
      delay(10);
    #endif 
  }
   digitalWrite(LED_BUILTIN, LOW);
  
  #ifdef TEST
  if(check_water_level() == !NO_DETECTION){
    Serial.println(analogRead(SENSOR_WATER_IO));    
    delay(10);
  }
  #endif

}
