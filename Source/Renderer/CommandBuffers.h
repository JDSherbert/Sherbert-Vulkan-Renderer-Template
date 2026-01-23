// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Sherbert
{
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
        const std::vector<VkFramebuffer>& GetFramebuffers() const { return framebuffers; }
        VkCommandPool GetCommandPool() const { return commandPool; }
        void Cleanup(VkDevice device);

    private:

        VkCommandPool commandPool{};
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> framebuffers;
    };
}