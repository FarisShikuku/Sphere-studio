#include "DamageGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void DamageGen::generateRubblePile(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    Vec3 orig = params_.origin;
    float r = params_.radius;
    int count = params_.rubbleChunkCount;

    uint32_t concreteMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t rebarMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // Mounded base of fine gravel / pulverized concrete
    BuildingComponents::appendBox(
        outVerts, outIndices,
        orig + Vec3{-r * 0.8f, 0.0f, -r * 0.8f},
        orig + Vec3{ r * 0.8f, 0.35f,  r * 0.8f},
        foundMat
    );

    // Scattered fractured concrete debris chunks of varied orientations and sizes
    for (int i = 0; i < count; ++i) {
        float angle = (i * 2.39996f); // Golden angle distribution
        float dist = (std::sqrt(static_cast<float>(i + 1) / count)) * (r * 0.75f);
        float cx = orig.x + std::cos(angle) * dist;
        float cz = orig.z + std::sin(angle) * dist;
        float pileHeight = (1.0f - dist / r) * 1.4f + 0.1f;

        float sx = 0.3f + 0.35f * std::sin(i * 1.7f);
        float sy = 0.2f + 0.25f * std::cos(i * 2.3f);
        float sz = 0.3f + 0.30f * std::sin(i * 3.1f);

        uint32_t mat = (i % 3 == 0) ? foundMat : concreteMat;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{cx - sx * 0.5f, pileHeight, cz - sz * 0.5f},
            Vec3{cx + sx * 0.5f, pileHeight + sy, cz + sz * 0.5f},
            mat
        );
    }

    // Twisted rebar segments protruding from rubble mound
    if (params_.exposeRebar) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            orig + Vec3{-0.5f, 0.2f, -0.1f},
            orig + Vec3{ 0.8f, 1.1f,  0.0f},
            rebarMat
        );
        BuildingComponents::appendBox(
            outVerts, outIndices,
            orig + Vec3{0.2f, 0.4f, -0.4f},
            orig + Vec3{0.3f, 1.4f,  0.5f},
            rebarMat
        );
    }
}

} // namespace procedural::buildings
