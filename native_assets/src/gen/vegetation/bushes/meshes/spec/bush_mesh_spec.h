#pragma once
#include <cstdint>

namespace vegetation {
struct BushMeshSpec {
    uint32_t maxStemVertices{4096};
    uint32_t maxFoliageCards{128};
    float sphereHullOcclusionWeight{0.75f};
};
}
