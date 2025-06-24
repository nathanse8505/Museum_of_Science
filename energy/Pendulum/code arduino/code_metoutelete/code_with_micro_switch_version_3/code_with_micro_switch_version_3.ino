#include "const.h"
#include <LowPower.h>

void setup() {
  Serial.begin(bauderate);
  pinMode(MICRO_SW,INPUT_PULLUP);
  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(MOTOR,OUTPUT);
  pinMode(LED_BUTTON,OUTPUT);
  Serial.println("init");

  digitalWrite(MOTOR,LOW);
}



bool SWITCH(){// check if the button is ready
    if(digitalRead(BUTTON) == LOW && check == LOW){ //verify that the button is pressed and be ready for the release  
      // Serial.println("the button is pushed");
        check = HIGH;
        delay(time_bouncing);
    }
    if(digitalRead(BUTTON) == HIGH && check == HIGH){ //verify that the button is released to activate the motor
      // Serial.println("the button is unpushed");
        check = LOW;        
        return HIGH;
    }
    return LOW;

}

void IEC(){//immediate engine cutoff
        digitalWrite(MOTOR,LOW);
        Serial.println("the arduino in low power"); 
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}


void loop() {
        
  button_check = SWITCH();    

  if((millis() - time_start) > activation_time){
    digitalWrite(LED_BUTTON,HIGH);

    if(button_check){
      Serial.println("the motor is ON");
      digitalWrite(MOTOR,HIGH);//active the motor until then micro switch is low
      time_safety = millis();
        //Serial.println((millis() - stop_time_safety));
      delay(motor_delay);
    }


    while(digitalRead(MICRO_SW) == NC_MICRO_SWITCH && digitalRead(MOTOR) ==HIGH){ // if the sensor doesnt disturbing continue to activate the motor      
      //Serial.println("the micro switch is unpushed");
      digitalWrite(LED_BUTTON,LOW);//turn off the light of the button

      if((millis() - time_safety) > stop_time_safety) {IEC();}//immediate engine cutoff

      if(digitalRead(MICRO_SW) == !NC_MICRO_SWITCH){ //if the micro switch is activate the motor and the arduino is power off

        Serial.println("the micro switch is pushed");
        digitalWrite(MOTOR,LOW);
        button_check = LOW;
        time_start = millis();
        break;
      }
    }
  }

}
