#include "command.h"
#include "junk/jnk_log.h"

#include <string.h>

b8 vk_cmdbuffer_alloc(vk_core_t *core, vk_cmdbuffer_t *cmd,
                      VkCommandPool pool) {
    memset(cmd, 0, sizeof(*cmd));

    if (core->gfx_cmd_pool == VK_NULL_HANDLE) {
        jnk_log_error(CH_GFX, "Command pool is null!");
        return false;
    }

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VK_CHK(jnk.vkAllocateCommandBuffers(core->logic_dvc, &alloc_info,
                                        &cmd->handle));
    return true;
}

void vk_cmdbuffer_free(vk_core_t *core, vk_cmdbuffer_t *cmd,
                       VkCommandPool pool) {
    jnk.vkFreeCommandBuffers(core->logic_dvc, pool, 1, &cmd->handle);
    cmd->handle = VK_NULL_HANDLE;
}

b8 vk_cmdbuffer_begin(vk_cmdbuffer_t *cmd, vk_cmdbuffer_usage_t usage) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = usage;

    VK_CHK(jnk.vkBeginCommandBuffer(cmd->handle, &begin_info));
    return true;
}

b8 vk_cmdbuffer_reset(vk_cmdbuffer_t *cmd) {
    VK_CHK(jnk.vkResetCommandBuffer(cmd->handle, 0));
    return true;
}

void vk_cmdbuffer_end(vk_cmdbuffer_t *cmd) {
    VK_CHK(jnk.vkEndCommandBuffer(cmd->handle));
}

void vk_cmdbuffer_temp_init(vk_core_t *core, vk_cmdbuffer_t *cmd,
                            VkCommandPool pool) {
    vk_cmdbuffer_alloc(core, cmd, pool);
    vk_cmdbuffer_begin(cmd, SUBMIT_ONE_TIME);
}

void vk_cmdbuffer_temp_kill(vk_core_t *core, vk_cmdbuffer_t *cmd,
                            VkCommandPool pool, VkQueue queue) {
    vk_cmdbuffer_end(cmd);

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd->handle;
    VK_CHK(jnk.vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE));
    VK_CHK(jnk.vkQueueWaitIdle(queue));

    vk_cmdbuffer_free(core, cmd, pool);
}
