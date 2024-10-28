#define IN_FET 10
#define OUT_FET 8

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  pinMode(IN_FET,INPUT_PULLUP);
  pinMode(OUT_FET,INPUT_PULLUP);
  Serial.begin(9600);
  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  display.clearDisplay();
} 
float timer_1 = 0.0;
float timer_2;
float delta_time;
float velocity=0; 
float meter = 0.067;
bool check = LOW;

void loop() {
  
  bool OUT = digitalRead(OUT_FET);
  bool IN  = digitalRead(IN_FET);
   /*Serial.print("in is ");
      Serial.println(IN);
      Serial.print("out is ");
      Serial.println(OUT);
      delay(10);*/
  /*if (IN == LOW && OUT == LOW)
  {
    continue;
  }*/
  if (OUT == LOW && check == LOW)
  {
      timer_1 = millis();
      Serial.print("in is ");
      Serial.println(IN);
      Serial.print("out is ");
      Serial.println(OUT);
      Serial.print(timer_1);
      Serial.println(" s timer 1 first if");
      check = HIGH;
      display.clearDisplay();

  }
  if (IN == LOW && OUT == HIGH && check == HIGH)
  {   
      timer_2 = millis();

      Serial.print("in is ");
      Serial.println(IN);
      Serial.print("out is ");
      Serial.println(OUT);
      Serial.print(timer_1);
      Serial.println(" s timer 1 second if");
      Serial.print(timer_2);
      Serial.println(" s timer 2");
     
      delta_time = (timer_2 - timer_1)/1000;
      check = LOW;
      velocity = meter / delta_time;
      Serial.print(delta_time);
      Serial.println(" s deltatime\n");
      Serial.print(velocity,3);
      Serial.println(" m/s\n");
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.println(velocity,3);
      display.setCursor(40, 0);
      display.println("  m/s\n");
      display.display();
      //delay(5000);
  }
  
  //Serial.print(velocity,3);
  //Serial.println(" m/s\n");
  

}
