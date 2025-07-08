#ifndef ROUTINES_H
#define ROUTINES_H

#include "Consts.h"
#include "Display.h"

void pulse_io(byte);
void off_all_valves(uint16_t);
void image_to_valves(byte, bool);
void init_drawing();
bool check_drawing();
//void do_encoder();
//int read_encoder();
void led_on(byte);
void led_off();

void pulse_io(byte io_num) {
  digitalWrite(io_num, HIGH);
  digitalWrite(io_num, LOW);
}

// without pulsing ST!
void off_all_valves(uint16_t num_of_valves) {
  digitalWrite(SR_data_pin, LOW);
  for (int i = 0; i < num_of_valves; i++)
    pulse_io(SR_clk_pin);
}


void image_to_valves(byte r_index, bool reverse) {
  if (reverse) {
    for (int i = image_w / 8 - 1; i >= 0; i--) {
      byte byteValue = image[r_index * 8 + i];
      for (int i = 0; i < 8; i++) {
        bool bitValue = bitRead(byteValue, i);
//        Serial.print(bitValue == true ? " " : "#");
        digitalWrite(SR_data_pin, !bitValue); // "true"=1=HIGH, "false"=0=LOW
        pulse_io(SR_clk_pin);
      }
    }
  }
  else {
    for (int i = 0; i < image_w / 8; i++) {
      byte byteValue = image[r_index * 8 + i];
      for (int i = 7; i >= 0; i--) {
        bool bitValue = bitRead(byteValue, i);
//        Serial.print(bitValue == true ? " " : "#");
        digitalWrite(SR_data_pin, !bitValue); // "true"=1=HIGH, "false"=0=LOW
        pulse_io(SR_clk_pin);
      }
    }
  }
//  Serial.println();
}



void init_drawing() {
  row_in_drawing = image_h - 1;//which row in the drawing is currently dropped (from bottom to top)
  drawing_flag = true;         // true if an image is currenlty being dropped
  valve_on_flag = false;       // true if the valves are on
  led_start_flag = true;       // true if waiting for lights (relevant if "full light" is off)
  led_on_flag = false;         // true if lights are on (relevant if "full light" is off)
  last_led_start = millis();    // last time delay in lights started
  if (full_light || led_start <= 0)
    led_on(color);
  else
    led_off();
  off_all_valves(num_of_valves);
  pulse_io(SR_st_pin);
//  Serial.println("drawing...");
}

bool check_drawing() {
  if (!valve_on_flag) {
    if (row_in_drawing == -1)
      return true;
    // drawing depth is the depth of the image - reapet for the neccesary amout of layers
    for (int i = 0; i < drawing_depth; i++)
        image_to_valves(row_in_drawing, cassette_drawing%2 + i%2);
    digitalWrite(SR_data_pin, LOW);
    for (int i = 0; i < cassette_drawing * image_w; i++)
      pulse_io(SR_clk_pin);
    pulse_io(SR_st_pin);
    valve_on_flag = true;
    last_valve_on = millis();
    row_in_drawing--;
  }
  return false;
}


void led_on(byte index) {
  led_off();
  analogWrite(red_led_pin, colors[index][0]);
  analogWrite(green_led_pin, colors[index][1]);
  analogWrite(blue_led_pin, colors[index][2]);
}
void led_off() {
  analogWrite(red_led_pin, LOW);
  analogWrite(green_led_pin, LOW);
  analogWrite(blue_led_pin, LOW);
}

boolean collect_variables() {
  image_h = Serial.read();
  valve_on_time = Serial.read();
  uint8_t buffer[sizeof(uint16_t)];
  Serial.readBytes(buffer, sizeof(buffer));
  memcpy(&led_start, buffer, sizeof(led_start));
  Serial.readBytes(buffer, sizeof(buffer));
  memcpy(&led_on_time, buffer, sizeof(led_on_time));
  drawing_depth = Serial.read();
  full_light = Serial.read();
  led_power = Serial.read();
  colors_num = Serial.read();
  for (int i = 0; i < colors_num; i++) {
    colors[i][0] = Serial.read();
    colors[i][1] = Serial.read();
    colors[i][2] = Serial.read();
  }
  if (Serial.available())
    return false;  // still got variables in buffer - not good
  return true;
}

void SEND_GOOD_KEY(int i){
  if ((i + 1) % NUM_OF_PIXEL == 0) {
    Serial.write(GOOD_KEY);
    }
}

#endif
