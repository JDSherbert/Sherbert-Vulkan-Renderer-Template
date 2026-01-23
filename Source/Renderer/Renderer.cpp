// Copyright (c) 2026 JDSherbert. All rights reserved.

#include "Renderer.h"

#include <iostream>
#include <stdexcept>
#include <windows.h>
#include <vulkan/vulkan_win32.h>


void Sherbert::Renderer::Initialize(const Window& window)
{
    CreateInstance(window);
    CreateSurface(window);

    // Initialize graphics device (select physical device, create logical device & queues)
    graphicsDevice.Initialize(instance, window);

    // --- 1. Create the swapchain ---
    CreateSwapchain();
    CreateSwapchainImageViews();

    // --- 2. Create the render pass ---
    renderPass.Initialize(graphicsDevice.GetDevice(), swapchainImageFormat);

    // --- 3. Create the graphics pipeline ---
    pipeline.Initialize(graphicsDevice.GetDevice(),
                              swapchainExtent,
                              renderPass.Get());

    // --- 4. Create framebuffers & command buffers ---
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
    VkDevice device = graphicsDevice.GetDevice();
    VkQueue graphicsQueue = graphicsDevice.GetGraphicsQueue();
    VkSwapchainKHR swapchainHandle = swapchain;

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapchainHandle, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer.Get()[imageIndex];

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

    // Enable required extensions for Win32 surface
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
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphicsDevice.GetPhysicalDevice(), surface, &capabilities);

    VkSurfaceFormatKHR surfaceFormat{};
    surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
    surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; // vsync

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

        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

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