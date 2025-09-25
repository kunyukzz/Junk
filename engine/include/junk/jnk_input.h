#ifndef JUNK_INPUT_H
#define JUNK_INPUT_H

#include "jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef enum {
    JNK_MB_LEFT,
    JNK_MB_RIGHT,
    JNK_MB_MIDDLE,
    JNK_MB_WHEEL_UP,
    JNK_MB_WHEEL_DOWN,
    JNK_MB_MAX
} jnk_button_t;

typedef enum {
    JNK_KEY_UNKNOWN = 0x00,

    // Alphanumeric
    JNK_KEY_A,
    JNK_KEY_B,
    JNK_KEY_C,
    JNK_KEY_D,
    JNK_KEY_E,
    JNK_KEY_F,
    JNK_KEY_G,
    JNK_KEY_H,
    JNK_KEY_I,
    JNK_KEY_J,
    JNK_KEY_K,
    JNK_KEY_L,
    JNK_KEY_M,
    JNK_KEY_N,
    JNK_KEY_O,
    JNK_KEY_P,
    JNK_KEY_Q,
    JNK_KEY_R,
    JNK_KEY_S,
    JNK_KEY_T,
    JNK_KEY_U,
    JNK_KEY_V,
    JNK_KEY_W,
    JNK_KEY_X,
    JNK_KEY_Y,
    JNK_KEY_Z,

    JNK_KEY_1,
    JNK_KEY_2,
    JNK_KEY_3,
    JNK_KEY_4,
    JNK_KEY_5,
    JNK_KEY_6,
    JNK_KEY_7,
    JNK_KEY_8,
    JNK_KEY_9,
    JNK_KEY_0,

    // Function Keys
    JNK_KEY_F1,
    JNK_KEY_F2,
    JNK_KEY_F3,
    JNK_KEY_F4,
    JNK_KEY_F5,
    JNK_KEY_F6,
    JNK_KEY_F7,
    JNK_KEY_F8,
    JNK_KEY_F9,
    JNK_KEY_F10,
    JNK_KEY_F11,
    JNK_KEY_F12,

    JNK_KEY_ESCAPE,
    JNK_KEY_ENTER,
    JNK_KEY_TAB,
    JNK_KEY_BACKSPACE,
    JNK_KEY_INSERT,
    JNK_KEY_DELETE,
    JNK_KEY_HOME,
    JNK_KEY_END,
    JNK_KEY_PAGEUP,
    JNK_KEY_PAGEDOWN,

    JNK_KEY_LEFT,
    JNK_KEY_RIGHT,
    JNK_KEY_UP,
    JNK_KEY_DOWN,

    // Modifier key
    JNK_KEY_LSHIFT,
    JNK_KEY_RSHIFT,
    JNK_KEY_LCONTROL,
    JNK_KEY_RCONTROL,
    JNK_KEY_LALT,
    JNK_KEY_RALT,
    JNK_KEY_LSUPER,
    JNK_KEY_RSUPER,
    JNK_KEY_CAPS,
    JNK_KEY_NUM,

    // Numpad
    JNK_KEY_KP_0,
    JNK_KEY_KP_1,
    JNK_KEY_KP_2,
    JNK_KEY_KP_3,
    JNK_KEY_KP_4,
    JNK_KEY_KP_5,
    JNK_KEY_KP_6,
    JNK_KEY_KP_7,
    JNK_KEY_KP_8,
    JNK_KEY_KP_9,
    JNK_KEY_KP_DECIMAL,
    JNK_KEY_KP_ENTER,
    JNK_KEY_KP_ADD,
    JNK_KEY_KP_SUBSTRACT,
    JNK_KEY_KP_MULTIPLY,
    JNK_KEY_KP_DIVIDE,

    // Symbols
    JNK_KEY_SPACE,
    JNK_KEY_MINUS,
    JNK_KEY_EQUAL,
    JNK_KEY_LBRACKET,
    JNK_KEY_RBRACKET,
    JNK_KEY_LBRACE,
    JNK_KEY_RBRACE,
    JNK_KEY_BACKSLASH,
    JNK_KEY_SEMICOLON,
    JNK_KEY_APOSTROPHE,
    JNK_KEY_GRAVE,
    JNK_KEY_COMMA,
    JNK_KEY_PERIOD,
    JNK_KEY_SLASH,

    // Misc
    JNK_KEY_PRINT_SCREEN,
    JNK_KEY_SCROLL_LOCK,
    JNK_KEY_PAUSE,

    JNK_KEY_MAX
} jnk_keys_t;

// keys
JNK_API b8 jnk_key_press(jnk_keys_t key);
JNK_API b8 jnk_key_release(jnk_keys_t key);
JNK_API b8 jnk_key_was_pressed(jnk_keys_t key);
JNK_API b8 jnk_key_was_released(jnk_keys_t key);

// mouse
JNK_API b8 jnk_button_press(jnk_button_t button);
JNK_API b8 jnk_button_release(jnk_button_t button);
JNK_API b8 jnk_button_was_pressed(jnk_button_t button);
JNK_API b8 jnk_button_was_released(jnk_button_t button);
JNK_API void jnk_get_mouse_pos(i32 *pos_x, i32 *pos_y);
JNK_API void jnk_get_mouse_prev_pos(i32 *pos_x, i32 *pos_y);

JNK_EXTERN_C_END
#endif // JUNK_INPUT_H
