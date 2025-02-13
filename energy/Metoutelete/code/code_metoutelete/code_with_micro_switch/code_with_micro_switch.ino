#include "const.h"

void setup() {
  Serial.begin(9600);
  pinMode(MICRO_SW,INPUT_PULLUP);
  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(MOTOR,OUTPUT);
  Serial.println("init");

  digitalWrite(MOTOR,LOW);
}


bool check = LOW;
float time_start = 0;
bool button_check = LOW;


bool SWITCH(){// check if the button is ready
    if(digitalRead(BUTTON) == LOW && check == LOW){ //verify that the button is pressed and be ready for the release  
        check = HIGH;
        delay(time_bouncing);
        Serial.println("the button is pushed");
    }
    if(digitalRead(BUTTON) == HIGH && check == HIGH){ //verify that the button is released to activate the motor
        Serial.println("the button is unpushed");
        Serial.print("the check is ");
        Serial.println(check);
        check = LOW;

        Serial.print("the button is ");
        Serial.println(digitalRead(BUTTON));
        Serial.println("");
        
        return HIGH;
    }
  
    return LOW;

}





void loop() {
      
      
    button_check = SWITCH();
    Serial.print("the button check is ");
    Serial.println(button_check); 
    Serial.println((millis() - time_start));

    if((millis() - time_start) > stop_time){

      if(button_check){
        Serial.println("the motor is ON");
        digitalWrite(MOTOR,HIGH);//active the motor until then micro switch is low
        delay(motor_delay);
      }
        
      while(digitalRead(MICRO_SW) == HIGH && digitalRead(MOTOR) ==HIGH){ // if the sensor doesnt disturbing continue to activate the motor      
        Serial.println("the micro switch is pushed");

        if(digitalRead(MICRO_SW) == LOW){ //if the  is the  micro switch is activate the motor is power off for stop time delay

            Serial.println("the micro switch is unpushed");
            digitalWrite(MOTOR,LOW);
            button_check = LOW;
            time_start = millis();
            break;
        }
    }
    }
    
   
    
}
