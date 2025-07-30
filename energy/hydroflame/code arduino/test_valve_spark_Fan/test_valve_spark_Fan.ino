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
  Serial.println("init");
 
}



void loop() {
  
  if(PRESS_BUTTON_IGNITION()){
    digitalWrite(LED_ACTIVATION, HIGH);  // Turn on activation LED
    while(!check_water_level()){
      FILL_WATER();
    }
    Serial.println("fill water end");
    
    TURN_ON_CURRENT();
    Serial.println("hydro start");
    delay(10000);
    
    Serial.println("spark start");
    SPARK_ON();
    Serial.println("spark end");
    
    delay(60000);
    TURN_OFF_CURRENT();
    Serial.println("hydro end");
    ACTIVE_FAN();
    Serial.println("Fan start");
    delay(10000);
    DEACTIVE_FAN();
    Serial.println("Fan end");
    
    digitalWrite(LED_ACTIVATION, LOW);  // Turn on activation LED

  }
  
 

}
