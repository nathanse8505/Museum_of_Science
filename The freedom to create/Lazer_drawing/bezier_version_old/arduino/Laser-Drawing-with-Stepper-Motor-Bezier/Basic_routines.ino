#include "Basic_routines.h"

//------ read pushbuttons/joystick and move the motors acording 
void read_pushbuttons(){
//  if (digitalRead(IS_EN_JUMPER)!= 0) {digitalWrite(STEPPER_EN_PIN, HIGH);}   //disable motors if jumper not set 
  for(uint8_t i=0; i< NUMBER_OF_MOVES; ++i){
    if (digitalRead(MOVE_PIN[i])==0) { //check move (direction 0/false) pushbutton
      if (is_position_valid(Current_Position[0] + (i==0?(mm_per_pulse[i/2]):(i==1?(-mm_per_pulse[i/2]):0))
          ,Current_Position[1] + (i==2?(mm_per_pulse[i/2]):(i==3?(-mm_per_pulse[i/2]):0)))) {
        digitalWrite(STEPPER_EN_PIN, LOW);  //enable motors 
        if (millis() >= last_pulse_time[i/2]+XYZ_rates[i]){// time for another pulse 
          one_step (i/2, i%2);// make another pulse, odd or even direction False(+), True(-), False, True, False, True
          last_pulse_time[i/2]= millis();// reset couner for continue steps rate 
        }
      }
    }
  }
}


//move to destinaion
void read_destination() {
  if (in_destination()) { 
    Is_destination_done = true;
//    Serial.println("Done!");
    return; 
  }
  
  // if not in destination, move one step towards it (with proportional speeds for each motor)
  update_delta();
  if (abs(Delta[0]) <= TOLERANCE) {
    XYZ_prop_rates[2] = XYZ_rates[2]; //get rate from potentiometer
  }
  else if (abs(Delta[1]) <= TOLERANCE) {
    XYZ_prop_rates[0] = XYZ_rates[0];
  }
  else {
    if (abs(Delta[0]) > abs(Delta[1])) {
      XYZ_prop_rates[0] = XYZ_rates[0];
      XYZ_prop_rates[2] = XYZ_rates[2]*abs(Delta[0]/Delta[1]);
    }
    else {
      XYZ_prop_rates[2] = XYZ_rates[2];
      XYZ_prop_rates[0] = XYZ_rates[0]*abs(Delta[1]/Delta[0]);
    }
  }
//  if (digitalRead(IS_EN_JUMPER)!= 0) {digitalWrite(STEPPER_EN_PIN, HIGH);}   //disable motors if jumper not set 
  for(uint8_t i=0; i< NUMBER_OF_MOTORS; ++i){ 
    if (abs(Delta[i]) > TOLERANCE) { 
      digitalWrite(STEPPER_EN_PIN, LOW);  //enable motors
      if (millis() >= last_pulse_time[2*i]+XYZ_prop_rates[2*i]){// time for another pulse 
        if (Delta[i] > 0 ? (Current_Position[i] < MAX_POSITION[i]) : (Current_Position[i] > MIN_POSITION[i])) {
          one_step (i, (Delta[i] > 0 ? false : true));// make another pulse, odd or even direction False(+), True(-), False, True, False, True
          last_pulse_time[2*i]= millis();// reset couner for continue steps rate 
        }
        else {
          Is_destination_done = true;
        }
      }
    }
  }   
}


//-------set current steps  ---------------------
void set_steps(int16_t Start_X, int16_t Start_Y) {
  Current_Steps[0] = Start_X;
  Current_Steps[1] = Start_Y;
//  Current_Steps[2] = Start_Z;
}

//-------set current position  ---------------------
void set_position(float current_X, float current_Y) {
  Current_Position[0] = current_X;
  Current_Position[1] = current_Y;
//  Current_Position[2] = current_Z;
}

//-------set destionation ----------------------------
void set_destination(float x, float y) {
  destination[0] = x;
  destination[1] = y;
//  destination[2] = z;
}


//---------print steps-----------
void print_step()
{
  char buffer [100];
  sprintf(buffer, "Steps[XYZ pulses]: %d,%d,%d" , Current_Steps[0] , Current_Steps[1] , Current_Steps[2]);
  Serial.println(buffer);  
}

// -----------print destination ----------
void print_destination() {
  Serial.print("Destination[XYZmm]: ");
  Serial.print(destination[0],3);
  Serial.print(", ");
  Serial.println(destination[1],3);
//  Serial.print(", ");
//  Serial.println(destination[2],3);
}

//-------print position ---------------------
void print_position()
{
  Serial.print("Position[XYZmm]: ");
  Serial.print(Current_Position[0],3);
  Serial.print(", ");
  Serial.println(Current_Position[1],3);
//  Serial.print(", ");
//  Serial.println(Current_Position[2],3);
}

//------------ print status--------------------
void print_status() {
  Serial.println("================STATUS=====================");
  print_step();  
  print_position();
  print_destination();
  
}


//----------check if laser reached the destination------
bool in_destination() {
  for (uint8_t i = 0; i < NUMBER_OF_MOTORS; i++ ) {
    if (abs(Current_Position[i]-destination[i]) > TOLERANCE)
      return false;
  }
  return true;
}


//--------check if destination is in boundries---------
bool is_destination_valid() {
  for (uint8_t i = 0; i < NUMBER_OF_MOTORS; i++) {
    if (destination[i] < MIN_POSITION[i] || destination[i] > MAX_POSITION[i])
      return false;
  }
  return true;
}

//--------check if position is in boundries---------
bool is_position_valid(float x, float y){
  if (x < MIN_POSITION[0] || x > MAX_POSITION[0] || y < MIN_POSITION[1] || y > MAX_POSITION[1])
    return false;
  return true;
}

// -------- limits int16_t value between min and max ------
int16_t max_min_limit(int16_t In_Value, int16_t Min, int16_t Max)
{
  int16_t temp = min(In_Value, Max); // limit to <= Max
  temp = max(temp, Min); // limit to >= Min
  return temp;
}

//---------limits steps between global min and max---------
void Limit_Steps_XYZ(){
    for(uint8_t i=0; i< NUMBER_OF_MOTORS; ++i){
      Current_Steps[i] = max_min_limit(Current_Steps[i], MIN_Steps[i], MAX_Steps[i]);
    }
}


//----------move a motor one step-----------------
void one_step(uint8_t motor_No, bool move_direction) {
  if (move_direction != motor_direction[motor_No]){
      motor_direction[motor_No] = move_direction ;// change motor direction
      digitalWrite(DIR_PIN[motor_No],move_direction);
      delay (DIRECTION_CHANGE_WAIT_TIME);// wait to IO stable   
  }
  digitalWrite(STEP_PIN[motor_No], HIGH);
  delayMicroseconds(2);// for fasr processors may not need with ATMEGA
  digitalWrite(STEP_PIN[motor_No], LOW);
  delayMicroseconds(2);
  if (move_direction == false) {
    ++Current_Steps[motor_No];
    Current_Position[motor_No] = Current_Position[motor_No]+ mm_per_pulse[motor_No]; }
  if (move_direction == true) {
    --Current_Steps[motor_No];
    Current_Position[motor_No] = Current_Position[motor_No]- mm_per_pulse[motor_No]; }
    
//  print_step(Current_Steps);  
//  print_position(Current_Position);
}


//--------update the distance between pos and dest------------
void update_delta() {
  for (uint8_t i = 0; i < NUMBER_OF_MOTORS; i++) {
    Delta[i] = destination[i] - Current_Position[i];
  }
}

//----------update the rates according to the potentiometer-----------
void update_rates()
{
//  rate = map(analogRead(SPEED_POT_INPUT_PIN), 0, 1023, MAX_RATE, MIN_RATE);// CW/DDW pot +/- connection 
  for(uint8_t i=0; i< NUMBER_OF_MOVES; ++i){
    XYZ_rates[i] = rate;
  }
}


// -----go to (0,0,0)-------
void go_home() {
  for(int16_t j=0; j< NUMBER_OF_MOTORS; ++j){
    destination[j] = 0;
  }
}


//------ go to the corner and calibrate to 0,0----------
void Homming(){
  digitalWrite(STEPPER_EN_PIN, LOW);  //enable motors 
  for(int16_t j=0; j< NUMBER_OF_MOTORS; ++j){
    for(int16_t i=0; i< HOMMING_Steps[j]; ++i){
    one_step(j,true);
    delay(XYZ_homming_rates[2*j]);
    }
  }
  // move to the center
  for(int16_t j=0; j< NUMBER_OF_MOTORS; ++j){
    for(int16_t i=0; i < STEPS_TO_CENTER; ++i){
    one_step(j,false);
    delay(XYZ_homming_rates[2*j]);
    }
  }
  set_position(0,0);
  set_steps(0,0);
  if (digitalRead(IS_EN_JUMPER)!= 0) {digitalWrite(STEPPER_EN_PIN, HIGH);}   //disable motors if jumper not set 
}


//-------draw a square---------------
void square() {
}


//----update scales-----------------
void update_scales() {
  screen_scale[0] = board_size[0]/(border[2]-border[0]);
  screen_scale[1] = board_size[1]/(border[3]-border[1]);
}

//-------turn led on----------------
void led_on() {
  digitalWrite(LED_OUT, HIGH);
  analogWrite(LASER_OUT, laser_power);
  is_laser_on = true;
}

//-------turn led off---------------
void led_off() {
  digitalWrite(LED_OUT, LOW);
  analogWrite(LASER_OUT, 0);
  is_laser_on = false;

}


//-------turn DC motor on---------------------
void dc_motor_on() {
  analogWrite(DC_MOTOR_OUT, dc_motor_power);
  is_dc_motor_on = true;
}

//-------turn DC motor off--------------------
void dc_motor_off() {
  analogWrite(DC_MOTOR_OUT, 0);
  is_dc_motor_on = false;
}


//--------check if need to turn off DC motor-----
void check_dc_motor() {
  if (millis() - last_time_dc_motor > MAX_DC_MOTOR_TIME) {
    dc_motor_off();
  }
}

//-------blink led XX times, Ton , Toff) --------------------
void Blink_Led(uint8_t blinks_number, uint16_t on_time, uint16_t off_time){
  for(uint8_t i=0; i< blinks_number; ++i){
    digitalWrite(LED_OUT, HIGH);  //led on 
    delay(on_time);
    digitalWrite(LED_OUT, LOW);  //led off 
    delay(off_time);
  }
}


//--------set bezier point i ---------------------
void set_bezier_point(float x, float y) {
  bezier_point[0] = x;
  bezier_point[1] = y;
}

//---------compute bezier variables-----------------
void compute_bezier_variable(int curve_index) {
  step_index = 0;
  for (int i = 0; i < points_per_curve; i++)
    b[i] = curves[points_per_curve*curve_index+i];
//    b[i] = curves[points_per_curve*curve_index+i] * MAX_POSITION[i%2];

 /*   
  float b0x = curves[points_per_curve*curve_index] * MAX_POSITION[0];  0
  float b0y = curves[points_per_curve*curve_index+1] * MAX_POSITION[1];  1
  float b1x = curves[points_per_curve*curve_index+2] * MAX_POSITION[0];  2
  float b1y = curves[points_per_curve*curve_index+3] * MAX_POSITION[1];  3
  float b2x = curves[points_per_curve*curve_index+4] * MAX_POSITION[0];  4
  float b2y = curves[points_per_curve*curve_index+5] * MAX_POSITION[1];  5
  float b3x = curves[points_per_curve*curve_index+6] * MAX_POSITION[0];  6
  float b3y = curves[points_per_curve*curve_index+7] * MAX_POSITION[1];  7
*/
  // Compute polynomial coefficients from Bezier points
  coeff[0] = (-b[0] + 3 * b[2] + -3 * b[4] + b[6]);   //ax
  coeff[1] = (-b[1] + 3 * b[3] + -3 * b[5] + b[7]);   //ay
  coeff[2] = (3 * b[0] + -6 * b[2] + 3 * b[4]);       //bx
  coeff[3] =  (3 * b[1] + -6 * b[3] + 3 * b[5]);      //by
  coeff[4] = (-3 * b[0] + 3 * b[2]);                  //cx
  coeff[5] = (-3 * b[1] + 3 * b[3]);                  //cy
  coeff[6] = b[0];                                    //dx
  coeff[7] = b[1];                                    //dy
  float h = 1.0 / numSteps;  // compute our step size
  // Compute forward differences from Bezier points and "h"
  pointX = b[0];
  pointY = b[1];
  firstFDX = (coeff[0] * (h * h * h) + coeff[2] * (h * h) + coeff[4] * h);
  firstFDY =(coeff[1] * (h * h * h) + coeff[3] * (h * h) + coeff[5] * h);
  secondFDX = (6 * coeff[0] * (h * h * h) + 2 * coeff[2] * (h * h));
  secondFDY =(6 * coeff[1] * (h * h * h) + 2 * coeff[3] * (h * h));
  thirdFDX = (6 * coeff[0] * (h * h * h));
  thirdFDY =(6 * coeff[1] * (h * h * h));

  set_bezier_point(pointX,pointY);
}

//-----------compute the bezier curve from the 4 points-----------
boolean compute_next_bezier_point() {
  if (step_index >= numSteps)
    return false;
  pointX += firstFDX;
  pointY += firstFDY;
  firstFDX += secondFDX;
  firstFDY += secondFDY;
  secondFDX += thirdFDX;
  secondFDY += thirdFDY;
  step_index++;
  set_bezier_point(pointX,pointY);
  return true;
}
