#ifndef PARSER_H
#define PARSER_H

#include "basic_routines.h"

byte process_input();
void print_help_menu();
void read_xy();
void read_rate();
void read_power();
void read_dc_power();
byte get_input();

#endif // PARSER_H