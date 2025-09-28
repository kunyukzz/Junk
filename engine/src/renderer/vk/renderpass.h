#ifndef JUNK_VULKAN_RENDERPASS_H
#define JUNK_VULKAN_RENDERPASS_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

typedef enum {
    NO_FLAG = 0x0,
    COLOR_BUFFER = 0x1,
    DEPTH_BUFFER = 0x2,
    STENCIL_BUFFER = 0x4
} vk_rpass_flag_t;

b8 vk_renderpass_init(vk_core_t *core, vk_renderpass_t *rpass,
                      vk_swapchain_t *swap, f32 depth, u32 stencil,
                      u8 clear_flag, b8 prev_pass, b8 next_pass,
                      VkClearColorValue clear_color);

void vk_renderpass_kill(vk_core_t *core, vk_renderpass_t *rpass);

void vk_renderpass_begin(vk_renderpass_t *rpass, VkCommandBuffer cmd,
                         VkFramebuffer framebuffer, VkExtent2D extent);

void vk_renderpass_end(vk_renderpass_t *rpass, VkCommandBuffer cmd);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_RENDERPASS_H
