#include "pipeline.h"
#include "junk/jnk_log.h"

b8 vk_pipeline_init(vk_core_t *core, vk_pipeline_t *pipeline,
                    vk_renderpass_t *rpass, const vk_pipeline_desc_t *desc,
                    pipe_config_t config) {
    /*** viewport state ***/
    VkPipelineViewportStateCreateInfo viewport_info = {};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.scissorCount = 1;

    /*** multisample state ***/
    VkPipelineMultisampleStateCreateInfo multi_sample_info = {};
    multi_sample_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multi_sample_info.sampleShadingEnable = VK_FALSE;
    multi_sample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multi_sample_info.minSampleShading = 1.0f;

    /*** raster state ***/
    VkPipelineRasterizationStateCreateInfo raster_info = {};
    raster_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster_info.polygonMode =
        config.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    raster_info.cullMode = config.cull_mode;
    raster_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    raster_info.lineWidth = 1.0f;

    /*** depth stencil state ***/
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = config.depth_test;
    depth_stencil.depthWriteEnable = config.depth_write;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;

    /*** color blend state ***/
    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,

        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,

        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
    };

    VkPipelineColorBlendStateCreateInfo color_blend_info = {};
    color_blend_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.logicOpEnable = VK_FALSE;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;
    color_blend_info.logicOp = VK_LOGIC_OP_COPY;

    /*** vertex input state ***/
    VkVertexInputBindingDescription bind_desc;
    bind_desc.binding = 0;
    bind_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bind_desc.stride = desc->vertex_stride;

    /* Attributes */
    VkPipelineVertexInputStateCreateInfo vert_info = {};
    vert_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vert_info.vertexBindingDescriptionCount = 1;
    vert_info.pVertexBindingDescriptions = &bind_desc;
    vert_info.vertexAttributeDescriptionCount = desc->attribute_count;
    vert_info.pVertexAttributeDescriptions = desc->attrs;

    /*** input assembly state ***/
    VkPipelineInputAssemblyStateCreateInfo input_asm = {};
    input_asm.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_asm.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_asm.primitiveRestartEnable = VK_FALSE;

    /*** push constant ***/
    VkPushConstantRange push_const[desc->push_constant_count];
    for (u32 i = 0; i < desc->push_constant_count; ++i) {
        push_const[i].stageFlags = desc->push_consts[i].stageFlags;
        push_const[i].offset = desc->push_consts[i].offset;
        push_const[i].size = desc->push_consts[i].size;
    }

    /*** pipeline layout ***/
    VkPipelineLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = desc->desc_layout_count;
    layout_info.pSetLayouts =
        desc->desc_layout_count > 0 ? desc->desc_layouts : JNK_NULL;
    layout_info.pushConstantRangeCount = desc->push_constant_count;
    layout_info.pushConstantRangeCount = desc->push_constant_count;
    layout_info.pPushConstantRanges =
        desc->push_constant_count > 0 ? push_const : JNK_NULL;

    VK_CHK(jnk.vkCreatePipelineLayout(core->logic_dvc, &layout_info,
                                      core->alloc, &pipeline->layout));

    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                       VK_DYNAMIC_STATE_SCISSOR,
                                       VK_DYNAMIC_STATE_LINE_WIDTH};

    VkPipelineDynamicStateCreateInfo dyn_info = {};
    dyn_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn_info.dynamicStateCount = 3;
    dyn_info.pDynamicStates = dynamic_states;

    /*** graphic pipeline ***/
    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.layout = pipeline->layout;
    pipeline_info.renderPass = rpass->handle;
    pipeline_info.stageCount = desc->stage_count;
    pipeline_info.pStages = desc->stages;
    pipeline_info.pVertexInputState = &vert_info;
    pipeline_info.pInputAssemblyState = &input_asm;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &raster_info;
    pipeline_info.pMultisampleState = &multi_sample_info;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pColorBlendState = &color_blend_info;
    pipeline_info.pDynamicState = &dyn_info;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.subpass = 0;

    VK_CHK(jnk.vkCreateGraphicsPipelines(core->logic_dvc, VK_NULL_HANDLE, 1,
                                         &pipeline_info, core->alloc,
                                         &pipeline->handle));

    jnk_log_debug(CH_GFX, "vulkan pipeline initialize");
    return true;
}

void vk_pipeline_bind(vk_pipeline_t *pipeline, VkCommandBuffer cmdbuffer,
                      VkPipelineBindPoint bind_point) {
    jnk.vkCmdBindPipeline(cmdbuffer, bind_point, pipeline->handle);
}

void vk_pipeline_kill(vk_core_t *core, vk_pipeline_t *pipeline) {
    if (pipeline) {
        if (pipeline->handle) {
            jnk.vkDestroyPipeline(core->logic_dvc, pipeline->handle,
                                  core->alloc);
            pipeline->handle = VK_NULL_HANDLE;
        }

        if (pipeline->layout) {
            jnk.vkDestroyPipelineLayout(core->logic_dvc, pipeline->layout,
                                        core->alloc);
            pipeline->layout = 0;
        }
        jnk_log_debug(CH_GFX, "vulkan pipeline kill");
    }
}
