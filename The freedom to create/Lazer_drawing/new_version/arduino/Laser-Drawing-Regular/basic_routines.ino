#include "basic_routines.h"

// read pushbuttons/joystick and move the motors acording 
void read_pushbuttons(){
  for (uint8_t i = 0; i < NUMBER_OF_MOVES; i++) {
    if (digitalRead(MOVE_PIN[i]) == 0) {  // check move (direction 0/false) pushbutton
      if (is_position_valid(current_position[0] + (i == 0 ? mm_per_pulse[i/2] : ( i == 1 ? (-mm_per_pulse[i/2]) : 0))
          , current_position[1] + ( i== 2 ? (mm_per_pulse[i/2]) : (i == 3 ? (-mm_per_pulse[i/2]) : 0)))) {
        digitalWrite(STEPPER_EN_PIN, LOW);  // enable motors 
        if (millis() >= last_pulse_time[i/2] + rates[i]){  // time for another pulse 
          one_step(i/2, i%2);  // make another pulse, odd or even direction False(+), True(-), False, True, False, True
          last_pulse_time[i/2] = millis();  // reset couner for continue steps rate 
        }
      }
    }
  }
}


// if not in destination, move one step towards it (with proportional speeds for each motor)
void move_to_destination() {
  if (in_destination()) { 
    is_destination_done = true;
    return; 
  }
  
  update_delta();
  if (abs(delta[0]) <= TOLERANCE) {  // X axis is good, move only with Y
    prop_rates[2] = rates[2];  // get rate from potentiometer
  }
  else if (abs(delta[1]) <= TOLERANCE) {
    prop_rates[0] = rates[0];  // Y axis is good, move only with X
  }
  else {  // calculate which axis needs the faster rate based on the slope
    if (abs(delta[0]) > abs(delta[1])) {
      prop_rates[0] = rates[0];
      prop_rates[2] = rates[2] * abs(delta[0] / delta[1]);
    }
    else {
      prop_rates[2] = rates[2];
      prop_rates[0] = rates[0] * abs(delta[1] / delta[0]);
    }
  }
  
  for(uint8_t i = 0; i < NUMBER_OF_MOTORS; i++){ 
    if (abs(delta[i]) > TOLERANCE) { 
      digitalWrite(STEPPER_EN_PIN, LOW);  //enable motors
      if (millis() >= last_pulse_time[2*i] + prop_rates[2*i]) {  // time for another pulse 
        if (delta[i] > 0 ? (current_position[i] < MAX_POSITION[i]) : (current_position[i] > MIN_POSITION[i])) {
          one_step(i, (delta[i] > 0 ? false : true));  // make another pulse, odd or even direction False(+), True(-), False, True, False, True
          last_pulse_time[2*i] = millis();  // reset couner for continue steps rate 
        }
        else
          is_destination_done = true;
      }
    }
  } 
}


// set current steps
void set_steps(int16_t Start_X, int16_t Start_Y) {
  current_steps[0] = Start_X;
  current_steps[1] = Start_Y;
}

// set current position
void set_position(float current_X, float current_Y) {
  current_position[0] = current_X;
  current_position[1] = current_Y;
}

// set destionation
void set_destination(float x, float y) {
  destination[0] = x;
  destination[1] = y;
}


// print steps
void print_step()
{
  char buffer[100];
  sprintf(buffer, "Steps[XY pulses]: %d,%d" , current_steps[0] , current_steps[1]);
  Serial.println(buffer);  
}

// print destination
void print_destination() {
  Serial.print("Destination[XY mm]: ");
  Serial.print(destination[0],3);
  Serial.print(", ");
  Serial.println(destination[1],3);
}

// print position
void print_position()
{
  Serial.print("Position[XY mm]: ");
  Serial.print(current_position[0],3);
  Serial.print(", ");
  Serial.println(current_position[1],3);
}

// print status
void print_status() {
  Serial.println("================STATUS=====================");
  print_step();  
  print_position();
  print_destination();
}


// check if laser reached the destination
bool in_destination() {
  for (uint8_t i = 0; i < NUMBER_OF_MOTORS; i++) {
    if (abs(current_position[i] - destination[i]) > TOLERANCE)
      return false;
  }
  return true;
}


// check if destination is in boundries
bool is_destination_valid() {
  for (uint8_t i = 0; i < NUMBER_OF_MOTORS; i++) {
    if (destination[i] < MIN_POSITION[i] || destination[i] > MAX_POSITION[i])
      return false;
  }
  return true;
}

// check if position is in boundries
bool is_position_valid(float x, float y) {
  if (x < MIN_POSITION[0] || x > MAX_POSITION[0] || y < MIN_POSITION[1] || y > MAX_POSITION[1])
    return false;
  return true;
}

// limits int16_t value between min and max
int16_t max_min_limit(int16_t value, int16_t min, int16_t max)
{
  int16_t temp = min(value, max);
  temp = max(temp, min);
  return temp;
}

// limits steps between global min and max
void limit_steps(){
    for(uint8_t i=0; i< NUMBER_OF_MOTORS; ++i){
      current_steps[i] = max_min_limit(current_steps[i], MIN_STEPS[i], MAX_STEPS[i]);
    }
}


// move a motor one step
void one_step(uint8_t motor_no, bool move_direction) {
  if (move_direction != motor_direction[motor_no]) {
      motor_direction[motor_no] = move_direction ;  // change motor direction
      digitalWrite(DIR_PIN[motor_no], move_direction);
      delay(DIRECTION_CHANGE_WAIT_TIME);  // wait to IO stable   
  }
  digitalWrite(STEP_PIN[motor_no], HIGH);
  delayMicroseconds(2);// for fasr processors may not need with ATMEGA
  digitalWrite(STEP_PIN[motor_no], LOW);
  delayMicroseconds(2);
  if (move_direction == false) {
    current_steps[motor_no]++;
    current_position[motor_no] += mm_per_pulse[motor_no];
  }
  if (move_direction == true) {
    current_steps[motor_no]--;
    current_position[motor_no] -= mm_per_pulse[motor_no];
  }
}


// update the distance between pos and dest
void update_delta() {
  for (uint8_t i = 0; i < NUMBER_OF_MOTORS; i++) {
    delta[i] = destination[i] - current_position[i];
  }
}

// update the rates according to the potentiometer (not really used)
void update_rates()
{
//  rate = map(analogRead(SPEED_POT_INPUT_PIN), 0, 1023, MAX_RATE, MIN_RATE);  // CW/DDW pot +/- connection 
  for(uint8_t i = 0; i < NUMBER_OF_MOVES; i++)
    rates[i] = rate;
}


// go to (0, 0)
void go_home() {
  for (int16_t i = 0; i < NUMBER_OF_MOTORS; i++)
    destination[i] = 0;
}


// go to the corner and calibrate to (0, 0)
void homming(){
  digitalWrite(STEPPER_EN_PIN, LOW);  // enable motors 
  for (int16_t j = 0; j < NUMBER_OF_MOTORS; j++){
    for (int16_t i = 0; i < HOMMING_STEPS[j]; i++) {
      one_step(j, true);
      delay(homming_rates[2*j]);
    }
  }
  // move to the center
  for (int16_t j = 0; j < NUMBER_OF_MOTORS; j++){
    for (int16_t i = 0; i < STEPS_TO_CENTER; i++) {
      one_step(j, false);
      delay(homming_rates[2*j]);
    }
  }
  set_position(0, 0);
  set_steps(0, 0);

  if (digitalRead(IS_EN_JUMPER)!= 0)
    digitalWrite(STEPPER_EN_PIN, HIGH);  // disable motors if jumper not set 
}

// update scales
void update_scales() {
  screen_scale[0] = board_size[0] / (border[2] - border[0]);
  screen_scale[1] = board_size[1] / (border[3] - border[1]);
}

// turn laser on
void laser_on() {
  digitalWrite(LED_OUT, HIGH);
  analogWrite(LASER_OUT, laser_power);
  is_laser_on = true;
}

// turn laser off
void laser_off() {
  digitalWrite(LED_OUT, LOW);
  analogWrite(LASER_OUT, 0);
  is_laser_on = false;
}

// turn DC motor on
void dc_motor_on() {
  analogWrite(DC_MOTOR_OUT, dc_motor_power);
  is_dc_motor_on = true;
  last_time_dc_motor = millis();
}

// turn DC motor off
void dc_motor_off() {
  analogWrite(DC_MOTOR_OUT, 0);
  is_dc_motor_on = false;
}


// check if need to turn off DC motor
void check_dc_motor() {
  if (!dc_motor_stay_on && millis() - last_time_dc_motor > MAX_DC_MOTOR_TIME) {
    dc_motor_off();
  }
}

// blink led X times
void blink_led(uint8_t blinks_number, uint16_t on_time, uint16_t off_time){
  for (uint8_t i = 0; i < blinks_number; i++) {
    digitalWrite(LED_OUT, HIGH);  // led on 
    delay(on_time);
    digitalWrite(LED_OUT, LOW);  // led off 
    delay(off_time);
  }
}

void init_drawing() {
  laser_off();
  processing_drawing = true;
  drawing_batch = false;
  drawing_frame = false;
  point_index = 0;
  batch_size = 0;
  laser_power = LASER_ON_POWER;
  rate = LASER_OFF_RATE;
  is_destination_done = true;
}

void init_params() {
  getting_params = true;
  param_index = 0;
}

void reset() {
  laser_off();
  drawing_frame = false;
  rate = LASER_OFF_RATE;
  processing_drawing = false;
  drawing_batch = false;
  point_index = 0;
  is_destination_done = true;
  homming();
}

