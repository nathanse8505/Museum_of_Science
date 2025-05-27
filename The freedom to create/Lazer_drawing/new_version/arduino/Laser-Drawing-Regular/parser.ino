#include "parser.h"

byte process_input()
{
    input.toUpperCase();
    if ((input.length() == 1)&&(input.substring(0,1) == "H")) {print_help_menu(); return 1;}
    else if ((input.length() == 3)&&(input.substring(0,3) == "PST")) {print_status(); return 1;} // print statuse 
    else if ((input.length() == 19) && (input.substring(0,1) == "X") && (input.substring(10,11) == "Y")) { read_xy(); return 1;}
    else if ((input.length() == 2)&&(input.substring(0,2) == "GO")){ is_destination_done = false; Serial.print("going to..."); print_destination(); return 1;}
    else if ((input.length() == 4)&&(input.substring(0,4) == "STOP")) {is_destination_done = true;Serial.println("laser stopped"); return 1;} 
    else if ((input.length() == 3)&&(input.substring(0,3) == "CLB")){set_position(0,0); set_steps(0,0); print_status(); return 1;}
    else if ((input.length() == 4)&&(input.substring(0,4) == "HOME")){Serial.println("homming..."); homming(); return 1;}  
    else if ((input.length() == 5)&&(input.substring(0,6) == "GO 00")) {set_destination(0,0);is_destination_done = false; Serial.println("going to (0,0)..."); return 1;}
    else if ((input.length() == 9) && (input.substring(0,5) == "POWER")) {read_power(); return 1;}
    else if ((input.length() == 6)&& (input.substring(0,6) == "LED ON")) {laser_on(); Serial.println("laser is ON"); return 1;} 
    else if ((input.length() == 7)&&(input.substring(0,7) == "LED OFF")) {laser_off(); Serial.println("laser is OFF"); return 1;} 
    else if ((input.length() == 8) && (input.substring(0,4) == "RATE")) {read_rate(); return 1;}
    else if ((input.length() == 10) && (input.substring(0,6) == "DPOWER")) {read_dc_power(); return 1;}
    else if ((input.length() == 10)&& (input.substring(0,10) == "DCMOTOR ON")) {dc_motor_stay_on = true; dc_motor_on(); Serial.println("DC motor is ON"); return 1;} 
    else if ((input.length() == 11)&&(input.substring(0,11) == "DCMOTOR OFF")) {dc_motor_stay_on = false; dc_motor_off(); Serial.println("DC motor is OFF"); return 1;}
    else if ((input.length() == 5)&&(input.substring(0,5) == START_KEY)) {init_drawing(); return 1;}
    else if ((input.length() == 6)&&(input.substring(0,6) == PARAMS_KEY)) {init_params(); return 1;}
    else if ((input.length() == 5)&&(input.substring(0,5) == RESET_KEY)) {reset(); return 1;}
    return 0;
}

void print_help_menu()  {
  Serial.println(F(
    "_________________________________________________________________\n"
    "|  Laser Engrave Control code Amitai@Amir Ben-Shalom             |\n"
    "|  H - Print (this) help                                         |\n"
    "|  PST -print status                                             |\n"
    "|  x +/-***.** y +/-***.** - go to (* is a digit, including 0)   |\n"
    "|  GO - continue to destination                                  |\n"
    "|  STOP - stop moving to the destination                         |\n"
    "|  CLB - calibrate to current position                           |\n"
    "|  HOME - go to the corner and calibrate the postion to (0,0)    |\n"
    "|  GO 00 - go to (0,0)                                           |\n"
    "|  POWER *** - change power of laser (000 <= *** <= 255)         |\n"
    "|  LED ON - turn the led on                                      |\n"
    "|  LED OFF - turn the led off                                    |\n"
    "|  RATE *** - change rate of XY motors (* is a digit)            |\n"
    "|  DPOWER *** - change rate of DC motor (000 <= *** <= 255)      |\n"
    "|  DCMOTOR ON - turn on DC motor                                 |\n"
    "|  DCMOTOR OFF - turn off DC motor                               |\n"
    "|________________________________________________________________|\n"
  ));
}


// parse power of laser
void read_power() {
  if (input.substring(6,9).toInt() != 0 || input.substring(6,9) == "000") {
    if (input.substring(6,9).toInt() <= 255 && input.substring(6,9).toInt() >= 0) {
      laser_power = input.substring(6,9).toInt();

      if (is_laser_on)
        laser_on();
        
      Serial.print("power of laser is now: ");
      Serial.println(laser_power);
    }
    else
      Serial.print("value out of range (000 - 255)");
  }
  else
    Serial.println("power not valid");
}


// parse dc motor power (speed)
void read_dc_power() {
  if (input.substring(7,10).toInt() != 0 || input.substring(7,10) == "000") {
    if (input.substring(7,10).toInt() <= 255 && input.substring(7,10).toInt() >= 0) {
      dc_motor_power = input.substring(7,10).toInt();

      if (is_dc_motor_on)
        dc_motor_on();
      
      Serial.print("power of DC motor is now: ");
      Serial.println(dc_motor_power);
    }
    else
      Serial.print("value out of range (000 - 255)");
  }
  else
    Serial.println("power not valid");
}

// parse rate
void read_rate() {
  if (input.substring(5,8).toInt() != 0 || input.substring(5,8) == "000") {
    if (input.substring(5,8).toInt() <= MAX_RATE && input.substring(5,8).toInt() >= MIN_RATE) {
      rate = input.substring(5,8).toInt();
      Serial.print("rate is now: ");
      Serial.println(rate);
    }
    else {
      Serial.print("value out of range (");
      Serial.print(MIN_RATE);
      Serial.print(" - ");
      Serial.print(MAX_RATE);
      Serial.println(")");
    }
  }
  else
    Serial.println("rate not valid");
}

// parse xy input
void read_xy() {
  float x,y;
  bool success = true;

  // handle x input
  if (input.substring(2,3) == "-") {
    if (input.substring(3,9).toFloat() != 0.0 || input.substring(3,9) == "000.00")
      x = -input.substring(3,9).toFloat();
    else 
      success = false;
  }
  else if (input.substring(2,3) == "+") {
    if (input.substring(3,9).toFloat() != 0.0 || input.substring(3,9) == "000.00")
      x = input.substring(3,9).toFloat();
    else 
      success = false;
  }
  else 
    success = false;

  // handle y input
  if (success) {
    if (input.substring(12,13) == "-") {
      if (input.substring(13,19).toFloat() != 0.0 || input.substring(13,19) == "000.00")
        y = -input.substring(13,19).toFloat();
      else 
        success = false;
    }
    else if (input.substring(12,13) == "+") {
      if (input.substring(13,19).toFloat() != 0.0 || input.substring(13,19) == "000.00")
        y = input.substring(13,19).toFloat();
      else 
        success = false;
    }
    else 
      success = false;
  }

  if (!success) {
    Serial.println("xy reading failed");
    return;
  }
  
  destination[0] = x;
  destination[1] = y;

  if (!is_destination_valid()) {
    Serial.println("destination out of range");
    is_destination_done = true;  
    return;
  }

  //else: turn on the flag that indicates that the laser still has not reached the destination
  is_destination_done = false;
  
  Serial.print("going to (");
  Serial.print(destination[0],3);
  Serial.print(", ");
  Serial.print(destination[1],3);
  Serial.println(")...");
}

// get the input from the serial
byte get_input() {
  static int input_len = 0;
  static char in_char = 0; 

  while (Serial.available()) {
    in_char = (char)Serial.read();
      if (in_char == '\n' || in_char == '\r' || input_len >= 35) {
        input_len = 0;
        return 1;
      }
      else {
        input += in_char;
        input_len++;
      }
  }
  return 0;
}
