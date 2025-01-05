#define TIME_CLK A0            // Analog input for the total cycle duration
#define PULSE_POSITIVE A1      // Analog input for the positive pulse duration
#define N_DIRECTION 3          // Pin to control the negative direction
#define P_DIRECTION 5          // Pin to control the positive direction
#define LED_CONTROL 7          // Pin for the control LED

float DELAY_RELAY = 20;         //delay between relays 
float cycle_max = 1000;        // Maximum cycle duration in milliseconds

void setup() {
  Serial.begin(9600);          // Initialize serial communication
  // Configure pins as output
  pinMode(N_DIRECTION, OUTPUT);
  pinMode(P_DIRECTION, OUTPUT);
  pinMode(LED_CONTROL, OUTPUT);
}

float time_positive;           // Duration of the positive signal (in microseconds)
float cycle_time;              // Total cycle duration (in milliseconds)

void loop() {
  // Condition: TIME_CLK signal must be greater than PULSE_POSITIVE
  while (analogRead(TIME_CLK) > analogRead(PULSE_POSITIVE)) {
    digitalWrite(LED_CONTROL, HIGH); // Turn on the control LED

    // Read and convert analog values
    time_positive = (float)analogRead(PULSE_POSITIVE) / 1023 * cycle_max;
    cycle_time = (float)analogRead(TIME_CLK) / 1023 * cycle_max;

    // Display information on the serial monitor
    Serial.print("Cycle Time: ");
    Serial.print(cycle_time);
    Serial.println(" ms");

    Serial.print("Positive Time: ");
    Serial.print(time_positive);
    Serial.println(" ms");

    // Activate positive direction for the duration of the positive signal
    digitalWrite(P_DIRECTION, HIGH);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, LOW);
    //delayMicroseconds(time_positive * 100);
    delay(time_positive);
  
    // Activate negative direction for the rest of the cycle
    digitalWrite(P_DIRECTION, LOW);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, LOW);
    //delay(cycle_time - time_positive/10);
    delay(cycle_time - time_positive);

    digitalWrite(P_DIRECTION, LOW);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, HIGH);
    //delayMicroseconds(time_positive * 100);
    delay(time_positive);
  
  
    // Activate negative direction for the rest of the cycle
    digitalWrite(P_DIRECTION, LOW);
    delay(DELAY_RELAY);
    digitalWrite(N_DIRECTION, LOW);
    delay(cycle_time - time_positive);
  }
  
  // Turn off the control LED when the condition is not met
  digitalWrite(LED_CONTROL, LOW);
}
