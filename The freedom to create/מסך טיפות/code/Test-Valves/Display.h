
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

void display_started() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(50, 32);
  display.println("ON");
  display.display();
  display.setTextSize(1);
}

void display_settings(){
  if (started) {
    display_started();  
  }
  else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(current_setting==0?">Cassette= ":" Cassette=");
    display.println(cassette_num+1);
    display.print(current_setting==1?">Board= ":" Board=");
    char board_char = 'a'+board_num;
    display.println(board_char);  // 0->a, 1->b...
    display.print(current_setting==2?">Valve= ":" Valve=");
    valve_num==valves_per_board ? display.println("all") : display.println(valve_num+1);
    display.print(current_setting==3?">Duration= ":" Duration=");
    display.println(valve_on_time);
    display.print(current_setting==4?">Cycle= ":" Cycle=");
    display.println(cycle_flag?"yes":"no");
    display.display();
  }
}
