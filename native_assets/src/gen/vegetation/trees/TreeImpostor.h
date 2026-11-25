#pragma once
// ============================================================================
// TreeImpostor.h - Octahedral / Multi-Angle Billboard Impostor Generator in C++17
// ============================================================================

#include <vector>
#include <cstdint>
#include "../common/VegetationMath.h"

namespace vegetation {

struct ImpostorPlane {
    Vec3 normal;
    float azimuthDeg{0.0f};
    VegVertex_C quadVerts[4];
};

class TreeImpostor {
public:
    static std::vector<ImpostorPlane> generateOctahedralPlanes(
        float treeHeightMeters,
        float treeWidthMeters,
        int angleSlices = 8
    );
};

} // namespace vegetation
