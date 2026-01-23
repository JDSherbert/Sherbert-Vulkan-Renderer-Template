// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace Sherbert
{
    class Pipeline
    {

    public:

        void Initialize(VkDevice device, VkExtent2D extent, VkRenderPass renderPass);
        [[nodiscard]] VkPipeline Get() const { return pipeline; }
        [[nodiscard]] VkPipelineLayout GetLayout() const { return pipelineLayout; }
        void Cleanup(VkDevice device);
        VkShaderModule LoadShaderModule(VkDevice device, const std::string& filepath);

    private:

        VkPipeline pipeline{};
        VkPipelineLayout pipelineLayout{};

    };
}
