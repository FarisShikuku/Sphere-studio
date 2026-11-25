#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void WarehouseGen::generateSilo(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int siloCount = 3;
    float radius = 3.5f;
    float siloH = std::max(12.0f, params_.height * 1.5f);
    float spacing = radius * 2.3f;
    float totalW = siloCount * spacing;
    float startX = -totalW * 0.5f + spacing * 0.5f;

    uint32_t siloMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);

    // Continuous reinforced concrete foundation pad
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{startX - radius - 1.0f, -0.8f, -radius - 1.0f},
        Vec3{-startX + radius + 1.0f, 0.0f, radius + 1.0f},
        foundMat
    );

    int segments = 16;
    float angleStep = 6.28318f / segments;

    for (int s = 0; s < siloCount; ++s) {
        Vec3 center{startX + s * spacing, 0.0f, 0.0f};

        // Cylindrical silo wall
        for (int i = 0; i < segments; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;

            Vec3 p0 = center + Vec3{std::cos(a0) * radius, 0.0f, std::sin(a0) * radius};
            Vec3 p1 = center + Vec3{std::cos(a1) * radius, 0.0f, std::sin(a1) * radius};
            Vec3 p2 = p1 + Vec3{0.0f, siloH, 0.0f};
            Vec3 p3 = p0 + Vec3{0.0f, siloH, 0.0f};

            Vec3 norm = (Vec3{std::cos((a0 + a1) * 0.5f), 0.0f, std::sin((a0 + a1) * 0.5f)}).normalized();
            BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, norm, siloMat);
        }

        // Conical silo roof cap
        Vec3 apex = center + Vec3{0.0f, siloH + radius * 0.6f, 0.0f};
        for (int i = 0; i < segments; ++i) {
            float a0 = i * angleStep;
            float a1 = (i + 1) * angleStep;
            Vec3 p0 = center + Vec3{std::cos(a0) * radius, siloH, std::sin(a0) * radius};
            Vec3 p1 = center + Vec3{std::cos(a1) * radius, siloH, std::sin(a1) * radius};

            uint32_t baseIdx = static_cast<uint32_t>(outVerts.size());
            Vec3 norm = (apex - (p0 + p1) * 0.5f).cross(p1 - p0).normalized();
            outVerts.push_back({p0, norm, Vec2{0, 0}, frameMat});
            outVerts.push_back({p1, norm, Vec2{1, 0}, frameMat});
            outVerts.push_back({apex, norm, Vec2{0.5f, 1}, frameMat});
            outIndices.push_back(baseIdx);
            outIndices.push_back(baseIdx + 1);
            outIndices.push_back(baseIdx + 2);
        }
    }

    // Top elevated catwalk spanning all silos
    if (params_.hasCatwalks) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{startX - radius * 0.8f, siloH, -1.0f},
            Vec3{-startX + radius * 0.8f, siloH + 1.1f, 1.0f},
            frameMat
        );
    }
}

} // namespace procedural::buildings
