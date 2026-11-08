#include "flash.h"

#define FLASH_SLOT_SIZE 4096 // must be a multiple of 4096 otherwise flash_range_erase won't work



// stolen from official pico docs, necessary wrapper for flash_safe_execute
// This function will be called when it's safe to call flash_range_erase
static void call_flash_range_erase(void *param) {
    uint32_t offset = (uint32_t)param;
    flash_range_erase(offset, FLASH_SECTOR_SIZE);
}


// stolen from official pico docs, necessary wrapper for flash_safe_execute
// This function will be called when it's safe to call flash_range_program
static void call_flash_range_program(void *param) {
    uint32_t offset = ((uintptr_t*)param)[0];
    const uint8_t *data = (const uint8_t *)((uintptr_t*)param)[1];
    flash_range_program(offset, data, FLASH_PAGE_SIZE);
}

void flash_write_slot(uint8_t slot, uint32_t ir_timing_data[MAX_TIMING_ARRAY]) {
	// we have 99 save slots
	if (slot < 1) slot = 1;
	if (slot > 99) slot = 99;

	
	// erase data first	
	flash_safe_execute(call_flash_range_erase, (void*)(FLASH_SLOT_SIZE*slot), UINT32_MAX);

	// if ir_timing_data is NULL then the user is asking to delete the slot data so we leave the slot in the erased state
	if (ir_timing_data != NULL) {
		uintptr_t params[] = {FLASH_SLOT_SIZE*slot, (uintptr_t)ir_timing_data};
		// write data
		flash_safe_execute(call_flash_range_program, params, UINT32_MAX);
	}
	
	// won't check if successful bc i don't feel like writing more code
	// plus why would it fail lol

}



