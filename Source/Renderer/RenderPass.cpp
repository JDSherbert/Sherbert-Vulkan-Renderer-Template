// Copyright (c) 2026 JDSherbert. All rights reserved.

#include "RenderPass.h"

#include <stdexcept>


void Sherbert::RenderPass::Initialize(VkDevice device, VkFormat swapchainFormat)
{
    /*
     * An attachment represents a resource that the render pass will read from or write to.
     * In this renderer the only attachment is the swapchain image, which is the
     * final colour output displayed on screen.
     */
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainFormat;
    /*
     * Multisampling is disabled for this simple renderer.
     * VK_SAMPLE_COUNT_1_BIT means each pixel is stored once.
     * A value such as VK_SAMPLE_COUNT_4_BIT would enable 4x MSAA.
     */
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; 
    /*
     * What happens when rendering begins.
     * CLEAR: The image is cleared before drawing starts.
     * LOAD: preserve the previous contents
     * DONT_CARE: contents are undefined
     */
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    /*
     * What happens after rendering completes.
     * STORE means keep the rendered result. 
     * This is required because the image will later be presented to the screen.
     */
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    /*
     * Vulkan requires image layouts to be explicitly managed.
     * The image starts undefined because we do not care about its previous contents.
     * After rendering, it transitions into PRESENT_SRC_KHR so the display system
     * can present it to the window.
     */
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    /*
     * A subpass represents a stage of work inside the render pass.
     * A simple forward renderer may only need one subpass:
     *     Vertex Shader -> Rasterization -> Fragment Shader -> Colour Attachment
     *
     * More advanced renderers may use multiple subpasses for techniques such as deferred rendering.
     */
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    /*
     * The render pass description is now sent to Vulkan.
     * Vulkan uses this information when validating and optimizing the rendering
     * pipeline. The graphics pipeline created later must reference this render pass.
     */
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass!");
    }
}

void Sherbert::RenderPass::Cleanup(VkDevice device)
{
    if (renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(device, renderPass, nullptr);
        renderPass = VK_NULL_HANDLE;
    }
}
