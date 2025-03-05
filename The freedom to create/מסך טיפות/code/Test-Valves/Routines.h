

#include "Display.h"

void pulse_io(int);
void off_all_valves(int);
void on_all_valves(int);
void cycle_all_valves(int);
void valve_on(int);
void reverse_cycle_all_valves(int);
void array_to_valves(bool[], int);
int read_encoder();
void do_encoder(bool);
void delay_with_encoder(uint32_t);
void valve_on_from_settings(int, int, int);
void on_all_board(int, int);

void pulse_io(int io_num){
  digitalWrite(io_num, HIGH);   
  digitalWrite(io_num, LOW);
}

void off_all_valves(int num_of_valves){
  digitalWrite(SR_data_pin, LOW);   
  for (int i = 0; i < num_of_valves; i++)
    pulse_io(SR_clk_pin);
  pulse_io(SR_st_pin);
}

void on_all_valves(int num_of_valves){
  digitalWrite(SR_data_pin, HIGH);   
  for (int i = 0; i < num_of_valves; i++) 
    pulse_io(SR_clk_pin);
  digitalWrite(SR_data_pin, LOW);
  pulse_io(SR_st_pin);
}

void valve_on(int valve_index) {
  digitalWrite(SR_data_pin, HIGH);
  pulse_io(SR_clk_pin);
  digitalWrite(SR_data_pin, LOW);
  for (int i = 0; i < valve_index; i++)
    pulse_io(SR_clk_pin);
  pulse_io(SR_st_pin);
}

void valve_on_from_settings(int cassette, int board, int valve) {
  valve_on(valves_per_cassette*cassette + valves_per_board*board + valve);
}

void on_all_board(int cassette, int board) {
  digitalWrite(SR_data_pin, HIGH);
  for (int i = 0; i < valves_per_board; i++)
    pulse_io(SR_clk_pin);
  digitalWrite(SR_data_pin, LOW);
  for (int i = 0; i < valves_per_cassette*cassette+valves_per_board*board; i++)
    pulse_io(SR_clk_pin);
  pulse_io(SR_st_pin);
}

void delay_with_encoder(uint32_t delay_time) {
  for (int i = 0; i < delay_time/delay_tick; i++) {
    delay(delay_tick);
    if (started && millis() - last_start > started_display_time) {
      started = false;
      display_settings();
    }
    if (started && cycle_flag) {
      started = false;
      Serial.println("Cycle ended by user");
    }
    do_encoder(false); // handle encoder without the option to confirm to avoid looping
  }
}

int read_encoder() {
  old_encoder_read = new_encoder_read;
  new_encoder_read = ((digitalRead(encoder_pinA)) << 1) + (digitalRead(encoder_pinB));
  byte dir = (old_encoder_read << 2) + new_encoder_read;
  switch (dir) {
    case 1: case 7: case 8: case 14:
      return 1;
    case 2: case 4: case 11: case 13:
      return -1;
    case 0: case 5: case 10: case 15:
      return 0;
    default:
      return 0; 
  }
}

void do_encoder(bool enable_confirm) {
  int value = read_encoder();
  encoder_pos += value;
  if (value != 0)
    Serial.println(encoder_pos);

  if (value == 0 && !on_button && digitalRead(encoder_sw)==LOW) {
    on_button = true;
    last_click = millis();
  }
  if (value == 0 && on_button && digitalRead(encoder_sw)==HIGH) {
    on_button = false;
    if (millis() - last_click < confirm_time) {
      current_setting++;
      if (current_setting >= max_settings)
        current_setting = 0;
      display_settings();
    }
  }
  if (value == 0 && on_button && digitalRead(encoder_sw)==LOW) {
    if (started == false && millis() - last_click >= confirm_time && enable_confirm) {
      started = true;
      if (!cycle_flag) {
        last_start = millis();
        display_started();
        if (valve_num == valves_per_board) {
          on_all_board(cassette_num,board_num);
          delay_with_encoder(valve_on_time);
          off_all_valves(num_of_valves);
          started = false;
          display_settings();
        }
        else {
          off_all_valves(num_of_valves);
          valve_on_from_settings(cassette_num, board_num, valve_num);
          delay_with_encoder(valve_on_time);
          off_all_valves(num_of_valves);
          started = false;
          display_settings();
        } 
      }
    }
  }
  if (value != 0 && (encoder_pos >= prescaler || encoder_pos <= -prescaler) && digitalRead(encoder_sw)==HIGH) {
    // TODO: write all the settings in a byte array. this is for now:
    encoder_pos = 0;
    switch(current_setting) {
      case 0:
        cassette_num += (cassette_num==max_cassettes-1&&value==1 ? 0 : (cassette_num==0&&value==-1 ? 0 : value));
        break;
      case 1:
        board_num += (board_num==boards_per_cassette-1&&value==1 ? 0 : (board_num==0&&value==-1 ? 0 : value));
        break;
      case 2:
        valve_num += (valve_num==valves_per_board&&value==1 ? 0 : (valve_num==0&&value==-1 ? 0 : value));
        break;
      case 3:
        valve_on_time += (valve_on_time==max_valve_on_time&&value==1 ? 0 : (valve_on_time==min_valve_on_time&&value==-1 ? 0 : value*valve_on_time_step));
        break;
      case 4:
        cycle_flag = !cycle_flag;
        break;
    }
    display_settings();
  }
}

void cycle_all_valves(int num_of_valves){
  digitalWrite(SR_data_pin, HIGH);
  pulse_io(SR_clk_pin);
  digitalWrite(SR_data_pin, LOW);
  pulse_io(SR_st_pin);
  delay_with_encoder(pulse_time);
  for (int i = 1; i <= num_of_valves; i++) {
    pulse_io(SR_clk_pin);
    pulse_io(SR_st_pin);// update all valves immediately after every shift
    delay_with_encoder(pulse_time);
  }
}



void reverse_cycle_all_valves(int num_of_valves){
  for (int i = num_of_valves-1; i >= 0; i--) {
    valve_on(i);
    delay_with_encoder(pulse_time);
    off_all_valves(num_of_valves);
  }
}

void array_to_valves(bool arr[], int arr_length){
  for (int i = arr_length-1; i >= 0; i--) {
    digitalWrite(SR_data_pin, arr[i]); // "true"=1=HIGH, "false"=0=LOW
    pulse_io(SR_clk_pin);
  }
  pulse_io(SR_st_pin);
}
