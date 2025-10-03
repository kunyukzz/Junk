#ifndef JUNK_VULKAN_BUFFER_H
#define JUNK_VULKAN_BUFFER_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

b8 vk_buffer_init(vk_core_t *core, vk_buffer_t *out, VkBufferUsageFlags usage,
                  VkDeviceSize size, VkMemoryPropertyFlags mem_prop,
                  vk_memtag_t tag);

void vk_buffer_kill(vk_core_t *core, vk_buffer_t *buffer,
                    VkMemoryPropertyFlags mem_prop, vk_memtag_t tag);

void vk_buffer_bind(vk_core_t *core, vk_buffer_t *buffer, VkDeviceSize offset);

void vk_buffer_copy(vk_core_t *core, VkBuffer src, VkBuffer dst,
                    VkDeviceSize src_offset, VkDeviceSize dst_offset,
                    VkDeviceSize size, VkCommandPool pool, VkQueue queue);

void vk_buffer_load(vk_core_t *core, vk_buffer_t *buffer, VkDeviceSize offset,
                    VkDeviceSize size, const void *data);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_BUFFER_H
