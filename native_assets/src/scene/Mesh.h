#pragma once
#include "Transform.h"
#include "AABB.h"
#include <vector>
#include <cstdint>
#include <string>

namespace sphere {

struct Vertex {
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 normal{0.0f, 1.0f, 0.0f};
    Vec4 tangent{1.0f, 0.0f, 0.0f, 1.0f};
    float uv[2]{0.0f, 0.0f};
    Vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

    Vertex() = default;
    Vertex(const Vec3& p, const Vec3& n, const Vec4& t, float u, float v, const Vec4& c = {1.0f, 1.0f, 1.0f, 1.0f})
        : position(p), normal(n), tangent(t), color(c) {
        uv[0] = u;
        uv[1] = v;
    }
};

struct SubMesh {
    std::string name = "DefaultSubMesh";
    uint32_t indexOffset = 0;
    uint32_t indexCount = 0;
    uint32_t vertexOffset = 0;
    uint32_t vertexCount = 0;
    uint32_t materialIndex = 0;
    AABB bounds;
};

enum class PrimitiveTopology {
    Triangles,
    Lines,
    Points
};

class Mesh {
public:
    std::string name = "ProceduralMesh";
    PrimitiveTopology topology = PrimitiveTopology::Triangles;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SubMesh> subMeshes;
    AABB bounds;

    Mesh() = default;

    void CalculateBounds();
    void CalculateSmoothNormals();
    void CalculateTangents();
    void Clear();
    bool IsEmpty() const { return vertices.empty(); }
    size_t GetVertexCount() const { return vertices.size(); }
    size_t GetIndexCount() const { return indices.size(); }
    size_t GetTriangleCount() const { return indices.size() / 3; }
};

} // namespace sphere