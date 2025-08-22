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
// RESET d'abord, puis PS. T_GAP_US = délai entre RESET_fall -> PS_rise.
const uint16_t T_RESET_US   = 650;  // largeur RESET
const uint16_t T_PS_US      = 230;  // largeur PS
const uint16_t T_GAP_US     = 100;  // délai entre fin RESET et début PS
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

// Clock standard pour tous les bits après BIT0
uint8_t regularClockPulse_andSample() {
  digitalWrite(CLK_PIN, HIGH);
  // échantillonnage au milieu du HIGH (DATA stable)
  delayMicroseconds(T_CLK_HIGH / 2);
  uint8_t v = readDataBitRaw();
  delayMicroseconds(T_CLK_HIGH / 2);
  clockLowBlocking();
  return v;
}

// ====== LECTURE COMPLÈTE ======
void readS88() {
  // Vider buffer
  for (uint8_t m = 0; m < NUM_MODULES; m++) moduleBits[m] = 0;

  // --- 1) RESET (remet le registre série sur BIT0) ---
  digitalWrite(RST_PIN, HIGH);
  delayMicroseconds(T_RESET_US);
  digitalWrite(RST_PIN, LOW);

  // --- 2) Délai entre RESET et PS ---
  delayMicroseconds(T_GAP_US);

  // --- 3) PS / LOAD (capture l’instantané) ---
  digitalWrite(PS_PIN, HIGH);
  delayMicroseconds(T_PS_US);
  digitalWrite(PS_PIN, LOW);

  // --- 4) Lecture de BIT0 immédiatement après PS (entrée 1 du module 0) ---
  uint16_t bitIndex = 0;
  if (readDataBitRaw()) {
    moduleBits[0] |= (1u << 0);
  }
  bitIndex++;

  // --- 5) CLOCKs pour les bits suivants ---
  while (bitIndex < TOTAL_BITS) {
    uint8_t val = regularClockPulse_andSample();
    if (val) {
      const uint8_t module = bitIndex / BITS_PER_MODULE;
      const uint8_t pos    = bitIndex % BITS_PER_MODULE;
      moduleBits[module] |= (1u << pos);
    }
    bitIndex++;
  }
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
  Serial.println(F("S88 Master (RESET → PS → CLOCK) prêt."));
}

void loop() {
  readS88();
  dumpS88();
  delay(500);
}
