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

  Serial.begin(BAUDRATE);                          // Start serial communication
  delay(100);                                      // Give time to establish connection
  wdt_enable(WDTO_4S);   
  setZeroCurent();                          // Enable watchdog timer with 2-second timeout
  thermocouple.begin();
}

void loop() {

  wdt_reset();

  // Check if ignition button is pressed
  buttonPressed = PRESS_BUTTON_IGNITION();

  // Start a new session
  flag_new_session = true;

  // Wait until the water tank is full
  while(check_water_level() == false){
    FILL_WATER();
  }

  // After some time, move on to hydrogen and ignition phase
  if ((millis() - time_start) > ACTIVATION_TIME) {
    
    // If it's the first entry into this phase
    if(flag_new_session){
      digitalWrite(LED_ACTIVATION, HIGH);  // Turn on activation LED
      DEACTIVE_FAN();   
      flag_ready_fan = false;                   // Turn off fan
      flag_new_session = false;             // new Session ready
      
    }

    // On first button press (ignition)
    if (buttonPressed && flag_first_press == false) {
      digitalWrite(LED_ACTIVATION, LOW);  // Turn off activation LED
      TURN_ON_CURRENT();                  // Start hydrogen generation
      time_start_hydro = millis();        // Start hydrogen timer
      flag_first_press = true;           // Mark ignition as pressed
      ready_flag_fire = true;            // Ready to spark
    }

    /*// If no current detected after first press, turn off system
    if(check_current() == false && flag_first_press == true){
      reset_session();
    }*/

    // If enough hydrogen time passed and spark is ready, activate spark
    if ((millis() - time_start_hydro) > HYGROGEN_TIME  && ready_flag_fire) {
      SPARK_ON();                       // Spark ignition
      ready_flag_fire = false;          // No more sparks until reset
      time_new_session = millis();      // Track session time
    }

    // If no fire detected after spark, turn off current
    if (millis() - time_new_session > FIRE_TIME && flag_first_press == true && ready_flag_fire == false && !check_fire()){
        reset_session();
    }

    // If session timeout reached after spark, shut down and reset
    if (millis() - time_new_session > SESSION_TIME && flag_first_press == true && ready_flag_fire == false && check_fire()){
        reset_session();
        flag_ready_fan = true;
    }
  }

  // If it's a new session, activate the fan
  if(flag_ready_fan){
     ACTIVE_FAN();
  }

}
