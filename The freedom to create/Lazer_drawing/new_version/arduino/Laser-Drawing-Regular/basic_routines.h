#ifndef BASIC_ROUTINES_H
#define BASIC_ROUTINES_H

#include "consts.h"

void read_pushbuttons();
void move_to_destination();
void one_step(uint8_t, bool);
bool in_destination();
bool is_destination_valid();
bool is_position_valid(float, float);

void set_steps(int16_t, int16_t);
void set_position(float, float);
void set_destination(float, float);

void print_step();
void print_destination();
void print_position();
void print_status();

int16_t max_min_limit(int16_t, int16_t, int16_t);
void limit_steps();

void update_delta();
void update_rates();
void update_scales();

void go_home();
void homming();

void blink_led(uint8_t, uint16_t, uint16_t);
void laser_on();
void laser_off();

void dc_motor_on();
void dc_motor_off();
void check_dc_motor();

void init_drawing();
void init_params();
void reset();
#endif // BASIC_ROUTINES_H
