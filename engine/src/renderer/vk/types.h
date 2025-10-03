#ifndef JUNK_VULKAN_TYPES_H
#define JUNK_VULKAN_TYPES_H

#include "loader.h"
#include "junk/jnk_define.h"
#include "junk/jnk_assert.h"
#include "junk/jnk_math_type.h"

#define VK_CHK(expr)                                                           \
    do {                                                                       \
        VkResult result = expr;                                                \
        jnk_assert(result == VK_SUCCESS);                                      \
    } while (0)

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define MAX_FRAME_IN_FLIGHT 3

JNK_EXTERN_C_BEGIN

// =============== IMAGES & MEMORY
// ===============
typedef struct {
    VkImage handle;
    VkImageView view;
    VkFormat format;

    VkDeviceMemory memory;
    VkDeviceSize size;

    u32 width;
    u32 height;
} vk_image_t;

// =============== SWAPCHAIN
// ===============
typedef struct vk_swapchain_t {
    VkImage *images;
    VkImageView *img_views;
    VkFramebuffer *framebuffer;

    VkSurfaceKHR surface;
    VkSwapchainKHR handle;

    VkSurfaceCapabilitiesKHR caps;
    VkSurfaceFormatKHR surface_format;

    VkExtent2D extents;

    VkPresentModeKHR present_mode;
    VkFormat image_format;
    u32 image_count;

    vk_image_t color_attach;
    vk_image_t depth_attach;
} vk_swapchain_t;

// =============== RENDERPASS
// ===============
typedef struct {
    VkRenderPass handle;
    f32 depth;
    u32 stencil;
    u8 clear_flag;
    b8 prev_pass;
    b8 next_pass;
    VkClearColorValue clear_color;
} vk_renderpass_t;

// =============== COMMAND BUFFER & BUFFER
// ===============
typedef struct {
    VkCommandBuffer handle;
} vk_cmdbuffer_t;

typedef struct {
    VkBuffer handle;
    VkDeviceSize size;
    VkDeviceMemory memory;
    b8 is_locked;
    void *mapped;
} vk_buffer_t;

// =============== PIPELINE - DESCRIPTOR - SHADER
// ===============
typedef struct {
    VkShaderModule vert;
    VkShaderModule frag;
    const char *entry_point;
} vk_shader_t;

typedef struct {
    mat4 view;
    mat4 proj;
    mat4 _reserved00; // padding for some graphics card
    mat4 _reserved01; // padding for some graphics card
} vk_camera_data_t;

typedef struct {
    vk_camera_data_t global;
    vk_camera_data_t object;
} vk_camera_ubo_t;

typedef struct {
    const VkPipelineShaderStageCreateInfo *stages;
    u32 stage_count;

    VkDescriptorSetLayout *desc_layouts;
    u32 desc_layout_count;

    VkPushConstantRange *push_consts;
    u32 push_constant_count;

    VkVertexInputAttributeDescription *attrs;
    u32 attribute_count;
    u32 vertex_stride;
} vk_pipeline_desc_t;

typedef struct {
    VkPipeline handle;
    VkPipelineLayout layout;
} vk_pipeline_t;

typedef struct {
    vk_camera_ubo_t cam_ubo;
    vk_buffer_t buffers[MAX_FRAME_IN_FLIGHT];
    vk_shader_t shaders;
    vk_pipeline_t pipelines;

    VkDescriptorSet desc_sets[MAX_FRAME_IN_FLIGHT];
    VkDescriptorPool desc_pool;
    VkDescriptorSetLayout desc_layout;
} vk_material_t;

// =============== CORE
// ===============
typedef enum {
    RDR_TAG_UNKNOWN = 0x00,
    RDR_TAG_TEXTURE,        // Base color, normal, roughness/metallic maps
    RDR_TAG_TEXTURE_HDR,    // HDR cubemaps, IBL textures
    RDR_TAG_TEXTURE_UI,     // UI atlas, fonts
    RDR_TAG_BUFFER_VERTEX,  // Vertex buffers for meshes
    RDR_TAG_BUFFER_INDEX,   // Index buffers
    RDR_TAG_BUFFER_UNIFORM, // Uniform buffers (per-frame, per-object)
    RDR_TAG_BUFFER_STAGING, // Staging buffers for uploads
    RDR_TAG_BUFFER_COMPUTE, // SSBOs for compute shaders
    RDR_TAG_RENDER_TARGET,  // Color attachments, G-Buffers
    RDR_TAG_DEPTH_TARGET,   // Depth/Stencil attachments
    RDR_TAG_COUNT
} vk_memtag_t;

typedef struct vk_core_t {
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

        u64 tag_alloc_count[RDR_TAG_COUNT];
        u64 tag_allocation[RDR_TAG_COUNT];
    } memories;

    VkQueue graphic_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkFormat default_depth_format;

    u32 graphic_idx;
    u32 present_idx;
    u32 transfer_idx;
} vk_core_t;

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_TYPES_H
