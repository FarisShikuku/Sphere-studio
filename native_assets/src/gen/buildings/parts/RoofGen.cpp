#include "RoofGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void RoofGen::generate(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const RoofGenParams& params
) {
    BuildingComponents::generateRoof(
        vertices,
        indices,
        BuildingVolumeShape::Rectangle,
        params.baseWidth,
        params.baseDepth,
        0.0f,
        params.baseTopY,
        params.type,
        params.roofHeight,
        params.overhang,
        params.showRooftopClutter
    );
}

} // namespace procedural::buildings
