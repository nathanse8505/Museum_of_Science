#define NUM_SENSORS 16
byte sensors[NUM_SENSORS] = {A0, A1, A2, A3, A4, A5, 4, 5, 6, 7, 8, 9, 10, 11, A6, A7};

uint16_t lastState = 0;
unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 100;  // ms

void setup() {
  Serial.begin(115200); // Compatible avec Rocrail
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(sensors[i], INPUT_PULLUP);
  }
}

void loop() {
  uint16_t currentState = 0;

  for (int i = 0; i < NUM_SENSORS; i++) {
    if (!digitalRead(sensors[i])) {
      bitSet(currentState, i);
    }
  }

  if (currentState != lastState || millis() - lastSend > SEND_INTERVAL) {
    sendDSD2010SensorState(currentState);
    lastState = currentState;
    lastSend = millis();
  }

  delay(5);
}

/*void loop() {
  sendDSD2010SensorState(0b0000000000000001); // bit 0 actif (capteur A0 = actif)
  delay(1000);
  sendDSD2010SensorState(0); // tout off
  delay(1000);
}*/


void sendDSD2010SensorState(uint16_t state) {
  byte lowByte = state & 0xFF;
  byte highByte = (state >> 8) & 0xFF;

  Serial.write('D');      // D = détecteurs
  Serial.write('0');      // Sous-type
  Serial.println(lowByte);  // bits 0-7
  Serial.println(highByte);  // bits 0-7
  Serial.write(lowByte);  // bits 0-7
  Serial.write(highByte); // bits 8-15
  Serial.write(0x0D);     // Fin de ligne (carriage return)
}
