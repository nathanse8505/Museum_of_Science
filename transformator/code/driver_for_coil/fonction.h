#ifndef fonction
#define fonction
#include "CONSTANTS.h"

void POSITIVE_CURRENT(){
    digitalWrite(P_DIRECTION, HIGH);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, LOW);
    delay(time_positive);
  
    digitalWrite(P_DIRECTION, LOW);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, LOW);
    delay(cycle_time - time_positive);
}

void NEGATIVE_CURRENT(){
    digitalWrite(P_DIRECTION,LOW);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, HIGH);
    delay(time_positive);
  
    digitalWrite(P_DIRECTION, LOW);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, LOW);
    delay(cycle_time - time_positive);
}

void Serial_disp(){
     // Display information on the serial monitor
    Serial.print("Cycle Time: ");
    Serial.print(cycle_time);
    Serial.println(" ms");

    Serial.print("Positive Time: ");
    Serial.print(time_positive);
    Serial.println(" ms");
}

#endif
