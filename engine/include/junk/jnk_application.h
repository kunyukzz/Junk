#ifndef JUNK_APPLICATION_H
#define JUNK_APPLICATION_H

#include "jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef struct user_entry_t user_entry_t;

typedef struct jnk_window_config_t {
    const char *title;
    u32 width;
    u32 height;
    b8 resizable;
    b8 hidden;
} jnk_window_config_t;

typedef struct jnk_renderer_config {
    b8 vsync; // true for stable framerate
} jnk_renderer_config_t;

typedef struct jnk_engine_config {
    jnk_window_config_t window;
    jnk_renderer_config_t renderer;
} jnk_engine_config_t;

struct user_entry_t {
    jnk_engine_config_t engine_config;

    b8 (*init)(user_entry_t *user);
    b8 (*update)(user_entry_t *user, f32 delta_time);
    b8 (*render)(user_entry_t *user, f32 delta_time);
    void (*kill)(user_entry_t *user);

    u64 memory_req;

    void *user_state;
    void *engine_state;
};

JNK_API b8 jnk_engine_start(user_entry_t *entry);

JNK_EXTERN_C_END
#endif // JUNK_APPLICATION_H
