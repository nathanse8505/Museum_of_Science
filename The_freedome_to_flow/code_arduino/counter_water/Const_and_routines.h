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
 *  LED_ACTIVATION  D8-|       |-A1  potSpeedPin
 *                  D9-|       |-A0  potAnglePin
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 *                 D12-|       |-D13
 *                      --USB--        
 */

// --- MOTOR IO ---
#define COIL1 4
#define COIL2 5
#define COIL3 6
#define COIL4 7
// ---  Analog INPUT ---
#define potSpeedPin A0   // potentiomètre pour vitesse
#define potAnglePin A1   // potentiomètre pour angle
// --- détection pulse IO ---
#define PULSE_IO 3

long BAUDERATE = 115200;
// --- Séquence half step ---
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
const int BIT_RESOLUTION = 1023;
const int MIN_DELAY = 2;// in ms
const int MAX_DELAY = 10;//in ms
const int MIN_ANGLE = 5;//in degree
const int MAX_ANGLE = 180;//in degree

int stepIndex = 0;
volatile bool newPulse = false;

// ----speed----
int rawSpeed = 0;
int currentDelay = 3;

//-----angle----
int rawAngle = 0;
float anglePerLiter = 90.0;//in degreee
const float STEPS_PER_TURNS = 32.0 * 63.68395;

void pulseDetected() {
  newPulse = true;
}

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
  int steps = abs(angleDeg) * STEPS_PER_TURNS / 360;

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






#endif