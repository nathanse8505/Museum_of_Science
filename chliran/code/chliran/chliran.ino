#include "CONST.h"
#include "FONCTION.h"




void setup() {
  Serial.begin(9600);

  pinMode(Data_Serial, OUTPUT);
  pinMode(ClockPin, OUTPUT);
  pinMode(LatchPin, OUTPUT);
  pinMode(Output_Enable, OUTPUT);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);

  init_shift_register();
  wdt_enable(WDTO_4S);

  out_data = SET_FIRST_SW_LED();
  Serial.println("init");
}

void loop() {
  reset_watchdog();
   
    Serial.print("checkSW1: ");
    Serial.println(checkSW1);
    Serial.print("checkSW2: ");
    Serial.println(checkSW2);
    Serial.print("checkSW3: ");
    Serial.println(checkSW3);
    Serial.print("checkSW4: ");
    Serial.println(checkSW4);
 
  if(PRESS_BUTTON(1) && (checkSW1 == LOW) && (checkSW2 == LOW)  && (checkSW3 == LOW) && (checkSW4 == LOW)){
    out_data = SHIFT_LEFT(out_data);
    delay(TIME_LED);
    out_data = SHIFT_LEFT(out_data);
    Serial.println(out_data);
    checkSW1 = HIGH;


  }

  if (PRESS_BUTTON(2) && (checkSW1 == HIGH) && (checkSW2 == LOW) && (checkSW3 == LOW) && (checkSW4 == LOW)){
    out_data = SHIFT_LEFT(out_data);
    delay(TIME_LED);
    out_data = SHIFT_LEFT(out_data);
    checkSW2 = HIGH;
   
  }
  
  if(PRESS_BUTTON(3) && (checkSW1 == HIGH) && (checkSW2 == HIGH) && (checkSW3 == LOW) && (checkSW4 == LOW)){
    out_data = SHIFT_LEFT(out_data);
    delay(TIME_LED);
    out_data = SHIFT_LEFT(out_data);
    checkSW3 = HIGH;

  }
  if(PRESS_BUTTON(4) && (checkSW1 == HIGH) && (checkSW2 == HIGH) && (checkSW3 == HIGH) && (checkSW4 == LOW)){
    out_data = SHIFT_LEFT(out_data);
    delay(TIME_LED);
    checkSW4 = HIGH;
   

  }
  if(checkSW4 == HIGH){
    checkSW1 = LOW;
    checkSW2 = LOW;
    checkSW3 = LOW;
    checkSW4 = LOW;
    out_data = SET_FIRST_SW_LED();
  }

  
}



