#ifndef DISPLAY_H
#define DISPLAY_H
#include "consts.h"

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
  /*
   * init display function
   */
//  display.begin(i2c_Address, true); // Address 0x3C default
  if (!display.begin(i2c_Address, true)) {
    Serial.println(F("SH1106 allocation failed"));
  //  for (;;);
  }
//  display.clearDisplay();// amir add 
//  display.display(); // Display initial screen buffer
//  delay(1000);       // Pause for a moment
  //display.setContrast (0); // dim display
  display.clearDisplay();
  display.display();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F(__FILE__ " " __DATE__ " " __TIME__));
//  display.println(F("test"));
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("START");
  display.display();
  delay(500);
  display.clearDisplay();
  display.display();
}


void display_current(){
  /*
   * show current on display
   */

  display.print(abs_avg_current_value, 2);
  //display.println(F(" Amp"));
  display.println(" Amp");
}


void display_charge(){
  /*
   * show charge on display
   */
  display.print(charge,1);
  //display.println(F(" Coul"));
  display.println(" Coul");
}


void display_message(){
  /* 
   * show message from text index
   */
  display.print(DISPLAY_TEXTS[text]);
//  display.print(msg);
}


void display_all(){
  /*
   * update display
   */
  display.clearDisplay();
  display.setCursor(0, 0);

  display_current();
  display.setCursor(0, 18);// asuming textsize 2 (10X16 pixels)
  display_charge();
  display.setCursor(0, 40);// asuming textsize 2 (10X16 pixels)
  display_message();
  display.display();
}


#endif /* DISPLAY_H */
