#include <Arduino.h>

// --- Broches A4988 ---
const uint8_t PIN_STEP  = 3;   // STEP
const uint8_t PIN_DIR   = 2;   // DIR
const uint8_t PIN_SLEEP = 4;   // SLEEP
const uint8_t PIN_MS1   = 7;   // MS1
const uint8_t PIN_MS2   = 6;   // MS2
const uint8_t PIN_MS3   = 5;   // MS3

// --- Boutons (INPUT_PULLUP : appui = LOW) ---
#define BUTTON_CLOCKWISE     8
#define BUTTON_COUNTERCLOCK  9

// --- Paramètres moteur ---
const int    STEPS_PER_REV = 200;  // pas par tour du moteur (full step)
int          RPM            = 100; // vitesse cible
int          MICROSTEPS     = 4;   // 1,2,4,8,16 (micro-stepping)

// --- Timing ---
const uint8_t STEP_PULSE_US = 3; // largeur d’impulsion STEP (>=1us)
unsigned long stepIntervalUs = 0;
unsigned long lastStepUs     = 0;

// Map microstepping -> états MS1..MS3
void setMicrostepping(int micro){
  bool ms1=LOW, ms2=LOW, ms3=LOW;
  switch (micro){
    case 1:   ms1=LOW; ms2=LOW; ms3=LOW; break; // Full
    case 2:   ms1=HIGH; ms2=LOW; ms3=LOW; break; // 1/2
    case 4:   ms1=LOW; ms2=HIGH; ms3=LOW; break; // 1/4
    case 8:   ms1=HIGH; ms2=HIGH; ms3=LOW; break; // 1/8
    case 16:  ms1=HIGH; ms2=HIGH; ms3=HIGH; break; // 1/16
    default:  ms1=LOW; ms2=LOW; ms3=LOW; MICROSTEPS=1; break;
  }
  digitalWrite(PIN_MS1, ms1);
  digitalWrite(PIN_MS2, ms2);
  digitalWrite(PIN_MS3, ms3);
}

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
  pinMode(PIN_SLEEP, OUTPUT);
  pinMode(PIN_MS1,   OUTPUT);
  pinMode(PIN_MS2,   OUTPUT);
  pinMode(PIN_MS3,   OUTPUT);

  pinMode(BUTTON_CLOCKWISE,    INPUT_PULLUP);
  pinMode(BUTTON_COUNTERCLOCK, INPUT_PULLUP);

  digitalWrite(PIN_STEP,  LOW);
  digitalWrite(PIN_DIR,   LOW);
  digitalWrite(PIN_SLEEP, LOW); // dort par défaut (chauffe moins)

  setMicrostepping(MICROSTEPS);
  updateStepInterval();
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
    // Réveil du driver
    digitalWrite(PIN_SLEEP, HIGH);

    // Choix du sens (inverse si besoin selon ton câblage)
    digitalWrite(PIN_DIR, moveCW ? HIGH : LOW);

    // Génére des pas au bon débit sans bloquer
    unsigned long now = micros();
    if (now - lastStepUs >= stepIntervalUs) {
      doOneStep();
      lastStepUs = now;
    }
  } else {
    // Personne n’appuie (ou les deux en même temps) → arrêt & sommeil
    digitalWrite(PIN_SLEEP, LOW);
  }

  // Petit “yield” pour laisser respirer
  // (inutile d’ajouter un delay si la fréquence de pas est réglée par micros())
}
