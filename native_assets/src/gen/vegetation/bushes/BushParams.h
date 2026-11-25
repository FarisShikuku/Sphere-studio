#pragma once
// ============================================================================
// BushParams.h - Shrub & Bush Botanical Archetypes in C++17
// ============================================================================

#include <cstdint>
#include <string>

namespace vegetation {

enum class BushArchetype : uint32_t {
    BOXWOOD_SPHERE = 0,
    WILD_BRAMBLE = 1,
    FLOWERING_HYDRANGEA = 2,
    MOUNTAIN_JUNIPER = 3,
    DESERT_SAGEBRUSH = 4,
    HIGHLAND_TEA_BUSH = 5,
    ARABICA_COFFEE_SHRUB = 6,
    THORNY_BOUGAINVILLEA = 7,
    TROPICAL_HIBISCUS = 8,
    WILD_SAGE_LANTANA = 9,
    COASTAL_ROSEMARY = 10,
    SISAL_AGAVE_ROSETTE = 11,
    KING_PROTEA = 12,
    SAVANNA_RED_THORN = 13,
    HIGHLAND_TREE_FERN = 14,
    DESERT_SALTBUSH = 15,
    // 10 Desert, Semi-Arid & Cactus Archetypes in Native C++17
    SAGUARO_CACTUS = 16,
    PRICKLY_PEAR_CACTUS = 17,
    GOLDEN_BARREL_CACTUS = 18,
    ORGAN_PIPE_CACTUS = 19,
    JUMPING_CHOLLA = 20,
    JOSHUA_TREE = 21,
    OCOTILLO_DESERT_CORAL = 22,
    FISHHOOK_BARREL_CACTUS = 23,
    ALOE_TORCH_SUCCULENT = 24,
    ZEBRA_HAWORTHIA = 25
};

struct BushSynthesisParams {
    BushArchetype archetype{BushArchetype::BOXWOOD_SPHERE};
    float radiusMeters{1.2f};
    float heightMeters{1.4f};
    int stemBranchCount{8};
    int leafClusterDensity{48};
    float leafDroopFactor{0.25f};
    float colorR{0.25f}, colorG{0.55f}, colorB{0.18f};
    float accentColorR{0.9f}, accentColorG{0.2f}, accentColorB{0.2f};
    bool hasAccents{true};
    float leafSizeMeters{0.32f};
    float windFlexibility{0.45f};
};

BushSynthesisParams getPredefinedBushParams(BushArchetype archetype);

} // namespace vegetation
