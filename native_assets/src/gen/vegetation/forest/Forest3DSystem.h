#pragma once
// ============================================================================
// Forest3DSystem.h
// Native C++17 Procedural 3D Forest Canopy & Ecosystem Architecture
// Part of the Unified Vegetation Subsystem (native-assets/src/vegetation/forest/)
//
// Synthesizes complete 3D forest stands including:
//  - Multi-tier canopy (Emergent, Canopy, Understory, Forest Floor)
//  - PlantGL Asymmetric Crown Hulls (Pradal & Boudon)
//  - Leonardo da Vinci Pipe Model L-System trunk branching
//  - Beer-Lambert understory solar irradiance attenuation
//  - Soil nutrient & moisture distribution
//  - Spatial Poisson-disk tree spacing
// ============================================================================

#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include "../common/VegetationMath.h"
#include "../trees/TreeSpecies.h"

namespace vegetation {

struct ForestTreeInstance {
    uint32_t instanceId{0};
    uint32_t speciesId{0};        // 0: Oak, 1: Pine, 2: Birch, 3: Palm
    float posX{0.0f};             // World X position (meters)
    float posY{0.0f};             // World Y / elevation (meters)
    float posZ{0.0f};             // World Z position (meters)
    float crownRadiusXPos{3.5f};
    float crownRadiusXNeg{3.0f};
    float crownRadiusYPos{3.4f};
    float crownRadiusYNeg{3.1f};
    float crownHeight{10.0f};     // Total crown vertical span
    float trunkHeight{4.0f};      // Clear bole height to first bough
    float dbhMeters{0.45f};       // Diameter at breast height
    float leafAreaIndex{4.2f};    // LAI (m^2 leaves / m^2 ground)
    float canopyTransmittance{0.18f}; // Beer-Lambert transmitted sunlight fraction
    float ageYears{45.0f};
};

struct ForestCanopyLayer {
    std::string layerName;
    float minHeightMeters;
    float maxHeightMeters;
    float crownCoverFraction;    // 0.0 - 1.0
    float meanLeafDensity;       // leaves / m^3
};

struct Forest3DConfig {
    float forestAreaWidthMeters{100.0f};
    float forestAreaDepthMeters{100.0f};
    uint32_t targetTreeCount{48};
    float minTreeDistanceMeters{6.5f};
    uint32_t randomSeed{1337};
    float canopyClosureTarget{0.78f}; // 78% canopy cover
    float solarElevationDeg{55.0f};
    float ambientWindSpeedMps{4.5f};
};

struct ForestSoilState {
    float leafLitterDepthCm{4.5f};
    float soilMoistureFraction{0.68f};
    float organicMatterRatio{0.14f};
    float slopeDeg{3.2f};
};

class Forest3DSystem {
public:
    explicit Forest3DSystem(const Forest3DConfig& config = Forest3DConfig{});

    // Procedural generation passes
    void initializeForestStand();
    void computeCanopyLightExtinction();
    void distributeUnderstoryMoisture();

    // Query methods
    [[nodiscard]] const std::vector<ForestTreeInstance>& getTrees() const { return m_trees; }
    [[nodiscard]] size_t getTreeCount() const { return m_trees.size(); }
    [[nodiscard]] const Forest3DConfig& getConfig() const { return m_config; }
    [[nodiscard]] const ForestSoilState& getSoilState() const { return m_soil; }

    // Solar irradiance at forest floor (x, z) using Beer-Lambert extinction:
    // I(x, z) = I_0 * exp(-k_ext * sum(LAI_i * overlap_i))
    [[nodiscard]] float computeFloorIrradiance(float worldX, float worldZ) const;

    // Wind speed at forest floor, attenuated by canopy roughness:
    // u(z) = u_canopy * exp(-alpha_wind * (1.0 - z / h_canopy))
    [[nodiscard]] float computeFloorWindSpeed(float worldX, float worldZ) const;

    // Check if position is sheltered under tree crown
    [[nodiscard]] bool isUnderCrown(float worldX, float worldZ, float& outCanopyHeight) const;

private:
    Forest3DConfig m_config;
    ForestSoilState m_soil;
    std::vector<ForestTreeInstance> m_trees;
    std::vector<ForestCanopyLayer> m_layers;

    float pseudoRandom(uint32_t& state) const;
};

} // namespace vegetation
