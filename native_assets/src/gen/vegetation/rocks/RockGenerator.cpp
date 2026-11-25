// ============================================================================
// RockGenerator.cpp - Procedural Icosahedral Displacement & Voronoi Faceting
// ============================================================================

#include "RockGenerator.h"
#include "RockErosion.h"
#include <cmath>

namespace vegetation {

RockSynthesisParams getPredefinedRockParams(RockGeology geology) {
    RockSynthesisParams p;
    p.geology = geology;
    switch (geology) {
        case RockGeology::GRANITE_BOULDER:
            p.scaleMeters = 2.4f;
            p.angularity = 0.70f;
            p.erosionFactor = 0.25f;
            p.colorR = 0.58f; p.colorG = 0.56f; p.colorB = 0.54f;
            break;
        case RockGeology::SANDSTONE_SLAB:
            p.scaleMeters = 3.2f;
            p.angularity = 0.45f;
            p.erosionFactor = 0.60f;
            p.colorR = 0.76f; p.colorG = 0.58f; p.colorB = 0.38f;
            break;
        case RockGeology::BASALT_HEXAGON:
            p.scaleMeters = 1.8f;
            p.angularity = 0.85f;
            p.erosionFactor = 0.15f;
            p.colorR = 0.25f; p.colorG = 0.26f; p.colorB = 0.28f;
            break;
        case RockGeology::LIMESTONE_CLIFF:
            p.scaleMeters = 3.8f;
            p.angularity = 0.75f;
            p.erosionFactor = 0.45f;
            p.colorR = 0.72f; p.colorG = 0.70f; p.colorB = 0.66f;
            break;
        case RockGeology::RIVER_COBBLE:
            p.scaleMeters = 1.1f;
            p.angularity = 0.15f;
            p.erosionFactor = 0.90f;
            p.colorR = 0.48f; p.colorG = 0.48f; p.colorB = 0.50f;
            break;
    }
    return p;
}

RockMeshData RockGenerator::generateRockMesh(const RockSynthesisParams& params, uint64_t seed) {
    FastRng rng(seed);
    RockMeshData mesh;

    // Start with 12 vertices of an icosahedron
    const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
    std::vector<Vec3> icosVerts = {
        {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
        { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
    };

    std::vector<uint32_t> icosIndices = {
        0, 11, 5,   0, 5, 1,    0, 1, 7,    0, 7, 10,   0, 10, 11,
        1, 5, 9,    5, 11, 4,   11, 10, 2,  10, 7, 6,   7, 1, 8,
        3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
        4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1
    };

    // Subdivide and displace with 3D fractal noise
    for (size_t i = 0; i < icosIndices.size(); i += 3) {
        uint32_t i0 = icosIndices[i];
        uint32_t i1 = icosIndices[i + 1];
        uint32_t i2 = icosIndices[i + 2];

        Vec3 p0 = icosVerts[i0].normalized();
        Vec3 p1 = icosVerts[i1].normalized();
        Vec3 p2 = icosVerts[i2].normalized();

        Vec3 faceNorm = cross(p1 - p0, p2 - p0).normalized();

        Vec3 tri[3] = {p0, p1, p2};
        uint32_t baseIdx = static_cast<uint32_t>(mesh.vertices.size());

        for (int v = 0; v < 3; ++v) {
            Vec3 dir = tri[v];
            float noise = veg_fbm3d(dir.x * 2.5f, dir.y * 2.5f, dir.z * 2.5f, 3, 2.0f, 0.5f);
            float disp = 1.0f + noise * params.angularity * 0.45f;
            Vec3 finalPos = dir * (params.scaleMeters * disp);

            // Moss on top upward faces
            float upAlignment = std::max(0.0f, faceNorm.y);
            float moss = (upAlignment > 0.6f) ? params.mossCoverage : 0.0f;
            float r = lerp(params.colorR, 0.22f, moss);
            float g = lerp(params.colorG, 0.45f, moss);
            float b = lerp(params.colorB, 0.16f, moss);

            mesh.vertices.push_back({finalPos.toC(), faceNorm.toC(), {0.5f, 0.5f}, {r, g, b, 1.0f}, 0.0f});
        }

        mesh.indices.push_back(baseIdx);
        mesh.indices.push_back(baseIdx + 1);
        mesh.indices.push_back(baseIdx + 2);
    }

    RockErosion::simulateHydraulicTalus(mesh, params.erosionFactor);

    return mesh;
}

} // namespace vegetation
