#include "FluidSimulator.hpp"
#include "CliffGeometry.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace sphere::water {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr uint32_t kPrime1 = 15823u;
constexpr uint32_t kPrime2 = 9737333u;
constexpr uint32_t kPrime3 = 440817757u;

inline float SpikyKernelPow2(float dst, float radius, float scale) {
    if (dst >= radius) return 0.0f;
    float v = radius - dst;
    return v * v * scale;
}

inline float SpikyKernelPow3(float dst, float radius, float scale) {
    if (dst >= radius) return 0.0f;
    float v = radius - dst;
    return v * v * v * scale;
}

inline float DerivativeSpikyPow2(float dst, float radius, float scale) {
    if (dst >= radius) return 0.0f;
    float v = radius - dst;
    return -v * scale;
}

inline float DerivativeSpikyPow3(float dst, float radius, float scale) {
    if (dst >= radius) return 0.0f;
    float v = radius - dst;
    return -v * v * scale;
}

inline float Poly6Kernel(float dstSq, float hSq, float scale) {
    if (dstSq >= hSq) return 0.0f;
    float v = hSq - dstSq;
    return v * v * v * scale;
}

inline uint32_t NextPow2(uint32_t v) {
    uint32_t p = 1024u;
    while (p < v) p <<= 1;
    return p;
}

inline float HashFloat(uint32_t n) {
    n = (n << 13u) ^ n;
    n = n * (n * n * 15731u + 789221u) + 1376312589u;
    return static_cast<float>(n & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
}
} // namespace

FluidSimulator::FluidSimulator() {
    Initialize(m_Params);
}

void FluidSimulator::Initialize(const FluidPhysicsParams& params) {
    m_Params = params;
    m_Params.particleCount = std::clamp(params.particleCount, 128u, 4096u);
    Reset(m_Params);
}

void FluidSimulator::Reset(const FluidPhysicsParams& params) {
    m_Params = params;
    m_Params.particleCount = std::clamp(params.particleCount, 128u, 4096u);
    m_ElapsedTime = 0.0f;
    m_WaveWallOffset = 0.0f;
    SpawnParticles(m_Params.scenario, m_Params.particleCount);
}

void FluidSimulator::UpdateParams(const FluidPhysicsParams& params) {
    bool needRespawn = (params.particleCount != m_Params.particleCount) || (params.scenario != m_Params.scenario);
    m_Params = params;
    m_Params.particleCount = std::clamp(params.particleCount, 128u, 4096u);
    if (needRespawn) {
        SpawnParticles(m_Params.scenario, m_Params.particleCount);
    }
}

void FluidSimulator::SpawnParticles(WaterSpawnScenario scenario, uint32_t count) {
    m_Positions.resize(count);
    m_PredictedPositions.resize(count);
    m_Velocities.assign(count, Vec3(0.0f, 0.0f, 0.0f));
    m_Densities.assign(count, Vec2(m_Params.targetDensity, 0.0f));

    m_HashTableSize = NextPow2(count * 2u);
    m_SpatialEntries.resize(count);
    m_SpatialOffsets.assign(m_HashTableSize, 0xFFFFFFFFu);

    float halfW = std::max(0.6f, m_Params.boundsWidth * 0.5f);
    float maxH = std::max(1.2f, m_Params.boundsHeight);
    float halfD = std::max(0.6f, m_Params.boundsDepth * 0.5f);

    uint32_t side = static_cast<uint32_t>(std::ceil(std::cbrt(static_cast<double>(count))));
    float spacing = std::clamp(m_Params.smoothingRadius * 0.52f, 0.09f, 0.22f);

    if (scenario == WaterSpawnScenario::DAM_BREAK) {
        // Tall water column on the -X side of the tank
        uint32_t nx = std::max(4u, side - 2u);
        uint32_t nz = std::max(4u, side);
        uint32_t ny = std::max(4u, (count + nx * nz - 1u) / (nx * nz));

        float spanX = std::min(halfW * 0.92f, static_cast<float>(nx) * spacing);
        float spanZ = std::min(halfD * 1.75f, static_cast<float>(nz) * spacing);
        float spanY = std::min(maxH * 0.88f, static_cast<float>(ny) * spacing);

        for (uint32_t i = 0; i < count; ++i) {
            uint32_t ix = i % nx;
            uint32_t iz = (i / nx) % nz;
            uint32_t iy = i / (nx * nz);

            float fx = (nx > 1) ? static_cast<float>(ix) / static_cast<float>(nx - 1) : 0.5f;
            float fz = (nz > 1) ? static_cast<float>(iz) / static_cast<float>(nz - 1) : 0.5f;
            float fy = (ny > 1) ? static_cast<float>(iy) / static_cast<float>(ny - 1) : 0.5f;

            float jx = (HashFloat(i * 3u + 1u) - 0.5f) * 0.025f;
            float jy = (HashFloat(i * 3u + 2u) - 0.5f) * 0.025f;
            float jz = (HashFloat(i * 3u + 3u) - 0.5f) * 0.025f;

            m_Positions[i] = Vec3(
                -halfW + 0.14f + fx * spanX + jx,
                0.14f + fy * spanY + jy,
                -spanZ * 0.5f + fz * spanZ + jz
            );
            m_PredictedPositions[i] = m_Positions[i];
        }
    } else if (scenario == WaterSpawnScenario::DUAL_WAVE) {
        uint32_t halfCount = count / 2u;
        uint32_t nx = std::max(3u, side / 2u + 2u);
        uint32_t nz = std::max(4u, side);
        uint32_t ny = std::max(3u, (halfCount + nx * nz - 1u) / (nx * nz));

        float spanX = std::min(halfW * 0.65f, static_cast<float>(nx) * spacing);
        float spanZ = std::min(halfD * 1.7f, static_cast<float>(nz) * spacing);
        float spanY = std::min(maxH * 0.78f, static_cast<float>(ny) * spacing);

        for (uint32_t i = 0; i < count; ++i) {
            bool rightBlock = (i >= halfCount);
            uint32_t localI = rightBlock ? (i - halfCount) : i;
            uint32_t ix = localI % nx;
            uint32_t iz = (localI / nx) % nz;
            uint32_t iy = localI / (nx * nz);

            float fx = (nx > 1) ? static_cast<float>(ix) / static_cast<float>(nx - 1) : 0.5f;
            float fz = (nz > 1) ? static_cast<float>(iz) / static_cast<float>(nz - 1) : 0.5f;
            float fy = (ny > 1) ? static_cast<float>(iy) / static_cast<float>(ny - 1) : 0.5f;

            float xPos = rightBlock
                ? (halfW - 0.14f - fx * spanX)
                : (-halfW + 0.14f + fx * spanX);

            m_Positions[i] = Vec3(
                xPos + (HashFloat(i * 5u + 1u) - 0.5f) * 0.02f,
                0.14f + fy * spanY,
                -spanZ * 0.5f + fz * spanZ
            );
            m_Velocities[i] = Vec3(rightBlock ? -1.4f : 1.4f, 0.0f, 0.0f);
            m_PredictedPositions[i] = m_Positions[i];
        }
    } else if (scenario == WaterSpawnScenario::FOUNTAIN_DROP) {
        // 65% resting pool + 35% elevated water sphere drop
        uint32_t poolCount = (count * 65u) / 100u;
        uint32_t dropCount = count - poolCount;

        uint32_t pSide = static_cast<uint32_t>(std::ceil(std::sqrt(static_cast<double>(poolCount))));
        for (uint32_t i = 0; i < poolCount; ++i) {
            uint32_t ix = i % pSide;
            uint32_t iz = (i / pSide) % pSide;
            uint32_t iy = i / (pSide * pSide);
            float fx = (pSide > 1) ? static_cast<float>(ix) / static_cast<float>(pSide - 1) : 0.5f;
            float fz = (pSide > 1) ? static_cast<float>(iz) / static_cast<float>(pSide - 1) : 0.5f;
            m_Positions[i] = Vec3(
                (-halfW + 0.15f) + fx * (halfW * 2.0f - 0.30f),
                0.12f + static_cast<float>(iy) * spacing * 0.85f,
                (-halfD + 0.15f) + fz * (halfD * 2.0f - 0.30f)
            );
            m_PredictedPositions[i] = m_Positions[i];
        }

        float dropRadius = std::min({halfW, halfD, maxH * 0.4f}) * 0.45f;
        Vec3 dropCenter(0.0f, maxH * 0.72f, 0.0f);
        for (uint32_t j = 0; j < dropCount; ++j) {
            uint32_t i = poolCount + j;
            float u = HashFloat(j * 7u + 11u);
            float v = HashFloat(j * 7u + 13u);
            float w = HashFloat(j * 7u + 17u);
            float theta = u * 2.0f * kPi;
            float phi = std::acos(1.0f - 2.0f * v);
            float r = dropRadius * std::cbrt(w);
            m_Positions[i] = dropCenter + Vec3(
                r * std::sin(phi) * std::cos(theta),
                r * std::cos(phi),
                r * std::sin(phi) * std::sin(theta)
            );
            m_Velocities[i] = Vec3(0.0f, -1.8f, 0.0f);
            m_PredictedPositions[i] = m_Positions[i];
        }
    } else if (scenario == WaterSpawnScenario::SLOSH_TANK) {
        // Full floor pool with initial slosh momentum
        uint32_t nx = std::max(5u, side + 1u);
        uint32_t nz = std::max(5u, side);
        float spanX = halfW * 1.82f;
        float spanZ = halfD * 1.82f;
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t ix = i % nx;
            uint32_t iz = (i / nx) % nz;
            uint32_t iy = i / (nx * nz);
            float fx = static_cast<float>(ix) / static_cast<float>(nx - 1);
            float fz = static_cast<float>(iz) / static_cast<float>(nz - 1);
            float x = -spanX * 0.5f + fx * spanX;
            float z = -spanZ * 0.5f + fz * spanZ;
            float y = 0.14f + static_cast<float>(iy) * spacing * 0.85f + (fx * 0.35f);
            m_Positions[i] = Vec3(x, y, z);
            m_Velocities[i] = Vec3(1.65f * (1.0f - fx), 0.35f * std::sin(fx * kPi), 0.25f * std::cos(fz * kPi));
            m_PredictedPositions[i] = m_Positions[i];
        }
    } else if (scenario == WaterSpawnScenario::WATERFALL_CLIFF) {
        // 32% upper river plateau + 33% falling nappe curtain + 35% plunge basin pool
        const float cliffH = std::clamp(m_Params.waterfallCliffHeight, 0.55f, std::max(0.85f, maxH - 0.22f));
        const float lipX = std::clamp(-halfW + m_Params.waterfallCliffDepth, -halfW + 0.35f, halfW - 0.65f);
        const float chuteHalfZ = std::clamp(m_Params.waterfallCliffWidth * 0.5f, 0.22f, halfD - 0.04f);
        const float streamH = std::clamp(0.16f + 0.16f * m_Params.waterfallFlowVolume, 0.12f, 0.52f);
        const float inflowVx = 0.95f + 1.25f * m_Params.waterfallFlowVolume;
        const float poolH = std::clamp(m_Params.plungePoolDepth, 0.06f, 0.85f);

        uint32_t upperCount = (count * 32u) / 100u;
        uint32_t cascadeCount = (count * 33u) / 100u;
        uint32_t poolCount = count - upperCount - cascadeCount;

        for (uint32_t i = 0; i < upperCount; ++i) {
            float u = HashFloat(i * 5u + 1u);
            float v = HashFloat(i * 5u + 2u);
            float w = HashFloat(i * 5u + 3u);
            float x = (-halfW + 0.08f) + u * std::max(0.18f, (lipX - (-halfW + 0.10f)));
            float slopeDrop = std::max(0.0f, (x - (lipX - 0.42f)) / 0.42f) * 0.36f *
                              std::tan(std::clamp(m_Params.cliffLipSlopeDeg, 0.0f, 45.0f) * (kPi / 180.0f));
            float y = (cliffH - slopeDrop) + 0.04f + v * streamH;
            float z = (w * 2.0f - 1.0f) * chuteHalfZ * 0.90f;
            m_Positions[i] = Vec3(x, y, z);
            m_Velocities[i] = Vec3(inflowVx, -0.15f, 0.0f);
            m_PredictedPositions[i] = m_Positions[i];
        }

        for (uint32_t j = 0; j < cascadeCount; ++j) {
            uint32_t i = upperCount + j;
            float t = (cascadeCount > 1) ? static_cast<float>(j) / static_cast<float>(cascadeCount - 1) : 0.5f;
            float jx = (HashFloat(j * 7u + 11u) - 0.5f) * 0.14f;
            float jz = (HashFloat(j * 7u + 13u) - 0.5f) * 2.0f;
            float fallDist = t * std::max(0.3f, cliffH - poolH * 0.5f);
            float fallTime = std::sqrt(2.0f * fallDist / std::max(2.0f, std::abs(m_Params.gravityY)));
            float x = std::min(halfW - 0.12f, lipX + inflowVx * fallTime * 0.68f + jx);
            float y = std::max(0.08f, cliffH - fallDist);
            float z = std::clamp(jz * chuteHalfZ * 0.92f, -chuteHalfZ * 0.95f, chuteHalfZ * 0.95f);
            m_Positions[i] = Vec3(x, y, z);
            m_Velocities[i] = Vec3(inflowVx * 0.85f, -std::abs(m_Params.gravityY) * fallTime * 0.75f, 0.0f);
            m_PredictedPositions[i] = m_Positions[i];
        }

        for (uint32_t j = 0; j < poolCount; ++j) {
            uint32_t i = upperCount + cascadeCount + j;
            float u = HashFloat(j * 11u + 19u);
            float v = HashFloat(j * 11u + 23u);
            float w = HashFloat(j * 11u + 29u);
            float poolMinX = std::min(halfW - 0.25f, lipX + 0.18f);
            float x = poolMinX + u * std::max(0.20f, (halfW - 0.08f - poolMinX));
            float y = 0.06f + v * poolH;
            float z = (w * 2.0f - 1.0f) * (halfD - 0.10f);
            m_Positions[i] = Vec3(x, y, z);
            m_Velocities[i] = Vec3(0.35f, 0.0f, 0.0f);
            m_PredictedPositions[i] = m_Positions[i];
        }
    } else {
        // ZERO_G_ORB: Cohesive floating water sphere in center of tank
        float orbRadius = std::min({halfW, halfD, maxH * 0.5f}) * 0.56f;
        Vec3 center(0.0f, maxH * 0.50f, 0.0f);
        for (uint32_t i = 0; i < count; ++i) {
            float u = HashFloat(i * 11u + 3u);
            float v = HashFloat(i * 11u + 5u);
            float w = HashFloat(i * 11u + 9u);
            float theta = u * 2.0f * kPi;
            float phi = std::acos(1.0f - 2.0f * v);
            float r = orbRadius * std::cbrt(w);
            Vec3 rel(
                r * std::sin(phi) * std::cos(theta),
                r * std::cos(phi),
                r * std::sin(phi) * std::sin(theta)
            );
            m_Positions[i] = center + rel;
            m_Velocities[i] = Vec3(-rel.z * 0.7f, 0.1f * rel.y, rel.x * 0.7f);
            m_PredictedPositions[i] = m_Positions[i];
        }
    }
}

uint32_t FluidSimulator::HashCell3D(int32_t cx, int32_t cy, int32_t cz) {
    uint32_t a = static_cast<uint32_t>(cx) * kPrime1;
    uint32_t b = static_cast<uint32_t>(cy) * kPrime2;
    uint32_t c = static_cast<uint32_t>(cz) * kPrime3;
    return a + b + c;
}

void FluidSimulator::Step(float frameDt) {
    auto t0 = std::chrono::steady_clock::now();

    uint32_t substeps = std::clamp(m_Params.substeps, 1u, 4u);
    float clampedFrameDt = std::clamp(frameDt, 0.001f, 0.032f) * std::clamp(m_Params.timeScale, 0.1f, 2.5f);
    float dt = clampedFrameDt / static_cast<float>(substeps);

    for (uint32_t s = 0; s < substeps; ++s) {
        m_ElapsedTime += dt;
        if (m_Params.waveMakerEnabled) {
            float maxAmp = std::min(m_Params.waveAmplitude, m_Params.boundsWidth * 0.28f);
            m_WaveWallOffset = 0.5f * maxAmp * (1.0f - std::cos(m_ElapsedTime * m_Params.waveFrequency * 2.0f * kPi));
        } else {
            m_WaveWallOffset = 0.0f;
        }

        // Continuous weir discharge recirculation when Waterfall Cliff mode is active
        if (m_Params.waterfallEnabled || m_Params.scenario == WaterSpawnScenario::WATERFALL_CLIFF) {
            const float halfW = std::max(0.6f, m_Params.boundsWidth * 0.5f);
            const float halfD = std::max(0.6f, m_Params.boundsDepth * 0.5f);
            const float maxH = std::max(1.2f, m_Params.boundsHeight);
            const float cliffH = std::clamp(m_Params.waterfallCliffHeight, 0.55f, std::max(0.85f, maxH - 0.22f));
            const float chuteHalfZ = std::clamp(m_Params.waterfallCliffWidth * 0.5f, 0.22f, halfD - 0.04f);
            const float streamH = std::clamp(0.14f + 0.16f * m_Params.waterfallFlowVolume, 0.10f, 0.48f);
            const float inflowVx = 0.85f + 1.35f * m_Params.waterfallFlowVolume;
            const float poolLimitY = std::clamp(m_Params.plungePoolDepth, 0.05f, 0.85f) + 0.28f;

            const uint32_t n = static_cast<uint32_t>(m_Positions.size());
            const uint32_t maxRecyclePerSubstep = std::max(
                2u,
                static_cast<uint32_t>(static_cast<float>(n) * 0.55f * m_Params.waterfallFlowVolume * dt)
            );
            uint32_t recycled = 0;
            uint32_t timeSalt = static_cast<uint32_t>(m_ElapsedTime * 1000.0f);

            for (uint32_t i = 0; i < n && recycled < maxRecyclePerSubstep; ++i) {
                const Vec3& p = m_Positions[i];
                bool atOutflowWall = (p.x > halfW - 0.20f && p.y < poolLimitY);
                bool stagnantFloor = (p.x > halfW * 0.35f && p.y < 0.22f && m_Velocities[i].LengthSq() < 0.65f);
                if (atOutflowWall || stagnantFloor) {
                    float r1 = HashFloat(i * 13u + timeSalt + recycled * 3u);
                    float r2 = HashFloat(i * 17u + timeSalt + recycled * 7u);
                    float r3 = HashFloat(i * 23u + timeSalt + recycled * 11u);
                    m_Positions[i] = Vec3(
                        -halfW + 0.05f + r1 * 0.24f,
                        cliffH + 0.04f + r2 * streamH,
                        (r3 * 2.0f - 1.0f) * chuteHalfZ * 0.88f
                    );
                    m_Velocities[i] = Vec3(inflowVx, -0.05f, (r2 - 0.5f) * 0.25f);
                    m_PredictedPositions[i] = m_Positions[i];
                    ++recycled;
                }
            }
        }

        PredictPositions(dt);
        BuildSpatialHashAndOffsets();
        ComputeDensities();
        ComputeForcesAndIntegrate(dt);
    }

    auto t1 = std::chrono::steady_clock::now();
    m_LastStepMs = std::chrono::duration<float, std::milli>(t1 - t0).count();
}

void FluidSimulator::PredictPositions(float dt) {
    const size_t n = m_Positions.size();
    const Vec3 externalAccel(
        m_Params.gravityX + m_Params.windForceX,
        m_Params.gravityY,
        m_Params.gravityZ + m_Params.windForceZ
    );
    const float predDt = dt * 0.75f;
    const bool waterfallActive = (m_Params.waterfallEnabled || m_Params.scenario == WaterSpawnScenario::WATERFALL_CLIFF);
    const float airDrag = waterfallActive ? std::clamp(1.0f - m_Params.airDragAndMist * 0.45f * dt, 0.85f, 1.0f) : 1.0f;
    const float mistAmp = waterfallActive ? m_Params.airDragAndMist * 1.45f : 0.0f;
    const uint32_t tSalt = static_cast<uint32_t>(m_ElapsedTime * 600.0f);

    for (size_t i = 0; i < n; ++i) {
        if (!std::isfinite(m_Positions[i].x) || !std::isfinite(m_Positions[i].y) || !std::isfinite(m_Positions[i].z) ||
            !std::isfinite(m_Velocities[i].x) || !std::isfinite(m_Velocities[i].y) || !std::isfinite(m_Velocities[i].z)) {
            m_Positions[i] = Vec3(0.0f, 0.5f, 0.0f);
            m_Velocities[i] = Vec3(0.0f, 0.0f, 0.0f);
        }
        m_Velocities[i] += externalAccel * dt;
        if (waterfallActive && mistAmp > 0.01f && m_Positions[i].y > m_Params.plungePoolDepth + 0.12f) {
            m_Velocities[i] *= airDrag;
            float jx = (HashFloat(static_cast<uint32_t>(i) * 31u + tSalt) - 0.5f) * mistAmp * dt;
            float jz = (HashFloat(static_cast<uint32_t>(i) * 37u + tSalt) - 0.5f) * mistAmp * dt;
            m_Velocities[i].x += jx;
            m_Velocities[i].z += jz;
        }
        m_PredictedPositions[i] = m_Positions[i] + m_Velocities[i] * predDt;
        if (!std::isfinite(m_PredictedPositions[i].x) || !std::isfinite(m_PredictedPositions[i].y) || !std::isfinite(m_PredictedPositions[i].z)) {
            m_PredictedPositions[i] = m_Positions[i];
        }
    }
}

void FluidSimulator::BuildSpatialHashAndOffsets() {
    const uint32_t n = static_cast<uint32_t>(m_Positions.size());
    const float h = std::max(m_Params.smoothingRadius, 0.08f);
    const float invH = 1.0f / h;

    std::fill(m_SpatialOffsets.begin(), m_SpatialOffsets.end(), 0xFFFFFFFFu);

    for (uint32_t i = 0; i < n; ++i) {
        const Vec3& p = m_PredictedPositions[i];
        float px = std::isfinite(p.x) ? std::clamp(p.x * invH, -1.0e6f, 1.0e6f) : 0.0f;
        float py = std::isfinite(p.y) ? std::clamp(p.y * invH, -1.0e6f, 1.0e6f) : 0.0f;
        float pz = std::isfinite(p.z) ? std::clamp(p.z * invH, -1.0e6f, 1.0e6f) : 0.0f;
        int32_t cx = static_cast<int32_t>(std::floor(px));
        int32_t cy = static_cast<int32_t>(std::floor(py));
        int32_t cz = static_cast<int32_t>(std::floor(pz));

        uint32_t hash = HashCell3D(cx, cy, cz);
        uint32_t key = hash & (m_HashTableSize - 1u);

        m_SpatialEntries[i].particleIndex = i;
        m_SpatialEntries[i].hash = hash;
        m_SpatialEntries[i].cellKey = key;
    }

    std::sort(m_SpatialEntries.begin(), m_SpatialEntries.end(),
        [](const SpatialEntry& a, const SpatialEntry& b) {
            return a.cellKey < b.cellKey;
        });

    uint32_t occupied = 0;
    uint32_t prevKey = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t key = m_SpatialEntries[i].cellKey;
        if (key != prevKey) {
            m_SpatialOffsets[key] = i;
            prevKey = key;
            ++occupied;
        }
    }
    m_OccupiedCells = occupied;
}

void FluidSimulator::ComputeDensities() {
    const uint32_t n = static_cast<uint32_t>(m_Positions.size());
    const float h = std::max(m_Params.smoothingRadius, 0.08f);
    const float invH = 1.0f / h;
    const float hSq = h * h;

    const float pow5 = h * h * h * h * h;
    const float pow6 = pow5 * h;
    const float spikyPow2Scale = 15.0f / (2.0f * kPi * pow5);
    const float spikyPow3Scale = 15.0f / (kPi * pow6);

    double densitySum = 0.0;

    for (uint32_t i = 0; i < n; ++i) {
        const Vec3& pos = m_PredictedPositions[i];
        float px = std::isfinite(pos.x) ? std::clamp(pos.x * invH, -1.0e6f, 1.0e6f) : 0.0f;
        float py = std::isfinite(pos.y) ? std::clamp(pos.y * invH, -1.0e6f, 1.0e6f) : 0.0f;
        float pz = std::isfinite(pos.z) ? std::clamp(pos.z * invH, -1.0e6f, 1.0e6f) : 0.0f;
        int32_t ox = static_cast<int32_t>(std::floor(px));
        int32_t oy = static_cast<int32_t>(std::floor(py));
        int32_t oz = static_cast<int32_t>(std::floor(pz));

        float density = 0.0f;
        float nearDensity = 0.0f;

        for (int32_t dx = -1; dx <= 1; ++dx) {
            for (int32_t dy = -1; dy <= 1; ++dy) {
                for (int32_t dz = -1; dz <= 1; ++dz) {
                    uint32_t hash = HashCell3D(ox + dx, oy + dy, oz + dz);
                    uint32_t key = hash & (m_HashTableSize - 1u);
                    uint32_t curr = m_SpatialOffsets[key];

                    while (curr < n) {
                        const SpatialEntry& entry = m_SpatialEntries[curr++];
                        if (entry.cellKey != key) break;
                        if (entry.hash != hash) continue;

                        uint32_t j = entry.particleIndex;
                        Vec3 diff = m_PredictedPositions[j] - pos;
                        float sqDst = diff.LengthSq();
                        if (sqDst < hSq) {
                            float dst = std::sqrt(sqDst);
                            density += SpikyKernelPow2(dst, h, spikyPow2Scale);
                            nearDensity += SpikyKernelPow3(dst, h, spikyPow3Scale);
                        }
                    }
                }
            }
        }

        m_Densities[i] = Vec2(std::max(density, 0.001f), std::max(nearDensity, 0.001f));
        densitySum += static_cast<double>(density);
    }

    m_AvgDensity = (n > 0) ? static_cast<float>(densitySum / static_cast<double>(n)) : 0.0f;
}

void FluidSimulator::ComputeForcesAndIntegrate(float dt) {
    const uint32_t n = static_cast<uint32_t>(m_Positions.size());
    const float h = std::max(m_Params.smoothingRadius, 0.08f);
    const float invH = 1.0f / h;
    const float hSq = h * h;

    const float pow5 = h * h * h * h * h;
    const float pow6 = pow5 * h;
    const float pow9 = pow6 * h * h * h;

    const float dSpikyPow2Scale = 15.0f / (pow5 * kPi);
    const float dSpikyPow3Scale = 45.0f / (pow6 * kPi);
    const float poly6Scale = 315.0f / (64.0f * kPi * pow9);

    const float targetDensity = m_Params.targetDensity;
    const float pressureMult = m_Params.pressureMultiplier;
    const float nearPressureMult = m_Params.nearPressureMultiplier;
    const float viscosity = m_Params.viscosityStrength;
    const float tension = m_Params.surfaceTension;

    float maxSqSpeed = 0.0f;
    double keSum = 0.0;

    for (uint32_t i = 0; i < n; ++i) {
        const Vec3& pos = m_PredictedPositions[i];
        const Vec3& vel = m_Velocities[i];
        float density = std::isfinite(m_Densities[i].x) ? std::max(m_Densities[i].x, 1e-5f) : 1.0f;
        float nearDensity = std::isfinite(m_Densities[i].y) ? std::max(m_Densities[i].y, 1e-5f) : 1.0f;

        float pressure = (density - targetDensity) * pressureMult;
        float nearPressure = nearDensity * nearPressureMult;

        Vec3 pressureForce(0.0f, 0.0f, 0.0f);
        Vec3 viscosityForce(0.0f, 0.0f, 0.0f);
        Vec3 cohesionForce(0.0f, 0.0f, 0.0f);

        float px = std::isfinite(pos.x) ? std::clamp(pos.x * invH, -1.0e6f, 1.0e6f) : 0.0f;
        float py = std::isfinite(pos.y) ? std::clamp(pos.y * invH, -1.0e6f, 1.0e6f) : 0.0f;
        float pz = std::isfinite(pos.z) ? std::clamp(pos.z * invH, -1.0e6f, 1.0e6f) : 0.0f;
        int32_t ox = static_cast<int32_t>(std::floor(px));
        int32_t oy = static_cast<int32_t>(std::floor(py));
        int32_t oz = static_cast<int32_t>(std::floor(pz));

        for (int32_t dx = -1; dx <= 1; ++dx) {
            for (int32_t dy = -1; dy <= 1; ++dy) {
                for (int32_t dz = -1; dz <= 1; ++dz) {
                    uint32_t hash = HashCell3D(ox + dx, oy + dy, oz + dz);
                    uint32_t key = hash & (m_HashTableSize - 1u);
                    uint32_t curr = m_SpatialOffsets[key];

                    while (curr < n) {
                        const SpatialEntry& entry = m_SpatialEntries[curr++];
                        if (entry.cellKey != key) break;
                        if (entry.hash != hash) continue;

                        uint32_t j = entry.particleIndex;
                        if (j == i) continue;

                        Vec3 offset = m_PredictedPositions[j] - pos;
                        float sqDst = offset.LengthSq();
                        if (sqDst < hSq) {
                            float dst = std::sqrt(sqDst);
                            Vec3 dir = (dst > 1e-5f) ? (offset / dst) : Vec3(0.0f, 1.0f, 0.0f);

                            float nDens = std::isfinite(m_Densities[j].x) ? std::max(m_Densities[j].x, 1e-5f) : 1.0f;
                            float nNearDens = std::isfinite(m_Densities[j].y) ? std::max(m_Densities[j].y, 1e-5f) : 1.0f;

                            float nPress = (nDens - targetDensity) * pressureMult;
                            float nNearPress = nNearDens * nearPressureMult;

                            float sharedPress = 0.5f * (pressure + nPress);
                            float sharedNearPress = 0.5f * (nearPressure + nNearPress);

                            pressureForce += dir * (DerivativeSpikyPow2(dst, h, dSpikyPow2Scale) * sharedPress / nDens);
                            pressureForce += dir * (DerivativeSpikyPow3(dst, h, dSpikyPow3Scale) * sharedNearPress / nNearDens);

                            float w = Poly6Kernel(sqDst, hSq, poly6Scale);
                            viscosityForce += (m_Velocities[j] - vel) * w;
                            cohesionForce += dir * (w * (dst * invH));
                        }
                    }
                }
            }
        }

        Vec3 totalAccel = (pressureForce / density) + viscosityForce * viscosity + cohesionForce * tension;
        if (std::isfinite(totalAccel.x) && std::isfinite(totalAccel.y) && std::isfinite(totalAccel.z)) {
            m_Velocities[i] += totalAccel * dt;
        }

        // Stability velocity clamp
        float vSq = m_Velocities[i].LengthSq();
        if (!std::isfinite(vSq)) {
            m_Velocities[i] = Vec3(0.0f, 0.0f, 0.0f);
            vSq = 0.0f;
        } else if (vSq > 225.0f) {
            m_Velocities[i] *= (15.0f / std::sqrt(vSq));
            vSq = 225.0f;
        }
        maxSqSpeed = std::max(maxSqSpeed, vSq);
        keSum += 0.5 * static_cast<double>(vSq);

        m_Positions[i] += m_Velocities[i] * dt;
        ResolveCollisions(i, dt);
    }

    m_MaxSpeed = std::sqrt(maxSqSpeed);
    m_KineticEnergy = static_cast<float>(keSum);
}

void FluidSimulator::ResolveCollisions(uint32_t idx, float dt) {
    Vec3& pos = m_Positions[idx];
    Vec3& vel = m_Velocities[idx];
    float damping = std::clamp(m_Params.collisionDamping, 0.05f, 0.99f);

    float halfW = std::max(0.6f, m_Params.boundsWidth * 0.5f);
    float maxH = std::max(1.2f, m_Params.boundsHeight);
    float halfD = std::max(0.6f, m_Params.boundsDepth * 0.5f);

    float minX = -halfW + m_WaveWallOffset;
    float maxX = halfW;
    float minY = 0.02f;
    float maxY = maxH;
    float minZ = -halfD;
    float maxZ = halfD;

    if (pos.x < minX) {
        pos.x = minX;
        vel.x = std::abs(vel.x) * damping;
        if (m_Params.waveMakerEnabled) {
            float wallVel = m_Params.waveAmplitude * m_Params.waveFrequency * kPi *
                            std::sin(m_ElapsedTime * m_Params.waveFrequency * 2.0f * kPi);
            vel.x += std::max(0.0f, wallVel) * 0.55f;
        }
    } else if (pos.x > maxX) {
        pos.x = maxX;
        vel.x = -std::abs(vel.x) * damping;
    }

    if (pos.y < minY) {
        pos.y = minY;
        vel.y = std::abs(vel.y) * damping;
    } else if (pos.y > maxY) {
        pos.y = maxY;
        vel.y = -std::abs(vel.y) * damping;
    }

    if (pos.z < minZ) {
        pos.z = minZ;
        vel.z = std::abs(vel.z) * damping;
    } else if (pos.z > maxZ) {
        pos.z = maxZ;
        vel.z = -std::abs(vel.z) * damping;
    }

    if (m_Params.waterfallEnabled || m_Params.scenario == WaterSpawnScenario::WATERFALL_CLIFF) {
        float cliffDist = CliffSDF(pos, m_Params);
        constexpr float kCliffMargin = 0.038f;
        if (cliffDist < kCliffMargin) {
            Vec3 n = CliffNormal(pos, m_Params);
            pos += n * (kCliffMargin - cliffDist);
            float vn = Vec3::Dot(vel, n);
            if (vn < 0.0f) {
                Vec3 vNormal = n * vn;
                Vec3 vTangent = vel - vNormal;
                float rockFriction = std::clamp(m_Params.cliffRoughness * 0.35f, 0.0f, 0.45f);
                vel = vTangent * (1.0f - rockFriction) - vNormal * (damping * 0.45f);
            }
        }
    }

    if (m_Params.obstacleEnabled) {
        Vec3 center(m_Params.obstacleX, m_Params.obstacleY, m_Params.obstacleZ);
        Vec3 rel = pos - center;
        float minDist = std::max(0.15f, m_Params.obstacleRadius) + 0.04f;
        float distSq = rel.LengthSq();
        if (distSq < minDist * minDist) {
            float dist = std::sqrt(distSq);
            Vec3 n = (dist > 1e-5f) ? (rel / dist) : Vec3(0.0f, 1.0f, 0.0f);
            pos = center + n * minDist;
            float vn = Vec3::Dot(vel, n);
            if (vn < 0.0f) {
                vel -= n * ((1.0f + damping) * vn);
            }
        }
    }
    (void)dt;
}

void FluidSimulator::InjectImpulse(const Vec3& worldPoint, const Vec3& impulseVelocity, float radius) {
    float rSq = radius * radius;
    const size_t n = m_Positions.size();
    for (size_t i = 0; i < n; ++i) {
        Vec3 diff = m_Positions[i] - worldPoint;
        float dSq = diff.LengthSq();
        if (dSq < rSq) {
            float falloff = 1.0f - std::sqrt(dSq) / radius;
            m_Velocities[i] += impulseVelocity * falloff;
        }
    }
}

void FluidSimulator::InjectVortex(float strength) {
    const size_t n = m_Positions.size();
    for (size_t i = 0; i < n; ++i) {
        const Vec3& p = m_Positions[i];
        float r = std::sqrt(p.x * p.x + p.z * p.z) + 0.1f;
        Vec3 tangent(-p.z / r, 0.22f, p.x / r);
        m_Velocities[i] += tangent * strength;
    }
}

} // namespace sphere::water
