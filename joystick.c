#include "joystick.h"

uint joystick_x = 26;
uint joystick_y = 27;
uint joystick_z = 25;

// for x and y select GPIO 26 or 27 or 28 or 29
void joystick_init() {
	gpio_init(joystick_x);
	gpio_init(joystick_y);
	gpio_init(joystick_z);
	gpio_set_dir(joystick_z, 0); // set up joystick buttona as input
	gpio_pull_down(joystick_z);

	adc_init();
}

bool joystick_left() {
	adc_select_input(joystick_x-26);
	if (adc_read() < 1000) return true;
	else return false;
}


bool joystick_right() {
	adc_select_input(joystick_x-26);
	if (adc_read() > 3000) return true;
	else return false;
}


bool joystick_up() {
	adc_select_input(joystick_y-26);
	if (adc_read() < 1000) return true;
	else return false;
}

bool joystick_down() {
	adc_select_input(joystick_y-26);
	if (adc_read() > 3000) return true;
	else return false;
}

void joystick_print_state() {
	adc_select_input(joystick_x-26);
	int x = adc_read();
	adc_select_input(joystick_y-26);
	int y = adc_read();	
	
	printf("X is %d and Y is %d\n",x , y);	
}
