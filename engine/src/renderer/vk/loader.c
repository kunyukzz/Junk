#include "loader.h"

#if defined(JNK_LINUX)
#    include <dlfcn.h>
#elif defined(JNK_WINDOWS)
#    include <windows.h>
#endif

#define JNK_PROC(name, type) ((type)get_proc(name))
#define JNK_INST_PROC(inst, name, type) ((type)s_gpa((inst), (name)))
#define JNK_DVC_PROC(dvc, name, type) ((type)s_dpa((dvc), (name)))

jnk_vk_table_t jnk = {0};
static void *s_vk_lib = JNK_NULL;
static PFN_vkGetInstanceProcAddr s_gpa = VK_NULL_HANDLE;
static PFN_vkGetDeviceProcAddr s_dpa = VK_NULL_HANDLE;

static void *get_proc(const char *name) {
#if defined(JNK_LINUX)
    return dlsym(s_vk_lib, name);
#elif defined(JNK_WINDOWS)
    return (void *)GetProcAddress((HMODULE)s_vk_lib, name);
#else
    return JNK_NULL;
#endif
}

VkResult open_library(void) {
#if defined(JNK_LINUX)
    s_vk_lib = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (!s_vk_lib) {
        s_vk_lib = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    }
    if (!s_vk_lib) return VK_ERROR_INITIALIZATION_FAILED;

    s_gpa = (PFN_vkGetInstanceProcAddr)dlsym(s_vk_lib, "vkGetInstanceProcAddr");
#elif defined(JNK_WINDOWS)
    s_vk_lib = (void *)LoadLibraryA("vulkan-1.dll");
    if (!s_vk_lib) return VK_ERROR_INITIALIZATION_FAILED;

    s_gpa = (PFN_vkGetInstanceProcAddr)GetProcAddress((HMODULE)s_vk_lib,
                                                      "vkGetInstanceProcAddr");
#endif

    return (s_gpa != VK_NULL_HANDLE) ? VK_SUCCESS
                                     : VK_ERROR_INITIALIZATION_FAILED;
}

b8 jnk_vk_init(void) {
    if (open_library() != VK_SUCCESS) return false;

    jnk.vkGetInstanceProcAddr = s_gpa;

    jnk.vkCreateInstance = JNK_PROC("vkCreateInstance", PFN_vkCreateInstance);

    jnk.vkEnumerateInstanceExtensionProperties =
        JNK_PROC("vkEnumerateInstanceExtensionProperties",
                 PFN_vkEnumerateInstanceExtensionProperties);

    jnk.vkEnumerateInstanceLayerProperties =
        JNK_PROC("vkEnumerateInstanceLayerProperties",
                 PFN_vkEnumerateInstanceLayerProperties);

    jnk.vkEnumerateInstanceVersion =
        JNK_PROC("vkEnumerateInstanceVersion", PFN_vkEnumerateInstanceVersion);

    return (jnk.vkCreateInstance != VK_NULL_HANDLE);
}

b8 jnk_vk_load_instance(VkInstance inst) {
    if (!s_gpa) return false;

    // ======================== CORE INSTANCE FUNCTIONS ====================
    jnk.vkDestroyInstance =
        JNK_INST_PROC(inst, "vkDestroyInstance", PFN_vkDestroyInstance);

    jnk.vkEnumeratePhysicalDevices =
        JNK_INST_PROC(inst, "vkEnumeratePhysicalDevices",
                      PFN_vkEnumeratePhysicalDevices);
    jnk.vkCreateDevice =
        JNK_INST_PROC(inst, "vkCreateDevice", PFN_vkCreateDevice);

    jnk.vkGetDeviceProcAddr =
        JNK_INST_PROC(inst, "vkGetDeviceProcAddr", PFN_vkGetDeviceProcAddr);

    // ================= PHYSICAL DEVICE QUERY FUNCTIONS ====================
    jnk.vkGetPhysicalDeviceFeatures =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceFeatures",
                      PFN_vkGetPhysicalDeviceFeatures);

    jnk.vkGetPhysicalDeviceProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceProperties",
                      PFN_vkGetPhysicalDeviceProperties);

    jnk.vkGetPhysicalDeviceQueueFamilyProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceQueueFamilyProperties",
                      PFN_vkGetPhysicalDeviceQueueFamilyProperties);

    jnk.vkGetPhysicalDeviceMemoryProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceMemoryProperties",
                      PFN_vkGetPhysicalDeviceMemoryProperties);

    jnk.vkGetPhysicalDeviceSparseImageFormatProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSparseImageFormatProperties",
                      PFN_vkGetPhysicalDeviceSparseImageFormatProperties);

    jnk.vkGetPhysicalDeviceFormatProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceFormatProperties",
                      PFN_vkGetPhysicalDeviceFormatProperties);

    // ======================== SURFACE FUNCTIONS ==========================
#if JNK_LINUX
    jnk.vkCreateXlibSurfaceKHR = JNK_INST_PROC(inst, "vkCreateXlibSurfaceKHR",
                                               PFN_vkCreateXlibSurfaceKHR);

#elif JNK_WINDOWS
    jnk.vkCreateWin32SurfaceKHR = JNK_INST_PROC(inst, "vkCreateWin32SurfaceKHR",
                                                PFN_vkCreateWin32SurfaceKHR);
#endif

    // ===================== EXTENSION ENUMERATIONS =========================
    jnk.vkEnumerateDeviceExtensionProperties =
        JNK_INST_PROC(inst, "vkEnumerateDeviceExtensionProperties",
                      PFN_vkEnumerateDeviceExtensionProperties);

    // ===================== COMMON SURFACE FUNCTIONS =======================
    jnk.vkDestroySurfaceKHR =
        JNK_INST_PROC(inst, "vkDestroySurfaceKHR", PFN_vkDestroySurfaceKHR);

    jnk.vkGetPhysicalDeviceSurfaceSupportKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfaceSupportKHR",
                      PFN_vkGetPhysicalDeviceSurfaceSupportKHR);

    jnk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR",
                      PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

    jnk.vkGetPhysicalDeviceSurfaceFormatsKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfaceFormatsKHR",
                      PFN_vkGetPhysicalDeviceSurfaceFormatsKHR);

    jnk.vkGetPhysicalDeviceSurfacePresentModesKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfacePresentModesKHR",
                      PFN_vkGetPhysicalDeviceSurfacePresentModesKHR);

    // ================= DEBUG UTILITIES FUNCTIONS ====================
    jnk.vkCreateDebugUtilsMessengerEXT =
        JNK_INST_PROC(inst, "vkCreateDebugUtilsMessengerEXT",
                      PFN_vkCreateDebugUtilsMessengerEXT);

    jnk.vkDestroyDebugUtilsMessengerEXT =
        JNK_INST_PROC(inst, "vkDestroyDebugUtilsMessengerEXT",
                      PFN_vkDestroyDebugUtilsMessengerEXT);

    s_dpa = jnk.vkGetDeviceProcAddr;
    return (jnk.vkCreateDevice && jnk.vkEnumeratePhysicalDevices);
}

b8 jnk_vk_load_device(VkDevice dvc) {
    if (!s_dpa) return false;

    // ==================== DEVICE MANAGEMENT ====================
    jnk.vkDestroyDevice =
        JNK_DVC_PROC(dvc, "vkDestroyDevice", PFN_vkDestroyDevice);

    jnk.vkDeviceWaitIdle =
        JNK_DVC_PROC(dvc, "vkDeviceWaitIdle", PFN_vkDeviceWaitIdle);

    // ==================== QUEUE MANAGEMENT ====================
    jnk.vkGetDeviceQueue =
        JNK_DVC_PROC(dvc, "vkGetDeviceQueue", PFN_vkGetDeviceQueue);

    jnk.vkQueueSubmit = JNK_DVC_PROC(dvc, "vkQueueSubmit", PFN_vkQueueSubmit);

    jnk.vkQueueWaitIdle =
        JNK_DVC_PROC(dvc, "vkQueueWaitIdle", PFN_vkQueueWaitIdle);

    jnk.vkQueuePresentKHR =
        JNK_DVC_PROC(dvc, "vkQueuePresentKHR", PFN_vkQueuePresentKHR);

    // =============== COMMAND BUFFER MANAGEMENT =================
    jnk.vkCreateCommandPool =
        JNK_DVC_PROC(dvc, "vkCreateCommandPool", PFN_vkCreateCommandPool);

    jnk.vkDestroyCommandPool =
        JNK_DVC_PROC(dvc, "vkDestroyCommandPool", PFN_vkDestroyCommandPool);

    jnk.vkResetCommandPool =
        JNK_DVC_PROC(dvc, "vkResetCommandPool", PFN_vkResetCommandPool);

    jnk.vkAllocateCommandBuffers = JNK_DVC_PROC(dvc, "vkAllocateCommandBuffers",
                                                PFN_vkAllocateCommandBuffers);

    jnk.vkFreeCommandBuffers =
        JNK_DVC_PROC(dvc, "vkFreeCommandBuffers", PFN_vkFreeCommandBuffers);

    jnk.vkBeginCommandBuffer =
        JNK_DVC_PROC(dvc, "vkBeginCommandBuffer", PFN_vkBeginCommandBuffer);

    jnk.vkEndCommandBuffer =
        JNK_DVC_PROC(dvc, "vkEndCommandBuffer", PFN_vkEndCommandBuffer);

    jnk.vkResetCommandBuffer =
        JNK_DVC_PROC(dvc, "vkResetCommandBuffer", PFN_vkResetCommandBuffer);

    // =========== COMMAND BUFFER RECORDING (Drawing) =============
    jnk.vkCmdBeginRenderPass =
        JNK_DVC_PROC(dvc, "vkCmdBeginRenderPass", PFN_vkCmdBeginRenderPass);

    jnk.vkCmdNextSubpass =
        JNK_DVC_PROC(dvc, "vkCmdNextSubpass", PFN_vkCmdNextSubpass);

    jnk.vkCmdEndRenderPass =
        JNK_DVC_PROC(dvc, "vkCmdEndRenderPass", PFN_vkCmdEndRenderPass);

    jnk.vkCmdBeginRenderPass2 =
        JNK_DVC_PROC(dvc, "vkCmdBeginRenderPass2", PFN_vkCmdBeginRenderPass2);

    jnk.vkCmdEndRenderPass2 =
        JNK_DVC_PROC(dvc, "vkCmdEndRenderPass2", PFN_vkCmdEndRenderPass2);

    jnk.vkCmdBindPipeline =
        JNK_DVC_PROC(dvc, "vkCmdBindPipeline", PFN_vkCmdBindPipeline);

    jnk.vkCmdBindDescriptorSets = JNK_DVC_PROC(dvc, "vkCmdBindDescriptorSets",
                                               PFN_vkCmdBindDescriptorSets);
    jnk.vkCmdBindVertexBuffers =
        JNK_DVC_PROC(dvc, "vkCmdBindVertexBuffers", PFN_vkCmdBindVertexBuffers);

    jnk.vkCmdBindIndexBuffer =
        JNK_DVC_PROC(dvc, "vkCmdBindIndexBuffer", PFN_vkCmdBindIndexBuffer);

    jnk.vkCmdDraw = JNK_DVC_PROC(dvc, "vkCmdDraw", PFN_vkCmdDraw);

    jnk.vkCmdDrawIndexed =
        JNK_DVC_PROC(dvc, "vkCmdDrawIndexed", PFN_vkCmdDrawIndexed);

    jnk.vkCmdPushConstants =
        JNK_DVC_PROC(dvc, "vkCmdPushConstants", PFN_vkCmdPushConstants);

    // =========== COMMAND BUFFER RECORDING (State) ===============
    jnk.vkCmdSetViewport =
        JNK_DVC_PROC(dvc, "vkCmdSetViewport", PFN_vkCmdSetViewport);

    jnk.vkCmdSetScissor =
        JNK_DVC_PROC(dvc, "vkCmdSetScissor", PFN_vkCmdSetScissor);

    jnk.vkCmdSetLineWidth =
        JNK_DVC_PROC(dvc, "vkCmdSetLineWidth", PFN_vkCmdSetLineWidth);

    jnk.vkCmdPipelineBarrier =
        JNK_DVC_PROC(dvc, "vkCmdPipelineBarrier", PFN_vkCmdPipelineBarrier);

    // ========= COMMAND BUFFER RECORDING (Clear/Copy) ============
    jnk.vkCmdClearAttachments =
        JNK_DVC_PROC(dvc, "vkCmdClearAttachments", PFN_vkCmdClearAttachments);

    jnk.vkCmdCopyBuffer =
        JNK_DVC_PROC(dvc, "vkCmdCopyBuffer", PFN_vkCmdCopyBuffer);

    jnk.vkCmdCopyBufferToImage =
        JNK_DVC_PROC(dvc, "vkCmdCopyBufferToImage", PFN_vkCmdCopyBufferToImage);

    jnk.vkCmdCopyImage =
        JNK_DVC_PROC(dvc, "vkCmdCopyImage", PFN_vkCmdCopyImage);

    jnk.vkCmdBlitImage =
        JNK_DVC_PROC(dvc, "vkCmdBlitImage", PFN_vkCmdBlitImage);

    jnk.vkCmdFillBuffer =
        JNK_DVC_PROC(dvc, "vkCmdFillBuffer", PFN_vkCmdFillBuffer);

    // ==================== MEMORY MANAGEMENT ====================
    jnk.vkAllocateMemory =
        JNK_DVC_PROC(dvc, "vkAllocateMemory", PFN_vkAllocateMemory);

    jnk.vkFreeMemory = JNK_DVC_PROC(dvc, "vkFreeMemory", PFN_vkFreeMemory);

    jnk.vkMapMemory = JNK_DVC_PROC(dvc, "vkMapMemory", PFN_vkMapMemory);

    jnk.vkUnmapMemory = JNK_DVC_PROC(dvc, "vkUnmapMemory", PFN_vkUnmapMemory);

    jnk.vkGetBufferMemoryRequirements =
        JNK_DVC_PROC(dvc, "vkGetBufferMemoryRequirements",
                     PFN_vkGetBufferMemoryRequirements);

    jnk.vkGetImageMemoryRequirements =
        JNK_DVC_PROC(dvc, "vkGetImageMemoryRequirements",
                     PFN_vkGetImageMemoryRequirements);

    jnk.vkBindBufferMemory =
        JNK_DVC_PROC(dvc, "vkBindBufferMemory", PFN_vkBindBufferMemory);

    jnk.vkBindImageMemory =
        JNK_DVC_PROC(dvc, "vkBindImageMemory", PFN_vkBindImageMemory);

    jnk.vkBindBufferMemory2 =
        JNK_DVC_PROC(dvc, "vkBindBufferMemory2", PFN_vkBindBufferMemory2);

    jnk.vkBindImageMemory2 =
        JNK_DVC_PROC(dvc, "vkBindImageMemory2", PFN_vkBindImageMemory2);

    // ============= RESOURCE CREATION/DESTRUCTION ==============
    jnk.vkCreateBuffer =
        JNK_DVC_PROC(dvc, "vkCreateBuffer", PFN_vkCreateBuffer);

    jnk.vkDestroyBuffer =
        JNK_DVC_PROC(dvc, "vkDestroyBuffer", PFN_vkDestroyBuffer);

    jnk.vkCreateImage = JNK_DVC_PROC(dvc, "vkCreateImage", PFN_vkCreateImage);

    jnk.vkDestroyImage =
        JNK_DVC_PROC(dvc, "vkDestroyImage", PFN_vkDestroyImage);

    jnk.vkCreateImageView =
        JNK_DVC_PROC(dvc, "vkCreateImageView", PFN_vkCreateImageView);

    jnk.vkDestroyImageView =
        JNK_DVC_PROC(dvc, "vkDestroyImageView", PFN_vkDestroyImageView);

    jnk.vkCreateSampler =
        JNK_DVC_PROC(dvc, "vkCreateSampler", PFN_vkCreateSampler);

    jnk.vkDestroySampler =
        JNK_DVC_PROC(dvc, "vkDestroySampler", PFN_vkDestroySampler);

    // ================= DESCRIPTOR MANAGEMENT ====================
    jnk.vkCreateDescriptorSetLayout =
        JNK_DVC_PROC(dvc, "vkCreateDescriptorSetLayout",
                     PFN_vkCreateDescriptorSetLayout);

    jnk.vkDestroyDescriptorSetLayout =
        JNK_DVC_PROC(dvc, "vkDestroyDescriptorSetLayout",
                     PFN_vkDestroyDescriptorSetLayout);

    jnk.vkCreateDescriptorPool =
        JNK_DVC_PROC(dvc, "vkCreateDescriptorPool", PFN_vkCreateDescriptorPool);

    jnk.vkDestroyDescriptorPool = JNK_DVC_PROC(dvc, "vkDestroyDescriptorPool",
                                               PFN_vkDestroyDescriptorPool);

    jnk.vkAllocateDescriptorSets = JNK_DVC_PROC(dvc, "vkAllocateDescriptorSets",
                                                PFN_vkAllocateDescriptorSets);

    jnk.vkFreeDescriptorSets =
        JNK_DVC_PROC(dvc, "vkFreeDescriptorSets", PFN_vkFreeDescriptorSets);

    jnk.vkUpdateDescriptorSets =
        JNK_DVC_PROC(dvc, "vkUpdateDescriptorSets", PFN_vkUpdateDescriptorSets);

    // ================== PIPELINE MANAGEMENT =====================
    jnk.vkCreatePipelineLayout =
        JNK_DVC_PROC(dvc, "vkCreatePipelineLayout", PFN_vkCreatePipelineLayout);

    jnk.vkDestroyPipelineLayout = JNK_DVC_PROC(dvc, "vkDestroyPipelineLayout",
                                               PFN_vkDestroyPipelineLayout);

    jnk.vkCreateShaderModule =
        JNK_DVC_PROC(dvc, "vkCreateShaderModule", PFN_vkCreateShaderModule);

    jnk.vkDestroyShaderModule =
        JNK_DVC_PROC(dvc, "vkDestroyShaderModule", PFN_vkDestroyShaderModule);

    jnk.vkCreateGraphicsPipelines =
        JNK_DVC_PROC(dvc, "vkCreateGraphicsPipelines",
                     PFN_vkCreateGraphicsPipelines);

    jnk.vkDestroyPipeline =
        JNK_DVC_PROC(dvc, "vkDestroyPipeline", PFN_vkDestroyPipeline);

    // ================= RENDERPASS MANAGEMENT ====================
    jnk.vkCreateRenderPass =
        JNK_DVC_PROC(dvc, "vkCreateRenderPass", PFN_vkCreateRenderPass);

    jnk.vkDestroyRenderPass =
        JNK_DVC_PROC(dvc, "vkDestroyRenderPass", PFN_vkDestroyRenderPass);

    jnk.vkCreateFramebuffer =
        JNK_DVC_PROC(dvc, "vkCreateFramebuffer", PFN_vkCreateFramebuffer);

    jnk.vkDestroyFramebuffer =
        JNK_DVC_PROC(dvc, "vkDestroyFramebuffer", PFN_vkDestroyFramebuffer);

    // ==================== SYNCHRONIZATION ====================
    jnk.vkCreateSemaphore =
        JNK_DVC_PROC(dvc, "vkCreateSemaphore", PFN_vkCreateSemaphore);

    jnk.vkDestroySemaphore =
        JNK_DVC_PROC(dvc, "vkDestroySemaphore", PFN_vkDestroySemaphore);

    jnk.vkCreateFence = JNK_DVC_PROC(dvc, "vkCreateFence", PFN_vkCreateFence);

    jnk.vkWaitForFences =
        JNK_DVC_PROC(dvc, "vkWaitForFences", PFN_vkWaitForFences);

    jnk.vkResetFences = JNK_DVC_PROC(dvc, "vkResetFences", PFN_vkResetFences);

    jnk.vkDestroyFence =
        JNK_DVC_PROC(dvc, "vkDestroyFence", PFN_vkDestroyFence);

    // ================= SWAPCHAIN MANAGEMENT ====================
    jnk.vkCreateSwapchainKHR =
        JNK_DVC_PROC(dvc, "vkCreateSwapchainKHR", PFN_vkCreateSwapchainKHR);

    jnk.vkDestroySwapchainKHR =
        JNK_DVC_PROC(dvc, "vkDestroySwapchainKHR", PFN_vkDestroySwapchainKHR);

    jnk.vkGetSwapchainImagesKHR = JNK_DVC_PROC(dvc, "vkGetSwapchainImagesKHR",
                                               PFN_vkGetSwapchainImagesKHR);

    jnk.vkAcquireNextImageKHR =
        JNK_DVC_PROC(dvc, "vkAcquireNextImageKHR", PFN_vkAcquireNextImageKHR);

// ==================== DEBUG UTILITIES ====================
#ifdef JNK_DEBUG
    jnk.vkSetDebugUtilsObjectNameEXT =
        JNK_DVC_PROC(dvc, "vkSetDebugUtilsObjectNameEXT",
                     PFN_vkSetDebugUtilsObjectNameEXT);

    jnk.vkSetDebugUtilsObjectTagEXT =
        JNK_DVC_PROC(dvc, "vkSetDebugUtilsObjectTagEXT",
                     PFN_vkSetDebugUtilsObjectTagEXT);
#endif
    return (jnk.vkDeviceWaitIdle != VK_NULL_HANDLE);
}

void jnk_vk_kill(void) {
    if (s_vk_lib) {
        dlclose(s_vk_lib);
        s_vk_lib = JNK_NULL;
    }
}
