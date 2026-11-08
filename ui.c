#include "ui.h"
#include "sh1106.h"

#define RIGHT_ARROW_TIP 127 
#define LEFT_ARROW_TIP	128
#define UP_ARROW_TIP 129
#define DOWN_ARROW_TIP 130 

// select which lines to use a separators for the ui, between 0 and 63 included
#define UI_UPPER_SEPARATOR_LINE 12
#define UI_LOWER_SEPARATOR_LINE 50
#define VERTICAL_OFFSET 3 	// how many pixels between the characters and the display limit; max value 8
#define UI_SEPARATOR_HEIGHT 12 // distance in pixels between the separator and the display limit; choose from [8;15]

const char yes[] = "Yes";
const char no[] = "No";
const char exit[] = "Exit";	

char *actions_mode_menu[] = {yes, no, exit};
char *items_mode_menu[] = {"Record", "Saves", "Settings"};

char *actons_record_menu[] = {exit};
char *items_record_menu = NULL; 


// i could write this with fewer lines, it would be less modular though
void ui_draw_separators() {	
	uint8_t buffer[sh1106_display_width];
	sh1106_set_column(0);
	
	// draw upper seperator
	uint8_t page = UI_UPPER_SEPARATOR_LINE/8;
	sh1106_set_page(page);
	memset(buffer, 1 << (UI_UPPER_SEPARATOR_LINE - 8*page), sh1106_display_width);	// light up the same pixel for each column 
	sh1106_send_buffer(buffer, sh1106_display_width);

	sh1106_set_column(0);
	
	// draw lower separator	
	page = UI_LOWER_SEPARATOR_LINE/8;
	sh1106_set_page(page);
	memset(buffer, 1 << (UI_LOWER_SEPARATOR_LINE - 8*page), sh1106_display_width);	// light up the same pixel for each column 
	sh1106_send_buffer(buffer, sh1106_display_width);	
}

// write characters between the display limit and the separator
void ui_write_char5x8_separator(char c, uint8_t col, bool lower_separator) {
	uint8_t offset = ( lower_separator ? (8-VERTICAL_OFFSET) : VERTICAL_OFFSET);
	uint8_t page = (lower_separator ? 6 : 0);	// if lower_separator then write in lower_separator otherwise in higher separator
	uint8_t separator_pixels = (lower_separator ? 1 << (UI_LOWER_SEPARATOR_LINE - 8*(UI_LOWER_SEPARATOR_LINE/8)) : 1 << (UI_UPPER_SEPARATOR_LINE - 8*(UI_UPPER_SEPARATOR_LINE/8)));
	
	// make sure that 'c' is whithin limits
	if (c < 32) c = 32;
	else if (c > 130) c = 130;
	c -= 32; // ajust c to the font array

	sh1106_set_page(page);
	sh1106_set_column(col);			

	uint8_t c_array[5] = {0};
	memcpy(c_array, font[c], 5);
	// shift each byte of the character downwards and OR it with separator_pixels to prevent the bar from desintegrating
	for(int i = 0; i < 5; i++) c_array[i] = (c_array[i] << offset)  | (lower_separator ? separator_pixels : 0);	
	sh1106_send_buffer(c_array, 5);
	
	sh1106_set_column(col);												// reset column position after send_buffer	
	sh1106_set_page(page+1);
	memcpy(c_array, font[c], 5);
	// shift each byte of the character upwards and OR it with separator_pixels to prevent the bar from desintegrating
	for(int i = 0; i < 5; i++) c_array[i] = (c_array[i] >> (8-offset)) | (lower_separator ? 0 : separator_pixels);	
	sh1106_send_buffer(c_array, 5);

}

void ui_print_centered_str5x8_separator(char *str, bool lower_separator) {
		uint8_t center = sh1106_display_width / 2 - 1;
		int str_length = strlen(str); // save length to save instructions
		uint8_t col = center - str_length*6/2;	// multiply by (5+1) to get the length in pixels, extra pixel is empty
		
		sh1106_set_column(col);
	
		for(int i = 0; i < str_length; i++) {
			ui_write_char5x8_separator(str[i], col, lower_separator);			
			col += 6;
		}

}

// clear the ram in the separators
void ui_clear_separator(bool lower_separator) {
	for(int i = 0; i < sh1106_display_width; i += 5) {
		ui_write_char5x8_separator(' ', i, lower_separator);
	}
}

void ui_draw_separator_arrows(bool left, bool right) {
	if(left) ui_write_char5x8_separator(LEFT_ARROW_TIP, 0,true);
	
	if(right) ui_write_char5x8_separator(RIGHT_ARROW_TIP, sh1106_display_width-5, true);
}

void ui_draw_fraction(uint8_t num, uint8_t den, uint8_t col) {
	// make sure our numbers are just 2 digits
	num = num > 99 ? 99 : num;
	den = den > 99 ? 99 : den;
	uint8_t fraction_bar = 0x80;
	uint8_t numbers[][3] = { {0x3E, 0x22, 0x3E},	// 0
							 {0x04, 0x3E, 0x00},	// 1
							 {0x3A, 0x2A, 0x2E},	// 2
							 {0x2A, 0x2A, 0x3E}, 	// 3
							 {0x0E, 0x08, 0x3E},	// 4
							 {0x2E, 0x1A, 0x3A},	// 5
							 {0x3E, 0x2A, 0x3A},	// 6
							 {0x02, 0x02, 0x3E},	// 7
							 {0x3E, 0x2A, 0x3E},	// 8
							 {0x2E, 0x2A, 0x3E}		// 9
 	};
	sh1106_set_page(3);
	sh1106_set_column(col);
	// write first digit then second digit
	for(int i = 0; i < 3; i++) {
		sh1106_write_ram(numbers[num/10][i] | fraction_bar);
	}
	sh1106_write_ram(0x00 | fraction_bar);
	for(int i = 0; i < 3; i++) {
		sh1106_write_ram(numbers[num%10][i] | fraction_bar);
	}
	// write denonimator
	sh1106_set_page(4);
	sh1106_set_column(col);
	sh1106_send_buffer(numbers[den/10],3);
	sh1106_write_ram(0x00);
	sh1106_send_buffer(numbers[den%100],3);

}
void ui_clear_center() {
	uint8_t buffer[sh1106_display_width];
	memset(buffer, 0x00, sh1106_display_width);
	for(int i = 2; i < 6; i++) {
		sh1106_set_page(i);
		sh1106_set_column(0);
		sh1106_send_buffer(buffer, sh1106_display_width);
	}
}
void ui_draw_screen_struct(menu *screen, bool upper_separator, bool center, bool lower_separator) {

	if (upper_separator) {
		ui_draw_separators();		// if we're changing the upper separator then we're redrawing everything; always draw this first
		ui_clear_separator(false);								// clear upper separator	
		ui_print_centered_str5x8_separator(screen->name, false);// write menu name on top
	}	

	if (center) {	
		ui_clear_center();
		// draw center screen	
		uint8_t center = sh1106_display_width / 2 - 1;	
		uint8_t scroll_fraction_col = 0;

		sh1106_set_page(3);
		sh1106_set_column(center - 6*strlen(screen->items[screen->selected_item])/2);	// center the text
		sh1106_print5x8(screen->items[screen->selected_item]);
		if (screen->selected_item > 0) {
			sh1106_set_page(2);
			sh1106_set_column(scroll_fraction_col+1); 
			sh1106_write_char5x8(UP_ARROW_TIP);
		
		}								 	
		if (screen->selected_item < (screen->number_of_items - 1)) {
			sh1106_set_page(5);
			sh1106_set_column(scroll_fraction_col+1); 
			sh1106_write_char5x8(DOWN_ARROW_TIP);
		}
		ui_draw_fraction(screen->selected_item + 1, screen->number_of_items, scroll_fraction_col);
	}

	if (lower_separator) {
		ui_clear_separator(true);	// clear lower separator	
		ui_print_centered_str5x8_separator(screen->actions[screen->selected_action], true);	// write selected actions down
		if (screen->selected_action > 0) ui_draw_separator_arrows(true, false);								 	// draw left arrow
		if (screen->selected_action < (screen->number_of_actions - 1)) ui_draw_separator_arrows(false, true); 	// draw right arrow

	}

}

void ui_draw_mode_screen(menu *screen) {
	
	strcpy(screen->name, "Mode");
	screen->actions = actions_mode_menu;
	screen->number_of_actions = sizeof(actions_mode_menu)/sizeof(actions_mode_menu[0]);
	screen->selected_action = 0;
	screen->items = items_mode_menu;
	screen->number_of_items = sizeof(items_mode_menu)/sizeof(items_mode_menu[0]);
	screen->selected_item = 0;
	screen->current_menu = Menu;
	ui_draw_screen_struct(screen, true , true, true);


	}


void ui_draw_record_screen(menu *screen) {
	
	strcpy(screen->name, "Record");
	screen->actions = actions_record_menu;
	screen->number_of_actions = sizeof(actions_record_menu)/sizeof(actions_record_menu[0]);
	screen->selected_action = 0;
	screen->items = items_record_menu;
	screen->number_of_items = 0; 
	screen->selected_item = 0;
	screen->current_menu = Record;
	ui_draw_screen_struct(screen, true , true, true);
}

void ui_handler(menu *screen, Direction dir) {
	if (dir == Left) {
		screen->selected_action = (screen->selected_action == 0 ? 0 : screen->selected_action - 1);
		ui_draw_screen_struct(screen, false, false, true);
	}

	else if (dir == Right) {
		int max = screen->number_of_actions - 1;
		screen->selected_action = (screen->selected_action == max ? max : screen->selected_action + 1);
		ui_draw_screen_struct(screen, false, false, true);
	}

	if (dir == Up) {
		screen->selected_item = (screen->selected_item == 0 ? 0 : screen->selected_item - 1);
		ui_draw_screen_struct(screen, false, true, false);
	}

	else if (dir == Down) {
		int max = screen->number_of_items - 1;
		screen->selected_item = (screen->selected_item == max ? max : screen->selected_item + 1);
		ui_draw_screen_struct(screen, false, true, false);
	}

	else if (dir == Button) {
		switch(screen->current_menu) {
			case Mode:
				// only one action which is select
				ui_draw_record_menu(screen);
				break;

		} 	
	}
	sleep_ms(750);
}
