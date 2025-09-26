#ifndef JUNK_VULKAN_TYPES_H
#define JUNK_VULKAN_TYPES_H

#include "loader.h"
#include "junk/jnk_define.h"
#include "junk/jnk_assert.h"

#define VK_CHK(expr)                                                           \
    do {                                                                       \
        VkResult result = expr;                                                \
        jnk_assert(result == VK_SUCCESS);                                      \
    } while (0)

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define MAX_FRAME_IN_FLIGHT 3

JNK_EXTERN_C_BEGIN

typedef struct {
#if JNK_DEBUG
    VkDebugUtilsMessengerEXT util_dbg;
#endif

    VkInstance instance;
    VkAllocationCallbacks *alloc;

    VkPhysicalDevice gpu;
    VkDevice logic_dvc;
    VkCommandPool gfx_cmd_pool;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties mem_prop;

    struct {
        VkDeviceSize total_allocated;
        VkDeviceSize budget;
        VkDeviceSize dvc_local_used;
        VkDeviceSize host_visible_used;

        // uint64_t tag_alloc_count[RDR_TAG_COUNT];
        // uint64_t tag_allocation[RDR_TAG_COUNT];
    } memories;

    VkQueue graphic_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkFormat default_depth_format;

    uint32_t graphic_idx;
    uint32_t present_idx;
    uint32_t transfer_idx;
} vk_core_t;

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_TYPES_H
