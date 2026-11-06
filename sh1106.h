#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// physical description of the display
#define SH1106_MAX_COLUMNS 132
#define SH1106_MAX_LINES   64

// a bunch of commands for the display
#define SH1106_DISPLAY_OFF 0xAE
#define SH1106_DISPLAY_ON 0xAF
#define SH1106_ENTIRE_DISPLAY_ON 0xA5
#define SH1106_SET_LINE 0x40
#define SH1106_CONTRAST_MODE 0x81
#define SH1106_SET_PAGE_ADDRESS 0xB0
#define SH1106_INVERT_DISPLAY_ON 0xA7
#define SH1106_INVERT_DISPLAY_OFF 0xA6
#define SH1106_COLUMNS_RIGHT 0xA1
#define SH1106_COLUMNS_LEFT 0xA0

#ifndef SH1106_H_
#define SH1106_H_

extern i2c_inst_t *sh1106_i2c_instance;
extern uint sh1106_i2c_baudrate;
extern uint sh1106_i2c_address;
// how many pixels in use in width and in length
extern uint sh1106_display_width;
extern uint sh1106_display_height;
// the offset is necessary bc the first 2 pixels aren't visible, same for the last 2 
extern uint sh1106_display_offset;
extern uint8_t font[][5];

void sh1106_init(uint scl, uint sda);
void sh1106_send_cmd(uint8_t cmd);
void sh1106_write_ram(uint8_t byte);
void sh1106_set_column(uint8_t col);
void sh1106_set_page(uint8_t page);
void sh1106_send_buffer(uint8_t *buffer, uint buffer_size);
void sh1106_clear_ram();
void sh1106_write_char5x8(uint8_t c);
void sh1106_print5x8(char *str);
#endif 
