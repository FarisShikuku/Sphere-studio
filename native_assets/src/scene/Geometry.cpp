#include "Geometry.h"
#include <cmath>
#include <algorithm>

namespace sphere {

Mesh GeometryGenerator::GenerateCube(float width, float height, float depth, const Vec3& center) {
    Mesh mesh;
    mesh.name = "Cube";
    mesh.topology = PrimitiveTopology::Triangles;

    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float hd = depth * 0.5f;

    // 6 Faces, 4 vertices each = 24 vertices
    // +Z Front
    mesh.vertices.emplace_back(Vec3{-hw, -hh,  hd} + center, Vec3{0, 0, 1}, Vec4{1, 0, 0, 1}, 0.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw, -hh,  hd} + center, Vec3{0, 0, 1}, Vec4{1, 0, 0, 1}, 1.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw,  hh,  hd} + center, Vec3{0, 0, 1}, Vec4{1, 0, 0, 1}, 1.0f, 0.0f);
    mesh.vertices.emplace_back(Vec3{-hw,  hh,  hd} + center, Vec3{0, 0, 1}, Vec4{1, 0, 0, 1}, 0.0f, 0.0f);

    // -Z Back
    mesh.vertices.emplace_back(Vec3{ hw, -hh, -hd} + center, Vec3{0, 0, -1}, Vec4{-1, 0, 0, 1}, 0.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{-hw, -hh, -hd} + center, Vec3{0, 0, -1}, Vec4{-1, 0, 0, 1}, 1.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{-hw,  hh, -hd} + center, Vec3{0, 0, -1}, Vec4{-1, 0, 0, 1}, 1.0f, 0.0f);
    mesh.vertices.emplace_back(Vec3{ hw,  hh, -hd} + center, Vec3{0, 0, -1}, Vec4{-1, 0, 0, 1}, 0.0f, 0.0f);

    // +X Right
    mesh.vertices.emplace_back(Vec3{ hw, -hh,  hd} + center, Vec3{1, 0, 0}, Vec4{0, 0, -1, 1}, 0.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw, -hh, -hd} + center, Vec3{1, 0, 0}, Vec4{0, 0, -1, 1}, 1.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw,  hh, -hd} + center, Vec3{1, 0, 0}, Vec4{0, 0, -1, 1}, 1.0f, 0.0f);
    mesh.vertices.emplace_back(Vec3{ hw,  hh,  hd} + center, Vec3{1, 0, 0}, Vec4{0, 0, -1, 1}, 0.0f, 0.0f);

    // -X Left
    mesh.vertices.emplace_back(Vec3{-hw, -hh, -hd} + center, Vec3{-1, 0, 0}, Vec4{0, 0, 1, 1}, 0.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{-hw, -hh,  hd} + center, Vec3{-1, 0, 0}, Vec4{0, 0, 1, 1}, 1.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{-hw,  hh,  hd} + center, Vec3{-1, 0, 0}, Vec4{0, 0, 1, 1}, 1.0f, 0.0f);
    mesh.vertices.emplace_back(Vec3{-hw,  hh, -hd} + center, Vec3{-1, 0, 0}, Vec4{0, 0, 1, 1}, 0.0f, 0.0f);

    // +Y Top
    mesh.vertices.emplace_back(Vec3{-hw,  hh,  hd} + center, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 0.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw,  hh,  hd} + center, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 1.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw,  hh, -hd} + center, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 1.0f, 0.0f);
    mesh.vertices.emplace_back(Vec3{-hw,  hh, -hd} + center, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 0.0f, 0.0f);

    // -Y Bottom
    mesh.vertices.emplace_back(Vec3{-hw, -hh, -hd} + center, Vec3{0, -1, 0}, Vec4{1, 0, 0, 1}, 0.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw, -hh, -hd} + center, Vec3{0, -1, 0}, Vec4{1, 0, 0, 1}, 1.0f, 1.0f);
    mesh.vertices.emplace_back(Vec3{ hw, -hh,  hd} + center, Vec3{0, -1, 0}, Vec4{1, 0, 0, 1}, 1.0f, 0.0f);
    mesh.vertices.emplace_back(Vec3{-hw, -hh,  hd} + center, Vec3{0, -1, 0}, Vec4{1, 0, 0, 1}, 0.0f, 0.0f);

    // Indices for each face
    for (uint32_t f = 0; f < 6; ++f) {
        uint32_t base = f * 4;
        mesh.indices.push_back(base + 0);
        mesh.indices.push_back(base + 1);
        mesh.indices.push_back(base + 2);
        mesh.indices.push_back(base + 0);
        mesh.indices.push_back(base + 2);
        mesh.indices.push_back(base + 3);
    }

    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateCalibrationCube(float size, float bevel) {
    Mesh cube = GenerateCube(size, size, size, Vec3{0, size * 0.5f, 0});
    cube.name = "CalibrationCube_1m";
    // Mark face colors for calibration (RGB alignment on primary axes)
    for (size_t i = 0; i < cube.vertices.size(); ++i) {
        size_t face = i / 4;
        if (face == 4) { // Top (+Y)
            cube.vertices[i].color = Vec4{0.1f, 0.8f, 0.95f, 1.0f}; // Vulkan Cyan
        } else if (face == 2) { // Right (+X)
            cube.vertices[i].color = Vec4{0.95f, 0.3f, 0.4f, 1.0f}; // Ruby Red
        } else {
            cube.vertices[i].color = Vec4{0.8f, 0.85f, 0.9f, 1.0f};
        }
    }
    return cube;
}

Mesh GeometryGenerator::GeneratePlane(float width, float length, int segmentsX, int segmentsZ, float uvScale) {
    Mesh mesh;
    mesh.name = "StagePlane";
    mesh.topology = PrimitiveTopology::Triangles;

    segmentsX = std::max(1, segmentsX);
    segmentsZ = std::max(1, segmentsZ);

    float hw = width * 0.5f;
    float hl = length * 0.5f;
    float dx = width / static_cast<float>(segmentsX);
    float dz = length / static_cast<float>(segmentsZ);

    for (int z = 0; z <= segmentsZ; ++z) {
        float pz = -hl + z * dz;
        float v = (static_cast<float>(z) / static_cast<float>(segmentsZ)) * uvScale;
        for (int x = 0; x <= segmentsX; ++x) {
            float px = -hw + x * dx;
            float u = (static_cast<float>(x) / static_cast<float>(segmentsX)) * uvScale;
            mesh.vertices.emplace_back(
                Vec3{px, 0.0f, pz},
                Vec3{0.0f, 1.0f, 0.0f},
                Vec4{1.0f, 0.0f, 0.0f, 1.0f},
                u, v
            );
        }
    }

    int rowStride = segmentsX + 1;
    for (int z = 0; z < segmentsZ; ++z) {
        for (int x = 0; x < segmentsX; ++x) {
            uint32_t i0 = z * rowStride + x;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = (z + 1) * rowStride + x;
            uint32_t i3 = i2 + 1;

            mesh.indices.push_back(i0);
            mesh.indices.push_back(i2);
            mesh.indices.push_back(i1);

            mesh.indices.push_back(i1);
            mesh.indices.push_back(i2);
            mesh.indices.push_back(i3);
        }
    }

    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateStagePlatform(float width, float length, float height, float bevel) {
    Mesh platform;
    platform.name = "VulkanStagePlatform";
    platform.topology = PrimitiveTopology::Triangles;

    // Top surface plane
    Mesh topPlane = GeneratePlane(width, length, 4, 4, 1.0f);
    for (auto& v : topPlane.vertices) {
        v.position.y = 0.0f;
        v.color = Vec4{0.15f, 0.18f, 0.22f, 1.0f};
    }
    MergeMeshes(platform, topPlane);

    // Beveled frame edge border
    float hw = width * 0.5f;
    float hl = length * 0.5f;
    float b = std::max(0.01f, bevel);

    // Border skirt with subtle rim
    Mesh boxRim = GenerateCube(width + b * 2.0f, height, length + b * 2.0f, Vec3{0, -height * 0.5f, 0});
    for (auto& v : boxRim.vertices) {
        v.color = Vec4{0.08f, 0.10f, 0.13f, 1.0f};
    }
    MergeMeshes(platform, boxRim);

    platform.CalculateBounds();
    return platform;
}

Mesh GeometryGenerator::GenerateGridLines(int lineCount, float spacing, bool addAxes) {
    Mesh mesh;
    mesh.name = "VulkanGridLines";
    mesh.topology = PrimitiveTopology::Lines;

    float extent = (lineCount * 0.5f) * spacing;
    Vec4 gridColor{0.12f, 0.35f, 0.55f, 0.65f}; // Cyan wireframe
    Vec4 centerColor{0.2f, 0.6f, 0.8f, 0.9f};

    uint32_t idx = 0;
    // Lines along X axis
    for (int i = -lineCount / 2; i <= lineCount / 2; ++i) {
        float z = i * spacing;
        Vec4 col = (i == 0) ? centerColor : gridColor;
        mesh.vertices.emplace_back(Vec3{-extent, 0.0f, z}, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 0, 0, col);
        mesh.vertices.emplace_back(Vec3{ extent, 0.0f, z}, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 1, 0, col);
        mesh.indices.push_back(idx++);
        mesh.indices.push_back(idx++);
    }

    // Lines along Z axis
    for (int i = -lineCount / 2; i <= lineCount / 2; ++i) {
        float x = i * spacing;
        Vec4 col = (i == 0) ? centerColor : gridColor;
        mesh.vertices.emplace_back(Vec3{x, 0.0f, -extent}, Vec3{0, 1, 0}, Vec4{0, 0, 1, 1}, 0, 0, col);
        mesh.vertices.emplace_back(Vec3{x, 0.0f,  extent}, Vec3{0, 1, 0}, Vec4{0, 0, 1, 1}, 0, 1, col);
        mesh.indices.push_back(idx++);
        mesh.indices.push_back(idx++);
    }

    // Add highlighted 3D coordinate axes
    if (addAxes) {
        float axisLen = spacing * 3.0f;
        // X axis (Red)
        mesh.vertices.emplace_back(Vec3{0, 0.01f, 0}, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 0, 0, Vec4{1.0f, 0.2f, 0.2f, 1.0f});
        mesh.vertices.emplace_back(Vec3{axisLen, 0.01f, 0}, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 1, 0, Vec4{1.0f, 0.2f, 0.2f, 1.0f});
        mesh.indices.push_back(idx++);
        mesh.indices.push_back(idx++);

        // Y axis (Green)
        mesh.vertices.emplace_back(Vec3{0, 0.0f, 0}, Vec3{0, 1, 0}, Vec4{0, 1, 0, 1}, 0, 0, Vec4{0.2f, 0.9f, 0.3f, 1.0f});
        mesh.vertices.emplace_back(Vec3{0, axisLen, 0}, Vec3{0, 1, 0}, Vec4{0, 1, 0, 1}, 0, 1, Vec4{0.2f, 0.9f, 0.3f, 1.0f});
        mesh.indices.push_back(idx++);
        mesh.indices.push_back(idx++);

        // Z axis (Blue)
        mesh.vertices.emplace_back(Vec3{0, 0.01f, 0}, Vec3{0, 1, 0}, Vec4{0, 0, 1, 1}, 0, 0, Vec4{0.2f, 0.5f, 1.0f, 1.0f});
        mesh.vertices.emplace_back(Vec3{0, 0.01f, axisLen}, Vec3{0, 1, 0}, Vec4{0, 0, 1, 1}, 0, 1, Vec4{0.2f, 0.5f, 1.0f, 1.0f});
        mesh.indices.push_back(idx++);
        mesh.indices.push_back(idx++);
    }

    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateInfiniteGridPlane(float radius, float uvScale) {
    Mesh mesh = GeneratePlane(radius * 2.0f, radius * 2.0f, 1, 1, uvScale);
    mesh.name = "InfiniteGroundGrid";
    for (auto& v : mesh.vertices) {
        v.position.y = -0.01f; // Slightly below stage
        v.color = Vec4{0.05f, 0.07f, 0.09f, 1.0f};
    }
    return mesh;
}

Mesh GeometryGenerator::GenerateSphere(float radius, uint32_t rings, uint32_t sectors) {
    Mesh mesh;
    mesh.name = "PBR_Sphere";
    mesh.topology = PrimitiveTopology::Triangles;

    rings = std::max(4u, rings);
    sectors = std::max(4u, sectors);

    constexpr float kPi = 3.14159265358979323846f;
    float const R = 1.0f / static_cast<float>(rings - 1);
    float const S = 1.0f / static_cast<float>(sectors - 1);

    for (uint32_t r = 0; r < rings; ++r) {
        float y = std::sin(-kPi * 0.5f + kPi * r * R);
        float ringRadius = std::cos(-kPi * 0.5f + kPi * r * R);
        for (uint32_t s = 0; s < sectors; ++s) {
            float x = std::cos(2.0f * kPi * s * S) * ringRadius;
            float z = std::sin(2.0f * kPi * s * S) * ringRadius;

            Vec3 norm{x, y, z};
            Vec3 pos = norm * radius;
            Vec4 tan{-std::sin(2.0f * kPi * s * S), 0.0f, std::cos(2.0f * kPi * s * S), 1.0f};
            float u = s * S;
            float v = r * R;

            mesh.vertices.emplace_back(pos, norm, tan, u, v);
        }
    }

    for (uint32_t r = 0; r < rings - 1; ++r) {
        for (uint32_t s = 0; s < sectors - 1; ++s) {
            uint32_t cur = r * sectors + s;
            uint32_t next = cur + sectors;

            mesh.indices.push_back(cur);
            mesh.indices.push_back(next);
            mesh.indices.push_back(cur + 1);

            mesh.indices.push_back(cur + 1);
            mesh.indices.push_back(next);
            mesh.indices.push_back(next + 1);
        }
    }

    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateSkyDome(float radius, uint32_t rings, uint32_t sectors, float hemisphereCutoff) {
    Mesh mesh;
    mesh.name = "AtmosphericSkyDome";
    mesh.topology = PrimitiveTopology::Triangles;

    constexpr float kPi = 3.14159265358979323846f;
    float const R = 1.0f / static_cast<float>(rings - 1);
    float const S = 1.0f / static_cast<float>(sectors - 1);

    for (uint32_t r = 0; r < rings; ++r) {
        // Hemisphere from zenith down to cutoff below horizon
        float phi = (kPi * 0.5f) * (1.0f - r * R) + hemisphereCutoff * r * R;
        float y = std::sin(phi);
        float ringRadius = std::cos(phi);

        for (uint32_t s = 0; s < sectors; ++s) {
            float theta = 2.0f * kPi * s * S;
            float x = std::cos(theta) * ringRadius;
            float z = std::sin(theta) * ringRadius;

            Vec3 norm{-x, -y, -z}; // Inverted inward towards viewer
            Vec3 pos = norm * (-radius);
            mesh.vertices.emplace_back(pos, norm, Vec4{1, 0, 0, 1}, s * S, r * R);
        }
    }

    for (uint32_t r = 0; r < rings - 1; ++r) {
        for (uint32_t s = 0; s < sectors - 1; ++s) {
            uint32_t cur = r * sectors + s;
            uint32_t next = cur + sectors;

            // Inward winding
            mesh.indices.push_back(cur);
            mesh.indices.push_back(cur + 1);
            mesh.indices.push_back(next);

            mesh.indices.push_back(cur + 1);
            mesh.indices.push_back(next + 1);
            mesh.indices.push_back(next);
        }
    }

    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateAtmosphereCloudHull(float innerRadius, float outerRadius, float angularSpanDeg) {
    Mesh mesh;
    mesh.name = "MeteorosCloudVolumeHull";
    mesh.topology = PrimitiveTopology::Triangles;

    // Curved spherical atmospheric shell section for Decima/Meteoros cloud marching
    constexpr float kPi = 3.14159265358979323846f;
    float spanRad = (angularSpanDeg * (kPi / 180.0f));
    int steps = 16;

    for (int layer = 0; layer <= 1; ++layer) {
        float rad = (layer == 0) ? innerRadius : outerRadius;
        for (int j = 0; j <= steps; ++j) {
            float v = static_cast<float>(j) / steps;
            float lat = (v - 0.5f) * spanRad;
            for (int i = 0; i <= steps; ++i) {
                float u = static_cast<float>(i) / steps;
                float lon = (u - 0.5f) * spanRad;

                float x = rad * std::cos(lat) * std::sin(lon);
                float y = rad * std::sin(lat) - innerRadius; // Centered relative to earth ground
                float z = rad * std::cos(lat) * std::cos(lon) - innerRadius;

                Vec3 norm = Vec3{x, y + innerRadius, z + innerRadius}.Normalized();
                mesh.vertices.emplace_back(Vec3{x, y, z}, norm, Vec4{1, 0, 0, 1}, u, v);
            }
        }
    }

    int rowStride = steps + 1;
    int layerStride = (steps + 1) * (steps + 1);

    for (int layer = 0; layer < 1; ++layer) {
        for (int j = 0; j < steps; ++j) {
            for (int i = 0; i < steps; ++i) {
                uint32_t i0 = j * rowStride + i;
                uint32_t i1 = i0 + 1;
                uint32_t i2 = (j + 1) * rowStride + i;
                uint32_t i3 = i2 + 1;

                // Inner shell
                mesh.indices.push_back(i0);
                mesh.indices.push_back(i2);
                mesh.indices.push_back(i1);

                mesh.indices.push_back(i1);
                mesh.indices.push_back(i2);
                mesh.indices.push_back(i3);

                // Outer shell
                mesh.indices.push_back(layerStride + i0);
                mesh.indices.push_back(layerStride + i1);
                mesh.indices.push_back(layerStride + i2);

                mesh.indices.push_back(layerStride + i1);
                mesh.indices.push_back(layerStride + i3);
                mesh.indices.push_back(layerStride + i2);
            }
        }
    }

    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateBoundingBox(const AABB& aabb, const Vec4& color) {
    Mesh mesh;
    mesh.name = "AABB_DebugWire";
    mesh.topology = PrimitiveTopology::Lines;

    Vec3 p0{aabb.min.x, aabb.min.y, aabb.min.z};
    Vec3 p1{aabb.max.x, aabb.min.y, aabb.min.z};
    Vec3 p2{aabb.max.x, aabb.max.y, aabb.min.z};
    Vec3 p3{aabb.min.x, aabb.max.y, aabb.min.z};

    Vec3 p4{aabb.min.x, aabb.min.y, aabb.max.z};
    Vec3 p5{aabb.max.x, aabb.min.y, aabb.max.z};
    Vec3 p6{aabb.max.x, aabb.max.y, aabb.max.z};
    Vec3 p7{aabb.min.x, aabb.max.y, aabb.max.z};

    std::vector<Vec3> pts = {p0, p1, p2, p3, p4, p5, p6, p7};
    for (const auto& p : pts) {
        mesh.vertices.emplace_back(p, Vec3{0, 1, 0}, Vec4{1, 0, 0, 1}, 0, 0, color);
    }

    // 12 edges
    mesh.indices = {
        0, 1, 1, 2, 2, 3, 3, 0, // bottom face
        4, 5, 5, 6, 6, 7, 7, 4, // top face
        0, 4, 1, 5, 2, 6, 3, 7  // vertical pillars
    };

    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateQuad(float width, float height, const Vec3& normal) {
    Mesh mesh;
    mesh.name = "Quad";
    mesh.topology = PrimitiveTopology::Triangles;

    float hw = width * 0.5f;
    float hh = height * 0.5f;

    mesh.vertices.emplace_back(Vec3{-hw, 0, -hh}, normal, Vec4{1, 0, 0, 1}, 0, 1);
    mesh.vertices.emplace_back(Vec3{ hw, 0, -hh}, normal, Vec4{1, 0, 0, 1}, 1, 1);
    mesh.vertices.emplace_back(Vec3{ hw, 0,  hh}, normal, Vec4{1, 0, 0, 1}, 1, 0);
    mesh.vertices.emplace_back(Vec3{-hw, 0,  hh}, normal, Vec4{1, 0, 0, 1}, 0, 0);

    mesh.indices = {0, 1, 2, 0, 2, 3};
    mesh.CalculateBounds();
    return mesh;
}

Mesh GeometryGenerator::GenerateTerrainPatch(int resX, int resZ, float width, float length, float maxDisplacement, uint32_t seed) {
    Mesh mesh = GeneratePlane(width, length, resX, resZ, 4.0f);
    mesh.name = "ProceduralTerrainPatch";

    // Multi-octave displacement synthesis
    for (auto& v : mesh.vertices) {
        float nx = (v.position.x + 50.0f) * 0.25f;
        float nz = (v.position.z + 50.0f) * 0.25f;

        // Simple analytical multi-frequency height function
        float h = std::sin(nx * 1.2f) * std::cos(nz * 1.2f) * 0.6f;
        h += std::sin(nx * 2.8f + 1.2f) * std::cos(nz * 2.5f + 0.8f) * 0.25f;
        h += std::sin(nx * 6.0f) * std::cos(nz * 5.8f) * 0.15f;

        v.position.y = h * maxDisplacement;
    }

    mesh.CalculateSmoothNormals();
    mesh.CalculateTangents();
    mesh.CalculateBounds();
    return mesh;
}

void GeometryGenerator::TransformMesh(Mesh& mesh, const Mat4& transform) {
    for (auto& v : mesh.vertices) {
        v.position = transform.TransformPoint(v.position);
        Vec4 normTr = transform.TransformVector(Vec4{v.normal, 0.0f});
        v.normal = Vec3{normTr.x, normTr.y, normTr.z}.Normalized();
        Vec4 tanTr = transform.TransformVector(Vec4{v.tangent.x, v.tangent.y, v.tangent.z, 0.0f});
        Vec3 ntan = Vec3{tanTr.x, tanTr.y, tanTr.z}.Normalized();
        v.tangent = Vec4(ntan.x, ntan.y, ntan.z, v.tangent.w);
    }
    mesh.CalculateBounds();
}

void GeometryGenerator::MergeMeshes(Mesh& target, const Mesh& source) {
    uint32_t vertexBase = static_cast<uint32_t>(target.vertices.size());
    target.vertices.insert(target.vertices.end(), source.vertices.begin(), source.vertices.end());

    for (uint32_t idx : source.indices) {
        target.indices.push_back(vertexBase + idx);
    }

    for (const auto& sm : source.subMeshes) {
        SubMesh copy = sm;
        copy.vertexOffset += vertexBase;
        copy.indexOffset += static_cast<uint32_t>(target.indices.size());
        target.subMeshes.push_back(copy);
    }

    target.CalculateBounds();
}

} // namespace sphere
