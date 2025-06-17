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
  pulse_io(SR_st_pin);
//  pinMode(encoder_sw, INPUT_PULLUP);
//  pinMode(encoder_pinA, INPUT); 
//  pinMode(encoder_pinB, INPUT); 
  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);
  led_off();
  
//  start_display();
  digitalWrite(SR_en_pin, LOW);
  // get variables:
//  display_vars_collect();
//  uint16_t last_collect = millis();
//  delay(2000);
//  while (Serial.available() == 0) {
//    delay(10);
////    if (millis() - last_collect > 8000) {
////      display_vars_error();
////      delay(5000);
//////      display_vars_collect();
////      last_collect = millis();
////    }
////  }
//  byte value;
//  Serial.readBytes((char *)&value, sizeof(value)); // Read the float value from serial
//  Serial.println(value);
  //get number of variables - serial is not empty because of loop
  // super duper important:
//  if (!collect_variables()) {
//    display_vars_more_error();
//    delay(4000);
//  }
//  else {
//    Serial.write(GOOD_KEY);
////    display_vars_good();
//    delay(1000);
//    display_done();
//  }
//  display_done();
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
  
  if (Serial.available() > 0 && !drawing_flag && got_param) {
    char key;
    key = Serial.read();
    if (key == DROP_KEY) {
      init_drawing();  // drop what is currently in the image buffer
    }
    else {
      if (key == START_KEY) {
        delay(TIME_DELAY_ARDUINO);
//        uint32_t start_time = millis();
        cassette_drawing = Serial.read();
//        cassette_drawing = 0;
        byte data;
        for (int i = 0; i < image_h*image_w/8; i++) {
//          if (millis() - start_time > MAX_TIME_TO_COLLECT_DATA) {
////            display_error();
//            break;
//          }
          if (Serial.available() == 0) {
            i--;
            continue;
          }
          data = Serial.read();
          image[i] = data;
          
  //        Serial.println(data, DEC);
  //        Serial.print(data); 
  //        Serial.write(data);
          if ((i + 1) % 8 == 0) {
            Serial.write(GOOD_KEY);
  //          Serial.write(image[i]);
  //        Serial.println();
  //        for (int j = 7; j >=0; j--) {
  //          Serial.write(image[i-j]);
  //        }
          }
        }
        delay(TIME_DELAY_ARDUINO);
        if (Serial.read() != END_KEY) {
//          display_error();
        }
        init_drawing();
      }
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
