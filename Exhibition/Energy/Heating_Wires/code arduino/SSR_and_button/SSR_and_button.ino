#include <Arduino.h>

/*
*==========Arduino Nano pinout====== 
 *                            _______
 *                       TXD-|       |-Vin 
 *                       RXD-|       |-Gnd  
 *                       RST-|       |-RST
 *                       GND-|       |-+5V  
 *             BUTTON_IO  D2-|       |-A7  
 *                        D3-|       |-A6  
 *         LED_BUTTON_IO  D4-|       |-A5   
 *                        D5-|       |-A4   
 *                        D6-|       |-A3   
 *                   SSR  D7-|       |-A2   
 *                        D8-|       |-A1   
 *                        D9-|       |-A0   
 *                       D10-|       |-Ref
 *                       D11-|       |-3.3V   
 *                       D12-|       |-D13
 *                            --USB--        
 */
#define BAUDRATE (115200)
////////////  I/O   ////////////
#define BUTTON_IO       2  
#define LED_BUTTON_IO   4
#define SSR             7  // 

bool buttonPressed = LOW;
bool check = LOW;
bool first_press = true;
bool first_led_on = true;
long timer_ssr_on = 0;
long timer_ssr_off = 0;
const int SSR_ON = 20000;
const int SSR_OFF = 10000;
const int BOUNCE_TIME = 50;


bool PRESS_BUTTON() {
  // Check if the button is pressed
  if (digitalRead(BUTTON_IO) == LOW && check == LOW) {
     //Serial.println("press :");
     check = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(BUTTON_IO) == HIGH && check == HIGH) {
    //Serial.println("unpress");
    check = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}

void setup() {
  pinMode(LED_BUTTON_IO, OUTPUT);       // Activation indicator LED
  pinMode(SSR, OUTPUT);                 // Activation SSR
  pinMode(BUTTON_IO, INPUT_PULLUP);     // Ignition button (active LOW)
  Serial.begin(BAUDRATE);               // Start serial communication
  delay(100);  
  Serial.println("init");  
  

}

void loop() {
  buttonPressed = PRESS_BUTTON();

  if(millis() - timer_ssr_off > SSR_OFF){

    if(first_led_on){
      digitalWrite(LED_BUTTON_IO,HIGH);
      first_led_on = false;
      Serial.println("Led ON");
    }

    if(buttonPressed && first_press){
      Serial.println("first press");
      Serial.println("SSR ON");
      digitalWrite(LED_BUTTON_IO,LOW);
      delay(10);
      digitalWrite(SSR,HIGH);
      first_press = LOW;
      timer_ssr_on = millis();
    }

    if(millis() - timer_ssr_on > SSR_ON && first_press == false){
      Serial.println("SSR OFF");
      first_press = true;
      first_led_on = true;
      timer_ssr_off = millis();
      digitalWrite(SSR,LOW);
      
    }

  }

  
}
















