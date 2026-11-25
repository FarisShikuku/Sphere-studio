#pragma once
#include <cstdint>

namespace vegetation {
struct VegetationVertexSpec {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
    float windWeight;
    uint32_t colorRgba;
};
}
