#include "material.h"
#include "pipeline.h"
#include "core/paths.h"
#include "resources/binary_loader.h"

#include "junk/jnk_memory.h"

#include <stdio.h>

static VkShaderModule vk_shader_create(vk_core_t *core, const void *bytecode,
                                       VkDeviceSize size) {
    VkShaderModuleCreateInfo info =
        {.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
         .codeSize = size,
         .pCode = (const u32 *)bytecode};

    VkShaderModule module;
    if (jnk.vkCreateShaderModule(core->logic_dvc, &info, core->alloc,
                                 &module) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }
    return module;
}

static void vk_shader_destroy(vk_core_t *core, VkShaderModule module) {
    jnk.vkDestroyShaderModule(core->logic_dvc, module, core->alloc);
}

static b8 set_shader(vk_core_t *core, vk_material_t *material,
                     const char *name) {
    char vert_path[XVX_MAX_PATH];
    char frag_path[XVX_MAX_PATH];
    snprintf(vert_path, sizeof(vert_path), "%s.vert.spv", name);
    snprintf(frag_path, sizeof(frag_path), "%s.frag.spv", name);

    u64 vert_size = 0, frag_size = 0;
    void *vert_code = read_file_binary(vert_path, &vert_size);
    void *frag_code = read_file_binary(frag_path, &frag_size);

    if (!vert_code || !frag_code) {
        if (vert_code) JFREE(vert_code, vert_size, MEM_RESOURCE);
        if (frag_code) JFREE(frag_code, frag_size, MEM_RESOURCE);
        return false;
    }

    // Store directly in the bundle
    material->shaders.vert = vk_shader_create(core, vert_code, vert_size);
    material->shaders.frag = vk_shader_create(core, frag_code, frag_size);
    material->shaders.entry_point = "main";

    JFREE(vert_code, vert_size, MEM_RESOURCE);
    JFREE(frag_code, frag_size, MEM_RESOURCE);

    return (material->shaders.vert != VK_NULL_HANDLE &&
            material->shaders.frag != VK_NULL_HANDLE);
}

static void unset_shader(vk_core_t *core, vk_material_t *material) {
    if (!material) return;

    if (material->shaders.vert) {
        vk_shader_destroy(core, material->shaders.vert);
        material->shaders.vert = VK_NULL_HANDLE;
    }
    if (material->shaders.frag) {
        vk_shader_destroy(core, material->shaders.frag);
        material->shaders.frag = VK_NULL_HANDLE;
    }
}

b8 vk_material_world_init(vk_core_t *core, vk_material_t *material,
                          vk_renderpass_t *rpass, const char *shader_name) {
    if (!set_shader(core, material, shader_name)) {
        return false;
    }

    VkDescriptorSetLayoutBinding ubo_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };
    VkDescriptorSetLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &ubo_binding,
    };

    VK_CHK(jnk.vkCreateDescriptorSetLayout(core->logic_dvc, &layout_info,
                                           core->alloc,
                                           &material->desc_layout));

    VkPipelineShaderStageCreateInfo stages[2] =
        {{
             .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = material->shaders.vert,
             .pName = material->shaders.entry_point,
         },
         {
             .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = material->shaders.frag,
             .pName = material->shaders.entry_point,
         }};

    VkPushConstantRange push_constants[] = {
        {.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
         .offset = 0,
         .size = sizeof(mat4)}};

    VkVertexInputAttributeDescription attrs[] =
        {{.location = 0,
          .binding = 0,
          .format = VK_FORMAT_R32G32B32_SFLOAT,
          .offset = offsetof(vertex_3d, position)},
         {.location = 1,
          .binding = 0,
          .format = VK_FORMAT_R32G32_SFLOAT,
          .offset = offsetof(vertex_2d, texcoord)}};

    vk_pipeline_desc_t world_desc = {.stages = stages,
                                     .stage_count = 2,
                                     .desc_layouts = &material->desc_layout,
                                     .desc_layout_count = 1,
                                     .push_consts = push_constants,
                                     .push_constant_count = 1,
                                     .attrs = attrs,
                                     .attribute_count = 2,
                                     .vertex_stride = sizeof(vertex_3d)};

    pipe_config_t world_config = {.wireframe = false,
                                  .depth_test = false,
                                  .depth_write = false,
                                  .cull_mode = VK_CULL_MODE_BACK_BIT};

    return vk_pipeline_init(core, &material->pipelines, rpass, &world_desc,
                            world_config);
}

void vk_material_kill(vk_core_t *core, vk_material_t *material) {
    vk_pipeline_kill(core, &material->pipelines);

    if (material->desc_layout) {
        jnk.vkDestroyDescriptorSetLayout(core->logic_dvc, material->desc_layout,
                                         core->alloc);
        material->desc_layout = VK_NULL_HANDLE;
    }

    unset_shader(core, material);
}
