#ifndef JUNK_CLOCK_H
#define JUNK_CLOCK_H

#include "junk/jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef struct {
    f64 start_time;
    f64 elapsed;
} time_clock_t;

void clock_start(time_clock_t *clock);
void clock_update(time_clock_t *clock);
void clock_stop(time_clock_t *clock);

JNK_EXTERN_C_END
#endif // JUNK_CLOCK_H
