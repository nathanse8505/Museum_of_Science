/*
  S88 MASTER + 6 RELAYS + APPROACH HOLD + INTERLOCK
  --------------------------------------------------
  - Génère PS/CLOCK et lit DATA (bus s88)
  - Contrôle 6 relais de coupure de voie
  - Règles:
      (1) Si capteur enterIndex[k] == 1 => COUPER bloc k (train dedans)
      (2) Si capteur releaseIndex[k] passe 0->1 => RECONNECTER bloc k
      (3) APPROCHE PROTÉGÉE (option): si la zone aval n'est pas libre => couper dès l'approche
      (4) INTERLOCK: si plusieurs blocs partagent une zone conflictuelle, un seul à la fois
  - Polarité relais configurable (actif-bas par défaut)

  Câblage bus s88 vers le premier module:
    PS_PIN   -> PS / LOAD / LATCH (jaune)
    CLK_PIN  -> CLOCK (vert)
    DATA_PIN <- DATA (blanc)
    GND commun (brun)
    +5V du bus (orange) optionnel; NE PAS alimenter le bus depuis l'Arduino quand on sniffe.

  Auteur: ChatGPT pour Julie :)
*/

#include <Arduino.h>

// ================== PARAMÈTRES S88 ==================
const uint8_t PS_PIN   = 3;    // PS/LATCH (sortie)
const uint8_t CLK_PIN  = 2;    // CLOCK    (sortie)
const uint8_t DATA_PIN = 12;   // DATA     (entrée)

const uint8_t  BITS_PER_MODULE = 16;
const uint8_t  NUM_MODULES     = 2;                   // <-- adapte à ton installation
const uint16_t TOTAL_BITS      = (uint16_t)NUM_MODULES * BITS_PER_MODULE;

// Temporisations (µs) – commence large puis réduis si OK
const uint16_t T_PS_US    = 50;
const uint16_t T_SETUP_US = 50;
const uint16_t T_HIGH_US  = 50;
const uint16_t T_LOW_US   = 50;

// DATA inversée ?
const bool INVERT_DATA = false;

// ================== PARAMÈTRES RELAIS ==================
const uint8_t N_BLOCKS = 6;

// Pins Arduino utilisées pour les relais (à ADAPTER si besoin)
const uint8_t RELAY_PINS[N_BLOCKS] = { 4, 5, 6, 7, 8, 9 };

// Polarité des relais (beaucoup de modules sont actifs-bas)
const uint8_t RELAY_ACTIVE_LEVEL   = LOW;   // ACTIVE = LOW
const uint8_t RELAY_INACTIVE_LEVEL = HIGH;  // INACTIVE = HIGH

// ------------ MAPPING capteurs → blocs --------------
// enterIndex[k]   = capteur qui "entre" dans le bloc k -> coupe dès qu'il vaut 1
// releaseIndex[k] = capteur "suivant" -> reconnecte bloc k sur FRONT 0->1
// nextSensorForBlock[k] = capteur aval pour l'approche protégée (par défaut = releaseIndex[k])
uint16_t enterIndex[N_BLOCKS];
uint16_t releaseIndex[N_BLOCKS];
uint16_t nextSensorForBlock[N_BLOCKS];   // aval logique

// ================== INTERLOCK (anti-intersection) ==================
// Chaque bloc peut appartenir à un groupe de conflit: -1 = aucun; 0..MAX_GROUPS-1 = ID
// Exemple ci-dessous: blocs 2 et 3 partagent le groupe 0 (à adapter à ton réseau)
const uint8_t MAX_GROUPS = 4;            // augmente si tu as >4 zones conflictuelles
int8_t INTERLOCK_GROUP[N_BLOCKS] = { -1, -1, 0, 0, -1, -1 };
int8_t groupOwner[MAX_GROUPS];           // -1 = libre; sinon = index du bloc propriétaire

// ================== STOCKAGE LECTURE ==================
uint16_t moduleBits[NUM_MODULES];     // snapshot courant (16 bits / module)
uint16_t prevModuleBits[NUM_MODULES]; // snapshot précédent (pour fronts montants)

// ================== BAS NIVEAU S88 ==================
inline void pulsePS() {
  digitalWrite(PS_PIN, HIGH);
  delayMicroseconds(T_PS_US);
  digitalWrite(PS_PIN, LOW);
}
inline void clockHigh() { digitalWrite(CLK_PIN, HIGH); delayMicroseconds(T_HIGH_US); }
inline void clockLow()  { digitalWrite(CLK_PIN, LOW);  delayMicroseconds(T_LOW_US); }

inline uint8_t readDataBitRaw() {
  uint8_t b = digitalRead(DATA_PIN);
  return INVERT_DATA ? (uint8_t)!b : b;
}

// Helpers pour lire un bit à l'index aplati (0..TOTAL_BITS-1)
inline bool getBitFlat(const uint16_t bitsPerModule[], uint16_t flatIdx) {
  uint8_t m = flatIdx / BITS_PER_MODULE;
  uint8_t p = flatIdx % BITS_PER_MODULE;
  return (bitsPerModule[m] >> p) & 0x1;
}

// Lecture S88 : PS ↑, lire bit0, puis CLOCK↑/↓ pour bits suivants
void readS88() {
  for (uint8_t m = 0; m < NUM_MODULES; m++) moduleBits[m] = 0;

  pulsePS();
  delayMicroseconds(T_SETUP_US);

  // bit 0
  uint16_t bitIndex = 0;
  if (readDataBitRaw()) moduleBits[0] |= (1u << 0);
  bitIndex++;

  // bits 1..TOTAL_BITS-1
  while (bitIndex < TOTAL_BITS) {
    clockHigh();
    uint8_t val = readDataBitRaw();
    clockLow();

    uint8_t module = bitIndex / BITS_PER_MODULE; // 0..NUM_MODULES-1
    uint8_t pos    = bitIndex % BITS_PER_MODULE; // 0..15
    if (val) moduleBits[module] |= (1u << pos);

    bitIndex++;
  }
}

// ================== RELAIS ==================
inline void blockCut(uint8_t k)      { digitalWrite(RELAY_PINS[k], RELAY_ACTIVE_LEVEL);   }
inline void blockConnect(uint8_t k)  { digitalWrite(RELAY_PINS[k], RELAY_INACTIVE_LEVEL); }

// ================== APPROCHE & INTERLOCK ==================
inline bool isSensorActive(uint16_t s)  { return getBitFlat(moduleBits, s);     }
inline bool wasSensorActive(uint16_t s) { return getBitFlat(prevModuleBits, s); }

bool hasInterlockRight(uint8_t k) {
  int8_t g = INTERLOCK_GROUP[k];
  if (g < 0) return true;
  if (g >= (int8_t)MAX_GROUPS) return false;
  return (groupOwner[g] == -1 || groupOwner[g] == (int8_t)k);
}
void acquireInterlock(uint8_t k) {
  int8_t g = INTERLOCK_GROUP[k];
  if (g < 0 || g >= (int8_t)MAX_GROUPS) return;
  groupOwner[g] = k;
}
void releaseInterlock(uint8_t k) {
  int8_t g = INTERLOCK_GROUP[k];
  if (g < 0 || g >= (int8_t)MAX_GROUPS) return;
  if (groupOwner[g] == (int8_t)k) groupOwner[g] = -1;
}

// Option d'approche protégée (ON conseillé)
const bool ENABLE_APPROACH_HOLD = true;

// ================== LOGIQUE PRINCIPALE ==================
void updateBlocksFromSensors() {
  for (uint8_t k = 0; k < N_BLOCKS; k++) {
    const uint16_t e   = enterIndex[k];         // capteur d'entrée du bloc k
    const uint16_t r   = releaseIndex[k];       // capteur suivant (sortie de k)
    const uint16_t nxt = nextSensorForBlock[k]; // aval surveillé

    const bool enterNow  = isSensorActive(e);
    const bool enterPrev = wasSensorActive(e);
    const bool relNow    = isSensorActive(r);
    const bool relPrev   = wasSensorActive(r);

    // (0) APPROCHE PROTÉGÉE : si l'aval n'est pas libre, retenir avant d'entrer
    if (ENABLE_APPROACH_HOLD) {
      const bool avalLibre = !isSensorActive(nxt); // libre si capteur aval == 0
      if (enterNow && !relNow && !avalLibre) {
        blockCut(k);
        continue; // passe au bloc suivant
      }
    }

    // (1) ENTRÉE: couper dès que le bloc est occupé
    if (enterNow) {
      // Front montant sur e => tenter d'acquérir l'interlock
      if (!enterPrev) {
        if (hasInterlockRight(k)) {
          acquireInterlock(k);
        } else {
          blockCut(k);   // pas le droit -> retenir
          continue;
        }
      }
      blockCut(k);
    }

    // (2) SORTIE: reconnecter sur front montant de r (i+1)
    if (!relPrev && relNow) {
      blockConnect(k);
      releaseInterlock(k);
    }

    // (3) Sinon, ne rien changer (l'état courant du relais reste tel quel)
  }
}

// ================== DEBUG (option) ==================
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

// ================== SETUP / LOOP ==================
void setup() {
  // S88
  pinMode(PS_PIN,  OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT_PULLUP);
  digitalWrite(PS_PIN, LOW);
  digitalWrite(CLK_PIN, LOW);

  // Relais
  for (uint8_t k = 0; k < N_BLOCKS; k++) {
    pinMode(RELAY_PINS[k], OUTPUT);
    digitalWrite(RELAY_PINS[k], RELAY_INACTIVE_LEVEL); // sécurité: alimenté (reconnecté) au boot
  }

  // Mapping par défaut: blocs 0..5 gérés par bits 0..5 pour entrer, 1..6 pour release
  // => A ADAPTER à ton plan réel !
  for (uint8_t k = 0; k < N_BLOCKS; k++) {
    enterIndex[k]         = (k)         % TOTAL_BITS;
    releaseIndex[k]       = (k + 1)     % TOTAL_BITS;  // "i+1"
    nextSensorForBlock[k] = releaseIndex[k];           // aval = i+1
  }

  // Interlock: tout libre au départ
  for (uint8_t g = 0; g < MAX_GROUPS; g++) groupOwner[g] = -1;

  // Snapshots
  for (uint8_t m = 0; m < NUM_MODULES; m++) {
    moduleBits[m]     = 0;
    prevModuleBits[m] = 0;
  }

  Serial.begin(115200);
  delay(200);
  Serial.println(F("S88 Master + Relays + Approach + Interlock prêt."));
}

void loop() {
  // Sauvegarder l'ancien snapshot
  for (uint8_t m = 0; m < NUM_MODULES; m++) prevModuleBits[m] = moduleBits[m];

  // Lire le bus s88
  readS88();

  // Appliquer la logique blocs (coupe/reconnecte + interlock + approche)
  updateBlocksFromSensors();

  // Debug (optionnel)
  dumpS88();

  delay(100); // cadence de supervision (ajuste si besoin)
}
