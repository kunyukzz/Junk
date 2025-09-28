#include "swapchain.h"
#include "images.h"

#include "junk/jnk_log.h"
#include "junk/jnk_memory.h"

static b8 create_surface(vk_swapchain_t *swp, vk_core_t *core,
                         jnk_window_t *window) {
    if (swp->surface != VK_NULL_HANDLE) return true;

#if JNK_LINUX
    VkXlibSurfaceCreateInfoKHR surf = {};
    surf.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surf.dpy = (Display *)window->dpy;
    surf.window = (Window)window->win;

    VK_CHK(jnk.vkCreateXlibSurfaceKHR(core->instance, &surf, core->alloc,
                                      &swp->surface));

#elif JNK_WINDOWS
    VkWin32SurfaceCreateInfoKHR surf = {};
    surf.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surf.hinstance = GetModuleHandle(JNK_NULL);
    surf.hwnd = (HWND)window->hwnd;

    VK_CHK(jnk.vkCreateWin32SurfaceKHR(core->instance, &surf, core->alloc,
                                       &swp->surface));
#else
#    error "Other platform not supported"
#endif
    jnk_log_debug(CH_GFX, "vulkan surface created");
    return true;
}

static b8 pick_present_queue(vk_core_t *core, VkSurfaceKHR surface) {
    u32 q_count = 0;
    jnk.vkGetPhysicalDeviceQueueFamilyProperties(core->gpu, &q_count, JNK_NULL);
    VkQueueFamilyProperties *families =
        JMALLOC(sizeof(VkQueueFamilyProperties) * q_count, MEM_RENDER);
    jnk.vkGetPhysicalDeviceQueueFamilyProperties(core->gpu, &q_count, families);

    for (u32 i = 0; i < q_count; ++i) {
        VkBool32 present_supported = VK_FALSE;
        jnk.vkGetPhysicalDeviceSurfaceSupportKHR(core->gpu, i, surface,
                                                 &present_supported);
        if (present_supported) {
            core->present_idx = i;
            JFREE(families, sizeof(VkQueueFamilyProperties) * q_count,
                  MEM_RENDER);
            return true;
        }
    }

    JFREE(families, sizeof(VkQueueFamilyProperties) * q_count, MEM_RENDER);
    jnk_log_fatal(CH_GFX, "No present queue found");
    return false;
}

static b8 query_surface_details(vk_swapchain_t *swp, vk_core_t *core) {
    jnk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(core->gpu, swp->surface,
                                                  &swp->caps);

    u32 format_count = 0;
    jnk.vkGetPhysicalDeviceSurfaceFormatsKHR(core->gpu, swp->surface,
                                             &format_count, JNK_NULL);
    VkSurfaceFormatKHR *formats =
        JMALLOC(sizeof(VkSurfaceFormatKHR) * format_count, MEM_RENDER);
    jnk.vkGetPhysicalDeviceSurfaceFormatsKHR(core->gpu, swp->surface,
                                             &format_count, formats);
    swp->surface_format = formats[0];
    JFREE(formats, sizeof(VkSurfaceFormatKHR) * format_count, MEM_RENDER);

    u32 mode_count = 0;
    jnk.vkGetPhysicalDeviceSurfacePresentModesKHR(core->gpu, swp->surface,
                                                  &mode_count, JNK_NULL);
    VkPresentModeKHR *modes =
        JMALLOC(sizeof(VkPresentModeKHR) * mode_count, MEM_RENDER);
    jnk.vkGetPhysicalDeviceSurfacePresentModesKHR(core->gpu, swp->surface,
                                                  &mode_count, modes);
    swp->present_mode = modes[0];
    JFREE(modes, sizeof(VkPresentModeKHR) * mode_count, MEM_RENDER);

    return true;
}

static void choose_surface_format(vk_swapchain_t *swp) {
    // prefer SRGB if available
    if (swp->surface_format.format == VK_FORMAT_UNDEFINED) {
        swp->surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
        swp->surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
}

static void choose_present_mode(vk_swapchain_t *swp) {
    // prefer MAILBOX > FIFO
    if (swp->present_mode != VK_PRESENT_MODE_MAILBOX_KHR) {
        swp->present_mode = VK_PRESENT_MODE_FIFO_KHR;
    }

    // prefer FIFO > MAILBOX
    // this for keep resource minimal
    // swp->present_mode = VK_PRESENT_MODE_FIFO_KHR;
}

static void choose_swap_extent(vk_swapchain_t *swp, jnk_window_t *window) {
    if (swp->caps.currentExtent.width != UINT32_MAX) {
        swp->extents = swp->caps.currentExtent;
    } else {
        u32 win_width = window->width;
        u32 win_height = window->height;
        swp->extents.width =
            JNK_CLAMP(win_width, swp->caps.minImageExtent.width,
                      swp->caps.maxImageExtent.width);
        swp->extents.height =
            JNK_CLAMP(win_height, swp->caps.minImageExtent.height,
                      swp->caps.maxImageExtent.height);
    }
}

static b8 create_swapchain(vk_swapchain_t *swp, vk_core_t *core,
                           VkSwapchainKHR old_handle) {
    u32 image_count = swp->caps.minImageCount + 1;
    if (swp->caps.maxImageCount > 0 && image_count > swp->caps.maxImageCount) {
        image_count = swp->caps.maxImageCount;
    }
    jnk_log_trace(CH_GFX, "image count from swapchain: %d", image_count);

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = swp->surface;
    info.minImageCount = image_count;
    info.imageFormat = swp->surface_format.format;
    info.imageColorSpace = swp->surface_format.colorSpace;
    info.imageExtent = swp->extents;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    u32 queue_indices[] = {core->graphic_idx, core->present_idx};
    if (core->graphic_idx != core->present_idx) {
        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = queue_indices;
    } else {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    info.preTransform = swp->caps.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = swp->present_mode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = old_handle;

    VK_CHK(jnk.vkCreateSwapchainKHR(core->logic_dvc, &info, core->alloc,
                                    &swp->handle));

    swp->image_format = swp->surface_format.format;
    return true;
}

static b8 create_image_views(vk_swapchain_t *swp, vk_core_t *core) {
    jnk.vkGetSwapchainImagesKHR(core->logic_dvc, swp->handle, &swp->image_count,
                                JNK_NULL);
    swp->images = JMALLOC(sizeof(VkImage) * swp->image_count, MEM_RENDER);

    jnk.vkGetSwapchainImagesKHR(core->logic_dvc, swp->handle, &swp->image_count,
                                swp->images);
    swp->img_views =
        JMALLOC(sizeof(VkImageView) * swp->image_count, MEM_RENDER);
    for (u32 i = 0; i < swp->image_count; ++i) {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = swp->images[i];
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = swp->image_format;
        info.components =
            (VkComponentMapping){.a = VK_COMPONENT_SWIZZLE_IDENTITY,
                                 .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                 .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                 .r = VK_COMPONENT_SWIZZLE_IDENTITY};

        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;

        VK_CHK(jnk.vkCreateImageView(core->logic_dvc, &info, core->alloc,
                                     &swp->img_views[i]));
    }
    return true;
}

static void destroy_image_views(vk_swapchain_t *swp, vk_core_t *core) {
    for (u32 i = 0; i < swp->image_count; ++i) {
        jnk.vkDestroyImageView(core->logic_dvc, swp->img_views[i], core->alloc);
    }
    JFREE(swp->img_views, sizeof(VkImageView) * swp->image_count, MEM_RENDER);
    JFREE(swp->images, sizeof(VkImage) * swp->image_count, MEM_RENDER);
}

b8 vk_swapchain_init(vk_swapchain_t *swp, vk_core_t *core, jnk_window_t *window,
                     VkSwapchainKHR old_handle) {
    if (!create_surface(swp, core, window)) return false;
    if (!pick_present_queue(core, swp->surface)) return false;
    if (!query_surface_details(swp, core)) return false;
    choose_surface_format(swp);
    choose_present_mode(swp);
    choose_swap_extent(swp, window);
    if (!create_swapchain(swp, core, old_handle)) return false;

    // image swapchain
    if (!create_image_views(swp, core)) return false;

    // color attachment
    if (!vk_image_init(&swp->color_attach, core, swp->image_format,
                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                           VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                       VK_IMAGE_ASPECT_COLOR_BIT, swp->extents.width,
                       swp->extents.height, true, RDR_TAG_RENDER_TARGET)) {
        jnk_log_error(CH_GFX, "failed to create color attachment swapchain");
        return false;
    }

    // depth attachment
    if (!vk_image_init(&swp->depth_attach, core, core->default_depth_format,
                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                       VK_IMAGE_ASPECT_DEPTH_BIT, swp->extents.width,
                       swp->extents.height, true, RDR_TAG_DEPTH_TARGET)) {
        jnk_log_error(CH_GFX, "failed to create depth attachment swapchain");
        return false;
    }

    if (old_handle && old_handle != VK_NULL_HANDLE) {
        jnk_log_debug(CH_GFX, "vulkan swapchain initialize with old handle: %p",
                      (void *)old_handle);
    } else {
        jnk_log_debug(CH_GFX, "vulkan swapchain initialize first-time");
    }

    return true;
}

void vk_swapchain_kill(vk_swapchain_t *swp, vk_core_t *core) {
    vk_image_kill(&swp->depth_attach, core, RDR_TAG_DEPTH_TARGET);
    vk_image_kill(&swp->color_attach, core, RDR_TAG_RENDER_TARGET);

    destroy_image_views(swp, core);

    if (swp->handle != VK_NULL_HANDLE) {
        jnk.vkDestroySwapchainKHR(core->logic_dvc, swp->handle, core->alloc);
        swp->handle = VK_NULL_HANDLE;
        jnk_log_debug(CH_GFX, "vulkan swapchain kill");
    }

    if (swp->surface != VK_NULL_HANDLE) {
        jnk.vkDestroySurfaceKHR(core->instance, swp->surface, core->alloc);
        swp->surface = VK_NULL_HANDLE;
        jnk_log_debug(CH_GFX, "vulkan surface kill");
    }
}

b8 vk_swapchain_reinit(vk_swapchain_t *swp, vk_core_t *core,
                       jnk_window_t *window) {
    VkSwapchainKHR old_swapchain = swp->handle;

    if (old_swapchain != VK_NULL_HANDLE) {
        vk_image_kill(&swp->depth_attach, core, RDR_TAG_DEPTH_TARGET);
        vk_image_kill(&swp->color_attach, core, RDR_TAG_RENDER_TARGET);
        destroy_image_views(swp, core);
    }

    if (!vk_swapchain_init(swp, core, window, old_swapchain)) return false;

    if (old_swapchain != VK_NULL_HANDLE) {
        jnk.vkDestroySwapchainKHR(core->logic_dvc, old_swapchain, core->alloc);
    }

    return true;
}
