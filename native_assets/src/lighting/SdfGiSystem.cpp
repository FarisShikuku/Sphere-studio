#include "SdfGiSystem.h"
#include <cmath>
#include <algorithm>

namespace sphere::lighting {

SdfGiSystem::SdfGiSystem() {
    InitializeSceneVolumes();
}

void SdfGiSystem::InitializeSceneVolumes() {
    m_volumes.clear();

    // Create synthetic SDF volume representing the PlainRenderer test stage:
    // A ground floor plane (y = 0), a back architectural wall (z = -2), and a central calibration box
    SdfMeshVolume stageVol;
    stageVol.meshName = "PlainRenderer_StudioStage";
    stageVol.aabbMin = m_worldAabbMin;
    stageVol.aabbMax = m_worldAabbMax;
    stageVol.resolutionX = m_resX;
    stageVol.resolutionY = m_resY;
    stageVol.resolutionZ = m_resZ;
    stageVol.voxelSize = (m_worldAabbMax.x - m_worldAabbMin.x) / static_cast<float>(m_resX);

    size_t totalCells = m_resX * m_resY * m_resZ;
    stageVol.distanceBuffer.resize(totalCells, 10.0f);
    stageVol.surfaceAlbedoBuffer.resize(totalCells, Vec4{0.7f, 0.7f, 0.7f, 1.0f});

    for (uint32_t z = 0; z < m_resZ; ++z) {
        float wz = m_worldAabbMin.z + (static_cast<float>(z) + 0.5f) * stageVol.voxelSize;
        for (uint32_t y = 0; y < m_resY; ++y) {
            float wy = m_worldAabbMin.y + (static_cast<float>(y) + 0.5f) * stageVol.voxelSize;
            for (uint32_t x = 0; x < m_resX; ++x) {
                float wx = m_worldAabbMin.x + (static_cast<float>(x) + 0.5f) * stageVol.voxelSize;
                size_t idx = z * (m_resX * m_resY) + y * m_resX + x;

                // Ground plane SDF: distance to y = 0
                float dGround = wy;

                // Back Wall SDF: box slab at z = -2.0, thickness 0.4
                float dWall = std::max({std::abs(wx) - 2.5f, wy - 3.5f, std::abs(wz - (-2.0f)) - 0.2f});

                // Central calibration cube SDF: at (0, 0.6, 0), size 1.2m
                float dCube = std::max({std::abs(wx) - 0.6f, std::abs(wy - 0.6f) - 0.6f, std::abs(wz) - 0.6f});

                // Left red wall (causes vivid red color bleeding bounce!)
                float dLeftRedWall = std::max({std::abs(wx - (-2.4f)) - 0.15f, wy - 3.5f, std::abs(wz) - 2.0f});

                // Union of all SDFs
                float dScene = std::min({dGround, dWall, dCube, dLeftRedWall});
                stageVol.distanceBuffer[idx] = dScene;

                // Color tagging for indirect bounce
                if (dLeftRedWall < 0.1f) {
                    stageVol.surfaceAlbedoBuffer[idx] = Vec4{0.92f, 0.15f, 0.12f, 1.0f}; // Red wall bounce
                } else if (dWall < 0.1f) {
                    stageVol.surfaceAlbedoBuffer[idx] = Vec4{0.20f, 0.45f, 0.85f, 1.0f}; // Blue wall bounce
                } else if (dGround < 0.1f) {
                    stageVol.surfaceAlbedoBuffer[idx] = Vec4{0.80f, 0.78f, 0.75f, 1.0f}; // Concrete floor
                } else {
                    stageVol.surfaceAlbedoBuffer[idx] = Vec4{0.85f, 0.85f, 0.85f, 1.0f};
                }
            }
        }
    }

    m_volumes.push_back(std::move(stageVol));
}

float SdfGiSystem::SampleDistance(const Vec3& worldPos) const {
    if (m_volumes.empty()) return 10.0f;
    const auto& vol = m_volumes[0];

    // Trilinear interpolation in SDF grid
    float u = (worldPos.x - vol.aabbMin.x) / (vol.aabbMax.x - vol.aabbMin.x);
    float v = (worldPos.y - vol.aabbMin.y) / (vol.aabbMax.y - vol.aabbMin.y);
    float w = (worldPos.z - vol.aabbMin.z) / (vol.aabbMax.z - vol.aabbMin.z);

    if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f || w < 0.0f || w > 1.0f) {
        return 10.0f; // Outside grid
    }

    float gx = u * static_cast<float>(vol.resolutionX - 1);
    float gy = v * static_cast<float>(vol.resolutionY - 1);
    float gz = w * static_cast<float>(vol.resolutionZ - 1);

    uint32_t x0 = static_cast<uint32_t>(gx);
    uint32_t y0 = static_cast<uint32_t>(gy);
    uint32_t z0 = static_cast<uint32_t>(gz);
    uint32_t x1 = std::min(x0 + 1, vol.resolutionX - 1);
    uint32_t y1 = std::min(y0 + 1, vol.resolutionY - 1);
    uint32_t z1 = std::min(z0 + 1, vol.resolutionZ - 1);

    float fx = gx - static_cast<float>(x0);
    float fy = gy - static_cast<float>(y0);
    float fz = gz - static_cast<float>(z0);

    auto getD = [&](uint32_t x, uint32_t y, uint32_t z) {
        return vol.distanceBuffer[z * (vol.resolutionX * vol.resolutionY) + y * vol.resolutionX + x];
    };

    float d000 = getD(x0, y0, z0); float d100 = getD(x1, y0, z0);
    float d010 = getD(x0, y1, z0); float d110 = getD(x1, y1, z0);
    float d001 = getD(x0, y0, z1); float d101 = getD(x1, y0, z1);
    float d011 = getD(x0, y1, z1); float d111 = getD(x1, y1, z1);

    float dx00 = d000 * (1.0f - fx) + d100 * fx;
    float dx10 = d010 * (1.0f - fx) + d110 * fx;
    float dx01 = d001 * (1.0f - fx) + d101 * fx;
    float dx11 = d011 * (1.0f - fx) + d111 * fx;

    float dxy0 = dx00 * (1.0f - fy) + dx10 * fy;
    float dxy1 = dx01 * (1.0f - fy) + dx11 * fy;

    return dxy0 * (1.0f - fz) + dxy1 * fz;
}

Vec3 SdfGiSystem::EvaluateIndirectDiffuse(const Vec3& surfacePos, const Vec3& normal, const Vec3& surfaceAlbedo) const {
    // PlainRenderer Cone-Tracing: march multiple diffuse cones along hemisphere
    constexpr int kConeCount = 6;
    // Pre-calculated cosine-weighted hemisphere cone directions
    static const Vec3 kConeDirections[kConeCount] = {
        Vec3{0.0f, 1.0f, 0.0f},
        Vec3{0.8f, 0.5f, 0.0f},
        Vec3{-0.8f, 0.5f, 0.0f},
        Vec3{0.0f, 0.5f, 0.8f},
        Vec3{0.0f, 0.5f, -0.8f},
        Vec3{0.577f, 0.577f, 0.577f}
    };

    Vec3 accumulatedRadiance{0.0f, 0.0f, 0.0f};
    float totalWeight = 0.0f;
    float startOffset = 0.08f; // Normal bias to escape starting surface

    for (int i = 0; i < kConeCount; ++i) {
        // Construct orthonormal basis around surface normal
        Vec3 up = std::abs(normal.y) < 0.99f ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        Vec3 tangent = Vec3::Cross(up, normal).Normalized();
        Vec3 bitangent = Vec3::Cross(normal, tangent).Normalized();

        Vec3 localDir = kConeDirections[i];
        Vec3 worldConeDir = tangent * localDir.x + normal * localDir.y + bitangent * localDir.z;
        worldConeDir = worldConeDir.Normalized();

        float cosTheta = std::max(0.0f, Vec3::Dot(worldConeDir, normal));
        if (cosTheta <= 0.0f) continue;

        // Trace cone through SDF
        float t = startOffset;
        for (int step = 0; step < 16; ++step) {
            Vec3 samplePoint = surfacePos + worldConeDir * t;
            float dist = SampleDistance(samplePoint);

            if (dist < 0.04f) {
                // Hit a bouncing surface! Collect indirect bounce color
                Vec3 bounceAlbedo = (samplePoint.x < -1.5f) ? Vec3{0.9f, 0.2f, 0.15f} :
                                    (samplePoint.z < -1.2f) ? Vec3{0.2f, 0.5f, 0.9f} :
                                    Vec3{0.6f, 0.6f, 0.6f};
                accumulatedRadiance += bounceAlbedo * cosTheta;
                break;
            }

            t += std::max(dist * m_stepSizeFactor, 0.05f);
            if (t > m_maxRayDistance) break;
        }
        totalWeight += cosTheta;
    }

    if (totalWeight > 0.001f) {
        accumulatedRadiance = accumulatedRadiance / totalWeight;
    }

    return accumulatedRadiance * m_bounceFactor * m_indirectColorBoost;
}

float SdfGiSystem::EvaluateAmbientOcclusion(const Vec3& surfacePos, const Vec3& normal) const {
    // Distance Field Ambient Occlusion (DFAO)
    // Samples SDF at 4 increasing steps along the surface normal
    constexpr float kSteps[4] = {0.05f, 0.12f, 0.25f, 0.45f};
    float occlusion = 0.0f;

    for (int i = 0; i < 4; ++i) {
        float stepDist = kSteps[i];
        Vec3 p = surfacePos + normal * stepDist;
        float actualDist = SampleDistance(p);
        float diff = std::max(0.0f, stepDist - actualDist);
        occlusion += (diff / stepDist) * (1.0f / (1.0f + static_cast<float>(i)));
    }

    float ao = 1.0f - std::clamp(occlusion * m_aoIntensity, 0.0f, 1.0f);
    return ao;
}

GpuSdfGi SdfGiSystem::BuildGpuSdfGi() const {
    GpuSdfGi gpu{};
    float voxelSize = (m_worldAabbMax.x - m_worldAabbMin.x) / static_cast<float>(m_resX);

    gpu.volumeMin = Vec4{m_worldAabbMin.x, m_worldAabbMin.y, m_worldAabbMin.z, voxelSize};
    gpu.volumeMax = Vec4{m_worldAabbMax.x, m_worldAabbMax.y, m_worldAabbMax.z, m_maxRayDistance};
    gpu.gridResolution = Vec4{static_cast<float>(m_resX), static_cast<float>(m_resY), static_cast<float>(m_resZ), static_cast<float>(m_resX * m_resY * m_resZ)};

    float coneRad = m_coneAngleDeg * 3.14159265f / 180.0f;
    gpu.giParameters = Vec4{m_bounceFactor, coneRad, m_stepSizeFactor, m_aoIntensity};
    gpu.colorBleedingParams = Vec4{m_indirectColorBoost, 0.05f, 1.0f, static_cast<float>(m_debugViewMode)};

    return gpu;
}

} // namespace sphere::lighting
