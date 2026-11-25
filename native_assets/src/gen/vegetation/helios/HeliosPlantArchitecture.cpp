// ============================================================================
// HeliosPlantArchitecture.cpp - Native C++17 Plant Geometry & Phytomer Architecture
// Based on PlantSimulationLab/Helios (UC Davis / Bailey et al.)
// https://github.com/PlantSimulationLab/Helios
// ============================================================================

#include "HeliosPlantArchitecture.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace helios {

static constexpr float PI_F = 3.14159265358979323846f;
static constexpr float TWO_PI = 6.28318530717958647692f;
static constexpr float GOLDEN_ANGLE_RAD = 2.39996322972865332f; // 137.507764 deg

void HeliosPlantArchitecture::appendCylinder(
    HeliosCanopyMesh& mesh,
    const Vec3& start,
    const Vec3& end,
    float radiusStart,
    float radiusEnd,
    const Vec3& color,
    int radialSegments
) {
    Vec3 axis = end - start;
    float len = axis.length();
    if (len < 1e-4f) return;
    Vec3 dir = axis / len;

    // Build orthonormal coordinate frame (dir, perp1, perp2)
    Vec3 up = (std::abs(dir.y) < 0.95f) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
    Vec3 perp1 = Vec3::cross(dir, up).normalized();
    Vec3 perp2 = Vec3::cross(dir, perp1).normalized();

    uint32_t baseIdx = static_cast<uint32_t>(mesh.vertices.size());

    // Generate vertices for bottom ring and top ring
    for (int ring = 0; ring < 2; ++ring) {
        float r = (ring == 0) ? radiusStart : radiusEnd;
        Vec3 center = (ring == 0) ? start : end;

        for (int i = 0; i < radialSegments; ++i) {
            float theta = (static_cast<float>(i) / radialSegments) * TWO_PI;
            float ct = std::cos(theta);
            float st = std::sin(theta);

            Vec3 normal = (perp1 * ct + perp2 * st).normalized();
            Vec3 pos = center + normal * r;

            HeliosVertex v;
            v.position = pos;
            v.normal = normal;
            v.uv = Vec2(static_cast<float>(i) / radialSegments, static_cast<float>(ring));
            v.color = color;
            v.absorbedPAR = 350.0f;
            mesh.vertices.push_back(v);

            mesh.aabbMin.x = std::min(mesh.aabbMin.x, pos.x);
            mesh.aabbMin.y = std::min(mesh.aabbMin.y, pos.y);
            mesh.aabbMin.z = std::min(mesh.aabbMin.z, pos.z);
            mesh.aabbMax.x = std::max(mesh.aabbMax.x, pos.x);
            mesh.aabbMax.y = std::max(mesh.aabbMax.y, pos.y);
            mesh.aabbMax.z = std::max(mesh.aabbMax.z, pos.z);
        }
    }

    // Connect rings with quads (2 triangles each)
    for (int i = 0; i < radialSegments; ++i) {
        int next = (i + 1) % radialSegments;
        uint32_t i0 = baseIdx + i;
        uint32_t i1 = baseIdx + next;
        uint32_t i2 = baseIdx + radialSegments + i;
        uint32_t i3 = baseIdx + radialSegments + next;

        mesh.indices.push_back(i0);
        mesh.indices.push_back(i2);
        mesh.indices.push_back(i1);

        mesh.indices.push_back(i1);
        mesh.indices.push_back(i2);
        mesh.indices.push_back(i3);
    }
}

void HeliosPlantArchitecture::appendLeafBlade(
    HeliosCanopyMesh& mesh,
    const Vec3& petioleStart,
    const Vec3& petioleEnd,
    float leafLength,
    float leafWidth,
    float rollAngleDeg,
    const Vec3& leafColor
) {
    // 1. Slender Petiole Cylinder
    appendCylinder(mesh, petioleStart, petioleEnd, 0.003f, 0.002f, Vec3(0.35f, 0.45f, 0.22f), 4);

    // 2. Leaf Blade Quad Patch
    Vec3 leafDir = (petioleEnd - petioleStart).normalized();
    Vec3 up(0, 1, 0);
    Vec3 leafNormal = Vec3::cross(leafDir, up).normalized();
    if (leafNormal.lengthSq() < 1e-4f) leafNormal = Vec3(1, 0, 0);
    Vec3 leafSide = Vec3::cross(leafNormal, leafDir).normalized();

    Vec3 tip = petioleEnd + leafDir * leafLength;
    Vec3 mid = petioleEnd + leafDir * (leafLength * 0.45f);
    Vec3 left = mid + leafSide * (leafWidth * 0.5f);
    Vec3 right = mid - leafSide * (leafWidth * 0.5f);

    uint32_t baseIdx = static_cast<uint32_t>(mesh.vertices.size());

    HeliosVertex v0{petioleEnd, leafNormal, Vec2(0.5f, 0.0f), leafColor, 450.0f};
    HeliosVertex v1{left, leafNormal, Vec2(0.0f, 0.5f), leafColor, 520.0f};
    HeliosVertex v2{tip, leafNormal, Vec2(0.5f, 1.0f), leafColor * 1.1f, 580.0f};
    HeliosVertex v3{right, leafNormal, Vec2(1.0f, 0.5f), leafColor, 520.0f};

    mesh.vertices.push_back(v0);
    mesh.vertices.push_back(v1);
    mesh.vertices.push_back(v2);
    mesh.vertices.push_back(v3);

    for (const auto& v : {v0, v1, v2, v3}) {
        mesh.aabbMin.x = std::min(mesh.aabbMin.x, v.position.x);
        mesh.aabbMin.y = std::min(mesh.aabbMin.y, v.position.y);
        mesh.aabbMin.z = std::min(mesh.aabbMin.z, v.position.z);
        mesh.aabbMax.x = std::max(mesh.aabbMax.x, v.position.x);
        mesh.aabbMax.y = std::max(mesh.aabbMax.y, v.position.y);
        mesh.aabbMax.z = std::max(mesh.aabbMax.z, v.position.z);
    }

    // Double-sided leaf triangles
    mesh.indices.push_back(baseIdx + 0);
    mesh.indices.push_back(baseIdx + 1);
    mesh.indices.push_back(baseIdx + 2);

    mesh.indices.push_back(baseIdx + 0);
    mesh.indices.push_back(baseIdx + 2);
    mesh.indices.push_back(baseIdx + 3);

    mesh.totalLeafAreaM2 += leafLength * leafWidth * 0.65f;
}

HeliosCanopyMesh HeliosPlantArchitecture::generateHeliosShrub(
    float heightMeters,
    float spreadMeters,
    int primaryCanes,
    int phytomersPerCane,
    float leafScale,
    uint64_t seed
) {
    HeliosCanopyMesh mesh;
    mesh.architectureName = "Helios Modular Botanical Shrub";
    int canes = std::clamp(primaryCanes, 4, 18);
    int phytomers = std::clamp(phytomersPerCane, 5, 20);

    for (int c = 0; c < canes; ++c) {
        float caneAngle = (static_cast<float>(c) / canes) * TWO_PI + (c % 3) * 0.15f;
        float caneSpread = spreadMeters * (0.35f + 0.15f * (c % 4));
        float caneH = heightMeters * (0.85f + 0.15f * ((c * 7) % 5));

        Vec3 pos(0.0f, 0.0f, 0.0f);
        float internodeLen = caneH / phytomers;

        for (int p = 0; p < phytomers; ++p) {
            float frac = static_cast<float>(p) / phytomers;
            float radArch = caneSpread * std::sin(frac * PI_F * 0.75f);

            Vec3 nextPos(
                std::cos(caneAngle) * radArch,
                pos.y + internodeLen,
                std::sin(caneAngle) * radArch
            );

            float rStart = 0.035f * (1.0f - frac * 0.7f);
            float rEnd = 0.035f * (1.0f - (frac + 1.0f / phytomers) * 0.7f);

            appendCylinder(mesh, pos, nextPos, rStart, rEnd, Vec3(0.42f, 0.30f, 0.22f), 6);

            // Phyllotactic Leaf Attachment
            float leafAzimuth = caneAngle + p * GOLDEN_ANGLE_RAD;
            Vec3 petioleDir(std::cos(leafAzimuth), 0.25f, std::sin(leafAzimuth));
            petioleDir = petioleDir.normalized();

            Vec3 petioleStart = (pos + nextPos) * 0.5f;
            Vec3 petioleEnd = petioleStart + petioleDir * (0.045f * leafScale);

            appendLeafBlade(
                mesh,
                petioleStart,
                petioleEnd,
                0.09f * leafScale,
                0.055f * leafScale,
                15.0f,
                Vec3(0.18f + 0.05f * (p % 3), 0.48f + 0.04f * (c % 3), 0.20f)
            );

            pos = nextPos;
            mesh.totalPhytomerCount++;
        }
    }

    return mesh;
}

HeliosCanopyMesh HeliosPlantArchitecture::generateHeliosCropRow(
    float rowLengthMeters,
    float plantSpacingMeters,
    float cropHeightMeters,
    uint64_t seed
) {
    HeliosCanopyMesh mesh;
    mesh.architectureName = "Helios Farmland Crop Furrow Row";

    int plantCount = std::max(2, static_cast<int>(rowLengthMeters / plantSpacingMeters));
    for (int i = 0; i < plantCount; ++i) {
        float z = -rowLengthMeters * 0.5f + i * plantSpacingMeters;
        Vec3 base(0.0f, 0.0f, z);
        Vec3 stalkTop(0.0f, cropHeightMeters, z);

        appendCylinder(mesh, base, stalkTop, 0.025f, 0.012f, Vec3(0.35f, 0.58f, 0.22f), 6);

        // Bilateral alternate crop leaves (e.g. Zea mays / maize)
        int leafNodes = 8;
        for (int n = 0; n < leafNodes; ++n) {
            float leafH = cropHeightMeters * (0.25f + 0.70f * (static_cast<float>(n) / leafNodes));
            float side = (n % 2 == 0) ? 1.0f : -1.0f;
            Vec3 leafNodePos(0.0f, leafH, z);
            Vec3 leafTip(side * 0.45f, leafH - 0.12f, z + 0.05f * side);

            appendLeafBlade(
                mesh,
                leafNodePos,
                leafTip,
                0.48f,
                0.085f,
                20.0f,
                Vec3(0.24f, 0.62f, 0.26f)
            );
            mesh.totalPhytomerCount++;
        }
    }

    return mesh;
}

HeliosCanopyMesh HeliosPlantArchitecture::generateHeliosDesertSucculent(
    int speciesArchetypeIndex,
    float heightMeters,
    float spreadMeters,
    int ribCount,
    uint64_t seed
) {
    HeliosCanopyMesh mesh;
    mesh.architectureName = "Helios Xerophytic Succulent & Cactus Architecture";

    int ribs = std::clamp(ribCount, 6, 24);
    int verticalSteps = 16;
    float dY = heightMeters / verticalSteps;

    for (int y = 0; y < verticalSteps; ++y) {
        float y0 = y * dY;
        float y1 = (y + 1) * dY;
        float ty0 = static_cast<float>(y) / verticalSteps;
        float ty1 = static_cast<float>(y + 1) / verticalSteps;

        float rad0 = (spreadMeters * 0.5f) * (1.0f - ty0 * ty0 * 0.35f);
        float rad1 = (spreadMeters * 0.5f) * (1.0f - ty1 * ty1 * 0.35f);

        appendCylinder(
            mesh,
            Vec3(0.0f, y0, 0.0f),
            Vec3(0.0f, y1, 0.0f),
            rad0,
            rad1,
            Vec3(0.18f, 0.42f, 0.20f),
            ribs
        );
        mesh.totalPhytomerCount++;
    }

    return mesh;
}

} // namespace helios
} // namespace vegetation
