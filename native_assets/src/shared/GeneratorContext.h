#pragma once
#include <vulkan/vulkan.h>
#include "scene/WorldState.h"
#include <cstdint>
namespace sphere {
struct GeneratorContext {
    VkDevice device = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    uint32_t seed = 1337;
    float lodLevel = 0.0f;
    const WorldState* worldState = nullptr;
    bool IsValid() const;
    float GetTime() const;
    Vec3 GetWindDirection() const;
    float GetWindSpeed() const;
    float GetRainIntensity() const;
    float GetWetness() const;
    float GetSnowDepth() const;
    uint32_t DeriveSeed(const char* salt) const;
};
} // namespace sphere