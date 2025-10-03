#ifndef JUNK_VULKAN_PIPELINE_H
#define JUNK_VULKAN_PIPELINE_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

typedef struct {
    b8 wireframe;
    b8 depth_test;
    b8 depth_write;
    VkCullModeFlags cull_mode;
} pipe_config_t;

b8 vk_pipeline_init(vk_core_t *core, vk_pipeline_t *pipeline,
                    vk_renderpass_t *rpass, const vk_pipeline_desc_t *desc,
                    pipe_config_t config);

void vk_pipeline_bind(vk_pipeline_t *pipeline, VkCommandBuffer cmdbuffer,
                      VkPipelineBindPoint bind_point);

void vk_pipeline_kill(vk_core_t *core, vk_pipeline_t *pipeline);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_PIPELINE_H
