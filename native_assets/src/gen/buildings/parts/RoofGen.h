#pragma once

#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

struct RoofGenParams {
    RoofType type = RoofType::FlatParapet;
    BuildingVolumeShape shape = BuildingVolumeShape::Rectangle;
    float baseWidth = 20.0f;
    float baseDepth = 14.0f;
    float baseTopY = 15.0f;
    float roofHeight = 2.5f;
    float overhang = 0.6f;
    bool showRooftopClutter = true;
};

class RoofGen {
public:
    static void generate(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const RoofGenParams& params
    );
};

} // namespace procedural::buildings
