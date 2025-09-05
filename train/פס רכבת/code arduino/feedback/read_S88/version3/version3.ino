/*
  Viessmann 5217 x2 modules
  - Séquence personnalisée: PS+CLK "simultanés", puis RESET, puis 15 CLK → lire D7 (module 1),
    puis 8 CLK supplémentaires → lire D7 (module 2).
  - Lecture S88 "classique": 32 bits (2 modules x 16 bits).

  Câblage (adapter si besoin):
    PS_PIN   -> PS / LOAD bus
    CLK_PIN  -> CLOCK bus
    RST_PIN  -> RESET bus
    DATA_PIN <- DATA OUT du module (chaîne S88)
    GND commun (+5V selon besoins du module)
*/

#include <Arduino.h>

// ====== PINS ======
const uint8_t PS_PIN   = 5;
const uint8_t CLK_PIN  = 4;
const uint8_t RST_PIN  = 2;
const uint8_t DATA_PIN = 3;

// ====== PARAM ======
const uint8_t NUM_MODULES = 2;       // <<< deux modules
const uint8_t BITS_PER_MODULE = 16;  // S88 standard

// ====== TIMINGS (µs) ======
const uint16_t T_PS_SETUP_US    = 5;  // délai PS↑ -> CLK↑
const uint16_t T_CLK_HIGH_US    = 5;
const uint16_t T_CLK_LOW_US     = 5;
const uint16_t T_PS_HOLD_US     = 5;  // maintien avant PS↓
const uint16_t T_RESET_US       = 5;
const uint16_t T_AFTER_RESET_US = 5;
const uint16_t T_DATA_STABLE_US = 3;

// ====== UTILS ======
inline void clockPulse() {
  digitalWrite(CLK_PIN, HIGH);
  delayMicroseconds(T_CLK_HIGH_US);
  digitalWrite(CLK_PIN, LOW);
  delayMicroseconds(T_CLK_LOW_US);
}

inline void pulseHigh(uint8_t pin, uint16_t high_us) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(high_us);
  digitalWrite(pin, LOW);
}

inline uint8_t readDATA() {
  delayMicroseconds(T_DATA_STABLE_US);
  return (uint8_t)digitalRead(DATA_PIN);
}

// ====== Séquence CUSTOM pour lire D7 des 2 modules ======
/*
  Étapes:
  1) PS ↑
  2) Pendant PS↑ : un pulse CLK
  3) PS ↓
  4) RESET court
  5) 15 CLK -> D7 (module 1) sur DATAOUT -> lire
  6) 8 CLK  -> D7 (module 2) sur DATAOUT -> lire
*/
void readD7_twoModules_custom(uint8_t &d7_mod1, uint8_t &d7_mod2) {
  // Init niveaux
  digitalWrite(PS_PIN, LOW);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(RST_PIN, LOW);

  // (1) PS ↑
  digitalWrite(PS_PIN, HIGH);
  delayMicroseconds(T_PS_SETUP_US);

  // (2) CLK pendant PS
  clockPulse();

  // (3) PS ↓
  delayMicroseconds(T_PS_HOLD_US);
  digitalWrite(PS_PIN, LOW);

  // (4) RESET (court)
  pulseHigh(RST_PIN, T_RESET_US);
  delayMicroseconds(T_AFTER_RESET_US);

  // (5) 15 clocks -> D7 module 1
  for (uint8_t i = 0; i < 15; i++) clockPulse();
  d7_mod1 = readDATA();

  // (6) 8 clocks supplémentaires -> D7 module 2
  for (uint8_t i = 0; i < 8; i++) clockPulse();
  d7_mod2 = readDATA();
}

// ====== Lecture S88 "classique" de 2 modules (32 bits) ======
/*
  - RESET (pointeur au bit 0)
  - PS ↑ puis PS ↓ pour capturer l’instantané
  - Bit0 dispo immédiatement -> lire
  - Puis 31 pulses CLK pour bits 1..31
  Ordre renvoyé: bit0..bit15 (module 1), puis bit16..bit31 (module 2).
*/
void read2Modules_S88_classic(uint8_t bits[32]) {
  // Remise du pointeur
  pulseHigh(RST_PIN, T_RESET_US);
  delayMicroseconds(T_AFTER_RESET_US);

  // LATCH / LOAD
  digitalWrite(PS_PIN, HIGH);
  delayMicroseconds(T_PS_SETUP_US);
  digitalWrite(PS_PIN, LOW);

  // Bit 0 disponible immédiatement
  bits[0] = readDATA();

  // Lire le reste
  for (uint8_t i = 1; i < 32; i++) {
    clockPulse();
    bits[i] = readDATA();
  }
}

void setup() {
  pinMode(PS_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT); // ou INPUT_PULLUP selon ton câblage

  digitalWrite(PS_PIN, LOW);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(RST_PIN, LOW);

  Serial.begin(115200);
  delay(50);
  Serial.println(F("S88 (Viessmann 5217) - 2 modules : sequence custom + lecture classique"));
}

void loop() {
  // --- Séquence CUSTOM: D7 des 2 modules ---
  uint8_t d7_1 = 0, d7_2 = 0;
  readD7_twoModules_custom(d7_1, d7_2);
  Serial.print(F("[CUSTOM] D7 M1=")); Serial.print(d7_1);
  Serial.print(F("  D7 M2=")); Serial.println(d7_2);

  // --- Lecture CLASSIQUE: 32 bits ---
  uint8_t bits[32] = {0};
  read2Modules_S88_classic(bits);

  Serial.print(F("[CLASSIC] 32 bits: "));
  for (uint8_t i = 0; i < 32; i++) {
    Serial.print(bits[i]);
    if (i < 31) Serial.print(' ');
  }
  Serial.println();

  // Indices utiles:
  //  - D7 module 1 = bits[7]
  //  - D7 module 2 = bits[16+7] = bits[23]
  Serial.print(F("Vérif classic: D7 M1=")); Serial.print(bits[7]);
  Serial.print(F("  D7 M2=")); Serial.println(bits[23]);

  delay(200);
}
