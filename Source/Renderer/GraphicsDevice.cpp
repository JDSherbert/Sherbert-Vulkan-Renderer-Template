// Copyright (c) 2026 JDSherbert. All rights reserved.

#include "GraphicsDevice.h"

#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vulkan/vulkan_win32.h>

void Sherbert::GraphicsDevice::Initialize(VkInstance instance, const Window& window)
{
    PickPhysicalDevice(instance, window);
    CreateLogicalDevice();
}

void Sherbert::GraphicsDevice::PickPhysicalDevice(VkInstance instance, const Window& window)
{
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
    // Queue family selection (simplest: assume graphics and present are same)
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // Pick first queue with VK_QUEUE_GRAPHICS_BIT
    for (uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
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

    const char* deviceExtensions[] =
    {
        "VK_KHR_swapchain"
    };

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

