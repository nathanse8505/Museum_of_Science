#include <Keyboard.h>

// === Hardware ===
#define BUTTON_IO_A2 A2   // ZOOM +
#define BUTTON_IO_A3 A3   // ZOOM -

// === Réglages ===
const bool   LAYOUT_AZERTY       = false;   // mets true si clavier AZERTY
const bool   USE_CMD_INSTEAD_OF_CTRL = false; // mets true si tu veux Cmd (macOS) au lieu de Ctrl

// Répétition façon clavier (appui long)
const uint16_t REPEAT_START_MS   = 350;     // délai avant 1ère répétition
const uint16_t REPEAT_INTERVAL_MS= 40;      // intervalle entre répétitions
// (tu peux descendre à 40–60 ms si tu veux un zoom plus rapide)

// === Internes ===
struct RepeatState {
  bool held = false;              // bouton actuellement maintenu ?
  unsigned long nextFireMs = 0;   // prochain envoi autorisé
};
RepeatState plusState, minusState;

const int16_t BAUDERATE = 115200;

void setup() {
  Serial.begin(BAUDERATE);
  pinMode(BUTTON_IO_A2, INPUT_PULLUP);
  pinMode(BUTTON_IO_A3, INPUT_PULLUP);
  Keyboard.begin();
  delay(1000); // temps de reconnaissance USB
}

static inline void sendZoom(bool plus) {
  // Choix du modificateur (Ctrl ou Cmd)
  uint8_t mod = USE_CMD_INSTEAD_OF_CTRL ? KEY_LEFT_GUI : KEY_LEFT_CTRL;

  Keyboard.press(mod);

  if (plus) {
    // Pour “+” : sur layout US, c'est '=' avec Ctrl.
    // Sur AZERTY, il faut Shift + '=' pour obtenir '+'.
    if (LAYOUT_AZERTY) Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press('=');
  } else {
    Keyboard.press('-');
  }

  delay(2);          // courte impulsion pour être sûr que l'OS capte
  Keyboard.releaseAll();
}

static inline void handleButton(uint8_t pin, bool isPlus, RepeatState &st) {
  bool pressedNow = (digitalRead(pin) == LOW); // INPUT_PULLUP : LOW = appuyé
  unsigned long now = millis();

  if (pressedNow) {
    if (!st.held) {
      // Transition NON->OUI : envoyer une étape immédiate puis armer la répétition
      st.held = true;
      sendZoom(isPlus);
      st.nextFireMs = now + REPEAT_START_MS;
      Serial.println(isPlus ? "Zoom+ (first)" : "Zoom- (first)");
    } else if ((long)(now - st.nextFireMs) >= 0) {
      // Répétition
      sendZoom(isPlus);
      st.nextFireMs = now + REPEAT_INTERVAL_MS;
      Serial.println(isPlus ? "Zoom+ (repeat)" : "Zoom- (repeat)");
    }
  } else if (st.held) {
    // Transition OUI->NON : relâchement
    st.held = false;
    // Tout a déjà été relâché à chaque envoi, donc rien à faire ici.
  }
}

void loop() {
  // Gestion indépendante des deux boutons
  handleButton(BUTTON_IO_A2, /*isPlus=*/true,  plusState);
  handleButton(BUTTON_IO_A3, /*isPlus=*/false, minusState);

  // Optionnel : petite sieste pour réduire l'usage CPU
  delay(1);
}
