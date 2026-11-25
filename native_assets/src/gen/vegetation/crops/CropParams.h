#pragma once
// ============================================================================
// CropParams.h - 9 Agricultural Row & Furrow Crop Systems in C++17
// ============================================================================

#include <cstdint>

namespace vegetation {

enum class CropArchetype : uint32_t {
    CORN_MAIZE = 0,
    WHEAT_FIELD = 1,
    LAVENDER_ROWS = 2,
    VINEYARD_TRELLIS = 3,
    POTATO_FURROWS = 4,
    BANANA_PLANTATION = 5,
    CASSAVA_FIELD = 6,
    COFFEE_PLANTATION = 7,
    TEA_CONTOUR_HEDGES = 8,
    SUGARCANE_ROWS = 9
};

struct CropSynthesisParams {
    CropArchetype archetype{CropArchetype::CORN_MAIZE};
    float rowSpacingMeters{0.95f};
    float plantIntervalMeters{0.42f};
    float furrowDepthMeters{0.16f};
    float cropHeightMeters{2.1f};
    float maturityStage01{0.85f}; // 0.0 = seedling, 1.0 = ready for harvest
    float windFlexibility{0.48f};
    bool showFurrows{true};
    bool showHarvestOrgans{true};
    int rowCount{6};
    int stalksPerRow{10};
    float rowLengthMeters{6.0f};
};

CropSynthesisParams getPredefinedCropParams(CropArchetype archetype);

} // namespace vegetation
