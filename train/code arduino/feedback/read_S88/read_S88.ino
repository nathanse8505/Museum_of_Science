/*
  S88 MASTER (remplace la centrale ECoS)
  - Génère PS (LOAD/LATCH) et CLOCK
  - Lit DATA depuis la chaîne de modules S88 (N modules x 16 bits)

  Câblage vers le premier module S88:
    Arduino PS_PIN   -> PS du bus S88 (parfois noté LOAD/LATCH/STROBE)
    Arduino CLK_PIN  -> CLOCK du bus S88
    Arduino DATA_PIN <- DATA OUT du module S88 (sortie série)
    GND commun
    +5V selon les besoins du module (vérifier la doc)

  Remarque protocole (classique S88):
    1) PS ↑ "fige" l’instantané des 16 entrées de chaque module
    2) Juste après PS, le BIT 0 est déjà disponible sur DATA
    3) Chaque CLOCK ↑ fait “avancer” au bit suivant (BIT 1, 2, ... 15, puis module suivant, etc.)

  Auteur: toi :)
*/

/*
s88 câble 6 fils (vue standard)

 1 : DATA   <-- données modules vers centrale
 2 : GND    -- masse
 3 : CLOCK  <-- impulsions centrale
 4 : PS     <-- latch centrale
 5 : RESET  (option, pas toujours câblé)
 6 : +5 V   -- alimentation logique
*/

#include <Arduino.h>

// ====== PARAMÈTRES ======
const uint8_t PS_PIN   = 3;     // Sortie PS
const uint8_t CLK_PIN  = 2;     // Sortie CLOCK
const uint8_t DATA_PIN = 12;    // Entrée DATA (depuis le module S88)

const uint8_t  BITS_PER_MODULE = 16;
const uint8_t  NUM_MODULES     = 1;      // <-- Mets 1, 2, 3... selon tes modules
const uint16_t TOTAL_BITS      = NUM_MODULES * BITS_PER_MODULE;

// Temporisations (µs) — commence large, puis réduis
const uint16_t T_PS_US    = 50;  // largeur d’impulsion PS
const uint16_t T_SETUP_US = 50;  // tps entre PS et 1ère lecture
const uint16_t T_HIGH_US  = 50;  // CLOCK haut
const uint16_t T_LOW_US   = 50;  // CLOCK bas

// Si tes modules sortent des niveaux inversés, mets à true
const bool INVERT_DATA = false;

// ====== STOCKAGE ======
uint16_t moduleBits[NUM_MODULES];  // 16 bits par module

// ====== FONCTIONS BAS NIVEAU ======
inline void pulsePS()
{
  digitalWrite(PS_PIN, HIGH);
  delayMicroseconds(T_PS_US);
  digitalWrite(PS_PIN, LOW);
}

inline void clockHigh()
{
  digitalWrite(CLK_PIN, HIGH);
  delayMicroseconds(T_HIGH_US);
}

inline void clockLow()
{
  digitalWrite(CLK_PIN, LOW);
  delayMicroseconds(T_LOW_US);
}

inline uint8_t readDataBitRaw()
{
  uint8_t b = digitalRead(DATA_PIN);
  if (INVERT_DATA) b = !b;
  return b;
}

// ====== LECTURE COMPLÈTE S88 ======
// Séquence “canonique”: PS ↑, petite attente, lire bit0,
// puis pour chaque bit suivant: CLOCK ↑ (attendre), lire, CLOCK ↓ (attendre).
void readS88()
{
  // Effacer le buffer
  for (uint8_t m = 0; m < NUM_MODULES; m++) moduleBits[m] = 0;

  // 1) LATCH / SNAPSHOT
  pulsePS();
  delayMicroseconds(T_SETUP_US);

  // 2) Lecture du premier bit (bit 0 du module 0)
  uint16_t bitIndex = 0;
  uint8_t bitVal = readDataBitRaw();
  if (bitVal) {
    uint8_t module = 0;
    uint8_t pos    = 0; // bit 0
    moduleBits[module] |= (1u << pos);
  }
  bitIndex++;

  // 3) Lire les bits suivants
  while (bitIndex < TOTAL_BITS)
  {
    // CLOCK ↑ -> le “prochain” bit est poussé sur DATA
    clockHigh();

    // Laisse le temps au signal de se stabiliser
    // (On lit pendant l'état haut; c'est robuste sur du S88 classique)
    uint8_t val = readDataBitRaw();

    // CLOCK ↓
    clockLow();

    // Stockage
    uint8_t module = bitIndex / BITS_PER_MODULE;      // 0..NUM_MODULES-1
    uint8_t pos    = bitIndex % BITS_PER_MODULE;      // 0..15
    if (val) moduleBits[module] |= (1u << pos);

    bitIndex++;
  }
}

// ====== DEBUG / AFFICHAGE ======
void printBits16(uint16_t v)
{
  for (int8_t i = 15; i >= 0; --i) {
    Serial.print((v & (1u << i)) ? '1' : '0');
    if (i == 8) Serial.print(' ');
  }
}

void dumpS88()
{
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
void setup()
{
  pinMode(PS_PIN,  OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT_PULLUP);  // souvent open-collector côté module; pull-up utile

  digitalWrite(PS_PIN, LOW);
  digitalWrite(CLK_PIN, LOW);

  Serial.begin(115200);
  delay(200);
  Serial.println(F("S88 Master prêt."));
}

void loop()
{
  // Exemple: lire la chaîne toutes les 200 ms
  readS88();
  dumpS88();

  // Exemple d’interprétation: l’entrée #k du module m est active si bit = 1
  // (bit 0 = entrée 1, bit 1 = entrée 2, ... bit 15 = entrée 16)
  // Tu peux traiter moduleBits[] ici…

  delay(200);
}
