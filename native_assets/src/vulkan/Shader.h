#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
namespace sphere::vk {
class Shader {
public:
    static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<uint32_t>& spirvCode);
    static VkShaderModule LoadSPIRV(VkDevice device, const std::string& filepath);
};
}
