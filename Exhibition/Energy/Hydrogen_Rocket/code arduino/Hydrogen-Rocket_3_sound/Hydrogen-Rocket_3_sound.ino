/*
 * Current measure for Hydrogen rocket 
 * energy exhebition 2024 
 * A0 connected to ASC712T-05 Hall Effect Based Linear Current Sensor
 * code base on "Analog input, analog output, serial output"example see old"_not_used" tab
*/

#include "consts.h"  
#include "basic_routines.h"

void setup() {

  pinMode(IGNITION_BUTTON_IO, INPUT_PULLUP);
  pinMode(SPARK_IO, OUTPUT);
  pinMode(HYDROGEN_VALVE_IO, OUTPUT);
  pinMode(CURRENT_INPUT_IO, INPUT);
  pinMode(LANG_BUTTON_IO, INPUT_PULLUP);
  
  Serial.begin(BAUDRATE);  // initialize serial communications 
  delay (100);  // wait to make sure serial begin
  wdt_enable(WDTO_2S);
//  start_display();  // initialize display

  // Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  // Serial.println("START");

  last_measure_time = millis();  // reset measure timer 
  last_display_time = millis();  // reset display timer 
  fill_initial_rolling_average_value();  // fill initial rolling average vector

  // ignite on start to clear charge
  ignite();
  //IGNITE_WITH_SOUND();
  
}

void loop() {
  reset_watchdog();

  if(ready_sound){
    ready_sound = Check_Ignite_Serial();
  }

  if (millis() - last_display_time >= DISPLAY_INTERVAL_TIME) {

    if (charge >= AUTO_EXPLOSION_THRESHOLD) {
      ready_sound = true;

      if(!flag_lost_com){
        timer_lost_com = millis();
        flag_lost_com = true;
        }
      if (millis() - timer_lost_com >= TIME_LOST_COM) {
        ignite();
      }
    }
    if (PRESS_BUTTON_IGNITION() && check_charge()) {  // button pressed
      //ready_sound = true;
      ignite();
    }

    Serial.println(String(abs_avg_current_value) + " " + String(charge) + " " + ready_sound + " " + lang);
    last_display_time = millis(); //reset timer

  }
  
 
  
if (PRESS_BUTTON_LANG()) {  // pressed on language button
    lang = lang >= 2 ? 0 : (lang + 1);  // toggle language
  }
  
  if (millis() - last_measure_time >= MEASURE_INTERVAL_TIME) {
    sensor_value = analogRead(CURRENT_INPUT_IO);// read the analog in value (the current sensor output):
    avg_sensor_value  = filter.addSample(sensor_value);
    avg_current_value = READ_TO_CURRENT * (avg_sensor_value - ZERO_CURRENT_READ); // calculate current
    abs_avg_current_value = abs(avg_current_value);  // use absolute value
    
    if (abs_avg_current_value <= CURRENT_THRESHOLD) {
      abs_avg_current_value = 0 ;  // ignore low current       
    }
    
    charge += ((abs_avg_current_value * MEASURE_INTERVAL_TIME) / 1000);  // calulate acumulated charge 
    last_measure_time = millis();  //reset timer
    
  }

 
}