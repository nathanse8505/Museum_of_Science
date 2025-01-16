/*
 * Current measure for Hydrogen rocket 
 * energy exhebition 2024 
 * A0 connected to ASC712T-05 Hall Effect Based Linear Current Sensor
 * code base on "Analog input, analog output, serial output"example see old"_not_used" tab
*/

#include "consts.h"  
#include "display.h"
#include "basic_routines.h"

void setup() {
  pinMode(IGNITION_BUTTON_IO, INPUT_PULLUP);
  pinMode(SPARK_IO, OUTPUT);
  pinMode(HYDROGEN_VALVE_IO, OUTPUT);
  pinMode(CURRENT_INPUT_IO, INPUT);
  pinMode(LANG_BUTTON_IO, INPUT_PULLUP);
  
  Serial.begin(BAUDRATE);  // initialize serial communications 
  delay (100);  // wait to make sure serial begin

//  start_display();  // initialize display

  // Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  // Serial.println("START");

  last_measure_time = millis();  // reset measure timer 
  last_display_time = millis();  // reset display timer 
  fill_initial_rolling_average_value();  // fill initial rolling average vector

  // ignite on start to clear charge
  ignite();
}

void loop() {

  if (charge >= AUTO_EXPLOSION_THRESHOLD) {
    ignite();
    ignited_now = true;
  }

  if (check_ignition_button() && check_charge()) {  // button pressed
    delay(BOUNCE_TIME);  // wait antibounce time to make sure button release 
    ignite();
    ignited_now = true;
  }

  if (check_lang_button()) {  // pressed on language button
    lang = lang >= 2 ? 0 : (lang + 1);  // toggle language
  }
  
  if (millis() - last_measure_time >= MEASURE_INTERVAL_TIME) {
    sensor_value = analogRead(CURRENT_INPUT_IO);// read the analog in value (the current semso output):
    avg_sensor_value  = filter.addSample(sensor_value);
    avg_current_value = READ_TO_CURRENT * (avg_sensor_value - ZERO_CURRENT_READ); // calculate current
    abs_avg_current_value = abs(avg_current_value);  // use absolute value
    
    if (abs_avg_current_value <= CURRENT_THRESHOLD) {
      abs_avg_current_value = 0 ;  // ignore low current       
    }
    
    charge += ((abs_avg_current_value * MEASURE_INTERVAL_TIME) / 1000);  // calulate acumulated charge 
    last_measure_time = millis();  //reset timer
  }

  if (millis() - last_display_time >= DISPLAY_INTERVAL_TIME) {
    text = LOW_CHARGE_TEXT;

    if (charge >= MIN_CHARGE) {
    text = CAN_IGNITE_TEXT;
   }

   if (charge >= MAX_CHARGE) {
     text = MAX_CHARGE_TEXT;
    }
    
//    display_all();  // dispaly all data 
    Serial.println(String(abs_avg_current_value) + " " + String(charge) + " " + ignited_now + " " + lang);
    last_display_time = millis(); //reset timer

    ignited_now = false;  // reset ignite flag (after sending it 1 time if there has been an ignition)
  }
}