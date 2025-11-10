#include "parser.h"

void setup() {
  Serial.begin(BAUDRATE);
  delay (1000);  // wait to make sure serial begin 
  
  pinMode(IS_EN_JUMPER, INPUT_PULLUP);  // jumper to GND for constant hold/active motors    
  pinMode(STEPPER_EN_PIN, OUTPUT);  // invert!
  pinMode(LED_OUT, OUTPUT); 
  digitalWrite(LED_OUT, LOW);
  blink_led(START_LED_BLINK_NUMBER, START_LED_BLINK_ON, START_LED_BLINK_OFF);
  digitalWrite(STEPPER_EN_PIN, HIGH);  // disable

  if (digitalRead(IS_EN_JUMPER) == 0)
    digitalWrite(STEPPER_EN_PIN, LOW);  // enable if jumper set

  for(uint8_t i = 0; i < NUMBER_OF_MOTORS; i++) {
    pinMode(DIR_PIN[i], OUTPUT);
    digitalWrite(DIR_PIN[i], LOW);  // initialy set direction to low/0
    pinMode(STEP_PIN[i], OUTPUT);
    digitalWrite(STEP_PIN[i], LOW);  // pulse low 
  }
//  for (uint8_t i=0; i<NUMBER_OF_MOVES; i++) {
//    pinMode(MOVE_PIN[i],INPUT_PULLUP);    
//  }
  pinMode(DC_MOTOR_OUT, OUTPUT);
  digitalWrite(DC_MOTOR_OUT, LOW);
  homming();
  update_rates();
  
  // Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  Serial.println("READY");
  point_index = 0;
  param_index = 0;
}

void loop() {

  //  read_pushbuttons();
  bool got_input = (get_input() > 0);

  if (got_input)
    last_time_got_data = millis();

  if (getting_params && got_input) {
    int value = input.toInt();
    if (param_index < 4) {
      border[param_index] = value;
    }
    else {
      switch(param_index) {
        case 4: LASER_ON_POWER = value; update_scales(); break;
        case 5: FRAME_POWER = value; break;
        case 6: LASER_OFF_RATE = value; break;
        case 7: LASER_ON_RATE = value; break;
        case 8: FRAME_RATE = value; break;
        case 9: MAX_DC_MOTOR_TIME = value; break;
        default: Serial.print("PARAM INDEX: "); Serial.println(param_index); break;
      }
    }
    param_index++;
    if (param_index >= PARAMS_NUMBER) {
      getting_params = false;
      param_index = 0;
      Serial.println(ACK_KEY);
    }
  }

  // collect batch / finish
  if (processing_drawing && is_destination_done && !drawing_batch && got_input) {
    if (input == BATCH_DONE_KEY) {
      drawing_batch = true;
      point_index = 0;
      // laser_off();
      // rate = LASER_OFF_RATE;
      if (points[0] == -1)
        is_destination_done = true;  // do not actually try to go to -1...
      else
        is_destination_done = false;  // go to the point in the destination
      set_destination(points[0], points[1]);
    }

    if (input == DRAWING_DONE_KEY) {
      // switch to frame configuration
      drawing_frame = true;
      batch_size = 0;
      point_index = 0;
    }

    if (input == FRAME_DONE_KEY) {
      // done all drawing
      laser_off();
      processing_drawing = false;
      rate = LASER_OFF_RATE;
      set_destination(0,0);
      is_destination_done = false;
      // turn on dc motor
      dc_motor_on();
    }

    if (input == "None") {
      points[point_index] = -1;
      points[point_index + 1] = -1;
      point_index += 2;
      batch_size++;
    }

    else {
      int comma_index = input.indexOf(',');  // input = "x,y" for each point
      if (comma_index != -1) {
        String x_str = input.substring(0, comma_index);
        String y_str = input.substring(comma_index + 1);
        x = x_str.toFloat();
        y = y_str.toFloat();
        points[point_index] = (y - border[1]) * screen_scale[1];  // X and Y need to be inverted trust me
        points[point_index + 1] = (x - border[0]) * screen_scale[0];
        point_index += 2;
        batch_size++;
      }
    }
  }

  // move to next point in batch
  if (processing_drawing && is_destination_done && drawing_batch) {
    if (in_destination()) {
      laser_on();
      rate = drawing_frame ? FRAME_RATE : LASER_ON_RATE;
    }
    else {
      laser_off();
      rate = LASER_OFF_RATE;
    }
    point_index += 2;
    if (point_index < 2 * batch_size) {
      if (points[point_index] == -1) {
        laser_off();
        rate = LASER_OFF_RATE;
        set_destination(points[point_index], points[point_index + 1]);  // put -1 in the destination, will be used later
        is_destination_done = true;  // do not actually try to go to -1...
      }
      else {
        set_destination(points[point_index], points[point_index + 1]);
        is_destination_done = false;
      }
    }
    else {
      Serial.println(ACK_KEY);
      drawing_batch = false;
      point_index = 0;
      batch_size = 0;
      last_time_got_data = millis();
    }
  }

  check_dc_motor();

  update_rates();
  
  if (!is_destination_done) {
    move_to_destination();
  }

  // IMPORTANT: check if serial was disconnected while drawing, with a timeout of not getting any values
  if (processing_drawing && is_destination_done && !drawing_batch && !got_input) {
    if (millis() - last_time_got_data > SERIAL_TIMEOUT_ERROR) {
      laser_off();
      processing_drawing = false;
      rate = LASER_OFF_RATE;
      set_destination(0,0);
      is_destination_done = false;
    }
  }

  if (got_input) {
    process_input();
    input = "";
  }
}
