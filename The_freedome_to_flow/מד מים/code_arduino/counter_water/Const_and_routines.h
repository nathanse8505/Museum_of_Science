#ifndef Const_and_routines
#define Const_and_routines


/*
*==========Arduino Nano pinout====== 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd  
 *                 RST-|       |-RST
 *                 GND-|       |-+5V  
 *                  D2-|       |-A7  
 *        PULSE_IO  D3-|       |-A6  
 *           COIL1  D4-|       |-A5 
 *           COIL2  D5-|       |-A4 
 *           COIL3  D6-|       |-A3
 *           COIL4  D7-|       |-A2   
 *                  D8-|       |-A1  potSpeedPin
 *                  D9-|       |-A0  potAnglePin
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */
// For the PULSE IO: connect a 1kΩ resistor between 5V and pin D3 
// --- MOTOR IO ---
#define COIL1 4
#define COIL2 5
#define COIL3 6
#define COIL4 7
// ---  Analog INPUT ---
#define potSpeedPin A0   // potentiomètre pour vitesse
#define potAnglePin A1   // potentiomètre pour angle
// --- détection pulse IO ---
#define PULSE_IO 2

long BAUDERATE = 115200;
// --- Séquence half step clockwise---
const int halfStepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

/* --- Séquence half step unclockwise---
const int halfStepSequence[8][4] = {
  {0, 0, 0, 1},
  {0, 0, 1, 1},
  {0, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {1, 0, 0, 0},
  {1, 0, 0, 1}
};*/
const int NUMBER_OF_TEST  = 2;
const int BIT_RESOLUTION = 1023;
const int MIN_DELAY = 2;// in ms
const int MAX_DELAY = 10;//in ms
const int MIN_ANGLE = 5;//in degree
const int MAX_ANGLE = 180;//in degree
//////// PULSE ///////
int stepIndex = 0;
bool flag_pulse = false;
const int BOUNCE_TIME = 50;

// ----speed----
int rawSpeed = 0;
int currentDelay = 3;

//-----angle----
int rawAngle = 0;
float anglePerLiter = 90.0;//in degreee
//const float STEPS_PER_TURNS = 64.0 * 63.68395;
const float STEPS_PER_TURNS = 4096;
const int ANGLE = 36; 


void stepMotor(int dir) {
  stepIndex = (stepIndex + dir + 8) % 8;

  digitalWrite(COIL1, halfStepSequence[stepIndex][0]);
  digitalWrite(COIL2, halfStepSequence[stepIndex][1]);
  digitalWrite(COIL3, halfStepSequence[stepIndex][2]);
  digitalWrite(COIL4, halfStepSequence[stepIndex][3]);

  delay(currentDelay);  // temps réglé par potentiomètre
}

void rotateAngle(float angleDeg) {
  int direction = (angleDeg >= 0) ? 1 : -1;
  int steps = (int)abs(angleDeg) * STEPS_PER_TURNS / 360;

  for (int i = 0; i < steps; i++) {
    stepMotor(direction);
  }
}

void reset_coil(){
  
  digitalWrite(COIL1, LOW);
  digitalWrite(COIL2, LOW);
  digitalWrite(COIL3, LOW);
  digitalWrite(COIL4, LOW);
}


bool PULSE(int BUTTON_IO) {

  // Check if the button is pressed
  if (digitalRead(BUTTON_IO) == LOW && flag_pulse == LOW) {
     //Serial.println("press :");
     flag_pulse = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(BUTTON_IO) == HIGH && flag_pulse == HIGH) {
    //Serial.println("unpress");
    flag_pulse = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}



#endif