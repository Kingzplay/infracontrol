#include <stdio.h>
#include "sh1106.h"
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

uint8_t k[8] = {0, 0, 0xFF, 0x10, 0x30, 0x6f, 0xC0, 0};
int main() {

	stdio_init_all();
	sh1106_init(13, 12);
	sh1106_set_page(1);
	sh1106_print_5x8("Je suis trop");
	sh1106_set_page(4);
	sh1106_set_column(45);
	sh1106_print_5x8("FORT !!!");
	while (1) { 
	}

	return 0;
} 
