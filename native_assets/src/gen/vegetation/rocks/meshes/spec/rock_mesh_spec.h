#pragma once
#include <cstdint>

namespace vegetation {
struct RockMeshSpec {
    uint32_t baseIcosahedronSubdivisions{2};
    uint32_t maxBarycentricTriangles{2048};
    float planarCleavageThreshold{0.7f};
};
}
