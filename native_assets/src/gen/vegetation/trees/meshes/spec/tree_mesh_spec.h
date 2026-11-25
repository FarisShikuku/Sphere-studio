#pragma once
#include <cstdint>

namespace vegetation {
struct TreeMeshSpec {
    uint32_t maxTrunkVertices{16384};
    uint32_t maxLeafVertices{32768};
    uint32_t radialResolution{8};
    float leafCardTessellation{1.0f};
    bool generateSmoothNormals{true};
    bool computeTangentSpace{true};
    bool includeWindWeights{true};
};
}
