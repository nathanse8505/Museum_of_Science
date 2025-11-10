#include "Routines.h"

void setup() {
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
  pinMode(encoder_sw, INPUT_PULLUP);
  pinMode(encoder_pinA, INPUT_PULLUP); 
  pinMode(encoder_pinB, INPUT_PULLUP); 
  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);
  led_off();
  Serial.begin(BAUDRATE);
  delay (100);// wait to make sure serial begin
  start_display();
  Serial.println("START");
  digitalWrite(SR_en_pin, LOW); 
  update_height(); // for auto factoring
  drawing_index = 0;
  display_settings();
}

void loop() {

  do_encoder();
  
  if (space_flag && millis() - last_space_time > space_time) {
    space_flag = false;
  }
  if (valve_on_flag && millis() - last_valve_on > (auto_factor_flag ? auto_valve_on_time : valve_on_time)) {
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

  if (!drawing_flag && !space_flag && !dim3_flag) {
    off_all_valves(num_of_valves);
    pulse_io(SR_st_pin);
    init_drawing(drawing_index);
  }
  if (drawing_flag && !dim3_flag) {
    if (check_drawing()) {
//      Serial.println("done drawing");
      drawing_flag = false;
      off_all_valves(num_of_valves);
      pulse_io(SR_st_pin);
      space_flag = true;
      last_space_time = millis();
      cassette_drawing += 2;
//      cassette_drawing = random(0,cassettes_num);
      color += 1;
      if (color >= colors_num)
        color = 0;
      if (cassette_drawing >= cassettes_num) {
        cassette_drawing = 0;
        drawing_index++;
        if (drawing_index >= drawings_num)
          drawing_index = 0;
      }
    }
  }

  if (dim3_flag && !drawing_flag && !space_flag) {
    off_all_valves(num_of_valves);
    pulse_io(SR_st_pin);
    init_drawing_3d(drawing_3d_index);
  }
  if (dim3_flag && drawing_flag) {
    if (check_drawing_3d()) {
      drawing_flag = false;
      off_all_valves(num_of_valves);
      pulse_io(SR_st_pin);
      space_flag = true;
      last_space_time = millis();
      drawing_3d_index++;
      if (drawing_3d_index >= drawings_num_3d)
        drawing_index = 0;
    }
  }
}
