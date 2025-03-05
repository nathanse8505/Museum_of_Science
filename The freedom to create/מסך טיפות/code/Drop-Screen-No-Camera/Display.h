#ifndef DISPLAY_H
#define DISPLAY_H

#include "Consts.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c //initialize with the I2C Oled's addr 0x3C. Typically aliexpress, 0x3D Adafrout

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void start_display(){
  display.begin(i2c_Address, true); // Address 0x3C default
  display.clearDisplay();
  display.display();

  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F(__FILE__ " " __DATE__ " " __TIME__));
  display.display();
  delay(4000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("START");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.display();
}

void display_settings(){
  display.clearDisplay();
  display.setCursor(0, 0);
//  display.print(current_setting==0?">Valve Time= ":" Valve Time=");
//  display.println(valve_on_time);  
  display.print(current_setting==1?">Space Time= ":" Space Time=");
  display.println(space_time);
  display.print(current_setting==2?">Led Start= ":" Led Start=");
  display.println(led_start);
  display.print(current_setting==3?">Led Time= ":" Led Time=");
  display.println(led_on_time);
  display.print(current_setting==4?">Depth= ":" Depth=");
  display.println(drawing_depth);
  display.print(current_setting==5?">Full Light= ":" Full Light=");
  display.println(full_light?"yes":"no");
//  display.print(current_setting==6?">Type= ":" Type=");
//  display.println(dim3_flag?"3D":"2D");
//  display.print(current_setting==7?">Auto Factoring= ":" Auto Factoring=");
//  display.println(auto_factor_flag?"yes":"no");
  display.print(current_setting==7?">Led PWM= ":" Led PWM=");
  display.println(led_power);
  display.display();
}

void display_settings2(){
  display.clearDisplay();
  display.setCursor(0, 0);
  switch(current_setting) {
    case 0:
      display.print("Valve_t=");
      display.println(valve_on_time); 
      break;
    case 1:
      display.print("Space_t=");
      display.println(space_time); 
      break;
    case 2:
      display.print("Led Start=");
      display.println(led_start); 
      break;
    case 3:
      display.print("Led Time=");
      display.println(led_on_time); 
      break;
    case 4:
      display.print("Depth=");
      display.println(drawing_depth);
      break;
    case 5:
      display.print("Full Light=");
      display.println(full_light?"yes":"no");
      break;
    case 6:
      display.print("Type=");
      display.println(dim3_flag?"3D":"2D");
      break;
    case 7:
      display.print("Light PWM=");
      display.println(led_power);
      break;
  }
  display.display();
}


#endif
