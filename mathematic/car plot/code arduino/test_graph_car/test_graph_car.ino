const int analogPin = A0; // Entrée analogique
const int outputPins[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // Pins de sortie

void setup() {
    Serial.begin(9600);
    // Définir les pins de sortie
    for (int i = 0; i < 8; i++) {
        pinMode(outputPins[i], OUTPUT);
    }
}

void loop() {
    
    //input_test();
    auto_test(); 
    
}

void input_test(){
    int analogValue = analogRead(analogPin); // Lire la valeur analogique
    int mappedValue = map(analogValue, 0, 1023, 0, 255); // Mapper de 0-1023 à 0-255
    Serial.println(mappedValue);
    // Écrire la valeur binaire sur les pins de sortie
    for (int i = 0; i < 8; i++) {
        digitalWrite(outputPins[i], (mappedValue >> i) & 1); // Extraction du bit i
    }
    delay(100); // Petite pause pour éviter trop de rafraîchissements
}

void auto_test(){
    
    // Écrire la valeur binaire sur les pins de sortie
    for(int j = 0; j<=255;j++){
      for (int i = 0; i < 8; i++) {
        digitalWrite(outputPins[i], (j >> i) & 1); // Extraction du bit i
      }
      delay(100); // Petite pause pour éviter trop de rafraîchissements
    }
}
