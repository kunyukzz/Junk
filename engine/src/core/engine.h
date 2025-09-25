#ifndef JUNK_ENGINE_H
#define JUNK_ENGINE_H

#include "junk/jnk_define.h"
#include "junk/jnk_application.h"

JNK_EXTERN_C_BEGIN

/*
b8 engine_init(const jnk_engine_config_t *cfg, const jnk_game_callbacks_t *cb,
               void **engine_state, void *user_state);
               */

b8 engine_init(struct user_entry_t *entry);

b8 engine_run(void);

JNK_EXTERN_C_END
#endif // JUNK_ENGINE_H
