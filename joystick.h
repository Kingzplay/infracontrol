#ifndef JOYSTICK_H_
#define JOYSTICK_H_
#include <stdbool.h>
#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"

extern uint joystick_x;
extern uint joystick_y;
extern uint joystick_z;

void joystick_init();
bool joystick_left();
bool joystick_right();
bool joystick_up();
bool joystick_down();
void joystick_print_state();
#endif
