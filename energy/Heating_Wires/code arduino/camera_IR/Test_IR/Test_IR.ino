#include <IRremote.h>

const int RECV_PIN = 2;  // Broche du TSSP vers l’Arduino (ex: D2)

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Démarre le récepteur IR
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.print("Code reçu : 0x");
    Serial.println(results.value, HEX);
    irrecv.resume(); // Recommence à écouter
  }
}
