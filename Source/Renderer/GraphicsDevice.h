// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>

#include "Application/Window.h"

namespace Sherbert
{
    /**
     * Handles Vulkan device initialization and GPU communication.
     * Vulkan separates the concept of a physical GPU from the interface used
     * to communicate with it:
     * Physical Device:
     *   Represents the actual hardware installed in the system.
     *   Used for:
     *   - Querying GPU properties
     *   - Checking supported features
     *   - Checking available memory
     *   - Selecting a suitable GPU
     *
     * Logical Device:
     *   A Vulkan-created interface to the physical GPU.
     *   Used for:
     *   - Creating resources
     *   - Creating pipelines
     *   - Submitting commands
     *   - Accessing GPU queues
     *
     * Queues:
     *   The execution channels where commands are submitted.
     *   Examples:
     *   - Graphics queue: drawing and rendering commands
     *   - Compute queue: general GPU computation
     *   - Transfer queue: copying data between resources
     *
     * In simple terms:
     *   physicalDevice = the GPU hardware
     *   device         = our Vulkan connection to that GPU
     *   queues         = where we send GPU work
     */
    class GraphicsDevice
    {

    public:

        // Finds a suitable GPU and creates the Vulkan logical device.
        void Initialize(VkInstance instance, const Window& window);

        [[nodiscard]] VkDevice GetDevice() const { return device; }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] VkQueue GetGraphicsQueue() const { return graphicsQueue; }
        [[nodiscard]] VkQueue GetPresentQueue() const { return presentQueue; }
        [[nodiscard]] uint32_t GetGraphicsQueueFamily() const { return graphicsQueueFamily; }
        [[nodiscard]] uint32_t GetPresentQueueFamily() const { return presentQueueFamily; }

    private:

        /*
         * The selected physical GPU.
         * This handle cannot be used to submit commands. It is only used to
         * inspect hardware capabilities and create a logical device.
         */
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

        // The Vulkan interface used to communicate with the physical GPU.
        VkDevice device{ VK_NULL_HANDLE };

        /*
         * Queues obtained from the logical device.
         * Command buffers are submitted to these queues for execution.
         */
        VkQueue graphicsQueue{ VK_NULL_HANDLE };
        VkQueue presentQueue{ VK_NULL_HANDLE };

        /*
         * Queue family indices identify which queues support which operations.
         * A GPU may expose multiple queue families:
         * - Graphics
         * - Compute
         * - Transfer
         *
         * This simple renderer assumes graphics and presentation use the same queue family.
         */
        uint32_t graphicsQueueFamily{ UINT32_MAX };
        uint32_t presentQueueFamily{ UINT32_MAX };

        void PickPhysicalDevice(VkInstance instance, const Window& window);
        void CreateLogicalDevice();
    };

}
