#ifndef FONCTION
#define FONCTION

#include "CONSTANTS.h"
#include <avr/wdt.h>

void Init_Output(int IO_Pin) {
  digitalWrite(IO_Pin, LOW); // Ensure the output is set to LOW initially
  pinMode(IO_Pin, OUTPUT); // Set the pin mode to OUTPUT
  digitalWrite(IO_Pin, LOW); // Ensure the output remains LOW
}

void Digits_from_Number(int in_number) {
  Digit_3_To_Display = in_number / 100; // Extract the hundreds digit
  Digit_2_To_Display = (in_number - 100 * Digit_3_To_Display) / 10; // Extract the tens digit
  Digit_1_To_Display = in_number % 10; // Extract the units digit
}

void Display_Digit(int digit_to_show) {
  digitalWrite(BCD_A, LOW); // Assume the bit is zero
  if ((digit_to_show & 1) == 1) {
    digitalWrite(BCD_A, HIGH); // Set BCD_A if bit 0 is 1
  }
  digitalWrite(BCD_B, LOW); // Assume the bit is zero
  if ((digit_to_show & 2) == 2) {
    digitalWrite(BCD_B, HIGH); // Set BCD_B if bit 1 is 1
  }
  digitalWrite(BCD_C, LOW); // Assume the bit is zero
  if ((digit_to_show & 4) == 4) {
    digitalWrite(BCD_C, HIGH); // Set BCD_C if bit 2 is 1
  }
  digitalWrite(BCD_D, LOW); // Assume the bit is zero
  if ((digit_to_show & 8) == 8) {
    digitalWrite(BCD_D, HIGH); // Set BCD_D if bit 3 is 1
  }
}

void enable(int digit_enable) {
  switch (digit_enable) {
    case 1: // Enable the rightmost (units) digit
      digitalWrite(LE_UNITS, LOW);
      digitalWrite(LE_UNITS, HIGH);
      break;
    case 2: // Enable the middle (tens) digit
      digitalWrite(LE_DOZENS, LOW);
      digitalWrite(LE_DOZENS, HIGH);
      break;
    case 3: // Enable the leftmost (hundreds) digit
      digitalWrite(LE_CENTURIES, LOW);
      digitalWrite(LE_CENTURIES, HIGH);
      break;
  }
}

void all_zero_digit() {
  digitalWrite(BCD_A, LOW); // Clear all BCD bits
  digitalWrite(BCD_B, LOW);
  digitalWrite(BCD_C, LOW);
  digitalWrite(BCD_D, LOW);
  for (int j = 1; j <= Number_OF_7SEG; j++) {
    enable(j); // Enable all digits
  }
}

void blank_Digit(int digit_enable) { // Blank a specific digit
  digitalWrite(BCD_A, HIGH); // Set all BCD bits to HIGH
  digitalWrite(BCD_B, HIGH);
  digitalWrite(BCD_C, HIGH);
  digitalWrite(BCD_D, HIGH);

  enable(digit_enable); // Enable the digit to blank
}

void blank_All_Digit() {
  for (int j = 1; j <= Number_OF_7SEG; j++) {
    blank_Digit(j); // Blank all digits
  }
}

bool SENSOR_OUT() {
  float volt_sensor = (float)(analogRead(SENSOR) * VREF) / BIT_RESOLUTION; // Calculate sensor voltage
  if (volt_sensor < MIN_VOLT) {
    return HIGH; // Sensor output is HIGH if below minimum voltage
  } else if (volt_sensor > MAX_VOLT) {
    return LOW; // Sensor output is LOW if above maximum voltage
  }
}

void Reset_Error_Sensor() {
  if ((millis() - timer) / ms_to_sec > (1.8 * delta_time_sec) && check == LOW) {
    check = HIGH; // Reset sensor error if conditions are met
  }
}

void Number_To_Display(float Number) {
  Digits_from_Number(Number); // Extract digits from the number
  Display_Digit(Digit_1_To_Display); // Display the units digit
  enable(RIGH_DIGIT);
  Display_Digit(Digit_2_To_Display); // Display the tens digit
  enable(MIDDLE_DIGIT);
  Display_Digit(Digit_3_To_Display); // Display the hundreds digit
  enable(LEFT_DIGIT);
}

#endif
