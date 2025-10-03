#include "buffer.h"
#include "core.h"
#include "command.h"

#include <string.h>

b8 vk_buffer_init(vk_core_t *core, vk_buffer_t *out, VkBufferUsageFlags usage,
                  VkDeviceSize size, VkMemoryPropertyFlags mem_prop,
                  vk_memtag_t tag) {
    memset(out, 0, sizeof(vk_buffer_t));

    VkBufferCreateInfo bf_info = {};
    bf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bf_info.size = size;
    bf_info.usage = usage;
    bf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHK(jnk.vkCreateBuffer(core->logic_dvc, &bf_info, core->alloc,
                              &out->handle));

    VkMemoryRequirements mem_req;
    jnk.vkGetBufferMemoryRequirements(core->logic_dvc, out->handle, &mem_req);
    out->size = mem_req.size;
    out->is_locked = false;
    out->mapped = JNK_NULL;

    if (vk_mem_alloc(core, &mem_req, mem_prop, &out->memory, tag)) {
        VK_CHK(jnk.vkBindBufferMemory(core->logic_dvc, out->handle, out->memory,
                                      0));
    } else {
        jnk.vkDestroyBuffer(core->logic_dvc, out->handle, core->alloc);
        out->handle = VK_NULL_HANDLE;
        return false;
    }
    return true;
}

void vk_buffer_kill(vk_core_t *core, vk_buffer_t *buffer,
                    VkMemoryPropertyFlags mem_prop, vk_memtag_t tag) {
    if (buffer->memory) {
        vk_mem_free(core, buffer->memory, mem_prop, buffer->size, tag);
        buffer->memory = 0;
    }
    if (buffer->handle) {
        jnk.vkDestroyBuffer(core->logic_dvc, buffer->handle, core->alloc);
        buffer->handle = VK_NULL_HANDLE;
    }

    buffer->size = 0;
    buffer->is_locked = false;
    buffer->mapped = 0;
}

void vk_buffer_bind(vk_core_t *core, vk_buffer_t *buffer, VkDeviceSize offset) {
    jnk.vkBindBufferMemory(core->logic_dvc, buffer->handle, buffer->memory,
                           offset);
}

void vk_buffer_copy(vk_core_t *core, VkBuffer src, VkBuffer dst,
                    VkDeviceSize src_offset, VkDeviceSize dst_offset,
                    VkDeviceSize size, VkCommandPool pool, VkQueue queue) {
    vk_cmdbuffer_t temp;
    vk_cmdbuffer_temp_init(core, &temp, pool);

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = src_offset;
    copy_region.dstOffset = dst_offset;
    copy_region.size = size;

    jnk.vkCmdCopyBuffer(temp.handle, src, dst, 1, &copy_region);
    vk_cmdbuffer_temp_kill(core, &temp, pool, queue);
}

void vk_buffer_load(vk_core_t *core, vk_buffer_t *buffer, VkDeviceSize offset,
                    VkDeviceSize size, const void *data) {
    void *mapped_ptr;
    jnk.vkMapMemory(core->logic_dvc, buffer->memory, offset, size, 0,
                    &mapped_ptr);

    memcpy(mapped_ptr, data, size);
    jnk.vkUnmapMemory(core->logic_dvc, buffer->memory);
}
