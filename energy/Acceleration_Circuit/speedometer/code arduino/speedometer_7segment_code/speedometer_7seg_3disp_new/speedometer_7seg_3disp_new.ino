#include "CONSTANTS.h"
#include "FONCTION.h"


void setup() {
  Serial.begin(SERIAL_BAUD_RATE); // Initialize serial communication at the specified baud rate

  pinMode(SENSOR, INPUT_PULLUP); // Set the sensor pin as input with an internal pull-up resistor

  Init_Output(BCD_A); // Initialize output pin for BCD_A
  Init_Output(BCD_B); // Initialize output pin for BCD_B
  Init_Output(BCD_C); // Initialize output pin for BCD_C
  Init_Output(BCD_D); // Initialize output pin for BCD_D

  Init_Output(LE_UNITS); // Initialize output pin for the right digit
  digitalWrite(LE_UNITS, HIGH); // Lock the right digit

  Init_Output(LE_DOZENS); // Initialize output pin for the middle digit
  digitalWrite(LE_DOZENS, HIGH); // Lock the middle digit

  Init_Output(LE_CENTURIES); // Initialize output pin for the left digit
  digitalWrite(LE_CENTURIES, HIGH); // Lock the left digit

  wdt_enable(WDTO_2S); // Enable the watchdog timer with a 2-second timeout

  Serial.println("init"); // Indicate initialization is complete
}

void loop() {
  wdt_reset(); // Reset the watchdog timer
  
  if (SENSOR_OUT() == HIGH && check == HIGH) { // Detect a transition from black to white and start timing
    Serial.println("we are in a black to white zone start"); 

    timer = millis(); // Record the start time
    check = LOW; // Mark the state as LOW to detect the stop condition
    delay(DELAY_BOUNCING); // Debounce delay to avoid false triggers
  }
  
  
  if (SENSOR_OUT() == HIGH && check == LOW) { // Detect a transition from black to white and stop timing
    Serial.println("we are in a black to white zone stop");  

    delta_time_sec = (millis() - timer) / ms_to_sec; // Calculate the time interval in seconds
    check = HIGH; // Reset the check state to HIGH
    check_zero = HIGH; // Indicate that speed should be calculated
    velocity = meter / delta_time_sec; // Calculate the velocity in m/s
    km_per_h = velocity * ms_to_KmH; // Convert velocity to km/h
    rpm = sec_to_min / delta_time_sec; // Calculate the rotations per minute

    Serial.print(velocity, 3); // Print the velocity in m/s to the serial monitor
    Serial.println(" m/s");
    Serial.print(rpm, 3); // Print the RPM to the serial monitor
    Serial.println(" rpm\n");

    km_per_h = km_per_h * CALIBRATION; // Apply calibration factor to the speed
    Number_To_Display(km_per_h); // Display the speed on the 7-segment displays
  }

  // Reset the sensor error if necessary
  // Reset_Error_Sensor();

  if ((millis() - timer) > TIME_RESET && check_zero) { // Reset the speedometer if no signal for a defined time
    Serial.println("reset the speedometer"); 

    timer = 0; // Reset the timer
    velocity = 0; // Reset the velocity
    km_per_h = 0; // Reset the speed in km/h
    rpm = 0; // Reset the RPM
    check_zero = LOW; // Indicate the reset has been performed
    check = HIGH; // Allow the sensor to re-detect transitions
    // blank_All_Digit(); // Optional: Blank all digits
    all_zero_digit(); // Set all digits to zero on the display
  }
}
