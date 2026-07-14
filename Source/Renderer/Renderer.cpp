// Copyright (c) 2026 JDSherbert. All rights reserved.

#include "Renderer.h"

#include <iostream>
#include <stdexcept>
#include <windows.h>
#include <vulkan/vulkan_win32.h>


void Sherbert::Renderer::Initialize(const Window& window)
{
    /*
     * Vulkan initialization must happen in a specific order because objects
     * depend on objects created before them.
     *
     * For example:
     * - A swapchain requires a surface
     * - A render pass requires a device
     * - A graphics pipeline requires a render pass
     * - Command buffers require the pipeline and render pass
     */
    CreateInstance(window);
    CreateSurface(window);

    // Initialize graphics device (select physical device, create logical device & queues)
    graphicsDevice.Initialize(instance, window);

    /*
     * The swapchain is Vulkan's connection between rendering and the display.
     * Vulkan does not render directly to the screen. Instead, it renders into
     * one of several images owned by the swapchain, then presents that image.
     */
    CreateSwapchain();
    CreateSwapchainImageViews();

    /*
     * The render pass describes what rendering operations will happen and
     * what attachments (such as the screen colour buffer) are used.
     */
    renderPass.Initialize(graphicsDevice.GetDevice(), swapchainImageFormat);

    /*
     * The graphics pipeline contains the GPU state required for drawing:
     * - Vertex shader
     * - Fragment shader
     * - Rasterization state
     * - Colour blending
     *
     * Unlike OpenGL, Vulkan requires this state to be created ahead of time.
     */
    pipeline.Initialize(graphicsDevice.GetDevice(), swapchainExtent, renderPass.Get());

    /*
     * Command buffers contain the actual GPU commands.
     * Vulkan uses a record-then-submit model:
     * 1. Record commands into command buffers
     * 2. Submit them to a GPU queue
     * 3. GPU executes them asynchronously
     */
    commandBuffer.Initialize(
        graphicsDevice.GetDevice(),
        renderPass.Get(),
        swapchainExtent,
        pipeline.Get(),
        swapchain,
        swapchainImages,
        swapchainImageViews
    );

}

void Sherbert::Renderer::DrawFrame()
{
/*
 * A single Vulkan frame follows this pattern:
 *
 * 1. Ask the swapchain for an available image
 * 2. Submit commands that render into that image
 * 3. Present the finished image to the window
 *
 * Unlike APIs such as OpenGL, Vulkan requires the application to manage
 * this entire process explicitly.
 */
    VkDevice device = graphicsDevice.GetDevice();
    VkQueue graphicsQueue = graphicsDevice.GetGraphicsQueue();
    VkSwapchainKHR swapchainHandle = swapchain;

    /*
     * Acquire an image from the swapchain.
     * The swapchain usually contains multiple images (double/triple buffering).
     * Vulkan gives us the index of an image that is safe to render into.
     */
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapchainHandle, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer.Get()[imageIndex];

    /*
     * Submit our recorded command buffer to the graphics queue.
     * The queue is owned by the GPU. Submitting work does not mean the GPU has
     * finished; it means the GPU has been given commands to execute.
     */
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchainHandle;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
}

void Sherbert::Renderer::WaitIdle()
{
    vkDeviceWaitIdle(graphicsDevice.GetDevice());

    // Cleanup in reverse order of creation
    commandBuffer.Cleanup(graphicsDevice.GetDevice());
    pipeline.Cleanup(graphicsDevice.GetDevice());
    renderPass.Cleanup(graphicsDevice.GetDevice());
    //swapchain.Cleanup();
    //graphicsDevice.Cleanup();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Sherbert::Renderer::CreateInstance(const Window& window)
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Sherbert Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "SherbertEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    /*
     * Vulkan is platform agnostic.
     * The core API does not know how to create a window surface. Platform-specific
     * extensions provide this functionality.
     *
     * On Windows we require:
     * VK_KHR_surface       - common surface functionality
     * VK_KHR_win32_surface - Windows HWND integration
     */
    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 2;
    createInfo.ppEnabledExtensionNames = extensions;

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan instance!");
}

void Sherbert::Renderer::CreateSurface(const Window& window)
{
    VkWin32SurfaceCreateInfoKHR surfaceInfo{};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = GetModuleHandle(nullptr);
    surfaceInfo.hwnd = static_cast<HWND>(window.handle);

    if(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Win32 surface!");
}

void Sherbert::Renderer::CreateSwapchain()
{
    /*
     * Before creating a swapchain, we query the GPU to find out what the surface supports:
     * - Available image formats
     * - Number of images allowed
     * - Supported resolutions
     * - Presentation modes
     *
     * Here we choose a simple configuration suitable for a basic renderer.
     */
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphicsDevice.GetPhysicalDevice(), surface, &capabilities);

    VkSurfaceFormatKHR surfaceFormat{};
    surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
    surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    /*
     * FIFO mode is Vulkan's equivalent of traditional VSync.
     * Images are presented in order and the application waits for the display
     * refresh interval, preventing screen tearing.
     */
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkExtent2D extent = capabilities.currentExtent;
    if (extent.width == UINT32_MAX) {
        extent.width = 800;
        extent.height = 600;
    }

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = extent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(graphicsDevice.GetDevice(), &swapchainInfo, nullptr, &swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain!");

    // Retrieve swapchain images
    uint32_t swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(graphicsDevice.GetDevice(), swapchain, &swapchainImageCount, nullptr);
    swapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(graphicsDevice.GetDevice(), swapchain, &swapchainImageCount, swapchainImages.data());

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;

    std::cout << "Swapchain created with " << swapchainImages.size() << " images\n";
}

void Sherbert::Renderer::CreateSwapchainImageViews()
{
    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapchainImageFormat;

        /*
         * Component swizzling controls how image channels are mapped when accessed
         * through this image view. For example, a view could remap:
         *   Red   -> Blue
         *   Green -> Green
         *   Blue  -> Red
         *
         * This is useful for things like interpreting different texture formats,
         * grayscale images, or special rendering techniques.
         *
         * VK_COMPONENT_SWIZZLE_IDENTITY means:
         * Use the channels exactly as stored:
         *   R -> R
         *   G -> G
         *   B -> B
         *   A -> A
         */
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        /*
         * Defines which part of the image this view accesses.
         * Vulkan images can contain:
         * - Multiple mip levels (different resolutions of the same texture)
         * - Multiple array layers (texture arrays, cubemaps, etc.)
         *
         * The swapchain images are simple 2D colour images, so we use:
         * - One mip level
         * - One array layer
         * - Colour aspect only
         */
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(
                graphicsDevice.GetDevice(),
                &viewInfo,
                nullptr,
                &swapchainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swapchain image view!");
        }
    }
}
