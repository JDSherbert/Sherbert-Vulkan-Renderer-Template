// Copyright (c) 2026 JDSherbert. All rights reserved.

#include "GraphicsDevice.h"

#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vulkan/vulkan_win32.h>

void Sherbert::GraphicsDevice::Initialize(VkInstance instance, const Window& window)
{
     /*
     * Device setup happens in two stages:
     * 1. Select a physical GPU that supports our requirements.
     * 2. Create a logical device and request access to queues.
     */
    PickPhysicalDevice(instance, window);
    CreateLogicalDevice();
}

void Sherbert::GraphicsDevice::PickPhysicalDevice(VkInstance instance, const Window& window)
{
    /*
     * Vulkan supports multiple GPUs.
     * A machine may contain:
     * - Integrated GPU
     * - Dedicated GPU
     * - Software rendering device
     *
     * We enumerate available devices and select one that meets our requirements.
     */
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) throw std::runtime_error("No Vulkan GPUs found!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& d : devices)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(d, &props);
        std::cout << "Found GPU: " << props.deviceName << std::endl;

        /*
         * This template intentionally uses a very simple selection rule.
         * A production renderer would usually check:
         * - Required Vulkan version
         * - Supported features
         * - Queue availability
         * - Required extensions
         * - Swapchain support
         *
         * and then score available GPUs.
         */
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            std::string(props.deviceName).find("NVIDIA") != std::string::npos)
        {
            physicalDevice = d;
            std::cout << "Selected GPU: " << props.deviceName << std::endl;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find a suitable NVIDIA discrete GPU!");
}

void Sherbert::GraphicsDevice::CreateLogicalDevice()
{
    /*
     * Queue families describe what types of commands a queue can execute.
     * A GPU may have separate queues for:
     * - Graphics rendering
     * - Compute workloads
     * - Memory transfers
     *
     * This renderer only requires graphics commands, so we find the first queue
     * family supporting VK_QUEUE_GRAPHICS_BIT.
     */
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount); // Queue family selection (simplest: assume graphics and present are same)
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // Pick first queue with VK_QUEUE_GRAPHICS_BIT
    for (uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            // TODO: Check VK_QUEUE_PRESENT support separately.
            // Many GPUs use the same queue family, but Vulkan does not guarantee this.
            graphicsQueueFamily = i;
            presentQueueFamily = i; // assume same family for simplicity
            break;
        }
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsQueueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    /*
     * Vulkan extensions add optional functionality to the core API.
     * The swapchain is not part of Vulkan core, so we must explicitly enable the
     * VK_KHR_swapchain extension before creating one.
     */
    const char* deviceExtensions[] =
    {
        "VK_KHR_swapchain"
    };

    /*
     * Creating the logical device gives our application permission to use the selected GPU.
     * The logical device is where we request:
     * - Queues
     * - Device features
     * - Extensions
     */
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
 
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device!");

    vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);
}

