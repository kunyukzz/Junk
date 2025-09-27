#include "core.h"
#include "junk/jnk_log.h"
#include "junk/jnk_memory.h"

#include <string.h> // strcmp

#if JNK_DEBUG
static const char *const req_validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

static const char *const req_device_ext[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static const char *const req_instance_ext[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(JNK_LINUX)
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(JNK_WINDOWS)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
#    error "Other platform not supported"
#endif

#if JNK_DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
};

static u32 find_mem_type(vk_core_t *core, u32 type_filter,
                         VkMemoryPropertyFlags props) {
    VkPhysicalDeviceMemoryProperties mem_props;
    jnk.vkGetPhysicalDeviceMemoryProperties(core->gpu, &mem_props);

    for (u32 i = 0; i < mem_props.memoryTypeCount; ++i) {
        if ((type_filter & (1 << i)) &&
            (mem_props.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }

    jnk_log_fatal(CH_GFX, "Failed to find suitable memory type");
    return UINT32_MAX;
}

#if JNK_DEBUG

static VKAPI_ATTR VkBool32 VKAPI_CALL dbg_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    VkDebugUtilsMessageTypeFlagsEXT msg_type,
    const VkDebugUtilsMessengerCallbackDataEXT *_callback, void *user_data) {
    (void)msg_type;
    (void)user_data;

    switch (msg_severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            jnk_log_error(CH_CORE, _callback->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            jnk_log_warn(CH_CORE, _callback->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            // jnk_log_info(CH_CORE, _callback->pMessage);
            break;
        default: break;
    }
    return VK_FALSE;
}

static b8 init_debugger(vk_core_t *core) {
    u32 log = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
    debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_info.messageSeverity = log;
    debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

    debug_info.pfnUserCallback = dbg_callback;

    VK_CHK(jnk.vkCreateDebugUtilsMessengerEXT(core->instance, &debug_info,
                                              core->alloc, &core->util_dbg));

    if (!jnk_vk_load_instance(core->instance)) {
        jnk_log_fatal(CH_GFX, "failed to load debugger vulkan");
        return false;
    }

    jnk_log_debug(CH_GFX, "vulkan debugger initialize");
    return true;
}

static b8 chk_validation_support(vk_core_t *core) {
    (void)core;
    u32 count = 0;
    jnk.vkEnumerateInstanceLayerProperties(&count, NULL);
    VkLayerProperties *avail =
        (VkLayerProperties *)JMALLOC(sizeof(VkLayerProperties) * count,
                                     MEM_RENDER);

    jnk.vkEnumerateInstanceLayerProperties(&count, avail);

    for (u32 i = 0; i < ARRAY_COUNT(req_validation_layers); ++i) {
        b8 found = false;
        for (u32 j = 0; j < count; ++j) {
            if (strcmp(req_validation_layers[i], avail[j].layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            JFREE(avail, sizeof(VkLayerProperties) * count, MEM_RENDER);
            return false;
        }
    }

    JFREE(avail, sizeof(VkLayerProperties) * count, MEM_RENDER);
    return true;
}

#endif

void get_req_ext(const char *const **out_list, uint32_t *ext_count) {
    if (out_list) *out_list = req_instance_ext;
    if (ext_count) *ext_count = ARRAY_COUNT(req_instance_ext);
}

static b8 chk_dvc_support(VkPhysicalDevice gpu) {
    u32 count = 0;

    jnk.vkEnumerateDeviceExtensionProperties(gpu, JNK_NULL, &count, JNK_NULL);
    VkExtensionProperties *avail =
        (VkExtensionProperties *)JMALLOC(sizeof(VkExtensionProperties) * count,
                                         MEM_RENDER);

    jnk.vkEnumerateDeviceExtensionProperties(gpu, JNK_NULL, &count, avail);

    VkPhysicalDeviceProperties dvc_prop;
    jnk.vkGetPhysicalDeviceProperties(gpu, &dvc_prop);

    for (u32 i = 0; i < ARRAY_COUNT(req_device_ext); ++i) {
        b8 found = false;

        for (u32 j = 0; j < count; ++j) {
            if (strcmp(req_device_ext[i], avail[j].extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            JFREE(avail, sizeof(VkExtensionProperties) * count, MEM_RENDER);
            return false;
        }
    }

    JFREE(avail, sizeof(VkExtensionProperties) * count, MEM_RENDER);
    return true;
}

static i32 score_device(VkPhysicalDevice gpu) {
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceMemoryProperties mems;
    jnk.vkGetPhysicalDeviceProperties(gpu, &props);
    jnk.vkGetPhysicalDeviceMemoryProperties(gpu, &mems);

    i32 score = 0;
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        score += 500;

    score += props.limits.maxImageDimension2D;

    for (u32 i = 0; i < mems.memoryHeapCount; ++i) {
        if (mems.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            score += (mems.memoryHeaps[i].size / (128 * 1024 * 1024));
    }
    return score;
}

static b8 pick_physical_device(vk_core_t *core) {
    u32 count = 0;
    jnk.vkEnumeratePhysicalDevices(core->instance, &count, JNK_NULL);
    if (count == 0) return false;

    VkPhysicalDevice *gpus =
        JMALLOC(sizeof(VkPhysicalDevice) * count, MEM_RENDER);
    jnk.vkEnumeratePhysicalDevices(core->instance, &count, gpus);

    VkPhysicalDevice best = VK_NULL_HANDLE;
    i32 best_score = -1;

    for (u32 i = 0; i < count; ++i) {
        if (!chk_dvc_support(gpus[i])) continue;

        i32 score = score_device(gpus[i]);
        if (score > best_score) {
            best_score = score;
            best = gpus[i];
        }
    }

    JFREE(gpus, sizeof(VkPhysicalDevice) * count, MEM_RENDER);
    if (best == VK_NULL_HANDLE) return false;

    core->gpu = best;
    jnk.vkGetPhysicalDeviceProperties(core->gpu, &core->properties);
    jnk.vkGetPhysicalDeviceFeatures(core->gpu, &core->features);
    jnk.vkGetPhysicalDeviceMemoryProperties(core->gpu, &core->mem_prop);

    jnk_log_debug(CH_GFX, "Selected GPU: %s", core->properties.deviceName);
    return true;
}

static VkFormat find_supported_format(vk_core_t *core,
                                      const VkFormat *candidates,
                                      u32 candidate_count, VkImageTiling tiling,
                                      VkFormatFeatureFlags features) {
    for (u32 i = 0; i < candidate_count; ++i) {
        VkFormatProperties props;
        jnk.vkGetPhysicalDeviceFormatProperties(core->gpu, candidates[i],
                                                &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features)
            return candidates[i];
        if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
            return candidates[i];
    }
    return VK_FORMAT_UNDEFINED;
}

static b8 pick_queue_families(vk_core_t *core) {
    u32 count;
    jnk.vkGetPhysicalDeviceQueueFamilyProperties(core->gpu, &count, JNK_NULL);
    VkQueueFamilyProperties *families =
        JMALLOC(sizeof(VkQueueFamilyProperties) * count, MEM_RENDER);
    jnk.vkGetPhysicalDeviceQueueFamilyProperties(core->gpu, &count, families);

    core->graphic_idx = UINT32_MAX;
    core->transfer_idx = UINT32_MAX;

    for (u32 i = 0; i < count; ++i) {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            core->graphic_idx = i;

        if ((families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
            !(families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            core->transfer_idx = i;
    }

    if (core->transfer_idx == UINT32_MAX)
        core->transfer_idx = core->graphic_idx;

    JFREE(families, sizeof(VkQueueFamilyProperties) * count, MEM_RENDER);
    return (core->graphic_idx != UINT32_MAX);
}

static b8 create_logical_device(vk_core_t *core) {
    f32 priority = 1.0f;
    VkDeviceQueueCreateInfo queues[2] = {};
    u32 unique_count = (core->graphic_idx != core->transfer_idx) ? 2 : 1;

    queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queues[0].queueFamilyIndex = core->graphic_idx;
    queues[0].queueCount = 1;
    queues[0].pQueuePriorities = &priority;

    if (unique_count == 2) {
        queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues[1].queueFamilyIndex = core->transfer_idx;
        queues[1].queueCount = 1;
        queues[1].pQueuePriorities = &priority;
    }

    VkPhysicalDeviceFeatures enable_feats = {};
    enable_feats.fillModeNonSolid = VK_TRUE;
    enable_feats.multiDrawIndirect = VK_TRUE;
    enable_feats.depthClamp = VK_TRUE;
    enable_feats.wideLines = VK_TRUE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = unique_count;
    info.pQueueCreateInfos = queues;
    info.enabledExtensionCount = ARRAY_COUNT(req_device_ext);
    info.ppEnabledExtensionNames = req_device_ext;
    info.pEnabledFeatures = &enable_feats;

    VK_CHK(jnk.vkCreateDevice(core->gpu, &info, core->alloc, &core->logic_dvc));

    if (!jnk_vk_load_device(core->logic_dvc)) return false;

    jnk.vkGetDeviceQueue(core->logic_dvc, core->graphic_idx, 0,
                         &core->graphic_queue);
    jnk.vkGetDeviceQueue(core->logic_dvc, core->transfer_idx, 0,
                         &core->transfer_queue);
    jnk.vkGetDeviceQueue(core->logic_dvc, core->present_idx, 0,
                         &core->present_queue);

    jnk_log_debug(CH_GFX, "vulkan device initialize");
    return true;
}

static b8 create_command_pool(vk_core_t *core) {
    VkCommandPoolCreateInfo pool = {};
    pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool.queueFamilyIndex = core->graphic_idx;
    pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHK(jnk.vkCreateCommandPool(core->logic_dvc, &pool, core->alloc,
                                   &core->gfx_cmd_pool));

    jnk_log_debug(CH_GFX, "vulkan command pool initialize");
    return true;
}

static b8 init_instance(vk_core_t *core) {
    if (!jnk_vk_init()) {
        jnk_log_fatal(CH_CORE, "Failed to load global vulkan table");
        return false;
    }
    core->alloc = JNK_NULL;

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_API_VERSION_1_0;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pApplicationName = "No Engine";

    const char *const *inst_exts;
    uint32_t inst_ext_count;
    get_req_ext(&inst_exts, &inst_ext_count);

    uint32_t layer_count = 0;
    const char *const *layer_names = JNK_NULL;

#if JNK_DEBUG
    if (chk_validation_support(core)) {
        layer_count = ARRAY_COUNT(req_validation_layers);
        layer_names = req_validation_layers;
    }
#endif

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledExtensionCount = inst_ext_count;
    inst_info.ppEnabledExtensionNames = inst_exts;
    inst_info.enabledLayerCount = layer_count;
    inst_info.ppEnabledLayerNames = layer_names;

    VK_CHK(jnk.vkCreateInstance(&inst_info, core->alloc, &core->instance));

    if (!jnk_vk_load_instance(core->instance)) {
        jnk_log_fatal(CH_CORE, "Failed to load instance vulkan table");
        return false;
    }

    return true;
}

static b8 memory_init(vk_core_t *core) {
    core->memories.total_allocated = 0;
    core->memories.dvc_local_used = 0;
    core->memories.host_visible_used = 0;

    core->memories.budget = 1 * 1024 * 1024 * 1024ULL;
    return true;
}

b8 vk_core_init(vk_core_t *core) {
    if (!init_instance(core)) return false;

#if JNK_DEBUG
    if (!init_debugger(core)) return false;
#endif

    if (!pick_physical_device(core)) return false;
    if (!pick_queue_families(core)) return false;

    VkFormat depth_candidates[] = {VK_FORMAT_D32_SFLOAT,
                                   VK_FORMAT_D32_SFLOAT_S8_UINT,
                                   VK_FORMAT_D24_UNORM_S8_UINT};

    core->default_depth_format =
        find_supported_format(core, depth_candidates,
                              ARRAY_COUNT(depth_candidates),
                              VK_IMAGE_TILING_OPTIMAL,
                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    if (core->default_depth_format == VK_FORMAT_UNDEFINED) {
        jnk_log_fatal(CH_GFX, "Failed to find a supported depth format");
        return false;
    }

    if (!create_logical_device(core)) return false;
    if (!create_command_pool(core)) return false;
    if (!memory_init(core)) return false;

    jnk_log_debug(CH_GFX, "vulkan core initialize");
    return true;
}

void vk_core_kill(vk_core_t *core) {
    jnk.vkDestroyCommandPool(core->logic_dvc, core->gfx_cmd_pool, core->alloc);
    core->gfx_cmd_pool = VK_NULL_HANDLE;
    jnk_log_debug(CH_GFX, "vulkan command pool kill");

    jnk.vkDestroyDevice(core->logic_dvc, core->alloc);
    jnk_log_debug(CH_GFX, "vulkan device kill");

#if JNK_DEBUG
    jnk.vkDestroyDebugUtilsMessengerEXT(core->instance, core->util_dbg,
                                        core->alloc);
    jnk_log_debug(CH_GFX, "vulkan debugger kill");
#endif

    jnk.vkDestroyInstance(core->instance, core->alloc);
    jnk_log_debug(CH_GFX, "vulkan core kill");
}

b8 vk_mem_alloc(vk_core_t *core, VkMemoryRequirements *memory_req,
                VkMemoryPropertyFlags flags, VkDeviceMemory *out,
                vk_memtag_t tag) {
    if ((core->memories.total_allocated + memory_req->size) >
        core->memories.budget) {
        jnk_log_error(CH_GFX,
                      "VRAM Budget exceeded! Could not allocate %llu bytes. "
                      "Total: %llu / %llu",
                      memory_req->size, core->memories.total_allocated,
                      core->memories.budget);
        return false;
    }

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_req->size;
    alloc_info.memoryTypeIndex =
        find_mem_type(core, memory_req->memoryTypeBits, flags);

    if (alloc_info.memoryTypeIndex == UINT32_MAX) {
        jnk_log_error(CH_GFX, "Invalid memory type index returned!");
        return false;
    }

    VkResult res =
        jnk.vkAllocateMemory(core->logic_dvc, &alloc_info, core->alloc, out);

    if (res == VK_SUCCESS) {
        core->memories.total_allocated += memory_req->size;
        core->memories.tag_alloc_count[tag]++;
        core->memories.tag_allocation[tag] += memory_req->size;

        if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            core->memories.dvc_local_used += memory_req->size;
        } else if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            core->memories.host_visible_used += memory_req->size;
        }

        return true;
    }

    return false;
}

void vk_mem_free(vk_core_t *core, VkDeviceMemory memory,
                 VkMemoryPropertyFlags flags, VkDeviceSize size,
                 vk_memtag_t tag) {
    jnk.vkFreeMemory(core->logic_dvc, memory, core->alloc);
    core->memories.total_allocated -= size;
    core->memories.tag_alloc_count[tag]--;
    core->memories.tag_allocation[tag] -= size;

    if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        core->memories.dvc_local_used -= size;
    } else if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        core->memories.host_visible_used -= size;
    }
}
