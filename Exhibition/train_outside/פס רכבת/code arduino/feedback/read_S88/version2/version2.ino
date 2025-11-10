#include <Arduino.h>

// ====== PINS ======
const uint8_t PS_PIN   = 5;   // PS / LOAD
const uint8_t CLK_PIN  = 4;   // CLOCK
const uint8_t DATA_PIN = 3;   // DATA (modules -> Arduino)
const uint8_t RST_PIN  = 2;   // RESET
// ====== PARAMÈTRES ======
const uint8_t  BITS_PER_MODULE = 16;
const uint8_t  NUM_MODULES     = 2;
const uint16_t TOTAL_BITS      = (uint16_t)NUM_MODULES * BITS_PER_MODULE;

// *** NE PAS TOUCHER (mesures oscillo) ***
const uint16_t T_RESET_US   = 650;  // largeur RESET
const uint16_t T_PS_US      = 230;  // largeur PS
const uint16_t T_GAP_US     = 100;  // délai PS_fall -> RESET_rise (délai demandé)
const uint16_t T_CLK_HIGH   = 320;  // CLOCK haut
const uint16_t T_CLK_LOW    = 20;   // CLOCK bas

const bool INVERT_DATA = false;     // Mets true si DATA inversée

// ====== STOCKAGE ======
uint16_t moduleBits[NUM_MODULES];

// ====== UTIL ======
inline uint8_t readDataBitRaw() {
  uint8_t b = digitalRead(DATA_PIN);
  return INVERT_DATA ? !b : b;
}

inline void clockLowBlocking() {
  digitalWrite(CLK_PIN, LOW);
  delayMicroseconds(T_CLK_LOW);
}

// Premier front d’horloge « normal » (après le tout premier)
uint8_t regularClockPulse_andSample() {
  digitalWrite(CLK_PIN, HIGH);
  // échantillonnage au milieu du HIGH
  delayMicroseconds(T_CLK_HIGH / 2);
  uint8_t v = readDataBitRaw();
  delayMicroseconds(T_CLK_HIGH - (T_CLK_HIGH / 2));
  clockLowBlocking();
  return v;
}

// ====== LECTURE COMPLÈTE ======
void readS88() {
  // Vider buffer
  for (uint8_t m = 0; m < NUM_MODULES; m++) moduleBits[m] = 0;

  // --- 1) PS: démarre avant le CLOCK ---
  unsigned long t0 = micros();
  digitalWrite(RST_PIN, HIGH);

  uint16_t bitIndex = 0;

  // Calcul des instants de référence :
  // PS_fall = t0 + T_PS_US
  // On veut que PS tombe pendant le premier CLOCK haut → on démarre le premier HIGH à :
  // CLK1_rise = PS_fall - (T_CLK_HIGH / 2)  (ainsi la chute de PS arrive au milieu du HIGH)
  const unsigned long t_rs_fall   = t0 + T_RESET_US;
  const unsigned long t_clk1_rise = t_rs_fall - (T_CLK_HIGH / 2);

  // RESET doit démarrer 100 µs après la chute de PS (pendant le train d’horloges)
  const unsigned long t_ps_rise  = t_rs_fall + T_GAP_US;
  const unsigned long t_ps_fall  = t_ps_rise + T_PS_US;

  // Attendre le moment de lancer le premier CLOCK haut
  while ((long)(micros() - t_clk1_rise) < 0) { /* spin-wait court */ }

  // --- 2) Premier CLOCK haut ---
  digitalWrite(CLK_PIN, HIGH);
  unsigned long th = micros(); // début HIGH #1

  // Pendant ce premier HIGH :
  // - on laisse PS tomber exactement à t_ps_fall
  // - on échantillonne BIT1 après la chute de PS, au 3/4 du plateau
  bool rsLowered = false;
  bool sampled   = false;

  // Boucle « haute précision » pendant T_CLK_HIGH du 1er HIGH
  while ((micros() - th) < T_CLK_HIGH) {
    unsigned long now = micros();

    // Faire tomber RESET quand on atteint sa largeur exacte
    if (!rsLowered && (long)(now - t_rs_fall) >= 0) {
      digitalWrite(RST_PIN, LOW);
      rsLowered = true;
    }

  }
  // Fin du premier HIGH
  clockLowBlocking();

  // --- 3) Déclencher RESET pendant le train d’horloges ---
  // On génère la suite des clocks tout en respectant la fenêtre RESET
  // RESET_rise à t_rst_rise, RESET_fall à t_rst_fall (peut chevaucher des clocks)
  bool psUp = false;

  while (bitIndex < TOTAL_BITS) {
    // Avant de remonter CLOCK, vérifie si on doit lancer PS
    unsigned long now = micros();

    if (!psUp && (long)(now - t_ps_rise) >= 0) {
      digitalWrite(PS_PIN, HIGH);
      psUp = true;
    }
    if (psUp && (long)(now - t_ps_fall) >= 0) {
      digitalWrite(PS_PIN, LOW);
    }

    // CLOCK suivant « normal »
    uint8_t val = regularClockPulse_andSample();

    // (Après l’impulsion, ajuste PS si sa fenêtre tombe ici)
    now = micros();
    if (!psUp && (long)(now - t_ps_rise) >= 0) {
      digitalWrite(PS_PIN, HIGH);
      psUp = true;
    }
    if (psUp && (long)(now - t_ps_fall) >= 0) {
      digitalWrite(PS_PIN, LOW);
    }

    // Stockage
    if (val) {
      uint8_t module = bitIndex / BITS_PER_MODULE;
      uint8_t pos    = bitIndex % BITS_PER_MODULE;
      moduleBits[module] |= (1u << pos);
    }
    bitIndex++;
  }

  // Sécurité : s’assurer que PS est retombé
  digitalWrite(PS_PIN, LOW);
}

// ====== DEBUG ======
void printBits16(uint16_t v) {
  for (int8_t i = 15; i >= 0; --i) {
    Serial.print((v & (1u << i)) ? '1' : '0');
    if (i == 8) Serial.print(' ');
  }
}

void dumpS88() {
  Serial.println(F("=== S88 Snapshot ==="));
  for (uint8_t m = 0; m < NUM_MODULES; m++) {
    Serial.print(F("Module "));
    Serial.print(m);
    Serial.print(F(" : "));
    printBits16(moduleBits[m]);
    Serial.println();
  }
  Serial.println();
}

// ====== SETUP / LOOP ======
void setup() {
  pinMode(PS_PIN,   OUTPUT);
  pinMode(CLK_PIN,  OUTPUT);
  pinMode(RST_PIN,  OUTPUT);
  pinMode(DATA_PIN, INPUT_PULLUP);   // DATA souvent en collecteur ouvert

  digitalWrite(PS_PIN,  LOW);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(RST_PIN, LOW);

  Serial.begin(115200);
  delay(200);
  Serial.println(F("S88 Master (profil central observe) prêt."));
}

void loop() {
  readS88();
  dumpS88();
  delay(500);
}
