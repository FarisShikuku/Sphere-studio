#pragma once

#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

struct DoorGenParams {
    DoorType type = DoorType::DoubleGlassCommercial;
    Vec3 centerGround{0.0f, 0.0f, 0.0f};
    Vec3 normal{0.0f, 0.0f, 1.0f};
    Vec3 tangent{1.0f, 0.0f, 0.0f};
    float width = 2.4f;
    float height = 2.6f;
    bool withCanopy = true;
};

class DoorGen {
public:
    static void generate(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const DoorGenParams& params
    );
};

} // namespace procedural::buildings
