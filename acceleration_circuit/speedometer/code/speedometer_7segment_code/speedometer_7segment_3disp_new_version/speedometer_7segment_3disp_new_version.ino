
#include "CONSTANTS.h"
#include "FONCTION.h"
#include <avr/wdt.h>

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
   //Serial.println(analogRead(SENSOR)); 
    wdt_reset();
    
    if (SENSOR_OUT() == HIGH && check == HIGH){ 
      Serial.println("we are in a black to white zone start"); 

      timer = millis();
      check = LOW;
      delay(DELAY_BOUNCING);
    }
    
    
    if(SENSOR_OUT() == HIGH && check == LOW){
      Serial.println("we are in a black to white zone stop");  

      delta_time_sec = (millis() - timer)/ ms_to_sec;
      check = HIGH;
      check_zero = HIGH;
      velocity = meter / delta_time_sec;
      km_per_h = velocity * 3.6;
      rpm = 60 / delta_time_sec;
    Serial.print(velocity,3);
    Serial.println(" m/s");
    Serial.print(rpm,3);
    Serial.println(" rpm\n");

    km_per_h = km_per_h * CALIBRATION;

    Digits_from_Number(km_per_h);
    Display_Digit(Digit_1_To_Display);//right digit
    enable(RIGH_DIGIT);
    Display_Digit(Digit_2_To_Display);//middle digit
    enable(MIDDLE_DIGIT);
    Display_Digit(Digit_3_To_Display);//left digit
    enable(LEFT_DIGIT); 
    }

    //Reset_Error_Sensor();
    
    

    if ((millis() - timer) > TIME_RESET && check_zero){
      Serial.println("reset the speedometer"); 
      timer = 0; 
      velocity = 0;
      km_per_h = 0;
      rpm = 0;
      check_zero = LOW;
      check = HIGH;

      //blank_All_Digit();
      all_zero_digit();
    }
      
   

}
