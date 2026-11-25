#include "WindowGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void WindowGen::generate(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const WindowGenParams& params
) {
    BuildingComponents::generateWindow(
        vertices,
        indices,
        params.center,
        params.width,
        params.height,
        params.normal,
        params.tangent,
        params.type,
        params.glassType
    );
}

} // namespace procedural::buildings
