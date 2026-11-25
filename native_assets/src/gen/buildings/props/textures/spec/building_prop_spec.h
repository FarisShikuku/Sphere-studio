#pragma once

#include <cstdint>

namespace procedural::buildings {

struct BuildingPropTextureSpec {
    uint32_t propAtlasResolution = 512;
    float hvacSheetMetalRoughness = 0.42f;
    float hvacSheetMetalMetallic = 0.82f;
    float chimneyMasonryRoughness = 0.86f;
};

} // namespace procedural::buildings
