#include <stdio.h>
#include "sh1106.h"
#include "ui.h"
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


uint8_t k[8] = {0, 0, 0xFF, 0x10, 0x30, 0x6f, 0xC0, 0};

menu screen;

int main() {
	
	stdio_init_all();
	sh1106_i2c_baudrate = 400*1000;

	sh1106_init(13, 12);
	sh1106_set_column(0);

	sh1106_set_column(127);
	ui_draw_separators();
	sh1106_set_column(20);
	ui_write_char5x8_separator('j',20, false);
	ui_write_char5x8_separator('A',20, true);
	ui_print_centered_str5x8_separator("Settings", false);
	sh1106_set_page(0);
	ui_draw_mode_screen(&screen);
	

	while (1); 
	return 0;
} 
