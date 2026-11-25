// ============================================================================
// Forest3DSystem.cpp
// Native C++17 Procedural 3D Forest Canopy & Ecosystem Implementation
// Part of the Unified Vegetation Subsystem (native-assets/src/vegetation/forest/)
// ============================================================================

#include "Forest3DSystem.h"
#include "../../core/Assert.h"
#include <algorithm>
#include <cmath>

namespace vegetation {

namespace {
constexpr float PI_F = 3.14159265358979323846f;
constexpr float BEER_LAMBERT_EXTINCTION_K = 0.52f; // Temperate broadleaf/conifer extinction
constexpr float WIND_CANOPY_ATTENUATION_ALPHA = 2.4f; // Canopy wind decay exponent
}

Forest3DSystem::Forest3DSystem(const Forest3DConfig& config)
    : m_config(config)
{
    // Define 4 structural vertical forest strata
    m_layers.push_back({"Emergent Layer", 22.0f, 32.0f, 0.15f, 0.8f});
    m_layers.push_back({"Main Canopy", 12.0f, 22.0f, 0.75f, 2.4f});
    m_layers.push_back({"Understory", 3.5f, 12.0f, 0.35f, 1.2f});
    m_layers.push_back({"Forest Floor", 0.0f, 3.5f, 0.95f, 0.5f});

    initializeForestStand();
    computeCanopyLightExtinction();
    distributeUnderstoryMoisture();
}

float Forest3DSystem::pseudoRandom(uint32_t& state) const {
    state = state * 1664525u + 1013904223u;
    return static_cast<float>(state & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
}

void Forest3DSystem::initializeForestStand() {
    m_trees.clear();
    uint32_t rng = m_config.randomSeed;

    const float halfW = m_config.forestAreaWidthMeters * 0.5f;
    const float halfD = m_config.forestAreaDepthMeters * 0.5f;
    const float minSqDist = m_config.minTreeDistanceMeters * m_config.minTreeDistanceMeters;

    // Poisson-disk sampling rejection iteration
    uint32_t attempts = 0;
    const uint32_t maxAttempts = m_config.targetTreeCount * 30;

    while (m_trees.size() < m_config.targetTreeCount && attempts < maxAttempts) {
        attempts++;
        float candidateX = -halfW + pseudoRandom(rng) * m_config.forestAreaWidthMeters;
        float candidateZ = -halfD + pseudoRandom(rng) * m_config.forestAreaDepthMeters;

        // Check distance to existing trees
        bool tooClose = false;
        for (const auto& t : m_trees) {
            float dx = t.posX - candidateX;
            float dz = t.posZ - candidateZ;
            if (dx * dx + dz * dz < minSqDist) {
                tooClose = true;
                break;
            }
        }
        if (tooClose) continue;

        // Valid position: synthesize tree parameters
        ForestTreeInstance tree;
        tree.instanceId = static_cast<uint32_t>(m_trees.size());
        tree.posX = candidateX;
        tree.posZ = candidateZ;
        // Mild undulating terrain elevation
        tree.posY = std::sin(candidateX * 0.04f) * 0.8f + std::cos(candidateZ * 0.05f) * 0.6f;

        // Mixed species distribution (60% Oak/Temperate, 25% Pine, 15% Birch)
        float speciesRoll = pseudoRandom(rng);
        if (speciesRoll < 0.60f) {
            tree.speciesId = 0; // Oak
            tree.crownHeight = 9.0f + pseudoRandom(rng) * 4.5f;
            tree.trunkHeight = 3.5f + pseudoRandom(rng) * 2.0f;
            tree.dbhMeters = 0.40f + pseudoRandom(rng) * 0.35f;
            tree.leafAreaIndex = 4.2f + pseudoRandom(rng) * 1.5f;
        } else if (speciesRoll < 0.85f) {
            tree.speciesId = 1; // Pine
            tree.crownHeight = 12.0f + pseudoRandom(rng) * 6.0f;
            tree.trunkHeight = 4.5f + pseudoRandom(rng) * 2.5f;
            tree.dbhMeters = 0.32f + pseudoRandom(rng) * 0.28f;
            tree.leafAreaIndex = 3.6f + pseudoRandom(rng) * 1.2f;
        } else {
            tree.speciesId = 2; // Birch
            tree.crownHeight = 11.0f + pseudoRandom(rng) * 4.0f;
            tree.trunkHeight = 4.0f + pseudoRandom(rng) * 2.2f;
            tree.dbhMeters = 0.28f + pseudoRandom(rng) * 0.20f;
            tree.leafAreaIndex = 3.2f + pseudoRandom(rng) * 1.0f;
        }

        // Asymmetric crown horizontal radii
        float baseRadius = 2.8f + pseudoRandom(rng) * 1.8f;
        tree.crownRadiusXPos = baseRadius * (0.85f + pseudoRandom(rng) * 0.30f);
        tree.crownRadiusXNeg = baseRadius * (0.85f + pseudoRandom(rng) * 0.30f);
        tree.crownRadiusYPos = baseRadius * (0.85f + pseudoRandom(rng) * 0.30f);
        tree.crownRadiusYNeg = baseRadius * (0.85f + pseudoRandom(rng) * 0.30f);

        tree.ageYears = 30.0f + pseudoRandom(rng) * 50.0f;
        m_trees.push_back(tree);
    }

    ENGINE_CORE_ASSERT(!m_trees.empty(), "Forest3DSystem must generate at least 1 tree instance");
}

void Forest3DSystem::computeCanopyLightExtinction() {
    for (auto& tree : m_trees) {
        // Beer-Lambert transmittance T = exp(-k * LAI)
        float k = BEER_LAMBERT_EXTINCTION_K;
        tree.canopyTransmittance = std::exp(-k * tree.leafAreaIndex);
        tree.canopyTransmittance = std::clamp(tree.canopyTransmittance, 0.05f, 0.45f);
    }
}

void Forest3DSystem::distributeUnderstoryMoisture() {
    m_soil.leafLitterDepthCm = 4.5f;
    m_soil.soilMoistureFraction = 0.68f;
    m_soil.organicMatterRatio = 0.14f;
    m_soil.slopeDeg = 3.2f;
}

float Forest3DSystem::computeFloorIrradiance(float worldX, float worldZ) const {
    float cumulativeLAI = 0.0f;

    for (const auto& tree : m_trees) {
        float dx = worldX - tree.posX;
        float dz = worldZ - tree.posZ;
        float distSq = dx * dx + dz * dz;

        float maxR = std::max({tree.crownRadiusXPos, tree.crownRadiusXNeg,
                               tree.crownRadiusYPos, tree.crownRadiusYNeg});
        if (distSq < maxR * maxR) {
            float normDist = std::sqrt(distSq) / maxR;
            float radialFalloff = 1.0f - normDist * normDist;
            cumulativeLAI += tree.leafAreaIndex * radialFalloff;
        }
    }

    // Solar constant normalized to 1.0 at top of atmosphere / open sky
    float solarSin = std::sin(m_config.solarElevationDeg * PI_F / 180.0f);
    float irradiance = solarSin * std::exp(-BEER_LAMBERT_EXTINCTION_K * cumulativeLAI);
    return std::clamp(irradiance, 0.08f, 1.0f);
}

float Forest3DSystem::computeFloorWindSpeed(float worldX, float worldZ) const {
    float dummy = 0.0f;
    float cumulativeCover = 0.0f;

    for (const auto& tree : m_trees) {
        float dx = worldX - tree.posX;
        float dz = worldZ - tree.posZ;
        float maxR = std::max({tree.crownRadiusXPos, tree.crownRadiusXNeg,
                               tree.crownRadiusYPos, tree.crownRadiusYNeg});
        float dist = std::sqrt(dx * dx + dz * dz);
        if (dist < maxR) {
            cumulativeCover += (1.0f - dist / maxR);
        }
    }

    float coverNorm = std::clamp(cumulativeCover, 0.0f, 1.0f);
    float windDecay = std::exp(-WIND_CANOPY_ATTENUATION_ALPHA * coverNorm);
    return m_config.ambientWindSpeedMps * windDecay;
}

bool Forest3DSystem::isUnderCrown(float worldX, float worldZ, float& outCanopyHeight) const {
    outCanopyHeight = 0.0f;
    bool under = false;

    for (const auto& tree : m_trees) {
        float dx = worldX - tree.posX;
        float dz = worldZ - tree.posZ;
        float rx = (dx >= 0.0f) ? tree.crownRadiusXPos : tree.crownRadiusXNeg;
        float rz = (dz >= 0.0f) ? tree.crownRadiusYPos : tree.crownRadiusYNeg;

        float normDist = (dx * dx) / (rx * rx + 0.001f) + (dz * dz) / (rz * rz + 0.001f);
        if (normDist <= 1.0f) {
            under = true;
            float heightAtPoint = tree.posY + tree.trunkHeight + tree.crownHeight * (1.0f - std::sqrt(normDist) * 0.4f);
            if (heightAtPoint > outCanopyHeight) {
                outCanopyHeight = heightAtPoint;
            }
        }
    }
    return under;
}

} // namespace vegetation
