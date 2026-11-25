// ============================================================================
// VegetationSystem.cpp - Unified Master Subsystem Implementation in C++17
// ============================================================================

#include "VegetationSystem.h"

namespace vegetation {

VegetationSystem::VegetationSystem() {
    initialize(0x5EEDCAFEULL);
}

void VegetationSystem::initialize(uint64_t seed) {
    m_seed = seed;
    m_globalTime = 0.0f;
    WindSynthesisParams wp;
    wp.baseSpeedMps = 5.2f;
    wp.windDirectionRad = 0.785398f;
    wp.gustFrequencyHz = 0.25f;
    wp.gustAmplitude = 3.0f;
    m_wind.setParams(wp);
}

void VegetationSystem::update(float deltaTimeSec) {
    m_globalTime += deltaTimeSec;
}

void VegetationSystem::scatterInstances(
    VegetationCategory category,
    float areaWidthMeters,
    float areaLengthMeters,
    float densityPerSqMeter,
    std::vector<VegetationInstance>& outInstances
) {
    FastRng rng(m_seed ^ (static_cast<uint64_t>(category) << 24));
    int count = static_cast<int>(areaWidthMeters * areaLengthMeters * densityPerSqMeter);
    outInstances.clear();
    outInstances.reserve(count);

    float halfW = areaWidthMeters * 0.5f;
    float halfL = areaLengthMeters * 0.5f;

    for (int i = 0; i < count; ++i) {
        VegetationInstance inst;
        inst.posX = rng.nextRange(-halfW, halfW);
        inst.posY = 0.0f;
        inst.posZ = rng.nextRange(-halfL, halfL);
        inst.rotYaw = rng.nextRange(0.0f, 360.0f);
        inst.rotPitch = 0.0f;
        inst.rotRoll = 0.0f;
        inst.scaleUniform = rng.nextRange(0.85f, 1.25f);
        inst.speciesId = rng.nextUint() % 5;
        inst.lodTier = 0;

        outInstances.push_back(inst);
    }
}

TreeMeshData VegetationSystem::generateTreeMesh(TreeSpeciesId species, uint64_t seed) {
    TreeSpeciesConfig cfg = getPredefinedTreeSpecies(species);
    LSystemTree tree(cfg);
    return tree.generateTreeMesh(seed);
}

BushMeshData VegetationSystem::generateBushMesh(BushArchetype archetype, uint64_t seed) {
    BushSynthesisParams p = getPredefinedBushParams(archetype);
    BushGenerator bush(p);
    return bush.generateBushMesh(seed);
}

std::vector<GrassBlade_C> VegetationSystem::generateGrassClump(float x, float z, int count, uint64_t seed) {
    return GrassScatter::scatterClump(x, z, 1.5f, count, GrassSpecies_C::GRASS_FESCUE, seed);
}

FlowerMeshData VegetationSystem::generateFlowerMesh(FlowerSpecies species, uint64_t seed) {
    FlowerSynthesisParams p = getPredefinedFlowerParams(species);
    return FlowerGen::generateFlower(p, seed);
}

CropFieldMesh VegetationSystem::generateCropField(CropArchetype archetype, uint64_t seed) {
    CropSynthesisParams p = getPredefinedCropParams(archetype);
    return CropRowGen::generateCropField(p, seed);
}

RockMeshData VegetationSystem::generateRockMesh(RockGeology geology, uint64_t seed) {
    RockSynthesisParams p = getPredefinedRockParams(geology);
    return RockGenerator::generateRockMesh(p, seed);
}

std::vector<DebrisItem> VegetationSystem::generateDebris(float minX, float maxX, float minZ, float maxZ, uint64_t seed) {
    DebrisScatterConfig cfg;
    return DebrisGen::scatterDebrisPatch(minX, maxX, minZ, maxZ, cfg, seed);
}

BiomeWeights_C VegetationSystem::sampleBiome(float tempC, float precipCm) {
    return BiomeRules::evaluate(tempC, precipCm);
}

Vec3 VegetationSystem::sampleWind(float x, float y, float z) const {
    return m_wind.sampleWindVelocity(x, y, z, m_globalTime);
}

Forest3DSystem VegetationSystem::generateForest3D(const Forest3DConfig& config) {
    return Forest3DSystem(config);
}

ForestGrassMesh3D VegetationSystem::generateForestGrass(const ForestGrassParams& params) {
    return ForestGrassGenerator::generateUnderstoryMesh(params);
}

} // namespace vegetation

