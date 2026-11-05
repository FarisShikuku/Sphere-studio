#pragma once
#include "scene/Transform.h"
#include <vector>
#include <string>
#include <memory>

namespace sphere::lighting {

/**
 * SdfGiSystem - Signed Distance Field Diffuse Global Illumination
 * Faithfully reproducing Gaukler/PlainRenderer's real-time GI architecture.
 *
 * PlainRenderer's signature technique:
 * 1. Geometry is represented as continuous 3D Signed Distance Fields (SDFs).
 * 2. Compute shaders perform sphere tracing / cone tracing across the distance field.
 * 3. Evaluates dynamic multi-bounce diffuse indirect illumination and color bleeding.
 * 4. Computes Distance Field Ambient Occlusion (DFAO) without costly monte-carlo rays.
 */

// GPU std430 uniform block for SDF Diffuse GI compute pass
struct alignas(16) GpuSdfGi {
    Vec4 volumeMin;             // xyz = world-space AABB min, w = voxelSize
    Vec4 volumeMax;             // xyz = world-space AABB max, w = maxRayDistance
    Vec4 gridResolution;        // xyz = grid dimensions (e.g. 64, 64, 64), w = totalVoxels
    Vec4 giParameters;          // x = bounceFactor, y = coneAngleRad, z = stepSizeFactor, w = aoIntensity
    Vec4 colorBleedingParams;   // x = indirectColorBoost, y = normalOffsetBias, z = multiBounceApprox, w = debugViewMode
};

struct SdfMeshVolume {
    std::string meshName;
    Vec3 aabbMin{-2.0f, -0.5f, -2.0f};
    Vec3 aabbMax{2.0f, 3.5f, 2.0f};
    uint32_t resolutionX = 64;
    uint32_t resolutionY = 64;
    uint32_t resolutionZ = 64;
    float voxelSize = 0.0625f; // In meters
    std::vector<float> distanceBuffer; // 3D distance values (negative = inside, positive = outside)
    std::vector<Vec4> surfaceAlbedoBuffer; // Surface albedo for radiance bounce
};

class SdfGiSystem {
public:
    SdfGiSystem();
    ~SdfGiSystem() = default;

    // GI Tuning Parameters
    void SetBounceFactor(float factor) { m_bounceFactor = std::clamp(factor, 0.0f, 3.0f); }
    float GetBounceFactor() const { return m_bounceFactor; }

    void SetAoIntensity(float ao) { m_aoIntensity = std::clamp(ao, 0.0f, 2.0f); }
    float GetAoIntensity() const { return m_aoIntensity; }

    void SetConeTracingAngleDeg(float deg) { m_coneAngleDeg = std::clamp(deg, 15.0f, 75.0f); }
    float GetConeTracingAngleDeg() const { return m_coneAngleDeg; }

    void SetMaxRayDistance(float dist) { m_maxRayDistance = std::clamp(dist, 1.0f, 50.0f); }
    float GetMaxRayDistance() const { return m_maxRayDistance; }

    void SetIndirectColorBoost(float boost) { m_indirectColorBoost = std::clamp(boost, 0.5f, 2.5f); }
    float GetIndirectColorBoost() const { return m_indirectColorBoost; }

    void SetDebugViewMode(int mode) { m_debugViewMode = mode; }
    int GetDebugViewMode() const { return m_debugViewMode; }

    // Generates test calibration SDF volume (e.g. box, floor, wall, sphere)
    void InitializeSceneVolumes();

    // CPU query: sample SDF distance at world position
    float SampleDistance(const Vec3& worldPos) const;

    // CPU query: cone trace indirect diffuse radiance at a surface point and normal
    Vec3 EvaluateIndirectDiffuse(const Vec3& surfacePos, const Vec3& normal, const Vec3& surfaceAlbedo) const;

    // Distance Field Ambient Occlusion (DFAO)
    float EvaluateAmbientOcclusion(const Vec3& surfacePos, const Vec3& normal) const;

    // Build GPU uniform buffer for Vulkan compute shader
    GpuSdfGi BuildGpuSdfGi() const;

private:
    float m_bounceFactor = 1.0f;
    float m_aoIntensity = 0.85f;
    float m_coneAngleDeg = 45.0f;
    float m_maxRayDistance = 12.0f;
    float m_indirectColorBoost = 1.25f;
    float m_stepSizeFactor = 0.8f;
    int m_debugViewMode = 0; // 0 = Lit + Indirect, 1 = Indirect Only, 2 = Distance Field, 3 = AO Only

    Vec3 m_worldAabbMin{-3.0f, -0.2f, -3.0f};
    Vec3 m_worldAabbMax{3.0f, 4.0f, 3.0f};
    uint32_t m_resX = 64;
    uint32_t m_resY = 64;
    uint32_t m_resZ = 64;

    std::vector<SdfMeshVolume> m_volumes;
};

} // namespace sphere::lighting
