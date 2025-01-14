#include "CONSTANTS.h"
#include "FONCTION.h"

#define TIME_RESTART 20000





void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(SENSOR,INPUT_PULLUP);

  Init_Output(BCD_A);
  Init_Output(BCD_B);
  Init_Output(BCD_C);
  Init_Output(BCD_D);

  Init_Output(LE_UNITS);
  digitalWrite(LE_UNITS, HIGH);// lock digit

  Init_Output(LE_DOZENS);
  digitalWrite(LE_DOZENS, HIGH);// lock digit

  Init_Output(LE_CENTURIES);
  digitalWrite(LE_CENTURIES, HIGH);// lock digit

  wdt_enable(WDTO_2S);

  Serial.println("init");

}


void loop() {

    //Serial.println(check);
    //Serial.println(analogRead(SENSOR)); 
    wdt_reset();
    float volt_sensor1=(float)(analogRead(SENSOR)*5)/1023;

    if (volt_sensor1 < 1 && check == 0){ 
      Serial.println("we are in a black to white zone start"); 
      timer_1 = millis();
      check = 1;
      
    }
    if(volt_sensor1 > 3 && check == 1 ){
      Serial.println("we are in a white to black zone"); 
      check = 2;
    }

    if(volt_sensor1 < 1 && check == 2){
        Serial.println("we are in a black to white zone stop");  

      
      delta_time = (millis() - timer_1)/1000;
      check = 0;
      check_zero = HIGH;
      velocity = meter / delta_time;
      velocity = (velocity * 100);//for 3 display
      km_per_h = (int)velocity * 3.6;
      rpm = 60 / delta_time;

      Serial.print(velocity,3);
      Serial.println(" m/s");
      Serial.print(km_per_h,3);
      Serial.println(" km/h\n");

    }  
      
    if ((millis() - timer_1) > TIME_RESTART && check_zero){
        all_zero_digit();
        velocity = 0;
        km_per_h = 0;
        rpm = 0;
        check_zero = LOW;
        //blank_All_Digit();
    }

    NUMBER_TO_DISPLAY( km_per_h);
          
    

}


      

