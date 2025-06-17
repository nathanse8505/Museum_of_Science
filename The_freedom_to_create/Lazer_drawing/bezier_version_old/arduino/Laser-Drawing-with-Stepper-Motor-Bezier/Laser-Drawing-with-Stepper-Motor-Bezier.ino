#include "Parser.h"

void setup() {
  Serial.begin(BAUDRATE);
  delay (1000);// wait to make sure serial begin 
  
 
  pinMode(IS_EN_JUMPER,INPUT_PULLUP);// jumper to GND for constant hold/active  motors    
  pinMode(STEPPER_EN_PIN, OUTPUT);// invert !
  pinMode(LED_OUT, OUTPUT);// 
  digitalWrite(LED_OUT, LOW);  //off led
  Blink_Led(START_LED_BLINK_NUMBER,START_LED_BLINK_ON,START_LED_BLINK_OFF);
  digitalWrite(STEPPER_EN_PIN, HIGH);  //disable    
  if (digitalRead(IS_EN_JUMPER)== 0) {digitalWrite(STEPPER_EN_PIN, LOW);}   //enable if jumper set 
  for(uint8_t i=0; i< NUMBER_OF_MOTORS; ++i){
    pinMode(DIR_PIN[i],OUTPUT);
    digitalWrite(DIR_PIN[i],LOW);// initialy set direction to low/0
    pinMode(STEP_PIN[i],OUTPUT);
    digitalWrite(STEP_PIN[i],LOW);// pulse low 
  }
//  for (uint8_t i=0; i<NUMBER_OF_MOVES; i++) {
//    pinMode(MOVE_PIN[i],INPUT_PULLUP);    
//  }
  pinMode(DC_MOTOR_OUT,OUTPUT);
  digitalWrite(DC_MOTOR_OUT,LOW);
  Homming();
  set_steps(0,0);// set (define) current (steps) position 
  set_position(0, 0);
  update_rates();
  
  // Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  Serial.println("START");
  // if (!py_flag)
  //  print_help_menu();
  curve_index = 0;
  step_index = 0;
  num_of_curves = 0;
  num_of_contour = 0;
  got_param = false;
  param_index = 0;
}

void loop() {

  //  read_pushbuttons();
  if (!py_flag && get_in_command()>0){
    process_in_command();
    in_command ="";
  }

// get all the initial parameters from python. The first 4 are the border values of the drawing area (X_left, Y_up, X_right, Y_down) in the screen (the grey box on the python)
  if (py_flag && !got_param && Serial.available()) {
    float value;
    Serial.readBytes((char *)&value, sizeof(value)); // Read the float value from serial
    Serial.println(value);
    if (param_index < 4) {
      border[param_index] = value;
    }
    else {
      switch(param_index) {
        case 4: LASER_ON_POWER = value; update_scales(); break;
        case 5: CONTOUR_POWER = value; break;
        case 6: LASER_OFF_RATE = value; break;
        case 7: LASER_ON_RATE = value; break;
        case 8: CONTOUR_RATE = value; break;
        case 9: MAX_DC_MOTOR_TIME = value; break;
        default: Serial.println("ERROR IN GETTING PARAMETERS"); break;
      }
    }
    param_index++;
    if (param_index >= PARAM_NUMBER) {
      got_param = true;
      param_index = 0;
      Serial.println("got all parameters - Im good to go");
    }
  }

  update_rates();
  if (py_flag)
    check_dc_motor();

  if (!Is_destination_done) {
    read_destination();
  }

  // to change !py_flag to py_flag
  if (py_flag && !drawing_curve && got_param && (num_of_contour == 0 || num_of_curves == 0) && Serial.available()) {
    float value;
    Serial.readBytes((char *)&value, sizeof(value)); // Read the float value from serial
    // if (value != 0.00)
    //   Serial.println(value);
    if (!start_flag) {
      // if arduino didnt get a starting key, check if it did now
      if (abs(value-starting_key) <= tolerance_float) {
      // arduino got a starting key from the python script
        start_flag = true;
        curve_index = 0;
        dc_motor_off();
        led_off();
        rate = LASER_OFF_RATE;
      }
    }
    else {
      // if already got a starting key, check number of curves (it is negative to distinguish between this and the curve's points)
      if (value < 0 && num_of_curves == 0)
        num_of_curves = -round(value);
      else if (value < 0 && num_of_contour == 0) 
        num_of_contour = -round(value);
    }
  }
  
  if (py_flag && num_of_curves > 0 && num_of_contour > 0 && Is_destination_done && !drawing_curve && Serial.available()) {
    // read points for curve
    float value;
    delay(TIME_DELAY_ARDUINO);
    for (int i = 0; i < points_per_curve; i++) {
        Serial.readBytes((char *)&value, sizeof(value));
        curves[i] = (value-border[1-i%2])* screen_scale[1-i%2]; // * mm_per_pixel[1-i%2]
    }
    Serial.println(finished_reading_key);    
    compute_bezier_variable(0);
    led_off();
    rate = LASER_OFF_RATE;
    set_destination(bezier_point[0],bezier_point[1]);
    Is_destination_done = false;
    drawing_curve = true;
  }
 
  if (py_flag && num_of_curves > 0 && num_of_contour > 0 && Is_destination_done && drawing_curve) {
    if (compute_next_bezier_point()) {
      if (in_destination()) {
        if (num_of_curves <= num_of_contour) {
          rate = CONTOUR_RATE;
          laser_power = CONTOUR_POWER;
        }
        else {
          rate = LASER_ON_RATE;
          laser_power = LASER_ON_POWER;
        }
        led_on();
        if (step_index == 1 && num_of_curves <= num_of_contour) {
          delay(TIME_DELAY_FOR_CONTOUR_CUTTING);
        }
      }
      else {
        led_off();
        rate = LASER_OFF_RATE;
      }
      set_destination(bezier_point[0],bezier_point[1]);
      Is_destination_done = false;
    }
    else {
      if (num_of_curves <= num_of_contour)
        delay(TIME_DELAY_FOR_CONTOUR_CUTTING);
      led_off();
      rate = LASER_OFF_RATE;
      drawing_curve = false;
//      curve_index++;
      num_of_curves--;
      if (num_of_curves > 0) {
        Serial.println(next_curve_key);
        delay(TIME_DELAY_ARDUINO);
      }
      else {
        float value;
        Serial.println(end_key);
        delay(TIME_DELAY_ARDUINO);
        Serial.readBytes((char *)&value, sizeof(value)); // Read the float value from serial
//        Serial.println(value);
        if (abs(value-end_key) <= tolerance_float) {
          start_flag = false;
          drawing_curve = false;
          num_of_contour = 0;
          for (int i = 0; i < points_per_curve*MAX_CURVES; i++)
            curves[i] = 0;
          set_destination(0,0);
          Is_destination_done = false;
          // Serial.println("Done!");
        }
        else {
          Serial.println("ERROR - DIDNT GET END KEY");
        }
        dc_motor_on();
        last_time_dc_motor = millis();
      }
    }   
  }
}
