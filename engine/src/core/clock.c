#include "clock.h"
#include "platform/platform.h"

void clock_start(time_clock_t *clock) {
    clock->start_time = platform_get_abs_time();
    clock->elapsed = 0;
}

void clock_update(time_clock_t *clock) {
    if (clock->start_time != 0) {
        clock->elapsed = platform_get_abs_time() - clock->start_time;
    }
}

void clock_stop(time_clock_t *clock) { clock->start_time = 0; }
