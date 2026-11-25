#pragma once
#include <cstdint>

namespace vegetation {
struct FlowerMeshSpec {
    uint32_t stemRadialSegments{6};
    uint32_t petalSubdivisionsU{3};
    uint32_t petalSubdivisionsV{4};
    bool generatePistilStamenHairs{true};
};
}
