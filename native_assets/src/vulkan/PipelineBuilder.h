#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Pipeline.h"

namespace sphere::vk {
class PipelineBuilder {
public:
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    Pipeline BuildGraphics(VkDevice device, VkRenderPass renderPass);
    Pipeline BuildCompute(VkDevice device, VkPipelineLayout layout, VkShaderModule computeShader);
};
}\n