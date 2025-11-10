#ifndef BASIC_ROUTINES_H
#define BASIC_ROUTINES_H

#include "CONST_N_GLOBAL.h"

void read_pushbuttons();
void read_destination();
void one_step(uint8_t, bool);
bool in_destination();
bool is_destination_valid();
bool is_position_valid(float, float);

void set_steps(int16_t, int16_t);
void set_position(float, float);
void set_destination(float, float);
void set_bezier_point(float, float);

void print_step();
void print_destination();
void print_position();
void print_status();

int16_t max_min_limit(int16_t, int16_t);
void Limit_Steps_XYZ();

void update_delta();
void update_rates();

void compute_bezier_variable(int);
boolean compute_next_bezier_point();

void go_home();
void Homming();

void square();

void update_scales();

void Blink_Led(uint8_t, uint16_t, uint16_t);
void led_on();
void led_off();

void dc_motor_on();
void dc_motor_off();
void check_dc_motor();

#endif /* BASIC_ROUTINES_H */
