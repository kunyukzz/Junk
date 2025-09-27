#ifndef JUNK_ENGINE_H
#define JUNK_ENGINE_H

#include "junk/jnk_define.h"
#include "junk/jnk_application.h"

JNK_EXTERN_C_BEGIN

typedef struct jnk_engine_context_t jnk_engine_context_t;

b8 engine_init(struct user_entry_t *entry);

b8 engine_run(void);

jnk_window_config_t *engine_get_window_config(void);
jnk_renderer_config_t *engine_get_render_config(void);

JNK_EXTERN_C_END
#endif // JUNK_ENGINE_H
