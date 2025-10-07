#include "engine.h"
#include "arena.h"
#include "clock.h"
#include "event/event.h"
#include "event/input.h"
#include "platform/filesystem.h"
#include "platform/window.h"
#include "platform/platform.h"
#include "renderer/re_frontend.h"

#include "junk/jnk_memory.h"
#include "junk/jnk_log.h"
#include "junk/jnk_application.h"
#include "junk/jnk_camera.h"

#include <string.h>

typedef struct {
    u64 memory_req;
    void *state;
} state;

typedef struct {
    state filesystem;
    state event;
    state input;
    state window;
    state render;
    state camera;
} subs;

struct jnk_engine_context_t {
    user_entry_t *entry;

    arena_alloc_t arena;
    time_clock_t time_clock;
    subs subs;

    f64 last_time;
    b8 is_running;
    b8 is_suspend;

    u32 width;
    u32 height;
};

static jnk_engine_context_t *g_engine = {};

static b8 validate_window_config(const jnk_window_config_t *config) {
    if (!config) {
        jnk_log_error(CH_CORE, "window config is NULL");
        return false;
    }
    if (!config->title || strlen(config->title) == 0) {
        jnk_log_error(CH_CORE, "window title is empty");
        return false;
    }
    if (config->width < 100 || config->width > 8192) {
        jnk_log_error(CH_CORE, "invalid window width: %u (must be 100-8192)",
                      config->width);
        return false;
    }
    if (config->height < 100 || config->height > 8192) {
        jnk_log_error(CH_CORE, "invalid window height: %u (must be 100-8192)",
                      config->height);
        return false;
    }
    if (config->width * config->height > 4096 * 4096) {
        jnk_log_error(CH_CORE, "Window resolution too large: %ux%u",
                      config->width, config->height);
        return false;
    }
    return true;
}

static b8 validate_renderer_config(const jnk_renderer_config_t *config) {
    if (!config) {
        jnk_log_error(CH_CORE, "renderer config is NULL");
        return false;
    }
    return true;
}

static b8 engine_pre_init(struct user_entry_t *entry) {
    u64 estimated_memory = 10 * 1024 * 1024;
    if (!memory_system_init(estimated_memory)) {
        jnk_log_error(CH_CORE,
                      "Failed to init memory system with estimated size: %lu",
                      estimated_memory);
        return false;
    }

    // TODO: if user-malloc outside engine, memory counter cannot count.
    entry->user_state = JMALLOC(sizeof(entry->user_state), MEM_GAME);
    entry->engine_state = JMALLOC(sizeof(jnk_engine_context_t), MEM_ENGINE);

    g_engine = entry->engine_state;
    g_engine->entry = entry;
    g_engine->is_running = false;
    g_engine->is_suspend = false;

    if (!validate_window_config(&entry->engine_config.window)) return false;
    if (!validate_renderer_config(&entry->engine_config.renderer)) return false;

    filesys_init(&g_engine->subs.filesystem.memory_req, JNK_NULL);
    event_system_init(&g_engine->subs.event.memory_req, JNK_NULL);
    input_system_init(&g_engine->subs.input.memory_req, JNK_NULL);
    window_system_init(&g_engine->subs.window.memory_req, JNK_NULL);
    renderer_system_init(&g_engine->subs.render.memory_req, JNK_NULL);
    camera_system_init(&g_engine->subs.camera.memory_req, JNK_NULL);

    // TODO: this is ugly as hell
    const u16 count = 7;
    u64 reqs[count];
    const char *labels[7] = {"filesys",  "event",  "input", "window",
                             "renderer", "camera", "entry"};

    reqs[0] = g_engine->subs.filesystem.memory_req;
    reqs[1] = g_engine->subs.event.memory_req;
    reqs[2] = g_engine->subs.input.memory_req;
    reqs[3] = g_engine->subs.window.memory_req;
    reqs[4] = g_engine->subs.render.memory_req;
    reqs[5] = g_engine->subs.camera.memory_req;
    reqs[6] = entry->memory_req;

    u64 total_memory = arena_calc_req(labels, reqs, count);

    if (total_memory > estimated_memory) {
        jnk_log_error(CH_CORE,
                      "Memory estimate too low! Estimated: %lu, Needed: %lu",
                      estimated_memory, total_memory);
    }

    jnk_log_debug(CH_CORE, "Estimated Memory: %lu, Needed Memory: %lu",
                  estimated_memory, total_memory);
    jnk_log_info(CH_CORE, "Memory system initialized");

    return arena_set(total_memory, JNK_NULL, &g_engine->arena);
    // return true;
}

b8 engine_on_input(u32 type, jnk_event_t *ev, void *sender, void *recipient);
b8 engine_on_event(u32 type, jnk_event_t *ev, void *sender, void *recipient);
b8 engine_on_resize(u32 type, jnk_event_t *ev, void *sender, void *recipient);

b8 engine_init(struct user_entry_t *entry) {
    if (!engine_pre_init(entry)) {
        jnk_log_fatal(CH_CORE, "Engine pre-initialization failed");
        return false;
    }
    arena_alloc_t *arena = &g_engine->arena;

    g_engine->subs.filesystem.state =
        arena_alloc(arena, g_engine->subs.filesystem.memory_req);
    filesys_init(JNK_NULL, g_engine->subs.filesystem.state);

    g_engine->subs.event.state =
        arena_alloc(arena, g_engine->subs.event.memory_req);
    event_system_init(JNK_NULL, g_engine->subs.event.state);

    g_engine->subs.input.state =
        arena_alloc(arena, g_engine->subs.input.memory_req);
    input_system_init(JNK_NULL, g_engine->subs.input.state);

    g_engine->subs.window.state =
        arena_alloc(arena, g_engine->subs.window.memory_req);
    window_system_init(JNK_NULL, g_engine->subs.window.state);

    g_engine->subs.render.state =
        arena_alloc(arena, g_engine->subs.render.memory_req);
    renderer_system_init(JNK_NULL, g_engine->subs.render.state);

    g_engine->subs.camera.state =
        arena_alloc(arena, g_engine->subs.camera.memory_req);
    camera_system_init(JNK_NULL, g_engine->subs.camera.state);

    if (!g_engine->entry->init(g_engine->entry)) {
        return false;
    }

    /*** register event system ***/
    event_reg(JNK_QUIT, engine_on_event, JNK_NULL);
    event_reg(JNK_SUSPEND, engine_on_event, JNK_NULL);
    event_reg(JNK_RESUME, engine_on_event, JNK_NULL);
    event_reg(JNK_RESIZE, engine_on_resize, JNK_NULL);
    event_reg(JNK_KEY_PRESS, engine_on_input, JNK_NULL);
    event_reg(JNK_KEY_RELEASE, engine_on_input, JNK_NULL);

    jnk_log_info(CH_CORE, "Engine Initialized");
    return true;
}

b8 engine_run(void) {
    user_entry_t *entry = (user_entry_t *)g_engine->entry;
    g_engine->is_running = true;

    clock_start(&g_engine->time_clock);
    clock_update(&g_engine->time_clock);
    g_engine->last_time = g_engine->time_clock.elapsed;

    f64 runtime = 0;
    u8 frame_count = 0;
    f32 target_frame_seconds = 1.0f / 60;
    const b8 limit = g_engine->entry->engine_config.renderer.vsync;
    // printf("vsync = %d\n", limit);

    jnk_log_info(CH_CORE, "%s", mem_debug_stat());
    jnk_log_info(CH_CORE, "%s", vram_status());

    while (g_engine->is_running) {
        if (!window_system_pump()) {
            g_engine->is_running = false;
        }
        if (!g_engine->is_suspend) {
            clock_update(&g_engine->time_clock);
            f64 curr_time = g_engine->time_clock.elapsed;
            f32 delta = (f32)(curr_time - g_engine->last_time);
            g_engine->last_time = curr_time;
            f64 frame_time_start = platform_get_abs_time();

            input_system_update(delta);

            if (!g_engine->entry->update(entry, delta)) {
                g_engine->is_running = false;
                break;
            }
            if (!g_engine->entry->render(entry, delta)) {
                g_engine->is_running = false;
                break;
            }

            re_bundle_t bundle = {};
            bundle.delta = delta;

            renderer_system_draw(&bundle);

            f64 next_frame_time = frame_time_start + target_frame_seconds;
            f64 frame_time_end = platform_get_abs_time();
            f64 frame_elapsed = frame_time_end - frame_time_start;
            runtime += frame_elapsed;

            if (limit && frame_time_end < next_frame_time) {
                platform_sleep(next_frame_time);
            }
            frame_count++;
            (void)runtime;
            (void)frame_count;
        }
    }

    g_engine->is_running = false;

    event_unreg(JNK_QUIT, engine_on_event, JNK_NULL);
    event_unreg(JNK_SUSPEND, engine_on_event, JNK_NULL);
    event_unreg(JNK_RESUME, engine_on_event, JNK_NULL);
    event_unreg(JNK_RESIZE, engine_on_resize, JNK_NULL);
    event_unreg(JNK_KEY_PRESS, engine_on_input, JNK_NULL);
    event_unreg(JNK_KEY_RELEASE, engine_on_input, JNK_NULL);

    camera_system_kill(g_engine->subs.camera.state);
    renderer_system_kill(g_engine->subs.render.state);
    window_system_kill(g_engine->subs.window.state);
    input_system_kill(g_engine->subs.input.state);
    event_system_kill(g_engine->subs.event.state);
    filesys_kill(g_engine->subs.filesystem.state);

    arena_end(&g_engine->arena);

    // set game/app using the engine to kill itself.
    if (g_engine->entry && g_engine->entry->kill) {
        g_engine->entry->kill(g_engine->entry);
    }

    JFREE(g_engine, sizeof(jnk_engine_context_t), MEM_ENGINE);
    memory_system_kill();
    jnk_log_info(CH_CORE, "Engine Kill");
    return true;
}

void engine_get_framebuff_size(u32 *width, u32 *height) {
    *width = g_engine->width;
    *height = g_engine->height;
}

b8 jnk_engine_start(user_entry_t *entry) {
    if (!entry->init || !entry->update || !entry->render || !entry->kill) {
        jnk_log_fatal(CH_CORE, "Missing required function pointers in user");
        return false;
    }

    if (!engine_init(entry)) {
        jnk_log_fatal(CH_CORE, "Engine Init failed");
        return false;
    }

    if (!engine_run()) {
        jnk_log_fatal(CH_CORE, "Engine: not Shutdown gracefully");
    }

    return true;
}

b8 engine_on_input(u32 type, jnk_event_t *ev, void *sender, void *recipient) {
    (void)sender;
    (void)recipient;
    if (type == JNK_KEY_PRESS) {
        u32 kc = ev->data.keys.keycode;
        if (kc == JNK_KEY_ESCAPE) {
            jnk_event_t evquit;
            event_push(JNK_QUIT, &evquit, JNK_NULL);
            return true;
        } else {
            jnk_log_debug(CH_INPUT, "'%s' pressed in window",
                          keycode_to_str(kc));
        }
    } else if (type == JNK_KEY_RELEASE) {
        u32 kc = ev->data.keys.keycode;
        if (kc == JNK_KEY_SPACE) {
            jnk_log_debug(CH_INPUT, "Explicit! '%s' released",
                          keycode_to_str(kc));
        } else {
            jnk_log_debug(CH_INPUT, "'%s' released", keycode_to_str(kc));
        }
    }
    return false;
}

b8 engine_on_event(u32 type, jnk_event_t *ev, void *sender, void *recipient) {
    (void)ev;
    (void)sender;
    (void)recipient;
    switch (type) {
        case JNK_QUIT: {
            jnk_log_info(CH_CORE, "JNK_QUIT received. Shutdown.");
            g_engine->is_running = false;
            return true;
        } break;
        case JNK_SUSPEND: {
            jnk_log_info(CH_CORE, "JNK_SUSPEND received. Suspend.");
            g_engine->is_suspend = true;
            return true;
        } break;
        case JNK_RESUME: {
            jnk_log_info(CH_CORE, "JNK_RESUME received. Resume.");
            g_engine->is_suspend = false;
            g_engine->is_running = true;
            return true;
        } break;
    }
    return false;
}

b8 engine_on_resize(u32 type, jnk_event_t *ev, void *sender, void *recipient) {
    (void)sender;
    (void)recipient;
    if (type == JNK_RESIZE) {
        u32 w = ev->data.resize.width;
        u32 h = ev->data.resize.height;

        if (w != g_engine->width || h != g_engine->height) {
            g_engine->width = w;
            g_engine->height = h;

            if (w == 0 || h == 0) {
                g_engine->is_suspend = true;
                return true;
            } else {
                if (g_engine->is_suspend) {
                    g_engine->is_suspend = false;
                }
                // g_engine->entry->resize(g_engine->entry, w, h);
                renderer_system_resize(w, h);
            }
        }
    }
    return false;
}

jnk_window_config_t *engine_get_window_config(void) {
    return &g_engine->entry->engine_config.window;
}

jnk_renderer_config_t *engine_get_render_config(void) {
    return &g_engine->entry->engine_config.renderer;
}
