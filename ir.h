#ifndef IR_H_
#define IR_H_
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#ifndef IR_RECEIVER_PIN
#define IR_RECEIVER_PIN 24
#endif
#ifndef IR_EMITTER_PIN
#define IR_EMITTER_PIN 14
#endif
#define MAX_TIMING_ARRAY 100 // max number of elements in data array
#define IR_EMITTER_FREQUENCY 38000 // Frequency of IR carrier signal in Hertz
#define IR_RECEIVER_SIGNAL_TIMEOUT 6000 // in microseconds
#endif
