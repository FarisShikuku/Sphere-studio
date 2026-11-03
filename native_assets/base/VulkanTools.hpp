#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <cassert>
#include <stdexcept>
#define VK_CHECK_RESULT(f)                                                                                 \
    {                                                                                                       \
        VkResult res = (f);                                                                                 \
        if (res != VK_SUCCESS) {                                                                           \
            throw std::runtime_error("Fatal : VkResult is "" + sphere::base::tools::ErrorString(res) + """); \
        }                                                                                                   \
    }
namespace sphere::base::tools {
    std::string ErrorString(VkResult errorCode);
    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
}
