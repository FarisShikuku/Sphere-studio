#include "Mesh.h"
#include <algorithm>
#include <cmath>

namespace sphere {

void Mesh::CalculateBounds() {
    if (vertices.empty()) {
        bounds.min = Vec3{0, 0, 0};
        bounds.max = Vec3{0, 0, 0};
        return;
    }

    bounds.min = vertices[0].position;
    bounds.max = vertices[0].position;
    for (const auto& v : vertices) {
        bounds.min.x = std::min(bounds.min.x, v.position.x);
        bounds.min.y = std::min(bounds.min.y, v.position.y);
        bounds.min.z = std::min(bounds.min.z, v.position.z);
        bounds.max.x = std::max(bounds.max.x, v.position.x);
        bounds.max.y = std::max(bounds.max.y, v.position.y);
        bounds.max.z = std::max(bounds.max.z, v.position.z);
    }

    // Also update sub-mesh bounds if populated
    for (auto& sm : subMeshes) {
        if (sm.vertexCount > 0 && sm.vertexOffset + sm.vertexCount <= vertices.size()) {
            sm.bounds.min = vertices[sm.vertexOffset].position;
            sm.bounds.max = vertices[sm.vertexOffset].position;
            for (uint32_t i = 1; i < sm.vertexCount; ++i) {
                const auto& pos = vertices[sm.vertexOffset + i].position;
                sm.bounds.min.x = std::min(sm.bounds.min.x, pos.x);
                sm.bounds.min.y = std::min(sm.bounds.min.y, pos.y);
                sm.bounds.min.z = std::min(sm.bounds.min.z, pos.z);
                sm.bounds.max.x = std::max(sm.bounds.max.x, pos.x);
                sm.bounds.max.y = std::max(sm.bounds.max.y, pos.y);
                sm.bounds.max.z = std::max(sm.bounds.max.z, pos.z);
            }
        }
    }
}

void Mesh::CalculateSmoothNormals() {
    if (vertices.empty() || indices.empty() || topology != PrimitiveTopology::Triangles) return;

    for (auto& v : vertices) {
        v.normal = Vec3{0.0f, 0.0f, 0.0f};
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) continue;

        Vec3 p0 = vertices[i0].position;
        Vec3 p1 = vertices[i1].position;
        Vec3 p2 = vertices[i2].position;

        Vec3 edge1 = p1 - p0;
        Vec3 edge2 = p2 - p0;
        Vec3 faceNormal = Vec3::Cross(edge1, edge2);

        vertices[i0].normal += faceNormal;
        vertices[i1].normal += faceNormal;
        vertices[i2].normal += faceNormal;
    }

    for (auto& v : vertices) {
        v.normal = v.normal.Normalized();
    }
}

void Mesh::CalculateTangents() {
    if (vertices.empty() || indices.empty() || topology != PrimitiveTopology::Triangles) return;

    std::vector<Vec3> tan1(vertices.size(), Vec3{0, 0, 0});
    std::vector<Vec3> tan2(vertices.size(), Vec3{0, 0, 0});

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) continue;

        const auto& v0 = vertices[i0];
        const auto& v1 = vertices[i1];
        const auto& v2 = vertices[i2];

        Vec3 p0 = v0.position;
        Vec3 p1 = v1.position;
        Vec3 p2 = v2.position;

        float u0 = v0.uv[0], u1 = v1.uv[0], u2 = v2.uv[0];
        float val0 = v0.uv[1], val1 = v1.uv[1], val2 = v2.uv[1];

        Vec3 edge1 = p1 - p0;
        Vec3 edge2 = p2 - p0;

        float du1 = u1 - u0;
        float dv1 = val1 - val0;
        float du2 = u2 - u0;
        float dv2 = val2 - val0;

        float denom = (du1 * dv2 - du2 * dv1);
        float r = (std::abs(denom) > 1e-6f) ? (1.0f / denom) : 0.0f;

        Vec3 sdir{(dv2 * edge1.x - dv1 * edge2.x) * r,
                  (dv2 * edge1.y - dv1 * edge2.y) * r,
                  (dv2 * edge1.z - dv1 * edge2.z) * r};

        Vec3 tdir{(du1 * edge2.x - du2 * edge1.x) * r,
                  (du1 * edge2.y - du2 * edge1.y) * r,
                  (du1 * edge2.z - du2 * edge1.z) * r};

        tan1[i0] += sdir;
        tan1[i1] += sdir;
        tan1[i2] += sdir;

        tan2[i0] += tdir;
        tan2[i1] += tdir;
        tan2[i2] += tdir;
    }

    for (size_t a = 0; a < vertices.size(); ++a) {
        const Vec3& n = vertices[a].normal;
        const Vec3& t = tan1[a];

        // Gram-Schmidt orthogonalize
        Vec3 tangent = (t - n * Vec3::Dot(n, t)).Normalized();

        // Calculate handedness
        float handedness = (Vec3::Dot(Vec3::Cross(n, t), tan2[a]) < 0.0f) ? -1.0f : 1.0f;
        vertices[a].tangent = Vec4(tangent.x, tangent.y, tangent.z, handedness);
    }
}

void Mesh::Clear() {
    vertices.clear();
    indices.clear();
    subMeshes.clear();
    bounds = AABB{};
}

} // namespace sphere