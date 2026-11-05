#include "CascadedShadows.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace sphere::lighting {

CascadedShadows::CascadedShadows() {
    UpdateCascades();
}

void CascadedShadows::SetCameraFrustum(float fovRad, float aspect, float zNear, float zFar) {
    m_fovRad = fovRad;
    m_aspect = aspect;
    m_cameraNear = zNear;
    m_cameraFar = zFar;
    UpdateCascades();
}

void CascadedShadows::SetCameraTransform(const Vec3& eyePos, const Vec3& targetPos, const Vec3& up) {
    m_cameraPos = eyePos;
    m_cameraTarget = targetPos;
    m_cameraUp = up;
    UpdateCascades();
}

void CascadedShadows::SetLightDirection(const Vec3& dir) {
    m_lightDirection = dir.Normalized();
    UpdateCascades();
}

void CascadedShadows::UpdateCascades() {
    // 1. Compute Practical Split Distances (logarithmic + linear)
    for (size_t i = 0; i < kCascadeCount; ++i) {
        float p = static_cast<float>(i + 1) / static_cast<float>(kCascadeCount);
        float logSplit = m_cameraNear * std::pow(m_cameraFar / m_cameraNear, p);
        float uniformSplit = m_cameraNear + (m_cameraFar - m_cameraNear) * p;
        m_cascadeSplits[i] = m_splitLambda * logSplit + (1.0f - m_splitLambda) * uniformSplit;
    }

    // Camera basis
    Vec3 camForward = (m_cameraTarget - m_cameraPos).Normalized();
    Vec3 camRight = Vec3::Cross(camForward, m_cameraUp).Normalized();
    Vec3 camUp = Vec3::Cross(camRight, camForward).Normalized();

    float lastSplitDist = m_cameraNear;

    for (size_t cascadeIdx = 0; cascadeIdx < kCascadeCount; ++cascadeIdx) {
        float splitDist = m_cascadeSplits[cascadeIdx];

        // 2. Frustum corners in world space for this cascade slice
        float tanHalfFov = std::tan(m_fovRad * 0.5f);
        float nearH = 2.0f * tanHalfFov * lastSplitDist;
        float nearW = nearH * m_aspect;
        float farH = 2.0f * tanHalfFov * splitDist;
        float farW = farH * m_aspect;

        Vec3 nearCenter = m_cameraPos + camForward * lastSplitDist;
        Vec3 farCenter = m_cameraPos + camForward * splitDist;

        Vec3 corners[8] = {
            nearCenter + camUp * (nearH * 0.5f) - camRight * (nearW * 0.5f),
            nearCenter + camUp * (nearH * 0.5f) + camRight * (nearW * 0.5f),
            nearCenter - camUp * (nearH * 0.5f) - camRight * (nearW * 0.5f),
            nearCenter - camUp * (nearH * 0.5f) + camRight * (nearW * 0.5f),
            farCenter + camUp * (farH * 0.5f) - camRight * (farW * 0.5f),
            farCenter + camUp * (farH * 0.5f) + camRight * (farW * 0.5f),
            farCenter - camUp * (farH * 0.5f) - camRight * (farW * 0.5f),
            farCenter - camUp * (farH * 0.5f) + camRight * (farW * 0.5f)
        };

        // Frustum center
        Vec3 frustumCenter{0, 0, 0};
        for (const auto& c : corners) {
            frustumCenter += c;
        }
        frustumCenter = frustumCenter / 8.0f;

        // Bounding sphere radius to make rotation invariant
        float sphereRadius = 0.0f;
        for (const auto& c : corners) {
            float dist = (c - frustumCenter).Length();
            sphereRadius = std::max(sphereRadius, dist);
        }

        // Sub-texel grid stabilization: snap frustum center to texel units
        float shadowMapSize = static_cast<float>(m_shadowMapResolution);
        float texelSize = (2.0f * sphereRadius) / shadowMapSize;

        // Light view matrix looking from frustumCenter along lightDirection
        Vec3 lightUp = std::abs(m_lightDirection.y) < 0.99f ? Vec3{0, 1, 0} : Vec3{0, 0, 1};
        Vec3 lightEye = frustumCenter - m_lightDirection * (sphereRadius * 2.0f);
        Mat4 lightView = Mat4::LookAt(lightEye, frustumCenter, lightUp);

        // Transform center to light space for snapping
        Vec3 centerLightSpace = lightView.TransformPoint(frustumCenter);
        centerLightSpace.x = std::floor(centerLightSpace.x / texelSize) * texelSize;
        centerLightSpace.y = std::floor(centerLightSpace.y / texelSize) * texelSize;

        // Reconstruct stabilized light eye
        // Ortho projection covering sphere radius
        float left = -sphereRadius;
        float right = sphereRadius;
        float bottom = -sphereRadius;
        float top = sphereRadius;
        float zNear = 0.1f;
        float zFar = sphereRadius * 4.0f;

        Mat4 lightProj = Mat4::Ortho(left, right, bottom, top, zNear, zFar);
        m_cascadeMatrices[cascadeIdx] = lightProj * lightView;

        lastSplitDist = splitDist;
    }
}

GpuCascadedShadow CascadedShadows::BuildGpuCascadedShadow() const {
    GpuCascadedShadow gpu{};
    for (size_t i = 0; i < kCascadeCount; ++i) {
        gpu.cascadeViewProj[i] = m_cascadeMatrices[i];
    }
    gpu.cascadeSplitDistances = Vec4{m_cascadeSplits[0], m_cascadeSplits[1], m_cascadeSplits[2], m_cascadeSplits[3]};
    gpu.shadowFilterParams = Vec4{
        m_pcfKernelSize,
        static_cast<float>(m_shadowMapResolution),
        m_normalBias,
        m_depthBias
    };
    return gpu;
}

} // namespace sphere::lighting
