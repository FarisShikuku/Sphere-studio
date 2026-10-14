#pragma once
#include "Transform.h"
#include "AABB.h"
#include <vector>

namespace sphere {
struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec4 tangent;
    float uv[2];
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    AABB bounds;

    void CalculateBounds();
};
}\n