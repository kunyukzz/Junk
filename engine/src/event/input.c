#include "junk/jnk_input.h"
#include "event.h"

#define MAX_KEYS 256
#define MAX_BUTTONS 8

typedef struct {
    b8 keys[MAX_KEYS];
} keyboard_state;

typedef struct {
    i16 pos_x;
    i16 pos_y;
    u8 wheel_delta;
    u8 buttons[MAX_BUTTONS];
} mouse_state;

typedef struct {
    keyboard_state kbd_curr;
    keyboard_state kbd_prev;
    mouse_state mouse_curr;
    mouse_state mouse_prev;
} input_state_t;

static input_state_t *g_input = JNK_NULL;

b8 input_system_init(u64 *memory_req, void *state) {
    if (memory_req) *memory_req = sizeof(input_state_t);
    if (!state) return true;

    g_input = (input_state_t *)state;
    return true;
}

void input_system_update(float delta_time) {
    (void)delta_time;

    for (u32 i = 0; i < MAX_KEYS; ++i) {
        g_input->kbd_prev.keys[i] = g_input->kbd_curr.keys[i];
    }
    for (u32 i = 0; i < MAX_BUTTONS; ++i) {
        g_input->mouse_prev.buttons[i] = g_input->mouse_curr.buttons[i];
    }

    g_input->mouse_prev.pos_x = g_input->mouse_curr.pos_x;
    g_input->mouse_prev.pos_y = g_input->mouse_curr.pos_y;

    g_input->mouse_curr.wheel_delta = 0;
}

void input_system_kill(void *state) {
    (void)state;
    g_input = 0;
}

void input_process_key(jnk_keys_t key, b8 is_press) {
    if (g_input->kbd_curr.keys[key] != is_press) {
        g_input->kbd_curr.keys[key] = is_press;

        jnk_event_t ev;
        ev.data.keys.keycode = (u16)key;
        event_push(is_press ? JNK_KEY_PRESS : JNK_KEY_RELEASE, &ev, JNK_NULL);
    }
}

void input_process_button(jnk_button_t button, b8 is_press) {
    if (g_input->mouse_curr.buttons[button] != is_press) {
        g_input->mouse_curr.buttons[button] = is_press;

        jnk_event_t ev;
        ev.data.mouse_button.button = (u8)button;
        event_push(is_press ? JNK_MOUSE_PRESS : JNK_MOUSE_RELEASE, &ev,
                   JNK_NULL);
    }
}

void input_process_mouse_move(i16 pos_x, i16 pos_y) {
    if (g_input->mouse_curr.pos_x != pos_x ||
        g_input->mouse_curr.pos_y != pos_y) {
        g_input->mouse_curr.pos_x = pos_x;
        g_input->mouse_curr.pos_y = pos_y;

        jnk_event_t ev;
        ev.data.mouse_move.x = pos_x;
        ev.data.mouse_move.y = pos_y;
        event_push(JNK_MOUSE_MOVE, &ev, JNK_NULL);
    }
}

void input_process_mouse_wheel(i8 delta_z) {
    g_input->mouse_curr.wheel_delta = (u8)delta_z;

    if (delta_z) {
        jnk_event_t ev;
        ev.data.mouse_button.wheel_delta = delta_z;
        event_push(JNK_MOUSE_WHEEL, &ev, JNK_NULL);
    }
}

b8 jnk_key_press(jnk_keys_t key) { return g_input->kbd_curr.keys[key] == true; }

b8 jnk_key_release(jnk_keys_t key) {
    return g_input->kbd_curr.keys[key] == false;
}

b8 jnk_key_was_pressed(jnk_keys_t key) {
    return g_input->kbd_prev.keys[key] == true;
}

b8 jnk_key_was_released(jnk_keys_t key) {
    return g_input->kbd_prev.keys[key] == false;
}

b8 jnk_button_press(jnk_button_t button) {
    return g_input->mouse_curr.buttons[button] == true;
}

b8 jnk_button_release(jnk_button_t button) {
    return g_input->mouse_curr.buttons[button] == false;
}

b8 jnk_button_was_pressed(jnk_button_t button) {
    return g_input->mouse_prev.buttons[button] == true;
}

b8 jnk_button_was_released(jnk_button_t button) {
    return g_input->mouse_prev.buttons[button] == false;
}

void jnk_get_mouse_pos(i32 *pos_x, i32 *pos_y) {
    *pos_x = g_input->mouse_curr.pos_x;
    *pos_y = g_input->mouse_curr.pos_y;
}

void jnk_get_mouse_prev_pos(i32 *pos_x, i32 *pos_y) {
    *pos_x = g_input->mouse_prev.pos_x;
    *pos_y = g_input->mouse_prev.pos_y;
}
