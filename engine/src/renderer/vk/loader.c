#include "loader.h"

#if defined(JNK_LINUX)
#    include <dlfcn.h>
#elif defined(JNK_WINDOWS)
#    include <windows.h>
#endif

#define JNK_PROC(name, type) ((type)get_proc(name))
#define JNK_INST_PROC(inst, name, type) ((type)s_gpa((inst), (name)))
#define JNK_DVC_PROC(dvc, name, type) ((type)s_dpa((dvc), (name)))

jnk_vk_table_t vk = {0};
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

    vk.vkGetInstanceProcAddr = s_gpa;

    vk.vkCreateInstance = JNK_PROC("vkCreateInstance", PFN_vkCreateInstance);

    vk.vkEnumerateInstanceExtensionProperties =
        JNK_PROC("vkEnumerateInstanceExtensionProperties",
                 PFN_vkEnumerateInstanceExtensionProperties);

    vk.vkEnumerateInstanceLayerProperties =
        JNK_PROC("vkEnumerateInstanceLayerProperties",
                 PFN_vkEnumerateInstanceLayerProperties);

    vk.vkEnumerateInstanceVersion =
        JNK_PROC("vkEnumerateInstanceVersion", PFN_vkEnumerateInstanceVersion);

    return (vk.vkCreateInstance != VK_NULL_HANDLE);
}

b8 jnk_vk_load_instance(VkInstance inst) {
    if (!s_gpa) return false;

    // ======================== CORE INSTANCE FUNCTIONS ====================
    vk.vkDestroyInstance =
        JNK_INST_PROC(inst, "vkDestroyInstance", PFN_vkDestroyInstance);

    vk.vkEnumeratePhysicalDevices =
        JNK_INST_PROC(inst, "vkEnumeratePhysicalDevices",
                      PFN_vkEnumeratePhysicalDevices);
    vk.vkCreateDevice =
        JNK_INST_PROC(inst, "vkCreateDevice", PFN_vkCreateDevice);

    vk.vkGetDeviceProcAddr =
        JNK_INST_PROC(inst, "vkGetDeviceProcAddr", PFN_vkGetDeviceProcAddr);

    // ================= PHYSICAL DEVICE QUERY FUNCTIONS ====================
    vk.vkGetPhysicalDeviceFeatures =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceFeatures",
                      PFN_vkGetPhysicalDeviceFeatures);

    vk.vkGetPhysicalDeviceProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceProperties",
                      PFN_vkGetPhysicalDeviceProperties);

    vk.vkGetPhysicalDeviceQueueFamilyProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceQueueFamilyProperties",
                      PFN_vkGetPhysicalDeviceQueueFamilyProperties);

    vk.vkGetPhysicalDeviceMemoryProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceMemoryProperties",
                      PFN_vkGetPhysicalDeviceMemoryProperties);

    vk.vkGetPhysicalDeviceSparseImageFormatProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSparseImageFormatProperties",
                      PFN_vkGetPhysicalDeviceSparseImageFormatProperties);

    vk.vkGetPhysicalDeviceFormatProperties =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceFormatProperties",
                      PFN_vkGetPhysicalDeviceFormatProperties);

    // ======================== SURFACE FUNCTIONS ==========================
#if JNK_LINUX
    vk.vkCreateXlibSurfaceKHR = JNK_INST_PROC(inst, "vkCreateXlibSurfaceKHR",
                                              PFN_vkCreateXlibSurfaceKHR);

#elif JNK_WINDOWS
    vk.vkCreateWin32SurfaceKHR = JNK_INST_PROC(inst, "vkCreateWin32SurfaceKHR",
                                               PFN_vkCreateWin32SurfaceKHR);
#endif

    // ===================== EXTENSION ENUMERATIONS =========================
    vk.vkEnumerateDeviceExtensionProperties =
        JNK_INST_PROC(inst, "vkEnumerateDeviceExtensionProperties",
                      PFN_vkEnumerateDeviceExtensionProperties);

    // ===================== COMMON SURFACE FUNCTIONS =======================
    vk.vkDestroySurfaceKHR =
        JNK_INST_PROC(inst, "vkDestroySurfaceKHR", PFN_vkDestroySurfaceKHR);

    vk.vkGetPhysicalDeviceSurfaceSupportKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfaceSupportKHR",
                      PFN_vkGetPhysicalDeviceSurfaceSupportKHR);

    vk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR",
                      PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

    vk.vkGetPhysicalDeviceSurfaceFormatsKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfaceFormatsKHR",
                      PFN_vkGetPhysicalDeviceSurfaceFormatsKHR);

    vk.vkGetPhysicalDeviceSurfacePresentModesKHR =
        JNK_INST_PROC(inst, "vkGetPhysicalDeviceSurfacePresentModesKHR",
                      PFN_vkGetPhysicalDeviceSurfacePresentModesKHR);

    // ================= DEBUG UTILITIES FUNCTIONS ====================
    vk.vkCreateDebugUtilsMessengerEXT =
        JNK_INST_PROC(inst, "vkCreateDebugUtilsMessengerEXT",
                      PFN_vkCreateDebugUtilsMessengerEXT);

    vk.vkDestroyDebugUtilsMessengerEXT =
        JNK_INST_PROC(inst, "vkDestroyDebugUtilsMessengerEXT",
                      PFN_vkDestroyDebugUtilsMessengerEXT);

    s_dpa = vk.vkGetDeviceProcAddr;
    return (vk.vkCreateDevice && vk.vkEnumeratePhysicalDevices);
}

b8 jnk_vk_load_device(VkDevice dvc) {
    if (!s_dpa) return false;

    // ==================== DEVICE MANAGEMENT ====================
    vk.vkDestroyDevice =
        JNK_DVC_PROC(dvc, "vkDestroyDevice", PFN_vkDestroyDevice);

    vk.vkDeviceWaitIdle =
        JNK_DVC_PROC(dvc, "vkDeviceWaitIdle", PFN_vkDeviceWaitIdle);

    // ==================== QUEUE MANAGEMENT ====================
    vk.vkGetDeviceQueue =
        JNK_DVC_PROC(dvc, "vkGetDeviceQueue", PFN_vkGetDeviceQueue);

    vk.vkQueueSubmit = JNK_DVC_PROC(dvc, "vkQueueSubmit", PFN_vkQueueSubmit);

    vk.vkQueueWaitIdle =
        JNK_DVC_PROC(dvc, "vkQueueWaitIdle", PFN_vkQueueWaitIdle);

    vk.vkQueuePresentKHR =
        JNK_DVC_PROC(dvc, "vkQueuePresentKHR", PFN_vkQueuePresentKHR);

    // =============== COMMAND BUFFER MANAGEMENT =================
    vk.vkCreateCommandPool =
        JNK_DVC_PROC(dvc, "vkCreateCommandPool", PFN_vkCreateCommandPool);

    vk.vkDestroyCommandPool =
        JNK_DVC_PROC(dvc, "vkDestroyCommandPool", PFN_vkDestroyCommandPool);

    vk.vkResetCommandPool =
        JNK_DVC_PROC(dvc, "vkResetCommandPool", PFN_vkResetCommandPool);

    vk.vkAllocateCommandBuffers = JNK_DVC_PROC(dvc, "vkAllocateCommandBuffers",
                                               PFN_vkAllocateCommandBuffers);

    vk.vkFreeCommandBuffers =
        JNK_DVC_PROC(dvc, "vkFreeCommandBuffers", PFN_vkFreeCommandBuffers);

    vk.vkBeginCommandBuffer =
        JNK_DVC_PROC(dvc, "vkBeginCommandBuffer", PFN_vkBeginCommandBuffer);

    vk.vkEndCommandBuffer =
        JNK_DVC_PROC(dvc, "vkEndCommandBuffer", PFN_vkEndCommandBuffer);

    vk.vkResetCommandBuffer =
        JNK_DVC_PROC(dvc, "vkResetCommandBuffer", PFN_vkResetCommandBuffer);

    // =========== COMMAND BUFFER RECORDING (Drawing) =============
    vk.vkCmdBeginRenderPass =
        JNK_DVC_PROC(dvc, "vkCmdBeginRenderPass", PFN_vkCmdBeginRenderPass);

    vk.vkCmdNextSubpass =
        JNK_DVC_PROC(dvc, "vkCmdNextSubpass", PFN_vkCmdNextSubpass);

    vk.vkCmdEndRenderPass =
        JNK_DVC_PROC(dvc, "vkCmdEndRenderPass", PFN_vkCmdEndRenderPass);

    vk.vkCmdBeginRenderPass2 =
        JNK_DVC_PROC(dvc, "vkCmdBeginRenderPass2", PFN_vkCmdBeginRenderPass2);

    vk.vkCmdEndRenderPass2 =
        JNK_DVC_PROC(dvc, "vkCmdEndRenderPass2", PFN_vkCmdEndRenderPass2);

    vk.vkCmdBindPipeline =
        JNK_DVC_PROC(dvc, "vkCmdBindPipeline", PFN_vkCmdBindPipeline);

    vk.vkCmdBindDescriptorSets = JNK_DVC_PROC(dvc, "vkCmdBindDescriptorSets",
                                              PFN_vkCmdBindDescriptorSets);
    vk.vkCmdBindVertexBuffers =
        JNK_DVC_PROC(dvc, "vkCmdBindVertexBuffers", PFN_vkCmdBindVertexBuffers);

    vk.vkCmdBindIndexBuffer =
        JNK_DVC_PROC(dvc, "vkCmdBindIndexBuffer", PFN_vkCmdBindIndexBuffer);

    vk.vkCmdDraw = JNK_DVC_PROC(dvc, "vkCmdDraw", PFN_vkCmdDraw);

    vk.vkCmdDrawIndexed =
        JNK_DVC_PROC(dvc, "vkCmdDrawIndexed", PFN_vkCmdDrawIndexed);

    vk.vkCmdPushConstants =
        JNK_DVC_PROC(dvc, "vkCmdPushConstants", PFN_vkCmdPushConstants);

    // =========== COMMAND BUFFER RECORDING (State) ===============
    vk.vkCmdSetViewport =
        JNK_DVC_PROC(dvc, "vkCmdSetViewport", PFN_vkCmdSetViewport);

    vk.vkCmdSetScissor =
        JNK_DVC_PROC(dvc, "vkCmdSetScissor", PFN_vkCmdSetScissor);

    vk.vkCmdSetLineWidth =
        JNK_DVC_PROC(dvc, "vkCmdSetLineWidth", PFN_vkCmdSetLineWidth);

    vk.vkCmdPipelineBarrier =
        JNK_DVC_PROC(dvc, "vkCmdPipelineBarrier", PFN_vkCmdPipelineBarrier);

    // ========= COMMAND BUFFER RECORDING (Clear/Copy) ============
    vk.vkCmdClearAttachments =
        JNK_DVC_PROC(dvc, "vkCmdClearAttachments", PFN_vkCmdClearAttachments);

    vk.vkCmdCopyBuffer =
        JNK_DVC_PROC(dvc, "vkCmdCopyBuffer", PFN_vkCmdCopyBuffer);

    vk.vkCmdCopyBufferToImage =
        JNK_DVC_PROC(dvc, "vkCmdCopyBufferToImage", PFN_vkCmdCopyBufferToImage);

    vk.vkCmdCopyImage = JNK_DVC_PROC(dvc, "vkCmdCopyImage", PFN_vkCmdCopyImage);

    vk.vkCmdBlitImage = JNK_DVC_PROC(dvc, "vkCmdBlitImage", PFN_vkCmdBlitImage);

    vk.vkCmdFillBuffer =
        JNK_DVC_PROC(dvc, "vkCmdFillBuffer", PFN_vkCmdFillBuffer);

    // ==================== MEMORY MANAGEMENT ====================
    vk.vkAllocateMemory =
        JNK_DVC_PROC(dvc, "vkAllocateMemory", PFN_vkAllocateMemory);

    vk.vkFreeMemory = JNK_DVC_PROC(dvc, "vkFreeMemory", PFN_vkFreeMemory);

    vk.vkMapMemory = JNK_DVC_PROC(dvc, "vkMapMemory", PFN_vkMapMemory);

    vk.vkUnmapMemory = JNK_DVC_PROC(dvc, "vkUnmapMemory", PFN_vkUnmapMemory);

    vk.vkGetBufferMemoryRequirements =
        JNK_DVC_PROC(dvc, "vkGetBufferMemoryRequirements",
                     PFN_vkGetBufferMemoryRequirements);

    vk.vkGetImageMemoryRequirements =
        JNK_DVC_PROC(dvc, "vkGetImageMemoryRequirements",
                     PFN_vkGetImageMemoryRequirements);

    vk.vkBindBufferMemory =
        JNK_DVC_PROC(dvc, "vkBindBufferMemory", PFN_vkBindBufferMemory);

    vk.vkBindImageMemory =
        JNK_DVC_PROC(dvc, "vkBindImageMemory", PFN_vkBindImageMemory);

    vk.vkBindBufferMemory2 =
        JNK_DVC_PROC(dvc, "vkBindBufferMemory2", PFN_vkBindBufferMemory2);

    vk.vkBindImageMemory2 =
        JNK_DVC_PROC(dvc, "vkBindImageMemory2", PFN_vkBindImageMemory2);

    // ============= RESOURCE CREATION/DESTRUCTION ==============
    vk.vkCreateBuffer = JNK_DVC_PROC(dvc, "vkCreateBuffer", PFN_vkCreateBuffer);

    vk.vkDestroyBuffer =
        JNK_DVC_PROC(dvc, "vkDestroyBuffer", PFN_vkDestroyBuffer);

    vk.vkCreateImage = JNK_DVC_PROC(dvc, "vkCreateImage", PFN_vkCreateImage);

    vk.vkDestroyImage = JNK_DVC_PROC(dvc, "vkDestroyImage", PFN_vkDestroyImage);

    vk.vkCreateImageView =
        JNK_DVC_PROC(dvc, "vkCreateImageView", PFN_vkCreateImageView);

    vk.vkDestroyImageView =
        JNK_DVC_PROC(dvc, "vkDestroyImageView", PFN_vkDestroyImageView);

    vk.vkCreateSampler =
        JNK_DVC_PROC(dvc, "vkCreateSampler", PFN_vkCreateSampler);

    vk.vkDestroySampler =
        JNK_DVC_PROC(dvc, "vkDestroySampler", PFN_vkDestroySampler);

    // ================= DESCRIPTOR MANAGEMENT ====================
    vk.vkCreateDescriptorSetLayout =
        JNK_DVC_PROC(dvc, "vkCreateDescriptorSetLayout",
                     PFN_vkCreateDescriptorSetLayout);

    vk.vkDestroyDescriptorSetLayout =
        JNK_DVC_PROC(dvc, "vkDestroyDescriptorSetLayout",
                     PFN_vkDestroyDescriptorSetLayout);

    vk.vkCreateDescriptorPool =
        JNK_DVC_PROC(dvc, "vkCreateDescriptorPool", PFN_vkCreateDescriptorPool);

    vk.vkDestroyDescriptorPool = JNK_DVC_PROC(dvc, "vkDestroyDescriptorPool",
                                              PFN_vkDestroyDescriptorPool);

    vk.vkAllocateDescriptorSets = JNK_DVC_PROC(dvc, "vkAllocateDescriptorSets",
                                               PFN_vkAllocateDescriptorSets);

    vk.vkFreeDescriptorSets =
        JNK_DVC_PROC(dvc, "vkFreeDescriptorSets", PFN_vkFreeDescriptorSets);

    vk.vkUpdateDescriptorSets =
        JNK_DVC_PROC(dvc, "vkUpdateDescriptorSets", PFN_vkUpdateDescriptorSets);

    // ================== PIPELINE MANAGEMENT =====================
    vk.vkCreatePipelineLayout =
        JNK_DVC_PROC(dvc, "vkCreatePipelineLayout", PFN_vkCreatePipelineLayout);

    vk.vkDestroyPipelineLayout = JNK_DVC_PROC(dvc, "vkDestroyPipelineLayout",
                                              PFN_vkDestroyPipelineLayout);

    vk.vkCreateShaderModule =
        JNK_DVC_PROC(dvc, "vkCreateShaderModule", PFN_vkCreateShaderModule);

    vk.vkDestroyShaderModule =
        JNK_DVC_PROC(dvc, "vkDestroyShaderModule", PFN_vkDestroyShaderModule);

    vk.vkCreateGraphicsPipelines =
        JNK_DVC_PROC(dvc, "vkCreateGraphicsPipelines",
                     PFN_vkCreateGraphicsPipelines);

    vk.vkDestroyPipeline =
        JNK_DVC_PROC(dvc, "vkDestroyPipeline", PFN_vkDestroyPipeline);

    // ================= RENDERPASS MANAGEMENT ====================
    vk.vkCreateRenderPass =
        JNK_DVC_PROC(dvc, "vkCreateRenderPass", PFN_vkCreateRenderPass);

    vk.vkDestroyRenderPass =
        JNK_DVC_PROC(dvc, "vkDestroyRenderPass", PFN_vkDestroyRenderPass);

    vk.vkCreateFramebuffer =
        JNK_DVC_PROC(dvc, "vkCreateFramebuffer", PFN_vkCreateFramebuffer);

    vk.vkDestroyFramebuffer =
        JNK_DVC_PROC(dvc, "vkDestroyFramebuffer", PFN_vkDestroyFramebuffer);

    // ==================== SYNCHRONIZATION ====================
    vk.vkCreateSemaphore =
        JNK_DVC_PROC(dvc, "vkCreateSemaphore", PFN_vkCreateSemaphore);

    vk.vkDestroySemaphore =
        JNK_DVC_PROC(dvc, "vkDestroySemaphore", PFN_vkDestroySemaphore);

    vk.vkCreateFence = JNK_DVC_PROC(dvc, "vkCreateFence", PFN_vkCreateFence);

    vk.vkWaitForFences =
        JNK_DVC_PROC(dvc, "vkWaitForFences", PFN_vkWaitForFences);

    vk.vkResetFences = JNK_DVC_PROC(dvc, "vkResetFences", PFN_vkResetFences);

    vk.vkDestroyFence = JNK_DVC_PROC(dvc, "vkDestroyFence", PFN_vkDestroyFence);

    // ================= SWAPCHAIN MANAGEMENT ====================
    vk.vkCreateSwapchainKHR =
        JNK_DVC_PROC(dvc, "vkCreateSwapchainKHR", PFN_vkCreateSwapchainKHR);

    vk.vkDestroySwapchainKHR =
        JNK_DVC_PROC(dvc, "vkDestroySwapchainKHR", PFN_vkDestroySwapchainKHR);

    vk.vkGetSwapchainImagesKHR = JNK_DVC_PROC(dvc, "vkGetSwapchainImagesKHR",
                                              PFN_vkGetSwapchainImagesKHR);

    vk.vkAcquireNextImageKHR =
        JNK_DVC_PROC(dvc, "vkAcquireNextImageKHR", PFN_vkAcquireNextImageKHR);

// ==================== DEBUG UTILITIES ====================
#ifdef _XVXDEBUG
    vk.vkSetDebugUtilsObjectNameEXT =
        JNK_DVC_PROC(dvc, "vkSetDebugUtilsObjectNameEXT",
                     PFN_vkSetDebugUtilsObjectNameEXT);

    vk.vkSetDebugUtilsObjectTagEXT =
        JNK_DVC_PROC(dvc, "vkSetDebugUtilsObjectTagEXT",
                     PFN_vkSetDebugUtilsObjectTagEXT);
#endif
    return (vk.vkDeviceWaitIdle != VK_NULL_HANDLE);
}

void jnk_vk_kill(void) {
    if (s_vk_lib) {
        dlclose(s_vk_lib);
        s_vk_lib = JNK_NULL;
    }
}
