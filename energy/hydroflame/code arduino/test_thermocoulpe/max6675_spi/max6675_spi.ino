#include "MAX6675.h"

// Broches pour SPI logiciel
int thermo1D0  = 6;   // SO (MISO)
int thermo1CS  = 7;   // CS
int thermo1CLK = 8;   // SCK

MAX6675 thermocouple(thermo1CS, thermo1D0, thermo1CLK);

void setup() {
  Serial.begin(115200);
  delay(500);  // Laisser au capteur le temps de démarrer

  thermocouple.begin();  // Initialisation (nécessaire avec cette lib)
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
