#pragma once

#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

struct WindowGenParams {
    WindowType type = WindowType::ModernCasement;
    GlassType glassType = GlassType::ClearFloat;
    Vec3 center{0.0f, 1.5f, 0.0f};
    Vec3 normal{0.0f, 0.0f, 1.0f};
    Vec3 tangent{1.0f, 0.0f, 0.0f};
    float width = 1.6f;
    float height = 1.8f;
    int32_t floorIndex = 0;
};

class WindowGen {
public:
    static void generate(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        const WindowGenParams& params
    );
};

} // namespace procedural::buildings
