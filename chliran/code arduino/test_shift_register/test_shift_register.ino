// Pins
const int  OE = 5; //Output Enable
const int dataserial = 4;  // DS
const int clockPin = 3; // SH_CP
const int latchPin = 2;  // ST_CP


void setup() {
  pinMode(dataserial, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(OE, OUTPUT);

  digitalWrite(OE,LOW);
}

void loop() {
  // Exemple : Compteur binaire sur les LEDs
  for (int i = 0; i < 8; i++) {
    digitalWrite(latchPin, LOW); 
    //delay(10);        
    //shiftOut(dataserial, clockPin, MSBFIRST, B11111111); // Envoie 10101010 en binaire
    shiftOut(dataserial, clockPin, MSBFIRST, i); // Envoie i
    digitalWrite(latchPin, HIGH);        
    delay(500);
  }
}



