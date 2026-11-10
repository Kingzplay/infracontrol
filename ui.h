#ifndef UI_H_
#define UI_H_
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"

typedef enum {Left, Right, Up, Down, Pressed} Direction;

typedef enum { ModeMenu, RecordMenu, SavesMenu, SettingsMenu, SaveSignalMenu} currentMenu;

typedef enum { Yes, No, Exit, Select, Play, Delete, Record, Saves, Settings, Recording } wordBank;

typedef struct {
	char *name;								// name of the menu, shown in the upper separator
	wordBank *actions;						// contains a list of strings shown in the lower separator
	uint8_t number_of_actions;
	int8_t selected_action;				// index of selected action
	wordBank *items;						// list of items shown on center screen
	int number_of_items;				
	int selected_item;					// index of the item currently shown on screen
	currentMenu current_menu;	
} menu;

void ui_draw_separators(); 
void ui_write_char5x8_separator(char c, uint8_t col, bool lower_separator);
void ui_print_centered_str5x8_separator(char *str, bool lower_separator);
void ui_draw_screen_struct(menu *screen, bool upper_separator, bool center, bool lower_separator);
void ui_draw_mode_screen(menu *screen);
void ui_handler(menu *screen, Direction dir);
void ui_init(menu *screen);
#endif
