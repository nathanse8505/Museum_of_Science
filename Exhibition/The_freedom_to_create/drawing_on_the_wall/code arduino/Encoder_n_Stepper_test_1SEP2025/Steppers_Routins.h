#ifndef STEPPERS_ROUTINS_H
#define STEPPERS_ROUTINS_H
#include "CONST_N_GLOBAL.h"

//----------move a motor one step-----------------
void one_step(uint8_t motor_No, bool move_direction)
  {
    if (move_direction != motor_direction[motor_No]){
        motor_direction[motor_No] = move_direction ;// change motor direction
        digitalWrite(DIR_PIN[motor_No],move_direction);
        delay (DIRECTION_CHANGE_WAIT_TIME);// wait to IO stable   
    }
  digitalWrite(STEP_PIN[motor_No], HIGH);
  delayMicroseconds(2);// for fast processors may not need with ATMEGA
  digitalWrite(STEP_PIN[motor_No], LOW);
  delayMicroseconds(2);
  if (move_direction == false) {++Current_Steps[motor_No];}
//    Current_Position[motor_No] = Current_Position[motor_No]+ mm_per_pulse[motor_No]; }
  if (move_direction == true) {--Current_Steps[motor_No];}
//    Current_Position[motor_No] = Current_Position[motor_No]- mm_per_pulse[motor_No]; }
//  if(Use_Serial){print_step();  }
//  print_position();
}

//-------set current steps  ---------------------
void set_steps(int16_t Start_X, int16_t Start_Y) {
  Current_Steps[0] = Start_X;
  Current_Steps[1] = Start_Y;
}


//----------update the rates according to the potentiometer-----------
void update_rates()
{
  rate = map(analogRead(SPEED_POT_INPUT_PIN), 0, 1023, MAX_RATE, MIN_RATE);// CW/DDW pot +/- connection 
  for(uint8_t i=0; i< NUMBER_OF_MOVES; ++i){
    XY_rates[i] = rate;
  }
}

//------ go to the corner and calibrate to 0,0----------
void Homming(){
//  digitalWrite(STEPPER_EN_PIN, LOW);  //enable motors - not nead at this code, enabled at setup
  for(int16_t j=0; j< NUMBER_OF_MOTORS; ++j){
    for(int16_t i=0; i< HOMMING_Steps[j]; ++i){
    one_step(j,true);
    delay(XY_homming_rates[2*j]);
    }
  }
//  set_position(0,0,0);
  set_steps(0,0);
  if(Use_Serial){print_step();}  
//  print_position();
  //if (digitalRead(IS_EN_JUMPER)!= 0) {digitalWrite(STEPPER_EN_PIN, HIGH);}   //disable motors if jumper not set 
}





#endif /* STEPPERS_ROUTINS_H */
