#ifndef JUNK_EVENT_H
#define JUNK_EVENT_H

#include "junk/jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef enum jnk_event_type_t {
    JNK_NONE = 0x00,
    JNK_QUIT = 0x01,
    JNK_SUSPEND = 0x02,
    JNK_RESUME = 0x03,
    JNK_RESIZE = 0x04,

    JNK_KEY_PRESS = 0x05,
    JNK_KEY_RELEASE = 0x06,
    JNK_MOUSE_PRESS = 0x07,
    JNK_MOUSE_RELEASE = 0x08,
    JNK_MOUSE_MOVE = 0x09,
    JNK_MOUSE_WHEEL = 0x0A,

    JNK_MAX = 0xFF
} jnk_event_type_t;

typedef struct {
    u32 type;
    u32 ticks;

    union {
        struct {
            u16 keycode;
            u8 is_repeat;
            u8 _pad0;
            u32 modifiers;
        } keys;

        struct {
            i16 dx, dy;
            i32 x, y;
        } mouse_move;

        struct {
            u8 button;
            u8 press;
            i16 wheel_delta;
            i32 x, y;
        } mouse_button;

        struct {
            u8 id;
            u8 button;
            u16 dpad;
            i16 lx, ly;
            i16 rx, ry;
            u8 lt, rt;
            u8 _pad[6];
        } gamepad;

        struct {
            u32 width;
            u32 height;
        } resize;

        u8 raw[24];
    } data;
} jnk_event_t;

STATIC_ASSERT(sizeof(jnk_event_t) == 32, jnk_event_t_must_be_32_bytes);

typedef b8 (*on_event)(u32 type, jnk_event_t *event, void *sender,
                       void *recipient);

b8 event_system_init(u64 *memory_req, void *state);
void event_system_kill(void *state);

b8 event_reg(u32 type, on_event handler, void *recipient);
b8 event_unreg(u32 type, on_event handler, void *recipient);
b8 event_push(u32 type, const jnk_event_t *event, void *sender);

JNK_EXTERN_C_END
#endif // JUNK_EVENT_H
