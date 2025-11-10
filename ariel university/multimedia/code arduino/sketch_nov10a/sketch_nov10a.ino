#include <Keyboard.h>

#define BUTTON_IO_A2 A2
#define BUTTON_IO_A3 A3

const int16_t BAUDERATE = 115200;

void setup() {
  Serial.begin(BAUDERATE);
  pinMode(BUTTON_IO_A2, INPUT_PULLUP);
  pinMode(BUTTON_IO_A3, INPUT_PULLUP);
  Keyboard.begin();
  delay(1000);
}

void loop() {

    /*
  //////////PRESS A/B
  if(PRESS_BUTTON(BUTTON_IO_A2, checkstate_button[0])){
   Serial.println("A");
   Keyboard.print("A");
  } 
   if(PRESS_BUTTON(BUTTON_IO_A3, checkstate_button[1])){
   Serial.println("B");
   Keyboard.print("B");
  }
  */

  /*
  /////////////ZOOM+-//////////////////////////
  while(digitalRead(BUTTON_IO_A2) == LOW){
     Serial.println("enter");
     Keyboard.press(KEY_LEFT_CTRL);
     Keyboard.press('=' );
     delay(10);
     }
  while(digitalRead(BUTTON_IO_A3) == LOW){
    Serial.println("enter"); 
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('-');
     delay(10);
  }
    Keyboard.releaseAll();

  */

   
  // ZOOM +
  if (digitalRead(BUTTON_IO_A2) == LOW) {
    Serial.println("Zoom +");
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('=' );   // touche “+” sur la rangée principale (Ctrl + =)
    delay(100);
    Keyboard.releaseAll();
    delay(300); // petit délai pour éviter les répétitions rapides
  }

  // ZOOM -
  if (digitalRead(BUTTON_IO_A3) == LOW) {
    Serial.println("Zoom -");
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('-');    // touche “-” principale (Ctrl + -)
    delay(100);
    Keyboard.releaseAll();
    delay(300);
  }

}



