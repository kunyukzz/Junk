#ifndef JUNK_VULKAN_CORE_H
#define JUNK_VULKAN_CORE_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

b8 vk_core_init(vk_core_t *core);
void vk_core_kill(vk_core_t *core);

b8 vk_mem_alloc(vk_core_t *core, VkMemoryRequirements *memory_req,
                VkMemoryPropertyFlags flags, VkDeviceMemory *out,
                vk_memtag_t tag);

void vk_mem_free(vk_core_t *core, VkDeviceMemory memory,
                 VkMemoryPropertyFlags flags, VkDeviceSize size,
                 vk_memtag_t tag);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_CORE_H
