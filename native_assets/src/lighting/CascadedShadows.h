#pragma once
#include "scene/Transform.h"
#include <array>

namespace sphere::lighting {

/**
 * CascadedShadows - Directional Cascaded Shadow Map (CSM) System
 * Referencing Gaukler/PlainRenderer shadow cascade architecture.
 *
 * Implements:
 * - 4 frustum split cascades using practical logarithmic-linear blend.
 * - Texel-aligned orthographic projection to prevent shadow map swimming/shimmering.
 * - Sub-pixel stabilization and PCF 5x5 kernel weights.
 */

constexpr size_t kCascadeCount = 4;

struct alignas(16) GpuCascadedShadow {
    Mat4 cascadeViewProj[kCascadeCount]; // Light view-projection for each cascade
    Vec4 cascadeSplitDistances;          // View-space z distances (c0, c1, c2, c3)
    Vec4 shadowFilterParams;             // x = pcfKernelSize, y = shadowMapResolution, z = normalBias, w = depthBias
};

class CascadedShadows {
public:
    CascadedShadows();
    ~CascadedShadows() = default;

    void SetCameraFrustum(float fovRad, float aspect, float zNear, float zFar);
    void SetCameraTransform(const Vec3& eyePos, const Vec3& targetPos, const Vec3& up);

    void SetLightDirection(const Vec3& dir);
    Vec3 GetLightDirection() const { return m_lightDirection; }

    void SetSplitLambda(float lambda) { m_splitLambda = std::clamp(lambda, 0.0f, 1.0f); UpdateCascades(); }
    float GetSplitLambda() const { return m_splitLambda; }

    void SetShadowMapResolution(uint32_t res) { m_shadowMapResolution = res; UpdateCascades(); }
    uint32_t GetShadowMapResolution() const { return m_shadowMapResolution; }

    void SetPcfKernelSize(float size) { m_pcfKernelSize = size; }
    float GetPcfKernelSize() const { return m_pcfKernelSize; }

    void SetNormalBias(float bias) { m_normalBias = bias; }
    float GetNormalBias() const { return m_normalBias; }

    void SetDepthBias(float bias) { m_depthBias = bias; }
    float GetDepthBias() const { return m_depthBias; }

    const std::array<float, kCascadeCount>& GetSplitDepths() const { return m_cascadeSplits; }
    const std::array<Mat4, kCascadeCount>& GetCascadeMatrices() const { return m_cascadeMatrices; }

    // Generates GPU uniform block for Vulkan shaders
    GpuCascadedShadow BuildGpuCascadedShadow() const;

private:
    void UpdateCascades();

    Vec3 m_lightDirection{0.45f, -0.75f, -0.48f};
    Vec3 m_cameraPos{0.0f, 2.0f, 5.0f};
    Vec3 m_cameraTarget{0.0f, 0.5f, 0.0f};
    Vec3 m_cameraUp{0.0f, 1.0f, 0.0f};

    float m_fovRad = 45.0f * 3.14159265f / 180.0f;
    float m_aspect = 16.0f / 9.0f;
    float m_cameraNear = 0.1f;
    float m_cameraFar = 100.0f;

    float m_splitLambda = 0.75f; // 0.0 = uniform, 1.0 = logarithmic
    uint32_t m_shadowMapResolution = 2048;
    float m_pcfKernelSize = 2.0f;
    float m_normalBias = 0.005f;
    float m_depthBias = 0.0015f;

    std::array<float, kCascadeCount> m_cascadeSplits{5.0f, 15.0f, 40.0f, 100.0f};
    std::array<Mat4, kCascadeCount> m_cascadeMatrices;
};

} // namespace sphere::lighting
