#include "junk/jnk_log.h"
#include "junk/jnk_assert.h"

#include <stdio.h>
#include <stdarg.h>

#define MSG_BUFFER 16000
#define FINAL_BUFFER 24000

#if JNK_LINUX
#    define WHITE "38;5;15m"
#    define RED "38;5;196m"
#    define ORANGE "38;5;208m"
#    define YELLOW "38;5;220m"
#    define GREEN "38;5;040m"
#    define CYAN "38;5;050m"
#    define MAGENTA "38;5;219m"
#elif JNK_WINDOWS
#    define WHITE 0x0F
#    define RED 0x0C
#    define ORANGE 0x06
#    define YELLOW 0x0E
#    define GREEN 0x0A
#    define CYAN 0x0B
#    define MAGENTA 0x0D
#endif

static const char *lvl_str[6] = {"[FATAL]", "[ERROR]", "[WARN]",
                                 "[INFO]",  "[DEBUG]", "[TRACE]"};

static const char *ch_str[5] = {"[CORE]: ", "[GFX]: ", "[MEMS]: ", "[INPUT]: ",
                                "[RSC]: "};

void report(const char *expr, const char *msg, const char *file, i32 line) {
    logs(LOG_FATAL, CH_CORE, "ASSERT: %s, Message: '%s', on: %s, line: %d\n",
         expr, msg, file, line);
}

void log_console(const char *msg, u8 color) {
#if JNK_LINUX
    const char *color_string[] = {
        RED,    // FATAL
        ORANGE, // ERROR
        YELLOW, // WARN
        WHITE,  // INFO
        CYAN,   // DEBUG
        MAGENTA // TRACE
    };

    printf("\033[%s%s\033[0m", color_string[color], msg);
#elif JNK_WINDOWS
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    static u8 types[6] = {
        RED,    // FATAL
        ORANGE, // ERROR
        YELLOW, // WARN
        WHITE,  // INFO
        CYAN,   // DEBUG
        MAGENTA // TRACE
    };

    SetConsoleTextAttribute(console_handle, types[color]);
    DWORD written = 0;
    WriteConsoleA(console_handle, msg, (DWORD)strlen(msg), &written, 0);
#endif
}

void logs(jnk_log_level_t lvl, jnk_log_channel_t ch, const char *msg, ...) {
    char msg_buffer[MSG_BUFFER];
    char final_buffer[FINAL_BUFFER];

    va_list p_arg;
    va_start(p_arg, msg);
    vsnprintf(msg_buffer, sizeof(msg_buffer), msg, p_arg);
    va_end(p_arg);

    snprintf(final_buffer, sizeof(final_buffer), "%s%s%s\n", lvl_str[lvl],
             ch_str[ch], msg_buffer);

    /*
    xvx_snpf(final_buffer, sizeof(final_buffer), "%s%s%s\n", lvl_str[lvl],
             ch_str[ch], msg_buffer);
             */

    log_console(final_buffer, (u8)lvl);
    // uint64_t total_length = strlen(final_buffer);

    // TODO:save file to computer
}
