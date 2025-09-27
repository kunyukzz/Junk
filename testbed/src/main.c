#include <junk/jnk_log.h>
#include <junk/jnk_memory.h>
#include <junk/jnk_application.h>

typedef struct {
    f32 delta;
} user_state_t;

b8 user_init(user_entry_t *entry) {
    user_state_t *state = (user_state_t *)entry->user_state;
    (void)state;
    jnk_log_info(CH_CORE, "User Initialized");
    return true;
}

b8 user_update(user_entry_t *entry, f32 delta) {
    user_state_t *state = (user_state_t *)entry->user_state;
    (void)state;
    (void)delta;
    return true;
}

b8 user_render(user_entry_t *entry, f32 delta) {
    user_state_t *state = (user_state_t *)entry->user_state;
    (void)state;
    (void)delta;
    return true;
}

void user_shutdown(user_entry_t *entry) {
    if (entry->user_state) {
        JFREE(entry->user_state, sizeof(user_state_t), MEM_GAME);
        entry->user_state = 0;
    }
    jnk_log_info(CH_CORE, "User Kill!");
}

b8 user_setup(user_entry_t *entry) {
    // Window setup
    entry->engine_config.window.width = 1280;
    entry->engine_config.window.height = 720;
    entry->engine_config.window.title = "Junk Testbed";
    entry->engine_config.window.resizable = true;

    // Renderer setup
    entry->engine_config.renderer.vsync = false;

    // Callbacks
    entry->init = user_init;
    entry->update = user_update;
    entry->render = user_render;
    entry->kill = user_shutdown;

    entry->memory_req = sizeof(user_state_t);

    return true;
}

int main(void) {
    user_entry_t user_instance;
    if (!user_setup(&user_instance)) {
        return false;
    }

    jnk_engine_start(&user_instance);

    return 0;
}
