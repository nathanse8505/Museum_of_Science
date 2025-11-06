#ifndef basic_routine
#define basic_routine

#include <Arduino.h>
#include <avr/wdt.h>

/*
*=================Arduino Nano pinout==================
 *                            _______
 *                       TXD-|       |-Vin 
 *                       RXD-|       |-Gnd  
 *                       RST-|       |-RST
 *                       GND-|       |-+5V  
 * SEGMENT_7_BARRIER_PIN  D2-|       |-A7 SEGMENT_7_SENSOR_PIN 
 * SEGMENT_6_BARRIER_PIN  D3-|       |-A6 SEGMENT_6_SENSOR_PIN 
 * SEGMENT_5_BARRIER_PIN  D4-|       |-A5 SEGMENT_5_SENSOR_PIN
 * SEGMENT_4_BARRIER_PIN  D5-|       |-A4 SEGMENT_4_SENSOR_PIN
 * SEGMENT_3_BARRIER_PIN  D6-|       |-A3 SEGMENT_3_SENSOR_PIN
 * SEGMENT_2_BARRIER_PIN  D7-|       |-A2 SEGMENT_2_SENSOR_PIN 
 * SEGMENT_1_BARRIER_PIN  D8-|       |-A1 SEGMENT_1_SENSOR_PIN  
 *                        D9-|       |-A0   
 *         BUTTON_OK_PIN D10-|       |-Ref
 *       BUTTON_NEXT_PIN D11-|       |-3.3V   
 *                       D12-|       |-D13
 *                            --USB--        
 */
#define BAUDRATE (115200)
// ====== CONFIG ======
const int Sample_Time = 2; // ms between measures

#define SEGMENT_1_SENSOR_PIN  A1
#define SEGMENT_2_SENSOR_PIN  A2
#define SEGMENT_3_SENSOR_PIN  A3
#define SEGMENT_4_SENSOR_PIN  A4
#define SEGMENT_5_SENSOR_PIN  A5
#define SEGMENT_6_SENSOR_PIN  A6
#define SEGMENT_7_SENSOR_PIN  A7

#define SEGMENT_1_BARRIER_PIN  8
#define SEGMENT_2_BARRIER_PIN  7
#define SEGMENT_3_BARRIER_PIN  6
#define SEGMENT_4_BARRIER_PIN  5
#define SEGMENT_5_BARRIER_PIN  4
#define SEGMENT_6_BARRIER_PIN  3
#define SEGMENT_7_BARRIER_PIN  2

#define BUTTON_OK_PIN    10
#define BUTTON_NEXT_PIN  11


const int trainDetectionSignal = LOW;   // capteur active to LOW (INPUT_PULLUP)
const int Station_Connected_Signal   = LOW;   // close = conected = LOW (relay NC)

// ====== TABLEAUX ======
const int sensorPins[]  = {
  SEGMENT_1_SENSOR_PIN, SEGMENT_2_SENSOR_PIN, SEGMENT_3_SENSOR_PIN,
  SEGMENT_4_SENSOR_PIN, SEGMENT_5_SENSOR_PIN, SEGMENT_6_SENSOR_PIN,
  SEGMENT_7_SENSOR_PIN
};
const int barrierPins[] = {
  SEGMENT_1_BARRIER_PIN, SEGMENT_2_BARRIER_PIN, SEGMENT_3_BARRIER_PIN,
  SEGMENT_4_BARRIER_PIN, SEGMENT_5_BARRIER_PIN, SEGMENT_6_BARRIER_PIN,
  SEGMENT_7_BARRIER_PIN
};
const int numOfSegments = 7;
//const int numOfSegments = sizeof(sensorPins) / sizeof(sensorPins[0]);

// ====== BOUTONS / DEBOUNCE ======
const unsigned long DEBOUNCE_MS      = 30;
const unsigned long DOUBLE_CLICK_MS  = 400;

bool lastNextState = HIGH;
bool lastOkState   = HIGH;
unsigned long lastNextChangeMs = 0;
unsigned long lastOkChangeMs   = 0;
unsigned long lastOkClickMs    = 0;
uint8_t okClickCount = 0;

// ====== PROTOTYPES ======
void Station_Disconnected(int segmentIndex);
void Station_Connected(int segmentIndex);
bool isStationConnected(int segmentIndex);
int  getPreviousSegment(int segment);
void manualRelaySelector();
bool nextButtonPressed();
bool okButtonPressed();
bool okDoubleClickDetected();


// ====== FONCTIONS BARRIÈRES ======
void Station_Disconnected(int segmentIndex){
  Serial.print("closing barrier: ");
  Serial.print(segmentIndex + 1);
  Serial.print("\r\n");
  digitalWrite(barrierPins[segmentIndex], !Station_Connected_Signal);
}

void Station_Connected(int segmentIndex){
  Serial.print("opening barrier: ");
  Serial.print(segmentIndex + 1);
  Serial.print("\r\n");
  digitalWrite(barrierPins[segmentIndex], Station_Connected_Signal);
}

int getPreviuosSegment(int segment){
  if (segment > 0)  {
    return segment - 1;
  }
  return numOfSegments - 1;
}

bool isStationConnected(int segmentIndex) {
  return (digitalRead(barrierPins[segmentIndex]) == Station_Connected_Signal);
}


// ====== MODE MANUEL (2 boutons) ======
void manualRelaySelector() {
  Serial.println("=== MANUAL MODE ===");
  int selected = 0;
  Serial.print("Selected relay: "); Serial.println(selected + 1);

  while (true) {
    // 1) NEXT : defile 1→7 et CLOSE le relais sélectionné
    if (nextButtonPressed()) {
      selected = (selected + 1) % numOfSegments;
      Serial.print("Selected relay: ");
      Serial.println(selected + 1);
      if (isStationConnected(selected)) {
        Station_Disconnected(selected);
      } else {
        Station_Connected(selected);
      }
    }

    // 2) OK : single = TOGGLE, double = EXIT
    if (okButtonPressed()) {
      okClickCount++;
      lastOkClickMs = millis();
    }
    if (okDoubleClickDetected()) {
      Serial.println("OK double-click: exit manual mode");
      okClickCount = 0;
      break; // sortir du while → retour à loop()
    }

    // Single click si délai écoulé sans second clic
    if (okClickCount == 1 && (millis() - lastOkClickMs) > DOUBLE_CLICK_MS) {
      // TOGGLE du relais sélectionné
     if (isStationConnected(selected)) {
        Station_Disconnected(selected);
      } else {
        Station_Connected(selected);
      }
      okClickCount = 0;
    }

    delay(1);
  }

  Serial.println("=== EXIT MANUAL MODE ===");
}

// ====== AIDES BOUTONS ======
bool nextButtonPressed() {
  bool reading = digitalRead(BUTTON_NEXT_PIN);
  unsigned long now = millis();
  if (reading != lastNextState && (now - lastNextChangeMs) > DEBOUNCE_MS) {
    lastNextChangeMs = now;
    lastNextState = reading;
    if (reading == LOW) return true; // press (active-LOW)
  }
  return false;
}

bool okButtonPressed() {
  bool reading = digitalRead(BUTTON_OK_PIN);
  unsigned long now = millis();
  if (reading != lastOkState && (now - lastOkChangeMs) > DEBOUNCE_MS) {
    lastOkChangeMs = now;
    lastOkState = reading;
    if (reading == LOW) return true; // press
  }
  return false;
}

bool okDoubleClickDetected() {
  if (okClickCount >= 2) {
    if ((millis() - lastOkClickMs) <= DOUBLE_CLICK_MS) {
      return true; // double-clic validé
    } else {
      okClickCount = 0; // trop lent, on repart
    }
  }
  return false;
}


#endif