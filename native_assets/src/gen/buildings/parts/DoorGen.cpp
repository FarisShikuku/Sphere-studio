#include "DoorGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void DoorGen::generate(
    std::vector<BuildingVertex>& vertices,
    std::vector<uint32_t>& indices,
    const DoorGenParams& params
) {
    if (params.type == DoorType::None) return;

    BuildingComponents::generateDoor(
        vertices,
        indices,
        params.centerGround,
        params.width,
        params.height,
        params.normal,
        params.tangent,
        params.type,
        params.withCanopy
    );
}

} // namespace procedural::buildings
