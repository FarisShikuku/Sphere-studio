#include "PropAttach.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void PropAttach::generateChimney(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float s = params_.scale;
    Vec3 pos = params_.attachPosition;
    uint32_t brickMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t capMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    float chimneyW = 1.2f * s;
    float chimneyD = 1.0f * s;
    float chimneyH = 3.2f * s;

    // Metal flashing base apron around roof penetration
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-chimneyW * 0.6f, -0.05f, -chimneyD * 0.6f},
        pos + Vec3{ chimneyW * 0.6f,  0.08f,  chimneyD * 0.6f},
        metalMat
    );

    // Main brick masonry chimney shaft
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-chimneyW * 0.5f, 0.0f, -chimneyD * 0.5f},
        pos + Vec3{ chimneyW * 0.5f, chimneyH, chimneyD * 0.5f},
        brickMat
    );

    // Corbelled decorative neckband near top
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-chimneyW * 0.55f, chimneyH - 0.4f * s, -chimneyD * 0.55f},
        pos + Vec3{ chimneyW * 0.55f, chimneyH - 0.25f * s,  chimneyD * 0.55f},
        brickMat
    );

    // Concrete crown cap slab
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-chimneyW * 0.6f, chimneyH, -chimneyD * 0.6f},
        pos + Vec3{ chimneyW * 0.6f, chimneyH + 0.18f * s, chimneyD * 0.6f},
        capMat
    );

    // Terracotta clay flue pots / rain hoods
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-0.3f * s, chimneyH + 0.18f * s, -0.25f * s},
        pos + Vec3{ 0.3f * s, chimneyH + 0.75f * s,  0.25f * s},
        metalMat
    );
}

} // namespace procedural::buildings
