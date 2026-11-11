#include "ui.h"

#define RIGHT_ARROW_TIP 127 
#define LEFT_ARROW_TIP	128
#define UP_ARROW_TIP 129
#define DOWN_ARROW_TIP 130 

// select which lines to use a separators for the ui, between 0 and 63 included
#define UI_UPPER_SEPARATOR_LINE 12
#define UI_LOWER_SEPARATOR_LINE 50
#define VERTICAL_OFFSET 3 	// how many pixels between the characters and the display limit; max value 8
#define UI_SEPARATOR_HEIGHT 12 // distance in pixels between the separator and the display limit; choose from [8;15]


uint16_t ir_timing_data[MAX_TIMING_ARRAY];
uint32_t ir_hash;

// a word bank useful in if and switch statements later
char *bank[] = {
	"Yes",
	"No",
	"Exit",
	"Ok",
	"Select",
	"Play",
	"Delete",
	"Record",
	"Saves",
	"Settings",
	"Recording...",
	"Credits"

}; 

// the state where you select Record mode, Saves mode etc...
menu mode_screen = {
	"Mode", 									// name
	(wordBank[]) {Select}, 						// list of actions showed in the lower box
	1,											// number of actions
	0,											// selected action in array
	(wordBank[]) {Record, Saves, Settings, Credits},		// list of items shows in the center
	4,											// number of items
	0,											// selected action in array
	ModeMenu,									// currentMenu
};

// the state where you record IR signals
menu record_screen = {
	"Record", 					// name
	(wordBank[]) {Exit},		// list of actions showed in the lower box
	1,							// number of actions
	0,							// selected action in array
	(wordBank[]) {Recording},	// list of items shows in the center
	1,							// number of items
	0,							// selected action in array
	RecordMenu,					// currentMenu
};

menu display_capture_screen = {
	"Save capture",
	(wordBank[]) {Ok},
	1,
	0,
	NULL,
	1,
	0,
	DisplayCaptureMenu

};

menu save_capture_screen = {
	"Save capture", 				// name
	(wordBank[]) {Select, Exit},	// list of actions showed in the lower box
	2,								// number of actions
	0,								// selected action in array
	NULL,							// list of items shows in the center
	99,								// number of items
	0,								// selected action in array
	SaveCaptureMenu,						// currentMenu
};

menu saves_screen = {
	"Saves", 							// name
	(wordBank[]) {Play, Delete, Exit}, 	// list of actions showed in the lower box
	3,									// number of actions
	0,									// selected action in array
	NULL,								// list of items shows in the center
	99,									// number of items
	0,									// selected action in array
	SavesMenu,							// currentMenu
};

menu credits_screen = {
	"Credits", 							// name
	(wordBank[]) {Exit}, 	// list of actions showed in the lower box
	1,									// number of actions
	0,									// selected action in array
	NULL,								// list of items shows in the center
	1,									// number of items
	0,									// selected action in array
	CreditsMenu,							// currentMenu
};

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
							 {0x2E, 0x2A, 0x3A},	// 5
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
	sh1106_send_buffer(numbers[den%10],3);

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
		// draw center screen and center the text
		uint8_t center = sh1106_display_width / 2 - 1;	
		uint8_t scroll_fraction_col = 0;

		sh1106_set_page(3);
		
		// handle the many exceptions
		if(screen->current_menu == SaveCaptureMenu) { 
			sh1106_set_column(center - 6*7/2); // center text
			char buffer[8];
			snprintf(buffer, 8, "Save %02d", screen->selected_item + 1);
			sh1106_print5x8(buffer);
		}
		else if(screen->current_menu == SavesMenu) { 
				sh1106_set_page(2);
				sh1106_set_column(center - 6*7/2); // center text
				char buffer[8];
				snprintf(buffer, 8, "Save %02d", screen->selected_item + 1);
				sh1106_print5x8(buffer);
				
				char buffer2[11];
				sh1106_set_page(4);
				flash_read_slot(screen->selected_item + 1, ir_timing_data);
				ir_hash = ir_hash_generator(ir_timing_data);						
				snprintf(buffer2, 11, "0x%08X", ir_hash);
				sh1106_set_column(center - 6*10/2); // center text
				sh1106_print5x8(buffer2);
		}
		
		else if(screen->current_menu == DisplayCaptureMenu) {
			sh1106_set_column(center - 6*10/2); // center text
			sh1106_set_page(2);
			sh1106_print5x8("Received: ");

			char buffer[11];
			sh1106_set_page(4);					
			snprintf(buffer, 11, "0x%08X", ir_hash);
			sh1106_set_column(center - 6*10/2); // center text
			sh1106_print5x8(buffer);
		}
		
		else if(screen->current_menu == CreditsMenu) {
			sh1106_set_column(center - 6*10/2); // center text
			sh1106_print5x8("Written by");
			sh1106_set_page(4);
			sh1106_set_column(center - 6*13/2);
			sh1106_print5x8("Kamel Hassoun");
			sh1106_set_page(3);
		}
		else {
			sh1106_set_column(center - 6*strlen(bank[screen->items[screen->selected_item]])/2);	// center the text
			sh1106_print5x8(bank[screen->items[screen->selected_item]]);
		}
		// draw fraction on side
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
		ui_print_centered_str5x8_separator(bank[screen->actions[screen->selected_action]], true);	// write selected actions down
		if (screen->selected_action > 0) ui_draw_separator_arrows(true, false);								 	// draw left arrow
		if (screen->selected_action < (screen->number_of_actions - 1)) ui_draw_separator_arrows(false, true); 	// draw right arrow

	}

}

void ui_handle_record(menu *screen) {
	*screen = record_screen;	
	ui_draw_screen_struct(screen, true, true, true);
	ir_hash = ir_receive(ir_timing_data);
	if(ir_hash) {
		*screen = display_capture_screen;
		ui_draw_screen_struct(screen, true, true, true);
		ir_print_timing(ir_timing_data);	
		while(!joystick_pressed());
		*screen = save_capture_screen;
	}
	// if the 'ir_record' yields false then the user pressed the joystick to exit
	else {
		*screen = mode_screen;
	}

}

void ui_init(menu *screen) {
	*screen = mode_screen;
	ui_draw_screen_struct(screen, true , true, true);
}

void ui_handle_button_press(menu *screen) {
	int i = screen->selected_action;
	int j = screen->selected_item;
	
	if (screen->current_menu == ModeMenu) {
		// enter Record, Saves or Settings screen 
		switch(screen->items[j]) {
			case Record:
				ui_handle_record(screen);
				break;
			case Saves:
				*screen = saves_screen;
				break;
			case Settings:
				printf("Bruh");
				break;			
			case Credits:
				*screen = credits_screen;
				break; 
		}
		ui_draw_screen_struct(screen, true, true, true);
	}
	else if (screen->current_menu == RecordMenu) {
		// in the Record screen the only possible action is 'Exit'
		*screen = mode_screen;
		ui_draw_screen_struct(screen, true , true, true);
		
	}
	else if (screen->current_menu == SavesMenu) {
		switch(screen->actions[i]) {
			case Play:
				flash_read_slot(j+1, ir_timing_data);
				ir_print_timing(ir_timing_data);
				ir_send(ir_timing_data);
				break;
			case Delete:
				flash_write_slot(j+1, NULL);	
				ui_draw_screen_struct(screen, false, true, false); // update hash value
				break;
			case Exit:
				*screen = mode_screen;
				ui_draw_screen_struct(screen, true, true, true);
				break;
		}	
	}	

	else if(screen->current_menu == SaveCaptureMenu) {
		switch(screen->actions[i]) {
			case Select:
				flash_write_slot(j+1, ir_timing_data); 
				*screen = mode_screen;
				break;
			case Exit:
				*screen = mode_screen;
		}
		ui_draw_screen_struct(screen, true, true, true);
	}

	else if(screen->current_menu == CreditsMenu) {
		*screen = mode_screen;
		ui_draw_screen_struct(screen, true, true, true);
	} 
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

	else if (dir == Pressed) {
		ui_handle_button_press(screen);
	}	
	sleep_ms(250);
}
