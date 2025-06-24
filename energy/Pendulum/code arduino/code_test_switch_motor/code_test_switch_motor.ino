
#include "const.h"

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(MICRO_SW,INPUT_PULLUP);
  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(MOTOR,OUTPUT);

  digitalWrite(MOTOR,LOW);
}

void loop() {
  
  while(digitalRead(BUTTON) == LOW){
    digitalWrite(MOTOR,HIGH);

    Serial.println(digitalRead(MOTOR));
  }
  digitalWrite(MOTOR,LOW);
  Serial.println(digitalRead(MOTOR));

}
