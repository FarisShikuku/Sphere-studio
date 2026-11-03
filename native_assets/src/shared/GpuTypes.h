#pragma once
#include "scene/Transform.h"
#include <cstdint>
namespace sphere {
// GPU alignment adhering strictly to std140 / std430 rules
struct alignas(16) GpuCamera {
    Mat4 view;
    Mat4 projection;
    Mat4 viewProjection;
    Mat4 invViewProjection;
    Vec4 positionAndNear; // xyz = position, w = nearPlane
    Vec4 forwardAndFar;   // xyz = forward, w = farPlane
};
struct alignas(16) GpuLight {
    Vec4 positionAndType;   // xyz = position, w = lightType (0=dir, 1=point, 2=spot)
    Vec4 directionAndRange; // xyz = direction, w = range
    Vec4 colorAndIntensity; // xyz = rgb, w = intensity
};
struct alignas(16) GpuMaterial {
    Vec4 baseColorFactor{1.0f, 1.0f, 1.0f, 1.0f};
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.5f;
    float occlusionFactor = 1.0f;
    uint32_t albedoTexIndex = 0;
    uint32_t normalTexIndex = 0;
    uint32_t ormTexIndex = 0;
    uint32_t emissiveTexIndex = 0;
    float pad = 0.0f;
};
struct alignas(16) GpuMeshlet {
    Vec4 centerAndRadius;     // xyz = center, w = radius
    Vec4 coneApexAndCutoff;   // xyz = cone apex, w = cone cutoff
    Vec4 coneAxis;           // xyz = cone axis, w = unused
};
struct alignas(16) GpuDrawCommand {
    uint32_t indexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t firstIndex = 0;
    int32_t  vertexOffset = 0;
    uint32_t firstInstance = 0;
};
struct alignas(16) GpuAtmosphere {
    Vec4 sunDirection; // xyz = dir, w = elevation
    Vec4 sunColor;     // xyz = rgb, w = ambient
    float turbidity = 2.0f;
    float rayleigh = 1.0f;
    float mie = 1.0f;
    float exposure = 1.0f;
};
} // namespace spher
