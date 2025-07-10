#include "Const_and_routines.h"


void setup() {
  pinMode(COIL1, OUTPUT);
  pinMode(COIL2, OUTPUT);
  pinMode(COIL3, OUTPUT);
  pinMode(COIL4, OUTPUT);
  reset_coil();

  pinMode(PULSE_IO, INPUT_PULLUP);

  Serial.begin(BAUDERATE);
  Serial.println("System ready. Waiting for pulses...");
  for(int i = 0 ;i < NUMBER_OF TEST;i++){
  rotateAngle(360);
  reset_coil();
  delay(1000);

  }

}

void loop() {
  //rawAngle = analogRead(potAnglePin);// read the potentiomèter angle
  //anglePerLiter = map(rawAngle, 0, BIT_RESOLUTION, MIN_ANGLE, MAX_ANGLE);  // 0 to 180°
  anglePerLiter = ANGLE;
  rawSpeed = analogRead(potSpeedPin);// read the potentiomèter speed
  currentDelay = map(rawSpeed, 0, BIT_RESOLUTION, MIN_DELAY, MAX_DELAY); //0 = slow , 1023 = fast
  
  if (PULSE(PULSE_IO,flag_pulse)) {
    Serial.println("Pulse détecté → 1L → Rotation de " + String(anglePerLiter) + "°");
    Serial.println("vitesse: " + String(currentDelay) + " ms");
    rotateAngle(anglePerLiter);
    reset_coil();
  }
  delay(10);
  
}

