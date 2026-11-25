#include "PropAttach.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void PropAttach::generateAntenna(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float s = params_.scale;
    Vec3 pos = params_.attachPosition;
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);

    float mastH = 8.0f * s;
    float mastRadius = 0.08f * s;

    // Structural steel base tripod mount
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-0.6f * s, 0.0f, -0.6f * s},
        pos + Vec3{ 0.6f * s, 0.2f * s,  0.6f * s},
        metalMat
    );

    // Vertical telecommunication mast
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-mastRadius, 0.0f, -mastRadius},
        pos + Vec3{ mastRadius, mastH,  mastRadius},
        metalMat
    );

    // Cross-dipole antenna elements at various heights
    float levels[3] = {mastH * 0.45f, mastH * 0.7f, mastH * 0.9f};
    for (int i = 0; i < 3; ++i) {
        float y = levels[i];
        float armL = (0.9f - i * 0.2f) * s;
        // X-axis dipole
        BuildingComponents::appendBox(
            outVerts, outIndices,
            pos + Vec3{-armL, y - 0.02f * s, -0.02f * s},
            pos + Vec3{ armL, y + 0.02f * s,  0.02f * s},
            metalMat
        );
        // Z-axis dipole
        BuildingComponents::appendBox(
            outVerts, outIndices,
            pos + Vec3{-0.02f * s, y - 0.02f * s, -armL * 0.8f},
            pos + Vec3{ 0.02f * s, y + 0.02f * s,  armL * 0.8f},
            metalMat
        );
    }

    // Parabolic microwave drum dish attached to mid-mast
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{0.1f * s, mastH * 0.55f, -0.45f * s},
        pos + Vec3{0.35f * s, mastH * 0.55f + 0.9f * s, 0.45f * s},
        hvacMat
    );

    // Pointed lightning arrester rod tip
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pos + Vec3{-0.02f * s, mastH, -0.02f * s},
        pos + Vec3{ 0.02f * s, mastH + 1.2f * s, 0.02f * s},
        metalMat
    );
}

} // namespace procedural::buildings
