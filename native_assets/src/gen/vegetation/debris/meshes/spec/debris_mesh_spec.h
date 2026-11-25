#pragma once
#include <cstdint>

namespace vegetation {
struct DebrisMeshSpec {
    uint32_t maxTwigSegments{6};
    uint32_t maxLeafVertices{8};
    float groundConformalSnapDistance{0.05f};
};
}
