#pragma once
#include "Mesh.h"

namespace sphere {
struct Meshlet {
    uint32_t vertexOffset = 0;
    uint32_t vertexCount = 0;
    uint32_t triangleOffset = 0;
    uint32_t triangleCount = 0;
    Vec4 coneApexAndCutoff{0, 0, 0, -1};
    Vec3 coneAxis{0, 1, 0};
};
}\n