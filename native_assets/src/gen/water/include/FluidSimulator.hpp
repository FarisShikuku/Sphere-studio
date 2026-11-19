#pragma once

#include "FluidParams.hpp"
#include <vector>
#include <cstdint>

namespace sphere::water {

struct alignas(16) SpatialEntry {
    uint32_t particleIndex = 0;
    uint32_t hash = 0;
    uint32_t cellKey = 0;
    uint32_t pad = 0;
};

/**
 * FluidSimulator
 *
 * Complete 3D Smoothed Particle Hydrodynamics (SPH) fluid solver ported from
 * talvinckb/OpenGL-Water-Simulation to the Sphere Vulkan C++20 engine.
 *
 * Implements the full GPU-equivalent multi-pass pipeline:
 *  1. External forces & predicted positions (sph_predict_positions.comp)
 *  2. 3D Spatial hash construction (build_spatial_hash.comp)
 *  3. Spatial key sorting & cell offset lookup (bitonic_sort.comp / compute_offsets.comp)
 *  4. Double-density estimation via Spiky Pow2 & Pow3 kernels (sph_density.comp)
 *  5. Pressure, near-pressure, viscosity & surface tension forces (sph_forces.comp)
 *  6. Symplectic integration & container/obstacle collision response (sph_integrate.comp)
 */
class FluidSimulator {
public:
    FluidSimulator();
    ~FluidSimulator() = default;

    void Initialize(const FluidPhysicsParams& params);
    void Reset(const FluidPhysicsParams& params);
    void UpdateParams(const FluidPhysicsParams& params);

    void Step(float frameDt);
    void InjectImpulse(const Vec3& worldPoint, const Vec3& impulseVelocity, float radius);
    void InjectVortex(float strength);

    const std::vector<Vec3>& GetPositions() const { return m_Positions; }
    const std::vector<Vec3>& GetVelocities() const { return m_Velocities; }
    const std::vector<Vec2>& GetDensities() const { return m_Densities; } // x = density, y = nearDensity
    const std::vector<SpatialEntry>& GetSpatialIndices() const { return m_SpatialEntries; }
    const std::vector<uint32_t>& GetSpatialOffsets() const { return m_SpatialOffsets; }

    const FluidPhysicsParams& GetParams() const { return m_Params; }
    uint32_t GetOccupiedCellCount() const { return m_OccupiedCells; }
    uint32_t GetHashTableSize() const { return m_HashTableSize; }
    float GetCurrentWaveOffset() const { return m_WaveWallOffset; }
    float GetElapsedTime() const { return m_ElapsedTime; }
    float GetLastStepMs() const { return m_LastStepMs; }
    float GetAverageDensity() const { return m_AvgDensity; }
    float GetMaxSpeed() const { return m_MaxSpeed; }
    float GetKineticEnergy() const { return m_KineticEnergy; }

private:
    void SpawnParticles(WaterSpawnScenario scenario, uint32_t count);
    void PredictPositions(float dt);
    void BuildSpatialHashAndOffsets();
    void ComputeDensities();
    void ComputeForcesAndIntegrate(float dt);
    void ResolveCollisions(uint32_t idx, float dt);

    static uint32_t HashCell3D(int32_t cx, int32_t cy, int32_t cz);

    FluidPhysicsParams m_Params{};
    std::vector<Vec3> m_Positions;
    std::vector<Vec3> m_PredictedPositions;
    std::vector<Vec3> m_Velocities;
    std::vector<Vec2> m_Densities; // x = density, y = nearDensity

    std::vector<SpatialEntry> m_SpatialEntries;
    std::vector<uint32_t> m_SpatialOffsets;
    uint32_t m_HashTableSize = 4096;
    uint32_t m_OccupiedCells = 0;

    float m_ElapsedTime = 0.0f;
    float m_WaveWallOffset = 0.0f;
    float m_LastStepMs = 0.0f;
    float m_AvgDensity = 0.0f;
    float m_MaxSpeed = 0.0f;
    float m_KineticEnergy = 0.0f;
};

} // namespace sphere::water
