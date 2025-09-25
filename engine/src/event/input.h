#ifndef JUNK_INTERNAL_INPUT_H
#define JUNK_INTERNAL_INPUT_H

#include "junk/jnk_define.h"
#include "junk/jnk_input.h"

JNK_EXTERN_C_BEGIN

b8 input_system_init(u64 *memory_req, void *state);
void input_system_update(float delta_time);
void input_system_kill(void *state);

void input_process_key(jnk_keys_t key, b8 is_press);
void input_process_button(jnk_button_t button, b8 is_press);
void input_process_mouse_move(i16 pos_x, i16 pos_y);
void input_process_mouse_wheel(i8 delta_z);

const char *keycode_to_str(jnk_keys_t key);
jnk_keys_t keycode_translate(u32 keycode);

JNK_EXTERN_C_END
#endif // JUNK_INTERNAL_INPUT_H
