#ifndef JUNK_WINDOW_H
#define JUNK_WINDOW_H

#include "junk/jnk_define.h"
#include "junk/jnk_application.h"
#include "platform/window_type.h"

JNK_EXTERN_C_BEGIN

typedef struct {
    const jnk_window_config_t *config;
    jnk_window_t window;
} window_state_t;

b8 window_system_init(u64 *memory_req, void *state);
b8 window_system_pump(void);
void window_system_kill(void *state);

window_state_t *system_get_window(void);

JNK_EXTERN_C_END
#endif // JUNK_WINDOW_H
