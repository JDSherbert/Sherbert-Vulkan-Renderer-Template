// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>

namespace Sherbert
{
    /**
     * Defines the structure of a rendering operation.
     *
     * A RenderPass does not execute rendering commands itself. Instead, it
     * describes the attachments used during rendering and how Vulkan should
     * transition those resources throughout the render operation.
     * For this basic renderer we only have one attachment:
     *     Swapchain Image -> Colour Output -> Present to Screen
     *
     * More advanced renderers may add:
     *     - Depth buffers
     *     - Stencil buffers
     *     - Multiple colour attachments
     *     - Multiple subpasses
     */
    class RenderPass
    {

    public:

        /**
         * Creates the Vulkan render pass.
         * The format must match the swapchain image format because the render
         * pass writes directly into the images that will be presented.
         */
        void Initialize(VkDevice device, VkFormat swapchainFormat);

        /**
         * Returns the Vulkan render pass handle.
         * This is passed into the graphics pipeline because the pipeline must
         * know what render targets it will be used with.
         */
        [[nodiscard]] VkRenderPass Get() const { return renderPass; }

        // Releases the Vulkan render pass resource.
        void Cleanup(VkDevice device);

    private:

        VkRenderPass renderPass{};
    };
}
