#pragma once
// ============================================================================
// VegetationSystem.h - Unified Master Subsystem in Native C++17
// Coordinates all 9 botanical domains: Trees, Bushes, Grass, Flowers, Crops,
// Rocks, Debris, Biomes, and Wind.
// ============================================================================

#include <vector>
#include <string>
#include <cstdint>
#include <memory>

#include "common/VegetationMath.h"
#include "trees/TreeSpecies.h"
#include "trees/LSystemTree.h"
#include "trees/TreeLOD.h"
#include "trees/TreeImpostor.h"
#include "bushes/BushParams.h"
#include "bushes/BushGenerator.h"
#include "grass/GrassTypes.h"
#include "grass/GrassBladeGen.h"
#include "grass/GrassScatter.h"
#include "grass/GrassSim.h"
#include "flowers/FlowerParams.h"
#include "flowers/FlowerGen.h"
#include "crops/CropParams.h"
#include "crops/CropRowGen.h"
#include "rocks/RockParams.h"
#include "rocks/RockGenerator.h"
#include "debris/DebrisParams.h"
#include "debris/DebrisGen.h"
#include "biomes/BiomeRules.h"
#include "wind/WindParams.h"
#include "wind/WindField.h"
#include "forest/Forest3DSystem.h"
#include "forest/ForestCanopyGenerator.h"
#include "grass/ForestGrassGenerator.h"

namespace vegetation {

enum class VegetationCategory : uint32_t {
    TREES = 0,
    BUSHES = 1,
    GRASS = 2,
    FLOWERS = 3,
    CROPS = 4,
    ROCKS = 5,
    DEBRIS = 6,
    BIOMES = 7,
    WIND = 8,
    FOREST_3D = 9,
    FOREST_GRASS = 10
};

struct VegetationInstance {
    float posX{0.0f}, posY{0.0f}, posZ{0.0f};
    float rotYaw{0.0f}, rotPitch{0.0f}, rotRoll{0.0f};
    float scaleUniform{1.0f};
    uint32_t speciesId{0};
    uint32_t lodTier{0};
};

class VegetationSystem {
public:
    VegetationSystem();
    ~VegetationSystem() = default;

    void initialize(uint64_t seed);
    void update(float deltaTimeSec);

    void scatterInstances(
        VegetationCategory category,
        float areaWidthMeters,
        float areaLengthMeters,
        float densityPerSqMeter,
        std::vector<VegetationInstance>& outInstances
    );

    TreeMeshData generateTreeMesh(TreeSpeciesId species, uint64_t seed);
    BushMeshData generateBushMesh(BushArchetype archetype, uint64_t seed);
    std::vector<GrassBlade_C> generateGrassClump(float x, float z, int count, uint64_t seed);
    FlowerMeshData generateFlowerMesh(FlowerSpecies species, uint64_t seed);
    CropFieldMesh generateCropField(CropArchetype archetype, uint64_t seed);
    RockMeshData generateRockMesh(RockGeology geology, uint64_t seed);
    std::vector<DebrisItem> generateDebris(float minX, float maxX, float minZ, float maxZ, uint64_t seed);
    BiomeWeights_C sampleBiome(float tempC, float precipCm);
    Vec3 sampleWind(float x, float y, float z) const;

    // Forest 3D and Forest Grass Unified Subsystems
    Forest3DSystem generateForest3D(const Forest3DConfig& config = Forest3DConfig{});
    ForestGrassMesh3D generateForestGrass(const ForestGrassParams& params = ForestGrassParams{});

    uint64_t getSeed() const { return m_seed; }
    float getGlobalTime() const { return m_globalTime; }
    const WindField& getWindField() const { return m_wind; }

private:
    uint64_t m_seed{1337420};
    float m_globalTime{0.0f};
    WindField m_wind;
};

} // namespace vegetation

