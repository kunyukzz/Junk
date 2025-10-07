#include <junk/junk.h>

typedef struct {
    f32 delta;
    jnk_camera_t *camera;
} user_state_t;

b8 user_init(user_entry_t *entry) {
    user_state_t *state = (user_state_t *)entry->user_state;

    const jnk_window_config_t *win = &entry->engine_config.window;
    f32 aspect = (f32)win->width / (f32)win->height;
    jnk_camera_create(45.0f, aspect, 0.1f, 1000.0f);

    state->camera = jnk_camera_get_active();
    state->camera->dirty = true;

    jnk_log_info(CH_CORE, "User Initialized");
    return true;
}

b8 user_update(user_entry_t *entry, f32 delta) {
    user_state_t *state = (user_state_t *)entry->user_state;

    f32 move_speed = 5.0f;
    if (jnk_key_press(JNK_KEY_LEFT)) {
        jnk_camera_yaw(state->camera, move_speed * delta);
    }
    if (jnk_key_press(JNK_KEY_RIGHT)) {
        jnk_camera_yaw(state->camera, -move_speed * delta);
    }
    if (jnk_key_press(JNK_KEY_UP)) {
        jnk_camera_pitch(state->camera, move_speed * delta);
    }
    if (jnk_key_press(JNK_KEY_DOWN)) {
        jnk_camera_pitch(state->camera, -move_speed * delta);
    }

    f32 temp = 5.0f;
    vec3 velo = vec3_zero();
    if (jnk_key_press(JNK_KEY_W)) {
        vec3 forward = mat4_forward(state->camera->view);
        velo = vec3_add(velo, forward);
    }
    if (jnk_key_press(JNK_KEY_S)) {
        vec3 backward = mat4_backward(state->camera->view);
        velo = vec3_add(velo, backward);
    }
    if (jnk_key_press(JNK_KEY_A)) {
        vec3 left = mat4_left(state->camera->view);
        velo = vec3_add(velo, left);
    }
    if (jnk_key_press(JNK_KEY_D)) {
        vec3 right = mat4_right(state->camera->view);
        velo = vec3_add(velo, right);
    }
    if (jnk_key_press(JNK_KEY_Q)) {
        velo.comp1.y += 1.0f;
    }
    if (jnk_key_press(JNK_KEY_E)) {
        velo.comp1.y -= 1.0f;
    }

    vec3 z = vec3_zero();
    if (!vec3_compared(z, velo, 0.0002f)) {
        vec3_normalized(&velo);
        state->camera->position.comp1.x += velo.comp1.x * temp * delta;
        state->camera->position.comp1.y += velo.comp1.y * temp * delta;
        state->camera->position.comp1.z += velo.comp1.z * temp * delta;

        state->camera->dirty = true;
    }
    jnk_camera_update(state->camera);

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
