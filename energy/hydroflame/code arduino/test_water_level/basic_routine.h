#ifndef basic_routine
#define basic_routine
#include "const.h"
//#include <MovingAverage.h>  // see https://github.com/careyi3/MovingAverage
//MovingAverage filter(MOVING_AVG_LENGTH);  // define/use the movinge average object



bool check_water_level(){
  if (analogRead(SENSOR_WATER_IO) > 800){
    return false;
  }
  return true;
}



void FILL_WATER(){

  digitalWrite(RELAY_VALVE_WATER_IO, HIGH);
  delay(DELAY_FILL_WATER);
  digitalWrite(RELAY_VALVE_WATER_IO, LOW);
  delay(DELAY_AFTER_FILL_WATER);
}


#endif