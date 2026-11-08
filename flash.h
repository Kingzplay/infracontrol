#ifndef FLASH_H_H
#define FLASH_H_
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "pico/flash.h"
#include "ir.h"
void flash_write_slot(uint8_t slot, uint32_t ir_timing_data[MAX_TIMING_ARRAY]);
#endif
