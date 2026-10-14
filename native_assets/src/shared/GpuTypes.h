#pragma once
#include "scene/Transform.h"

namespace sphere {

// GPU alignment (std140 / std430)
struct alignas(16) GpuCamera {
    Mat4 view;
    Mat4 projection;
    Mat4 viewProjection;
    Vec4 positionAndNear; // xyz = pos, w = near
    Vec4 forwardAndFar;   // xyz = forward, w = far
};

struct alignas(16) GpuLight {
    Vec4 positionAndType;  // xyz = pos, w = type
    Vec4 directionAndRange;// xyz = dir, w = range
    Vec4 colorAndIntensity;// xyz = color, w = intensity
};

struct alignas(16) GpuMaterial {
    Vec4 baseColor;
    float metallic;
    float roughness;
    float occlusion;
    uint32_t albedoIndex;
};
}\n