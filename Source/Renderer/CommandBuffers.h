// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Sherbert
{
    /**
     * Manages Vulkan command pools, framebuffers, and command buffers.
     * Command buffers are Vulkan's way of recording GPU commands.
     * Unlike immediate-mode APIs, Vulkan does not execute draw calls instantly.
     * Instead:
     * 1. Record commands into command buffers
     * 2. Submit command buffers to a queue
     * 3. GPU executes the recorded work
     *
     * This class creates one command buffer per swapchain image.
     */
    class CommandBuffers
    {

    public:

        void Initialize
        (
            VkDevice device,
            VkRenderPass renderPass,
            VkExtent2D extent,
            VkPipeline pipeline,
            VkSwapchainKHR swapchain,
            const std::vector<VkImage>& swapchainImages,
            const std::vector<VkImageView>& swapchainImageViews
        );

        const std::vector<VkCommandBuffer>& Get() const { return commandBuffers; }

        /*
         * A framebuffer connects the render pass to actual image resources.
         * RenderPass: "I need a colour attachment"
         * Framebuffer: "Here is the specific swapchain image to use"
         * Each swapchain image gets its own framebuffer.
         */
        const std::vector<VkFramebuffer>& GetFramebuffers() const { return framebuffers; }
        VkCommandPool GetCommandPool() const { return commandPool; }
        void Cleanup(VkDevice device);

    private:

        /*
         * A command pool manages memory used for allocating command buffers.
         * Command buffers must come from a pool associated with the queue family that will execute them.
         */
        VkCommandPool commandPool{};
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> framebuffers;
    };
}
