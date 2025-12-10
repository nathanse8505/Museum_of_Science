#include "const.h"
#include "basic_routine.h"


void setup() {
  // Configure pin modes
  pinMode(NTC_PIN, INPUT);       // Ignition button (active LOW)
  pinMode(IGNITION_BUTTON_IO, INPUT_PULLUP);       // Ignition button (active LOW)
  pinMode(SENSOR_PRESSURE_HYDRO_IO, INPUT_PULLUP); // Ignition button (active LOW)
  pinMode(SENSOR_WATER_IO, INPUT_PULLUP);          // Water level sensor

  pinMode(SPARK_IO, OUTPUT);                       // Spark output
  pinMode(TURN_ON_OFF_CURRENT_IO, OUTPUT);           // Power relay
  pinMode(FAN_IO, OUTPUT);                         // Fan relay
  pinMode(LED_BUTTON_IO, OUTPUT);                 // Activation indicator LED
  pinMode(VALVE_WATER_IO, OUTPUT);            // Water valve control

  Serial.begin(BAUDRATE);                          // Start serial communication
  delay(100);                                      // Give time to establish connection
   
  
  //readTemperature();
  if(!init_current_sensor()){
    Serial.println("reset the arduino");
    while(1);
  }

  Serial.println("init");
  wdt_enable(WDTO_4S);  
}

void loop() {

  wdt_reset();
  // Check if ignition button is pressed

  buttonPressed = true;
  //buttonPressed =PRESS_BUTTON_IGNITION();


  // After some time, move on to hydrogen and ignition phase
  if ((millis() - time_start) > ACTIVATION_TIME) {
    
    // If it's the first entry into this phase
    if(flag_new_session){
      DEACTIVE_FAN(); 
      Serial.println("THE FAN IS OFF");
      // Wait until the water tank is full
      while(check_water_level() == false){
        wdt_reset();
        FILL_WATER();
        counter_fill_water++;
        Serial.println(String(counter_fill_water) + " fill water");
        if (counter_fill_water == MAX_NUMBER_FILL_WATER){
          while(1){
            wdt_reset();
            BLINK_LED_STATE(ERROR_WATER_DELAY);
          }
        }
      }

      digitalWrite(LED_BUTTON_IO, HIGH);  // Turn on activation LED  
      flag_ready_fan = false;                   // Turn off fan
      flag_new_session = false;             // new Session ready
      counter_fill_water = 0;
      Serial.println("we can push the button");
      Serial.println("led activated");
      Serial.println();
    }

    // On first button press (ignition)
    if (buttonPressed && flag_first_press == false) {
      digitalWrite(LED_BUTTON_IO, LOW);   // Turn off activation LED
      TURN_ON_CURRENT();                   // Start hydrogen generation
      time_start_hydro = millis();         // Start hydrogen timer
      time_read_current = time_start_hydro; // Start current timer
      flag_first_press = true;            // Mark ignition as pressed
      flag_spark_ready = true;             // Ready to spark
      //Serial.println("button has been pressed");
      Serial.println("the hydro start");
    }
    /*
    if(high_pressure_hydro() && flag_first_press){
      reset_session();
    }*/

    // If no current detected after first press, turn off system
    
    if ((millis() - time_read_current) >  CURRENT_INTERVAL_TIME  && flag_spark_ready) {
      time_read_current = millis();//timer to read current every CURRENT_INTERVAL_TIME
      current_flag = current_valid();//check and print current
      if(current_flag != 2 && flag_first_press == true){
        reset_session();
        if(current_flag == 0){
          Serial.println("reset session because No communication with the current sensor");
          Serial.println("reset the arduino");
          while(1){
            wdt_reset();
            BLINK_LED_STATE(ERROR_CURRENT_DELAY);
          }
        }
        if(current_flag == 1){
          Serial.println("reset session because no current");
        }
        
      }
    }

    // If enough hydrogen time passed and spark is ready, activate spark
    if ((millis() - time_start_hydro) > HYGROGEN_TIME  && flag_spark_ready) {
      SPARK_ON();                       // Spark ignition
      flag_spark_ready = false;          // No more sparks until reset
      time_new_session = millis();      // Track session time
      first_temp = true;
      Serial.println("the spark has been activated");
    }
    
    // If no fire detected after spark, turn off current
    if (millis() - time_new_session > FIRE_TEMP_TIME && flag_first_press == true && flag_spark_ready == false){
      if (millis() - lastReadTemp >= DELAY_TEMP) {
        lastReadTemp = millis();
        //current_valid();
        if(detect_drop_temp()){
          Serial.println("reset session because no fire");
          reset_session();
        }
      }
    }

    // If session timeout reached after spark, shut down and reset
    if (millis() - time_new_session > SESSION_TIME && flag_first_press == true && flag_spark_ready == false){
      average_current = 0;
      read = 0;
      first_time_delta = true;
      first_time_current = true;
      detect_drop_temp();//to read the DELTA TEMPERATURE and the TEMPERATURE
      Serial.println("end session");
      reset_session();
      flag_ready_fan = true;
    }
    delay(1);
  }

  // If it's a new session, activate the fan
  if(flag_ready_fan){
     ACTIVE_FAN();
     
  }

}
