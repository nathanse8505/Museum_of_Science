#include "MAX6675.h"

// Broches pour SPI logiciel
int thermo1D0  = A1;   // SO (MISO)
int thermo1CS  = 12;   // CS
int thermo1CLK = A2;   // SCK

MAX6675 thermocouple(thermo1CS, thermo1D0, thermo1CLK);

void setup() {
  Serial.begin(115200);
  delay(500);  // Laisser au capteur le temps de démarrer

  thermocouple.begin();  // Initialisation (nécessaire avec cette lib)
  Serial.println("init");
}

void loop() {
  // Lecture obligatoire pour mettre à jour la température
  uint8_t status = thermocouple.read();

  if (status == STATUS_OK) {
    float tempC = thermocouple.getCelsius();
    Serial.print("Température : ");
    Serial.print(tempC);
    Serial.println(" °C");
  } else {
    Serial.print("Erreur capteur : code ");
    Serial.println(status);
  }
  //Serial.println(thermocouple.lastRead()); // Affiche le timestamp (ms) de la dernière lecture


  delay(220);
}
