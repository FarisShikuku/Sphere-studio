#pragma once
#include "Transform.h"
#include <string>
namespace sphere {
struct Material {
    std::string name = "PBR_Material";
    Vec4 baseColorFactor{1.0f, 1.0f, 1.0f, 1.0f};
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.5f;
    float occlusionStrength = 1.0f;
    Vec3 emissiveFactor{0.0f, 0.0f, 0.0f};
    uint32_t albedoTextureIndex = 0;
    uint32_t normalTextureIndex = 1;
    uint32_t ormTextureIndex = 2; // Occlusion, Roughness, Metallic
};
}
