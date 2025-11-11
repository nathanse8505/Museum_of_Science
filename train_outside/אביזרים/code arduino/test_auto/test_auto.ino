#include <Servo.h>

Servo servo_truck;

#define COIL_NADNEDA 3
#define SERVO_TRUCK_PIN 5

// Variables pour le servo
int pos_servo = 90;
int servo_step = 1;
unsigned long last_servo_update = 0;
const unsigned long servo_interval = 20; // ms

// Variables pour la bobine
int pos_pwm = 0;
int pwm_step = 1;
unsigned long last_pwm_update = 0;
const unsigned long pwm_interval = 10; // ms

void setup() {
  servo_truck.attach(SERVO_TRUCK_PIN);
  pinMode(COIL_NADNEDA, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // Gestion de la bobine
  if (now - last_pwm_update >= pwm_interval) {
    last_pwm_update = now;
    analogWrite(COIL_NADNEDA, pos_pwm);
    pos_pwm += pwm_step;
    if (pos_pwm >= 255 || pos_pwm <= 0) {
      pwm_step = -pwm_step; // Inverse la direction
    }
  }

  // Gestion du servo
  if (now - last_servo_update >= servo_interval) {
    last_servo_update = now;
    servo_truck.write(pos_servo);
    pos_servo += servo_step;
    if (pos_servo >= 180 || pos_servo <= 90) {
      servo_step = -servo_step; // Inverse la direction
    }
  }
}
