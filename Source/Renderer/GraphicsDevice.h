// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>

#include "Application/Window.h"

namespace Sherbert
{

    /*
    GraphicsDevice

    Encapsulates all Vulkan device-level functionality:
    - physicalDevice: represents the actual GPU hardware (read-only)
        Use this to query properties, supported features, memory, etc.
    - device: the logical device handle we use to submit commands
        Created from the physical device, enables queues, features, and extensions.
    - graphicsQueue / presentQueue: handles to queues where command buffers are submitted
        These come from the logical device, not the physical device.

    In short:
        physicalDevice = the GPU itself
        device         = software interface to the GPU
        queues         = where we actually send draw/compute commands

    */
    class GraphicsDevice
    {

    public:

        void Initialize(VkInstance instance, const Window& window);
        [[nodiscard]] VkDevice GetDevice() const { return device; }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] VkQueue GetGraphicsQueue() const { return graphicsQueue; }
        [[nodiscard]] VkQueue GetPresentQueue() const { return presentQueue; }
        [[nodiscard]] uint32_t GetGraphicsQueueFamily() const { return graphicsQueueFamily; }
        [[nodiscard]] uint32_t GetPresentQueueFamily() const { return presentQueueFamily; }

    private:

        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkDevice device{ VK_NULL_HANDLE };
        VkQueue graphicsQueue{ VK_NULL_HANDLE };
        VkQueue presentQueue{ VK_NULL_HANDLE };
        uint32_t graphicsQueueFamily{ UINT32_MAX };
        uint32_t presentQueueFamily{ UINT32_MAX };

        void PickPhysicalDevice(VkInstance instance, const Window& window);
        void CreateLogicalDevice();
    };

}