#include "PipelineBuilder.h"
namespace sphere::vk {
Pipeline PipelineBuilder::BuildGraphics(VkDevice device, VkRenderPass renderPass) {
    Pipeline pipe;
    pipe.layout = pipelineLayout;
    pipe.bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return pipe;
}
Pipeline PipelineBuilder::BuildCompute(VkDevice device, VkPipelineLayout layout, VkShaderModule computeShader) {
    Pipeline pipe;
    pipe.layout = layout;
    pipe.bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    return pipe;
}
}
