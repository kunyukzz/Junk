#ifndef JUNK_LOG_H
#define JUNK_LOG_H

#include "jnk_define.h"

JNK_EXTERN_C_BEGIN

#define LOG_DEBUG_ENABLE 1
#define LOG_TRACE_ENABLE 1

#ifdef JNK_RELEASE
#    undef LOG_DEBUG_ENABLE
#    define LOG_DEBUG_ENABLE 0
#    undef LOG_TRACE_ENABLE
#    define LOG_TRACE_ENABLE 0
#endif

typedef enum {
    LOG_FATAL = 0x00,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE
} jnk_log_level_t;

typedef enum {
    CH_CORE = 0x00,
    CH_GFX,
    CH_MEMS,
    CH_INPUT,
    CH_RSC
} jnk_log_channel_t;

JNK_API void logs(jnk_log_level_t lvl, jnk_log_channel_t ch, const char *msg,
                  ...);

#define jnk_log_fatal(ch, msg, ...) logs(LOG_FATAL, ch, msg, ##__VA_ARGS__);
#define jnk_log_error(ch, msg, ...) logs(LOG_ERROR, ch, msg, ##__VA_ARGS__);
#define jnk_log_warn(ch, msg, ...) logs(LOG_WARN, ch, msg, ##__VA_ARGS__);
#define jnk_log_info(ch, msg, ...) logs(LOG_INFO, ch, msg, ##__VA_ARGS__);

#if LOG_DEBUG_ENABLE
#    define jnk_log_debug(ch, msg, ...) logs(LOG_DEBUG, ch, msg, ##__VA_ARGS__);
#else
#    define jnk_log_debug(ch, msg, ...) ((void)0);
#endif

#if LOG_TRACE_ENABLE
#    define jnk_log_trace(ch, msg, ...) logs(LOG_TRACE, ch, msg, ##__VA_ARGS__);
#else
#    define jnk_log_trace(ch, msg, ...) ((void)0);
#endif

JNK_EXTERN_C_END
#endif // JUNK_LOG_H
