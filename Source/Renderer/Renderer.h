// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Application/Window.h"
#include "GraphicsDevice.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "CommandBuffers.h"

namespace Sherbert
{
    class Renderer
    {

    public:

        void Initialize(const Window& window);
        void DrawFrame();
        void WaitIdle();

    private:

        VkInstance instance{};
        VkSurfaceKHR surface{};

        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkSwapchainKHR swapchain{};
        VkFormat swapchainImageFormat{};
        VkExtent2D swapchainExtent{};

        GraphicsDevice graphicsDevice{};
        void CreateInstance(const Window& window);
        void CreateSurface(const Window& window);
        void CreateSwapchain();
        void CreateSwapchainImageViews();

        Pipeline pipeline;
        RenderPass renderPass;
        CommandBuffers commandBuffer;

    };
}
