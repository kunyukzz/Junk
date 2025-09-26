#ifndef JUNK_VULKAN_CORE_H
#define JUNK_VULKAN_CORE_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

b8 vk_core_init(vk_core_t *core);
void vk_core_kill(vk_core_t *core);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_CORE_H
