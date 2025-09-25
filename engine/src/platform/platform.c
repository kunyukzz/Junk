#include "platform.h"

#include <stdlib.h>

#if JNK_LINUX
#    include <time.h> // struct timespec
#elif JNK_WINDOWS
#    include <windows.h>
#    include <windowsx.h>
#endif

void *platform_allocate(u64 size, b8 is_align) {
    (void)is_align;
    return malloc(size);
}

void platform_free(void *block, b8 is_align) {
    (void)is_align;
    free(block);
}

#if JNK_LINUX
double platform_get_abs_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

void platform_sleep(double wake) {
    struct timespec ts;
    ts.tv_sec = (time_t)wake;
    ts.tv_nsec = (long)((wake - (double)ts.tv_sec) * 1e9);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, JNK_NULL);
}

const char *platform_get_env(const char *path) {
    const char *value = getenv(path);
    return value ? value : JNK_NULL;
}
#endif
