#include "Routines.h"

void setup() {

  Serial.begin(BAUDRATE);
  delay (1000);// wait to make sure serial begin
  pinMode(SR_en_pin, OUTPUT);
  digitalWrite(SR_en_pin, HIGH);
  pinMode(SR_st_pin, OUTPUT);
  pinMode(SR_clk_pin, OUTPUT);
  pinMode(SR_data_pin, OUTPUT);
  digitalWrite(SR_st_pin, LOW);
  digitalWrite(SR_clk_pin, LOW);
  digitalWrite(SR_data_pin, LOW);
  off_all_valves(num_of_valves);
  pulse_io(SR_st_pin);//latch all valve to be off
  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);
  led_off();
  digitalWrite(SR_en_pin, LOW);//transfer the data to the valve

  got_param = false;
  Serial.println("START");
}

void loop() {

  if (!got_param && Serial.available()) {
    byte value;
//    Serial.readBytes((char *)&value, sizeof(value));
    value = Serial.read();
    Serial.println(value);
    switch(param_index) {
      case 0: image_h = value; break;
      case 1: valve_on_time = value; break;
      case 2: drawing_depth = value; break;
      default: Serial.println("ERROR: GOT TOO MUCH PARAMETERS"); break;
    }
    param_index++;
    if (param_index >= PARAM_NUMBER) {
      got_param = true;
      param_index = 0;
      Serial.println("got all parameters - Im good to go");
    }
  }
  
  if (Serial.available() && !drawing_flag && got_param) {
    char key;
    key = Serial.read();
    if (key == DROP_KEY) {
      init_drawing();  // drop what is currently in the image buffer
    }
    else if (key == START_KEY) {
        byte data;
        for (int i = 0; i < image_h*image_w/8; i++) {
          if (!Serial.available()) {
            i--;
            continue;
          }
          data = Serial.read();
          image[i] = data;
          if ((i + 1) % 8 == 0) {
            Serial.write(GOOD_KEY);
          }
        }
        delay(TIME_DELAY_ARDUINO);
        if (Serial.read() != END_KEY) {
//          display_error();
        }
        init_drawing();
      }
  }

  if (valve_on_flag && millis() - last_valve_on > valve_on_time) {
    off_all_valves(num_of_valves); // without pulsing ST because layers should be continuous
    valve_on_flag = false;
  }
  if (led_on_flag && millis() - last_led_on > led_on_time) {
    if (!full_light)
      led_off();
    led_on_flag = false;
  }
  if (led_start_flag && millis() - last_led_start > led_start) {
    led_start_flag = false;
    led_on_flag = true;
    last_led_on = millis();
    if (!full_light && led_on_time > 0)
      led_on(color);
  }
  
  if (drawing_flag) {
    if (check_drawing()) {
      drawing_flag = false;
      off_all_valves(num_of_valves);
      pulse_io(SR_st_pin);
      color += 1;
      if (color >= colors_num)
        color = 0;
      Serial.write(READY_KEY);
    }
  }
}
