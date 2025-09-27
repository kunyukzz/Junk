#ifndef JUNK_VULKAN_IMAGES_H
#define JUNK_VULKAN_IMAGES_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

b8 vk_image_init(vk_image_t *out, vk_core_t *core, VkFormat format,
                 VkImageUsageFlags usage, VkImageAspectFlags flags, u32 width,
                 u32 height, b8 create_view, vk_memtag_t tag);

void vk_image_kill(vk_image_t *image, vk_core_t *core, vk_memtag_t tag);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_IMAGES_H
