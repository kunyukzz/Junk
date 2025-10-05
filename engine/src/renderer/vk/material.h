#ifndef JUNK_VULKAN_MATERIAL_H
#define JUNK_VULKAN_MATERIAL_H

#include "junk/jnk_define.h"
#include "types.h"

JNK_EXTERN_C_BEGIN

b8 vk_material_world_init(vk_core_t *core, vk_material_t *material,
                          vk_renderpass_t *rpass, const char *shader_name);

b8 vk_material_overlay_init(vk_core_t *core, vk_material_t *material,
                            vk_renderpass_t *rpass, const char *shader_name);

void vk_material_kill(vk_core_t *core, vk_material_t *material);

JNK_EXTERN_C_END
#endif // JUNK_VULKAN_MATERIAL_H
