#pragma once
#include <vulkan/vulkan.h>
namespace sphere::base::initializers {
inline VkMemoryAllocateInfo MemoryAllocateInfo() {
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    return memAllocInfo;
}
inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t bufferCount) {
    VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
    cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocInfo.commandPool = commandPool;
    cmdBufferAllocInfo.level = level;
    cmdBufferAllocInfo.commandBufferCount = bufferCount;
    return cmdBufferAllocInfo;
}
inline VkCommandBufferBeginInfo CommandBufferBeginInfo() {
    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    return cmdBufferBeginInfo;
}
inline VkSubmitInfo SubmitInfo() {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    return submitInfo;
}
inline VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(
    VkDescriptorType type,
    VkShaderStageFlags stageFlags,
    uint32_t binding,
    uint32_t descriptorCount = 1) {
    VkDescriptorSetLayoutBinding setlayoutBinding{};
    setlayoutBinding.descriptorType = type;
    setlayoutBinding.stageFlags = stageFlags;
    setlayoutBinding.binding = binding;
    setlayoutBinding.descriptorCount = descriptorCount;
    return setlayoutBinding;
}
inline VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule) {
    VkPipelineShaderStageCreateInfo shaderStage{};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = stage;
    shaderStage.module = shaderModule;
    shaderStage.pName = "main";
    return shaderStage;
}
}
