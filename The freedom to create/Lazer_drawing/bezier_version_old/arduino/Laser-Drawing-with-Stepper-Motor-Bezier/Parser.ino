#include "Parser.h"

//----- parse the command ------------
void process_in_command()
{
    in_command.toUpperCase();
    if ((in_command.length() == 1)&&(in_command.substring(0,1) == "H")) {print_help_menu();}
    else if ((in_command.length() == 3)&&(in_command.substring(0,3) == "PST")) {print_status();} // print statuse 
    else if ((in_command.length() == 19) && (in_command.substring(0,1)=="X") && (in_command.substring(10,11)=="Y")) { read_xyz(); }
    else if ((in_command.length() == 2)&&(in_command.substring(0,2) == "GO")){ Is_destination_done = false; Serial.print("going to..."); print_destination();}
    else if ((in_command.length() == 4)&&(in_command.substring(0,4) == "STOP")) {Is_destination_done = true;Serial.println("laser stopped");} 
    else if ((in_command.length() == 3)&&(in_command.substring(0,3) == "CLB")){set_position(0,0); set_steps(0,0); print_status();}
    else if ((in_command.length() == 4)&&(in_command.substring(0,4) == "HOME")){Serial.println("homming..."); Homming();}  
    else if ((in_command.length() == 5)&&(in_command.substring(0,6) == "GO 00")) {set_destination(0,0);Is_destination_done = false; Serial.println("going to (0,0)...");}
    else if ((in_command.length() == 9) && (in_command.substring(0,5) == "POWER")) {read_power();}
    else if ((in_command.length() == 6)&& (in_command.substring(0,6) == "LED ON")) {led_on(); Serial.println("led is ON");} 
    else if ((in_command.length() == 7)&&(in_command.substring(0,7) == "LED OFF")) {led_off(); Serial.println("led is OFF");} 
    else if ((in_command.length() == 6) && (in_command.substring(0,6) == "SQUARE")) {square();}
    else if ((in_command.length() == 8) && (in_command.substring(0,4) == "RATE")) {read_rate();}
    else if ((in_command.length() == 2) && (in_command.substring(0,2) == "PY")) {py_flag = true; Serial.println("INPUT IS ONLY FROM PYTHON NOW AND CANNOT BE CHANGED BACK");}
//    else if ((in_command.length() == 3) && (in_command.substring(0,3) == "EPY")) {py_flag = false;Serial.println("Input is only from serial monitor now");}
    else if ((in_command.length() == 10) && (in_command.substring(0,6) == "DPOWER")) {read_dpower();}
    else if ((in_command.length() == 10)&& (in_command.substring(0,10) == "DCMOTOR ON")) {dc_motor_on(); Serial.println("DC motor is ON");} 
    else if ((in_command.length() == 11)&&(in_command.substring(0,11) == "DCMOTOR OFF")) {dc_motor_off(); Serial.println("DC motor is OFF");} 
    
}

void print_help_menu()  {
  Serial.println(F(
    "_________________________________________________________________\n"
    "|  Laser Engrave Control code Amitai@Amir Ben-Shalom             |\n"
    "|  H - Print (this) help                                         |\n"
    "|  PST -print status                                             |\n"
    "|  x +/-***.** y +/-***.** - go to (* is a digit)                |\n"
    "|  GO - continue to destination                                  |\n"
    "|  STOP - stop moving to the destination                         |\n"
    "|  CLB - calibrate to current position                           |\n"
    "|  HOME - go to the corner and calibrate the postion to (0,0)    |\n"
    "|  GO 00 - go to (0,0)                                           |\n"
    "|  POWER *** - change power of laser (000 <= *** <= 255)         |\n"
    "|  LED ON - turn the led on                                      |\n"
    "|  LED OFF - turn the led off                                    |\n"
    "|  SQUARE - draw a sqare                                         |\n"
    "|  RATE *** - change rate of XY motors (* is a digit)            |\n"
    "|  DPOWER *** - change rate of DC motor (000 <= *** <= 255)      |\n"
    "|  DCMOTOR ON - turn on DC motor                                 |\n"
    "|  DCMOTOR OFF - turn off DC motor                               |\n"
    "|  PY - INPUT FROM PYTHON ONLY, THIS MONITOR WILL BE DISABLED!!  |\n"
    "|________________________________________________________________|\n"
  ));
}


//----------parse power of laser---------------------
void read_power() {
  int new_power = 0;
  if (in_command.substring(6,9).toInt() != 0 || in_command.substring(6,9) == "000") {
      if (in_command.substring(6,9).toInt() <= 255 && in_command.substring(6,9).toInt() >= 0) {
        laser_power = in_command.substring(6,9).toInt();
        if (is_laser_on)
          led_on();
        Serial.print("power of laser is now: ");
        Serial.println(laser_power);
      }
      else {
        Serial.print("value out of range (000 - 255)");
      }
    }
    else { 
      Serial.println("power not valid");
    }
}


//----------parse dc motor power (speed)----------
void read_dpower() {
  int new_dpower = dc_motor_power;
  if (in_command.substring(7,10).toInt() != 0 || in_command.substring(7,10) == "000") {
      if (in_command.substring(7,10).toInt() <= 255 && in_command.substring(7,10).toInt() >= 0) {
        dc_motor_power = in_command.substring(7,10).toInt();
        if (is_dc_motor_on)
          dc_motor_on();
        Serial.print("power of DC motor is now: ");
        Serial.println(dc_motor_power);
      }
      else {
        Serial.print("value out of range (000 - 255)");
      }
    }
    else { 
      Serial.println("power not valid");
    }
}

//----------parse rate---------------------
void read_rate() {
  int new_rate = rate;
  if (in_command.substring(5,8).toInt() != 0 || in_command.substring(5,8) == "000") {
      if (in_command.substring(5,8).toInt() <= MAX_RATE && in_command.substring(5,8).toInt() >= MIN_RATE) {
        rate = in_command.substring(5,8).toInt();
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
    else { 
      Serial.println("rate not valid");
    }
}

// --------------parse xyz input------------
void read_xyz() {
  float x,y;
  bool success = true;
  // handle x input
  if (in_command.substring(2,3) == "-") {
    if (in_command.substring(3,9).toFloat() != 0.0 || in_command.substring(3,9) == "000.00") {
      x = -in_command.substring(3,9).toFloat(); }
    else { success = false; }
  }
  else if (in_command.substring(2,3) == "+") {
    if (in_command.substring(3,9).toFloat() != 0.0 || in_command.substring(3,9) == "000.00") {
      x = in_command.substring(3,9).toFloat(); }
    else { success = false; }
  }
  else { success = false; }
  // handle y input
  if (success) {
    if (in_command.substring(12,13) == "-") {
      if (in_command.substring(13,19).toFloat() != 0.0 || in_command.substring(13,19) == "000.00") {
        y = -in_command.substring(13,19).toFloat(); }
      else { success = false; }
    }
    else if (in_command.substring(12,13) == "+") {
      if (in_command.substring(13,19).toFloat() != 0.0 || in_command.substring(13,19) == "000.00") {
        y = in_command.substring(13,19).toFloat(); }
      else { success = false; }
    }
    else { success = false; }
  }
  // hadle z input
//  if (success) {
//    if (in_command.substring(22,23) == "-") {
//      if (in_command.substring(23,29).toFloat() != 0.0 || in_command.substring(23,29) == "000.00") {
//        z = -in_command.substring(23,29).toFloat(); }
//      else { success = false; }
//    }
//    else if (in_command.substring(22,23) == "+") {
//      if (in_command.substring(23,29).toFloat() != 0.0 || in_command.substring(23,29) == "000.00") {
//        z = in_command.substring(23,29).toFloat(); }
//      else { success = false; }
//    }
//    else { success = false; }
//  }
  if (!success) {
    Serial.println("xy reading failed");
    return; 
  }
  
  destination[0] = x;
  destination[1] = y;
//  destination[2] = z;

  if (!is_destination_valid()) {
    Serial.println("destination out of range");
    Is_destination_done = true;  
    return;
  }

  //else: turn on the flag that indicates that the laser still has not reached the destination
  Is_destination_done = false;
  
  Serial.print("going to (");
  Serial.print(destination[0],3);
  Serial.print(", ");
  Serial.print(destination[1],3);
//  Serial.print(", ");
//  Serial.print(destination[2],3);
  Serial.println(")...");
}


//--------get the command from the serial-------
byte get_in_command() {
  static int in_command_len = 0;
  static char in_char = 0; 
  while (Serial.available()) {
    in_char = (char)Serial.read();
      if (((in_char == '\n')||(in_char == '\r'))||in_command_len>=35) {
        in_command_len = 0;  return 1;}
      else{ in_command += in_char; in_command_len++; }
   }
   return 0;
}
