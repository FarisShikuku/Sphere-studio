#pragma once

#include "../BuildingTypes.h"
#include "socket.h"
#include <vector>

namespace procedural::buildings {

struct WallSegmentParams {
    Vec3 start;
    Vec3 end;
    float height = 3.0f;
    float thickness = 0.3f;
    int32_t floorIndex = 0;
    bool hasOpening = false;
    float openingStartT = 0.3f;
    float openingEndT = 0.7f;
    float sillHeight = 0.8f;
    float headerHeight = 2.6f;
    uint32_t materialId = 0; // Wall concrete / brick
};

class WallSegment {
public:
    static void generateWall(
        std::vector<BuildingVertex>& vertices,
        std::vector<uint32_t>& indices,
        std::vector<Socket>& sockets,
        const WallSegmentParams& params
    );
};

} // namespace procedural::buildings
