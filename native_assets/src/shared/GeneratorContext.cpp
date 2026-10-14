#include "GeneratorContext.h"
#include <cstring>

namespace sphere {

bool GeneratorContext::IsValid() const {
    return (device != VK_NULL_HANDLE || commandBuffer != VK_NULL_HANDLE);
}

float GeneratorContext::GetTime() const {
    return worldState ? worldState->time : 0.0f;
}

Vec3 GeneratorContext::GetWindDirection() const {
    return worldState ? worldState->windDirection : Vec3{1.0f, 0.0f, 0.0f};
}

float GeneratorContext::GetWindSpeed() const {
    return worldState ? worldState->windSpeed : 0.0f;
}

float GeneratorContext::GetRainIntensity() const {
    return worldState ? worldState->rainIntensity : 0.0f;
}

float GeneratorContext::GetWetness() const {
    return worldState ? worldState->wetness : 0.0f;
}

float GeneratorContext::GetSnowDepth() const {
    return worldState ? worldState->snowDepth : 0.0f;
}

uint32_t GeneratorContext::DeriveSeed(const char* salt) const {
    // 32-bit FNV-1a hash algorithm
    uint32_t hash = 2166136261u ^ seed;
    if (salt) {
        size_t len = std::strlen(salt);
        for (size_t i = 0; i < len; ++i) {
            hash ^= static_cast<uint8_t>(salt[i]);
            hash *= 16777619u;
        }
    }
    return hash;
}

} // namespace sphere
