#include "images.h"
#include "core.h"
#include "junk/jnk_log.h"

static b8 create_image_view(vk_image_t *out, vk_core_t *core, VkFormat format,
                            VkImageAspectFlags flags) {
    if (out->handle == VK_NULL_HANDLE) {
        jnk_log_error(CH_GFX, "cannot create view for null image!");
        return false;
    }
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = out->handle;
    info.format = format;
    info.components = (VkComponentMapping){.a = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .r = VK_COMPONENT_SWIZZLE_IDENTITY};

    info.subresourceRange.aspectMask = flags;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;

    VK_CHK(
        jnk.vkCreateImageView(core->logic_dvc, &info, core->alloc, &out->view));

    return true;
}

b8 vk_image_init(vk_image_t *out, vk_core_t *core, VkFormat format,
                 VkImageUsageFlags usage, VkImageAspectFlags flags, u32 width,
                 u32 height, b8 create_view, vk_memtag_t tag) {
    out->width = width;
    out->height = height;
    out->format = format;
    out->handle = VK_NULL_HANDLE;
    out->memory = VK_NULL_HANDLE;
    out->view = VK_NULL_HANDLE;

    VkImageCreateInfo img_info = {};
    img_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    img_info.imageType = VK_IMAGE_TYPE_2D;
    img_info.extent.width = width;
    img_info.extent.height = height;
    img_info.extent.depth = 1;
    img_info.mipLevels = 1;
    img_info.arrayLayers = 1;
    img_info.format = format;
    img_info.samples = VK_SAMPLE_COUNT_1_BIT;
    img_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    img_info.usage = usage;
    img_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    img_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHK(jnk.vkCreateImage(core->logic_dvc, &img_info, core->alloc,
                             &out->handle));

    VkMemoryRequirements mem_req;
    jnk.vkGetImageMemoryRequirements(core->logic_dvc, out->handle, &mem_req);

    out->size = mem_req.size;

    if (vk_mem_alloc(core, &mem_req, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     &out->memory, tag)) {
        VK_CHK(jnk.vkBindImageMemory(core->logic_dvc, out->handle, out->memory,
                                     0));
    } else {
        jnk.vkDestroyImage(core->logic_dvc, out->handle, core->alloc);
        vk_mem_free(core, out->memory, flags, out->size, tag);
        return false;
    }

    if (create_view) {
        out->view = 0;
        create_image_view(out, core, format, flags);
    }

    return true;
}

void vk_image_kill(vk_image_t *image, vk_core_t *core, vk_memtag_t tag) {
    if (image->view) {
        jnk.vkDestroyImageView(core->logic_dvc, image->view, core->alloc);
        image->view = 0;
    }

    if (image->memory) {
        vk_mem_free(core, image->memory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    image->size, tag);
        image->memory = VK_NULL_HANDLE;
    }

    if (image->handle) {
        jnk.vkDestroyImage(core->logic_dvc, image->handle, core->alloc);
        image->handle = 0;
    }

    image->size = 0;
}
