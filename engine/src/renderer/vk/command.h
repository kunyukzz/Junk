#ifndef JUNK_VULKAN_COMMANDBUFFER_H
#define JUNK_VULKAN_COMMANDBUFFER_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

typedef enum {
    SUBMIT_ONE_TIME = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    SUBMIT_RENDERPASS_CONTINUE =
        VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
    SUBMIT_SIMULTANEOUS = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
} vk_cmdbuffer_usage_t;

b8 vk_cmdbuffer_alloc(vk_core_t *core, vk_cmdbuffer_t *cmd, VkCommandPool pool);
void vk_cmdbuffer_free(vk_core_t *core, vk_cmdbuffer_t *cmd,
                       VkCommandPool pool);

b8 vk_cmdbuffer_begin(vk_cmdbuffer_t *cmd, vk_cmdbuffer_usage_t usage);
b8 vk_cmdbuffer_reset(vk_cmdbuffer_t *cmd);
void vk_cmdbuffer_end(vk_cmdbuffer_t *cmd);

void vk_cmdbuffer_temp_init(vk_core_t *core, vk_cmdbuffer_t *cmd,
                            VkCommandPool pool);

void vk_cmdbuffer_temp_kill(vk_core_t *core, vk_cmdbuffer_t *cmd,
                            VkCommandPool pool, VkQueue queue);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_COMMANDBUFFER_H
