#ifndef INTERFACE_H
#define INTERFACE_H

#include "constants.h"

// led output
void setup_led();
void show_field();
void startgame_sequence();
void gameover_sequence();
void output_ledstr(byte str_output[], int strsize);
void show_screen(byte screen[FIELD_SIZE]);

// controls
void get_direction();
void get_skipcond();

#endif
