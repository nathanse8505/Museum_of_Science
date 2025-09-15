#include <Arduino.h>

// --- Broches A4988 ---
const uint8_t PIN_STEP  = 6;   // STEP
const uint8_t PIN_DIR   = 7;   // DIR
const uint8_t PIN_EN    = 8;   // EN

//////////////PIN_RST  5V;   // RST
/////////////PIN_SLEEP 5V;   // SLEEP

// --- Boutons (INPUT_PULLUP : appui = LOW) ---
#define BUTTON_CLOCKWISE     A0
#define BUTTON_COUNTERCLOCK  A1

// --- Paramètres moteur ---
const int STEPS_PER_REV = 200;  // pas par tour du moteur (full step)
const int RPM           = 200; // vitesse cible
const int MICROSTEPS    = 4;   // 1,2,4,8,16 (micro-stepping)

// --- Timing ---
const uint8_t STEP_PULSE_US  = 3; // largeur d’impulsion STEP (>=1us)
unsigned long stepIntervalUs = 0;
unsigned long lastStepUs     = 0;



// Recalcule l’intervalle entre deux pas en fonction de RPM & microsteps
void updateStepInterval(){
  // steps/s = RPM * (STEPS_PER_REV * MICROSTEPS) / 60
  // période (us) = 1e6 / steps/s
  double stepsPerSecond = (double)RPM * (double)STEPS_PER_REV * (double)MICROSTEPS / 60.0;
  if (stepsPerSecond < 1.0) stepsPerSecond = 1.0;
  stepIntervalUs = (unsigned long)(1000000.0 / stepsPerSecond);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_STEP,  OUTPUT);
  pinMode(PIN_DIR,   OUTPUT);

  pinMode(BUTTON_CLOCKWISE,    INPUT_PULLUP);
  pinMode(BUTTON_COUNTERCLOCK, INPUT_PULLUP);

  digitalWrite(PIN_STEP,  LOW);
  digitalWrite(PIN_DIR,   LOW);

  updateStepInterval();
  Serial.println("init");
}

void doOneStep(){
  // impulsion STEP
  digitalWrite(PIN_STEP, HIGH);
  delayMicroseconds(STEP_PULSE_US);
  digitalWrite(PIN_STEP, LOW);
}

void loop() {
  // Lecture boutons (appui = LOW)
  bool cwPressed  = (digitalRead(BUTTON_CLOCKWISE)    == LOW);
  bool ccwPressed = (digitalRead(BUTTON_COUNTERCLOCK) == LOW);
  
  // Si un seul des deux est appuyé → on tourne
  bool moveCW  = cwPressed  && !ccwPressed;
  bool moveCCW = ccwPressed && !cwPressed;

  if (moveCW || moveCCW) {

    // Choix du sens (inverse si besoin selon ton câblage)
    digitalWrite(PIN_DIR, moveCW ? HIGH : LOW);

    // Génére des pas au bon débit sans bloquer
    if (micros() - lastStepUs >= stepIntervalUs) {
      doOneStep();
      lastStepUs = micros();
    }
  }
  // (inutile d’ajouter un delay si la fréquence de pas est réglée par micros())
}
