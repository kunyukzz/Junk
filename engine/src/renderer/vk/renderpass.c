#include "renderpass.h"

b8 vk_renderpass_init(vk_core_t *core, vk_renderpass_t *rpass,
                      vk_swapchain_t *swap, f32 depth, u32 stencil,
                      u8 clear_flag, b8 prev_pass, b8 next_pass,
                      VkClearColorValue clear_color) {
    rpass->clear_flag = clear_flag;
    rpass->depth = depth;
    rpass->stencil = stencil;
    rpass->prev_pass = prev_pass;
    rpass->next_pass = next_pass;
    rpass->clear_color = clear_color;

    VkAttachmentDescription attachments[2];
    u32 attach_count = 0;

    /*** Color Attachment ***/
    b8 do_clear_color = (rpass->clear_flag & COLOR_BUFFER) != 0;
    VkAttachmentDescription color = {};
    color.format = swap->image_format;
    color.samples = VK_SAMPLE_COUNT_1_BIT;
    color.loadOp = do_clear_color ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                  : VK_ATTACHMENT_LOAD_OP_LOAD;
    color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color.initialLayout = prev_pass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                                    : VK_IMAGE_LAYOUT_UNDEFINED;
    color.finalLayout = next_pass ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                                  : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[attach_count++] = color;

    VkAttachmentReference color_ref = {};
    color_ref.attachment = 0;
    color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    /*** Depth Attachment ***/
    VkAttachmentReference depth_ref = {};
    b8 do_clear_depth = (rpass->clear_flag & DEPTH_BUFFER) != 0;
    if (do_clear_depth) {
        VkAttachmentDescription depth = {};
        depth.format = core->default_depth_format;
        depth.samples = VK_SAMPLE_COUNT_1_BIT;
        depth.loadOp = do_clear_depth ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
        depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[attach_count++] = depth;

        depth_ref.attachment = 1;
        depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    /*** Subpass ***/
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_ref;
    subpass.pDepthStencilAttachment = do_clear_depth ? &depth_ref : JNK_NULL;

    VkSubpassDependency depend[2];
    u32 depend_count = 1;

    depend[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    depend[0].dstSubpass = 0;
    depend[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depend[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depend[0].srcAccessMask = 0;
    depend[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    depend[0].dependencyFlags = 0;

    if (next_pass) {
        depend[depend_count].srcSubpass = 0;
        depend[depend_count].dstSubpass = VK_SUBPASS_EXTERNAL;
        depend[depend_count].srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        depend[depend_count].srcAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        depend[depend_count].dstStageMask =
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        depend[depend_count].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        depend_count++;
    }

    /*** Renderpass ***/
    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.attachmentCount = attach_count;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = depend_count;
    rp_info.pDependencies = depend;

    VK_CHK(jnk.vkCreateRenderPass(core->logic_dvc, &rp_info, core->alloc,
                                  &rpass->handle));
    return true;
}

void vk_renderpass_kill(vk_core_t *core, vk_renderpass_t *rpass) {
    jnk.vkDestroyRenderPass(core->logic_dvc, rpass->handle, core->alloc);
}

void vk_renderpass_begin(vk_renderpass_t *rpass, VkCommandBuffer cmd,
                         VkFramebuffer framebuffer, VkExtent2D extent) {
    VkClearValue clvl[2];
    u32 clear_count = 0;

    if (rpass->clear_flag & COLOR_BUFFER) {
        clvl[clear_count].color = rpass->clear_color;
        clear_count++;
    }
    if ((rpass->clear_flag & DEPTH_BUFFER) ||
        (rpass->clear_flag & STENCIL_BUFFER)) {
        clvl[clear_count].depthStencil =
            (VkClearDepthStencilValue){.depth = rpass->depth,
                                       .stencil = rpass->stencil};
        clear_count++;
    }

    VkRenderPassBeginInfo rp_begin = {};
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.renderPass = rpass->handle;
    rp_begin.framebuffer = framebuffer;
    rp_begin.renderArea.offset = (VkOffset2D){0, 0};
    rp_begin.renderArea.extent = extent;
    rp_begin.clearValueCount = clear_count;
    rp_begin.pClearValues = clvl;

    jnk.vkCmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
}

void vk_renderpass_end(vk_renderpass_t *rpass, VkCommandBuffer cmd) {
    (void)rpass;
    jnk.vkCmdEndRenderPass(cmd);
}
