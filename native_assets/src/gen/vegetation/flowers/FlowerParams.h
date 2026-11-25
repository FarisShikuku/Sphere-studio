#pragma once
// ============================================================================
// FlowerParams.h - 10 Botanical Wildflowers & Corolla Morphology in C++17
// ============================================================================

#include <cstdint>

namespace vegetation {

enum class FlowerSpecies : uint32_t {
    RED_POPPY = 0,
    OXEYE_DAISY = 1,
    ENGLISH_LAVENDER = 2,
    COMMON_SUNFLOWER = 3,
    MOUNTAIN_BLUEBELL = 4,
    AFRICAN_MARIGOLD = 5,
    PURPLE_CONEFLOWER = 6,
    KENYA_PYRETHRUM = 7,
    BIRD_OF_PARADISE_WILD = 8,
    FOXGLOVE_SPIRE = 9
};

struct FlowerSynthesisParams {
    FlowerSpecies species{FlowerSpecies::OXEYE_DAISY};
    int petalCount{21};
    int stemCount{7};
    float petalLengthMeters{0.08f};
    float petalWidthMeters{0.035f};
    float petalCurvature{0.35f};
    float stemHeightMeters{0.45f};
    float stemRadiusMeters{0.008f};
    float centerDiskRadiusMeters{0.025f};
    float clumpSpreadMeters{0.55f};
    float windFlexibility{0.55f};
    bool showLeaves{true};
    float petalColorR{0.95f}, petalColorG{0.95f}, petalColorB{0.90f};
    float centerColorR{0.90f}, centerColorG{0.75f}, centerColorB{0.15f};
};

FlowerSynthesisParams getPredefinedFlowerParams(FlowerSpecies species);

} // namespace vegetation
