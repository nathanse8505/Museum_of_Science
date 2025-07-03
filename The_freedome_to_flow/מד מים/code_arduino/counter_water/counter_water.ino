#include "Const_and_routines.h"


void setup() {
  pinMode(COIL1, OUTPUT);
  pinMode(COIL2, OUTPUT);
  pinMode(COIL3, OUTPUT);
  pinMode(COIL4, OUTPUT);
  reset_coil();

  pinMode(PULSE_IO, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PULSE_IO), pulseDetected, FALLING);

  Serial.begin(BAUDERATE);
  Serial.println("System ready. Waiting for pulses...");
}

void loop() {
  // Lire le potentiomètre de vitesse
  rawSpeed = analogRead(potSpeedPin);
  currentDelay = map(rawSpeed, 0, BIT_RESOLUTION, MIN_DELAY, MAX_DELAY);  // inverse : 0 rapide, 1023 lent

  // Lire le potentiomètre d’angle
  rawAngle = analogRead(potAnglePin);
  anglePerLiter = map(rawAngle, 0, BIT_RESOLUTION, MIN_ANGLE, MAX_ANGLE);  // 0 à 180°
   
  if (newPulse) {
    newPulse = false;
    Serial.println("Pulse détecté → 1L → Rotation de " + String(anglePerLiter) + "°");
    Serial.println("vitesse: " + String(currentDelay) + " ms");
    rotateAngle(anglePerLiter);
    reset_coil();
  }
  delay(10);
  
}

