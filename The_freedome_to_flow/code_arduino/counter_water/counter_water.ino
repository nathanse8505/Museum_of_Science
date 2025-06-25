// --- Pins moteur ---
int in1 = 4;
int in2 = 5;
int in3 = 6;
int in4 = 7;

// --- Entrées analogiques ---
const int potSpeedPin = A0;   // potentiomètre pour vitesse
const int potAnglePin = A1;   // potentiomètre pour angle

// --- Pin de détection de pulse ---
const int pulsePin = 3;

// --- Séquence demi-pas ---
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

int stepIndex = 0;
volatile bool newPulse = false;

int currentDelay = 3;
float anglePerLiter = 90.0;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  reset_coil();

  pinMode(pulsePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDetected, FALLING);

  Serial.begin(9600);
  Serial.println("System ready. Waiting for pulses...");
}

void loop() {
  // Lire le potentiomètre de vitesse
  int rawSpeed = analogRead(potSpeedPin);
  currentDelay = map(rawSpeed, 0, 1023, 2, 10);  // inverse : 0 rapide, 1023 lent

  // Lire le potentiomètre d’angle
  int rawAngle = analogRead(potAnglePin);
  anglePerLiter = map(rawAngle, 0, 1023, 5, 180);  // 0 à 180°

   
  if (newPulse) {
    newPulse = false;
    Serial.print("Pulse détecté → 1L → Rotation de ");
    Serial.print(anglePerLiter);
    Serial.println("°");
    Serial.print("vitesse: ");
    Serial.print(currentDelay);
    Serial.println("ms");

    rotateAngle(anglePerLiter);
  }
  reset_coil();
}

void pulseDetected() {
  newPulse = true;
}

void stepMotor(int dir) {
  stepIndex = (stepIndex + dir + 8) % 8;

  digitalWrite(in1, halfStepSequence[stepIndex][0]);
  digitalWrite(in2, halfStepSequence[stepIndex][1]);
  digitalWrite(in3, halfStepSequence[stepIndex][2]);
  digitalWrite(in4, halfStepSequence[stepIndex][3]);

  delay(currentDelay);  // temps réglé par potentiomètre
}

void rotateAngle(float angleDeg) {
  int direction = (angleDeg >= 0) ? 1 : -1;
  int steps = abs(angleDeg) * 2048 / 360;

  for (int i = 0; i < steps; i++) {
    stepMotor(direction);
  }
}

void reset_coil(){
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
