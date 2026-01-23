// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>

namespace Sherbert
{
    class RenderPass
    {

    public:

        void Initialize(VkDevice device, VkFormat swapchainFormat);
        [[nodiscard]] VkRenderPass Get() const { return renderPass; }
        void Cleanup(VkDevice device);

    private:

        VkRenderPass renderPass{};
    };

}