#include <Keyboard.h>

#define BUTTON_IO_A2 A2
#define BUTTON_IO_A3 A3

#define SHORT_PRESS


const int16_t BAUDERATE = 115200;
bool first_entry = HIGH;
//////////Press Button/////////////
const int BOUNCE_TIME = 50;
bool checkstate_button[2] = {LOW,LOW};

bool PRESS_BUTTON(int BUTTON_IO, bool &check) {

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
  Serial.begin(BAUDERATE);
  pinMode(BUTTON_IO_A2, INPUT_PULLUP);
  pinMode(BUTTON_IO_A3, INPUT_PULLUP);
  Keyboard.begin();
  delay(1000);
}

void loop() {
  
  #ifdef SHORT_PRESS

    if(PRESS_BUTTON(BUTTON_IO_A2, checkstate_button[0])){
    //Serial.println("A");
    Keyboard.write(KEY_UP_ARROW);
    } 
    if(PRESS_BUTTON(BUTTON_IO_A3, checkstate_button[1])){
    //Serial.println("B");
    Keyboard.write(KEY_DOWN_ARROW);
    }
  #else

    while(digitalRead(BUTTON_IO_A2) == LOW){
      //Serial.println("enter");
      if (first_entry){
        Keyboard.press(KEY_UP_ARROW);
        first_entry = LOW;
      }
      delay(10);
    }

    while(digitalRead(BUTTON_IO_A3) == LOW){
      //Serial.println("enter"); 
      if (first_entry){
        Keyboard.press(KEY_DOWN_ARROW);
        first_entry = LOW;
      }
      delay(10);
    }

    first_entry = HIGH;
    Keyboard.releaseAll();
  #endif  

  
}




