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
    /**
     * Main Vulkan rendering interface.
     *
     * Responsible for setting up the Vulkan rendering pipeline and submitting
     * frames to the GPU.
     *
     * The initialization flow follows the typical Vulkan rendering setup:
     *
     *  1. Create Vulkan instance
     *  2. Create a window surface
     *  3. Select physical device and create logical device
     *  4. Create swapchain
     *  5. Create render pass
     *  6. Create graphics pipeline
     *  7. Create command buffers
     *
     * Once initialized, DrawFrame() handles the per-frame process:
     *
     *  - Acquire an image from the swapchain
     *  - Submit rendering commands to the GPU
     *  - Present the completed image to the screen
     */
    class Renderer
    {

    public:

        
        /**
         * Creates all Vulkan resources required for rendering.
         *
         * The Window is used to create a Vulkan surface, which represents
         * the connection between Vulkan and the operating system window.
         */
        void Initialize(const Window& window);

        /**
         * Renders a single frame.
         *
         * Vulkan does not automatically render frames like higher-level APIs.
         * The application must explicitly acquire a swapchain image, submit
         * command buffers, and present the result.
         */
        void DrawFrame();

        /**
         * Waits for the GPU to finish all queued work before shutting down.
         *
         * Vulkan is asynchronous. The CPU may continue running while the GPU
         * is executing commands, so we must wait before destroying resources.
         */
        void WaitIdle();

    private:

        /*
         * Vulkan Instance
         *
         * The entry point into Vulkan. This represents our connection to the
         * Vulkan API and is used to create most other Vulkan objects.
         */
        VkInstance instance{};

        /*
         * Vulkan Surface
         *
         * A platform-specific connection between Vulkan and the window system.
         * On Windows this is created using VK_KHR_win32_surface.
         *
         * The surface is used by the swapchain to display images on screen.
         */
        VkSurfaceKHR surface{};

        /*
         * Swapchain images are the images that Vulkan will render into before
         * presenting them to the screen.
         *
         * The GPU owns these images. We do not create them manually; they are
         * provided by the presentation system when creating the swapchain.
         */
        std::vector<VkImage> swapchainImages;

        /*
         * Image views describe how Vulkan should interpret an image.
         *
         * Vulkan does not directly render to VkImage objects. Pipelines and
         * render passes interact with image views instead.
         */
        std::vector<VkImageView> swapchainImageViews;

        /*
         * Swapchain manages the images displayed by the window.
         *
         * Rendering is performed into one swapchain image while another may
         * be displayed, allowing smooth presentation and synchronization.
         */
        VkSwapchainKHR swapchain{};

        /*
         * Format of the swapchain images.
         *
         * Defines how colour data is stored, for example:
         * VK_FORMAT_B8G8R8A8_SRGB
         */
        VkFormat swapchainImageFormat{};

        /*
         * Resolution of the swapchain images.
         *
         * Usually matches the window size, but can differ depending on the
         * capabilities of the graphics device and window system.
         */
        VkExtent2D swapchainExtent{};

        /*
         * Handles physical device selection, logical device creation, and
         * graphics queue management.
         *
         * This abstraction hides the boilerplate involved in selecting a GPU
         * and creating a device capable of running Vulkan commands.
         */
        GraphicsDevice graphicsDevice{};

        /*
         * Vulkan setup functions.
         *
         * These are kept separate to make the initialization order clear and
         * match the way Vulkan resources depend on each other.
         */
        void CreateInstance(const Window& window);
        void CreateSurface(const Window& window);
        void CreateSwapchain();
        void CreateSwapchainImageViews();

        /*
         * Defines how the GPU processes geometry:
         *
         * - Vertex shaders
         * - Fragment shaders
         * - Rasterization
         * - Blending
         *
         * Unlike OpenGL, Vulkan requires this state to be explicitly created.
         */
        Pipeline pipeline;

        /*
         * Describes how rendering operations use attachments such as the
         * swapchain image.
         */
        RenderPass renderPass;

        /*
         * Stores GPU commands that will be submitted to the graphics queue.
         * Vulkan uses recorded command buffers rather than immediate mode
         * rendering calls.
         */
        CommandBuffers commandBuffer;

    };
}
