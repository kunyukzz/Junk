#ifndef JUNK_VULKAN_SWAPCHAIN_H
#define JUNK_VULKAN_SWAPCHAIN_H

#include "junk/jnk_define.h"
#include "types.h"
#include "platform/window_type.h"

JNK_EXTERN_C_BEGIN

b8 vk_swapchain_init(vk_swapchain_t *swp, vk_core_t *core, jnk_window_t *window,
                     VkSwapchainKHR old_handle);

void vk_swapchain_kill(vk_swapchain_t *swp, vk_core_t *core);

b8 vk_swapchain_reinit(vk_swapchain_t *swp, vk_core_t *core,
                       jnk_window_t *window);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_SWAPCHAIN_H
