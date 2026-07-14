// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace Sherbert
{
    /**
     * Represents a Vulkan graphics pipeline.
     * Unlike APIs such as OpenGL where rendering state is changed dynamically,
     * Vulkan requires most GPU state to be created ahead of time.
     * The graphics pipeline describes:
     *  - Which shaders are executed
     *  - How vertex data is interpreted
     *  - How triangles are assembled
     *  - How geometry is rasterized
     *  - How colours are written to the framebuffer
     *
     * Once created, the pipeline is bound during command buffer recording and
     * used by the GPU to execute rendering commands.
     */
    class Pipeline
    {

    public:

        /**
         * Creates the Vulkan graphics pipeline.
         * The pipeline is linked to a render pass because Vulkan needs to know
         * what attachments the pipeline will render into.
         */
        void Initialize(VkDevice device, VkExtent2D extent, VkRenderPass renderPass);

        // Returns the graphics pipeline handle.
        [[nodiscard]] VkPipeline Get() const { return pipeline; }

        /**
         * Returns the pipeline layout.
         * The layout describes resources accessible to shaders, such as:
         * - Descriptor sets
         * - Uniform buffers
         * - Push constants
         */
        [[nodiscard]] VkPipelineLayout GetLayout() const { return pipelineLayout; }

        // Releases pipeline resources.
        void Cleanup(VkDevice device);

        /**
         * Loads a compiled SPIR-V shader and creates a Vulkan shader module.
         * Vulkan does not consume GLSL/HLSL directly. 
         * Shaders must first be compiled into SPIR-V bytecode.
         */
        VkShaderModule LoadShaderModule(VkDevice device, const std::string& filepath);

    private:

        VkPipeline pipeline{};
        VkPipelineLayout pipelineLayout{};

    };
}
