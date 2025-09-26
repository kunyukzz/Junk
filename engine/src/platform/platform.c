#include "platform.h"

#include <stdlib.h>
#include <unistd.h>

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

b8 platform_get_current_dir(char *out_path, u64 max_len) {
    if (getcwd(out_path, max_len) != JNK_NULL) {
        return true;
    }
    return false;
}

const char *platform_get_env(const char *path) {
    const char *value = getenv(path);
    return value ? value : JNK_NULL;
}

#elif JNK_WINDOWS

static f64 clock_freq = 0.0;
static LARGE_INTEGER start_time;

void clock_setup() {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_freq = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);
}

f64 platform_get_abs_time(void) {
    if (!clock_freq) {
        clock_setup();
    }

    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (now_time.QuadPart - start_time.QuadPart) * clock_freq;
}

void platform_sleep(f64 wake) {
    f64 now = platform_get_abs_time();
    if (wake <= now) return;

    f64 remaining = wake - now;
    DWORD ms = (DWORD)(remaining * 1000.0 + 0.5);
    if (ms) Sleep(ms);

    while (platform_get_abs_time() < wake) {
        YieldProcessor();
    }
}

b8 platform_get_current_dir(char *out_path, u64 max_len) {
    if (_getcwd(out_path, (u64)max_len) != JNK_NULL) {
        return true;
    }
    return false;
}

const char *platform_get_env(const char *path) {
    static char buffer[1024];
    DWORD len = GetEnvironmentVariableA(name, buffer, sizeof(buffer));
    if (len == 0 || len >= sizeof(buffer)) {
        return JNK_NULL;
    }
    return buffer;
}

#endif
