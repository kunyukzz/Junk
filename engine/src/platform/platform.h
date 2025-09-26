#ifndef JUNK_PLATFORM_H
#define JUNK_PLATFORM_H

#include "junk/jnk_define.h"

JNK_EXTERN_C_BEGIN

void *platform_allocate(u64 size, b8 is_align);
void platform_free(void *block, b8 is_align);

double platform_get_abs_time(void);
void platform_sleep(f64 wake);

b8 platform_get_current_dir(char *out_path, u64 max_len);
const char *platform_get_env(const char *path);

JNK_EXTERN_C_END
#endif // JUNK_PLATFORM_H
