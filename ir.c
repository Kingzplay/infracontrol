#include "ir.h"



// Stolen from gemini lol (partially modified) 
void set_pwm_frequency(uint slice_num, uint channel, uint32_t target_freq, uint8_t duty_cycle) {
    duty_cycle = (duty_cycle > 100 ? 100 : duty_cycle); // make sure duty cycle isn't superior to 100%
	
	uint32_t sys_clk = clock_get_hz(clk_sys);
    
    // Use maximum wrap value (65535) for max resolution
    uint32_t top = 65535; 
    
    // Calculate required clock divider
    float divider = (float)sys_clk / (target_freq * (top + 1));
    
    // If target frequency is too high for top=65535, scale top down
    if (divider < 1.0f) {
        divider = 1.0f;
        top = (sys_clk / target_freq) - 1;
    }

    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, top);
	pwm_set_chan_level(slice_num, channel, top*duty_cycle/100); // set duty cycle

}

uint16_t normalize(uint16_t x) {
	if (x % 150 < 75) return x - x%150;
	else return x + 150 - x%150;
}

// inspired by gemini
// FNV-1 hash function
uint32_t ir_hash_generator(uint16_t *ir_timing_data) {
	if(!ir_timing_data[0]) return 0; // check if array is empty
	uint32_t hash = 2166136261;
	
	for(int i = 0; i < MAX_TIMING_ARRAY; i++) {
		if (!ir_timing_data[i]) break; // 0 marks end of array
		uint32_t value = (uint32_t)normalize(ir_timing_data[i]);
		hash ^= value;
		hash *= 16777619;
	}	
	
	return hash;
}

uint32_t ir_receive(uint16_t *ir_timing_data) {
	gpio_init(IR_RECEIVER_PIN);
	gpio_pull_up(IR_RECEIVER_PIN);
	
	bool ir_transmission_started =  false;
	uint64_t length = 0;	// stores how much time passed in microseconds since last change on ir receiver pin
	// stores the timing of each high state and low state

	int index = 0;			// index the ir timing array
	
	while (index < MAX_TIMING_ARRAY)  {
		while (gpio_get(IR_RECEIVER_PIN) && !joystick_pressed() && (!ir_transmission_started || ((time_us_64() - length) < IR_RECEIVER_SIGNAL_TIMEOUT)));
		if (joystick_pressed()) return 0;

		if (!ir_transmission_started) ir_transmission_started = true;

		else if ((time_us_64() - length) >= IR_RECEIVER_SIGNAL_TIMEOUT) {
			index--; // compensate for the last unnecessary increment before the timeout
			break; // timemout has been reached, probably means that the transmission is done so break the timing loop
		}
		else {
			ir_timing_data[index] = time_us_64() - length;
			index++;

		}
		
		length = time_us_64();			
		
		while(!gpio_get(IR_RECEIVER_PIN));	// wait for the rising edge of the pin
	
		ir_timing_data[index] = time_us_64() - length;
		index++;	
		length = time_us_64();	
	}
	return ir_hash_generator(ir_timing_data);	
}



void ir_send(uint16_t *ir_timing_data) {
	gpio_init(IR_EMITTER_PIN);
	gpio_set_function(IR_EMITTER_PIN, GPIO_FUNC_PWM);	
	
	uint slice = pwm_gpio_to_slice_num(IR_EMITTER_PIN);	// get which pwm slice is connected to the pin
	uint channel = pwm_gpio_to_channel(IR_EMITTER_PIN);	// get which pwm channel is connected to the pin	
	set_pwm_frequency(slice, channel, IR_EMITTER_FREQUENCY, 50);	


	for(int i = 0; i < MAX_TIMING_ARRAY; i++) {
		if(!ir_timing_data[i]) break;	// 0ms marks the end of the IR data	
	
		else if(i % 2) {	
			sleep_us(ir_timing_data[i]);
		}	
		else {
			pwm_set_enabled(slice, true);	
			sleep_us(ir_timing_data[i]);
			pwm_set_enabled(slice, false);
		}	

	}

}

void ir_print_timing(uint16_t *ir_timing_data) {
	printf("{");
	for(int i = 0; i < MAX_TIMING_ARRAY; i++) {
		printf("%d, ", ir_timing_data[i]);
	}	
	printf("}\n");
}
