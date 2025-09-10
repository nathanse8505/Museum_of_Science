#include "basic_routine.h"

// ====== SETUP ======
void setup() {
  Serial.begin(BAUDRATE);

  if (trainDetectionSignal == LOW) {
    for (int i = 0; i < numOfSegments; i++) pinMode(sensorPins[i], INPUT_PULLUP);
  } else {
    for (int i = 0; i < numOfSegments; i++) pinMode(sensorPins[i], INPUT);
  }
  for (int i = 0; i < numOfSegments; i++) pinMode(barrierPins[i], OUTPUT);

  pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_OK_PIN,   INPUT_PULLUP);

  // Tout ouvert au départ
  for (int i = 0; i < numOfSegments; i++) openBarrier(i);
  // Exemple d'état initial
  closeBarrier(1 % numOfSegments);

  // Mode manuel au boot
  manualRelaySelector();

  wdt_enable(WDTO_2S);
}

// ====== BOUCLE AUTO ======
void loop() {
  
  wdt_reset();
  int barrierCurrentState;
  for (int i = 0; i < numOfSegments; i++) {
    int sensorPin = sensorPins[i];
    if(i == 5 || i == 6 || i == 7){
      barrierCurrentState = analogRead(sensorPin);
    }
    else{
      barrierCurrentState = digitalRead(sensorPin);
    }
    
    int prevBarrierPin = barrierPins[getPreviuosSegment(i)];
    if (barrierCurrentState == trainDetectionSignal) {  
      Serial.println("sensorPin:" +String(i));
      Serial.println("barrierCurrentState:" +String(barrierCurrentState));
      openBarrier(i);
      delay(100);
      closeBarrier(getPreviuosSegment(i));  
      delay(100);
    }
  }
}
