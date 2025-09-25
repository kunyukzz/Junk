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
    u32 vulkan_version; // 1000 for 1.0, 1002 for 1.2, etc

    u64 vram_budget_mb; // 1024 or 1532 (1-1.5GB)

    u32 msaa_samples; // 1, 2, 4 (no 8x to save VRAM)
    b8 vsync;         // true for stable framerate

    u32 max_texture_resolution; // 2048 or 4096 to limit VRAM
    u32 max_vertex_count;       // Prevent insane geometry
    u32 max_texture_count;      // Reasonable material limit
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
