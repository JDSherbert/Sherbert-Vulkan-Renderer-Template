// Copyright (c) 2026 JDSherbert. All rights reserved.

#include "Pipeline.h"

#include <fstream>
#include <vector>
#include <stdexcept>

void Sherbert::Pipeline::Initialize(VkDevice device, VkExtent2D extent, VkRenderPass renderPass)
{
    /*
     * The pipeline layout describes what resources the shaders can access.
     * Currently this renderer has no external resources, so the layout is empty.
     * Future features such as textures, camera matrices, and lighting data would
     * be exposed here through descriptor sets or push constants.
     */
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    /*
     * Vulkan does not compile shaders at runtime.
     * Source shaders: (triangle.vert, triangle.frag)
     * are compiled beforehand into SPIR-V: (triangle.vert.spv, triangle.frag.spv)
     * SPIR-V is an intermediate binary format that Vulkan drivers understand.
     */
    VkShaderModule vertShaderModule = LoadShaderModule(device, "Shaders/triangle.vert.spv");
    VkShaderModule fragShaderModule = LoadShaderModule(device, "Shaders/triangle.frag.spv");

    /*
     * A shader stage describes one step of the GPU pipeline.
     * Vertex shader: Converts vertex data into clip-space positions.
     * Fragment shader: Calculates the final colour of each pixel.
     */
    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertShaderModule;
    vertStage.pName  = "main";

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragShaderModule;
    fragStage.pName  = "main";

    /*
     * Defines how vertex data is passed into the vertex shader.
     * This triangle uses no vertex buffer. Instead, the vertex shader generates
     * the triangle positions internally using gl_VertexIndex.
     *
     * A normal renderer would describe:
     * - Vertex buffer bindings
     * - Position attributes
     * - Texture coordinates
     * - Normals
     * - Colours
     */
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertStage, fragStage };

    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 0;
    vertexInput.vertexAttributeDescriptionCount = 0;

    /*
     * Defines how vertices are assembled into primitives.
     * Triangle list means: Vertex 0 + Vertex 1 + Vertex 2 = Triangle
     * Other options include:
     *     LINE_LIST
     *     TRIANGLE_STRIP
     *     POINT_LIST
     */
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    /*
     * The viewport transforms coordinates from the vertex shader into screen coordinates.
     * The scissor rectangle defines the area where pixels are allowed to be written.
     * Together they control where rendering appears on screen.
     */
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0,0};
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    /*
     * Rasterization converts geometric primitives into fragments (potential pixels).
     * This controls things like:
     * - Polygon fill mode
     * - Backface culling
     * - Triangle winding direction
     *
     * For example, backface culling prevents drawing triangles facing away from the camera.
     */
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    /*
     * Controls anti-aliasing. VK_SAMPLE_COUNT_1_BIT means multisampling is disabled.
     * Increasing this value enables techniques such as MSAA, where multiple
     * samples are taken per pixel to smooth triangle edges.
     */
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    /*
     * Controls how the output colour is combined with the existing framebuffer.
     * Blending is commonly used for:
     * - Transparent objects
     * - Particle effects
     * - UI elements
     *
     * This renderer simply overwrites the existing colour.
     */
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    /*
     * Create the complete graphics pipeline.
     * After this call, Vulkan has a preconfigured GPU program describing exactly
     * how rendering should occur.
     * Vulkan pipelines are immutable. If a setting changes, a new pipeline is
     * usually created rather than modifying the existing one.
     */
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }

    // Cleanup shader modules
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

void Sherbert::Pipeline::Cleanup(VkDevice device)
{
    if (pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device, pipeline, nullptr);
        pipeline = VK_NULL_HANDLE;
    }
    if (pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        pipelineLayout = VK_NULL_HANDLE;
    }
}

VkShaderModule Sherbert::Pipeline::LoadShaderModule(VkDevice device, const std::string& filepath)
{
    /*
     * Shader files are loaded as binary SPIR-V data.
     * The Vulkan driver does not receive GLSL source code. The shader compiler
     * produces SPIR-V first, then Vulkan creates a shader module from that binary.
     */
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + filepath);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    // Create shader module
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module from: " + filepath);
    }

    return shaderModule;
}
