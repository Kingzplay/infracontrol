#include <stdio.h>
#include "sh1106.h"
#include "ui.h"
#include "joystick.h"
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// initialize our ui
menu screen;

int main() {
	
	stdio_init_all();
	sh1106_i2c_baudrate = 400*1000;

	sh1106_init(13, 12);
	sh1106_set_column(0);

	ui_init(&screen);
	joystick_init();
	
	while (1) {
		if (joystick_down()) {
			ui_handler(&screen, Down);
			printf("Down\n");
		}
		else if (joystick_up()) {
			ui_handler(&screen, Up);
			printf("Up\n");
		}
		else if (joystick_left()) {
			ui_handler(&screen, Left);
			printf("Left\n");		
		}
		else if (joystick_right()) {
			ui_handler(&screen, Right);
			printf("Right\n");
		}
		else if (joystick_pressed()) {
			ui_handler(&screen, Pressed);
			printf("Right\n");

		}		
		sleep_ms(100);
	} 
	return 0;
} 
