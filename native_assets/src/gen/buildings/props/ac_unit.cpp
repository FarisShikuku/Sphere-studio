#include "PropAttach.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void PropAttach::generateAcUnit(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float s = params_.scale;
    Vec3 pos = params_.attachPosition;
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    int count = params_.clusterUnits ? params_.clusterCount : 1;

    for (int i = 0; i < count; ++i) {
        Vec3 base = pos + Vec3{i * 2.2f * s, 0.0f, 0.0f};

        // Vibration isolation mounting curbs
        BuildingComponents::appendBox(
            outVerts, outIndices,
            base + Vec3{-0.85f * s, 0.0f, -0.6f * s},
            base + Vec3{ 0.85f * s, 0.15f * s,  0.6f * s},
            frameMat
        );

        // Main sheet metal condenser cabinet
        BuildingComponents::appendBox(
            outVerts, outIndices,
            base + Vec3{-0.8f * s, 0.15f * s, -0.55f * s},
            base + Vec3{ 0.8f * s, 1.25f * s,  0.55f * s},
            hvacMat
        );

        // Circular fan exhaust cowl on top
        BuildingComponents::appendBox(
            outVerts, outIndices,
            base + Vec3{-0.4f * s, 1.25f * s, -0.4f * s},
            base + Vec3{ 0.4f * s, 1.45f * s,  0.4f * s},
            frameMat
        );

        // Refrigerant piping conduit connection
        BuildingComponents::appendBox(
            outVerts, outIndices,
            base + Vec3{0.7f * s, 0.2f * s, -0.65f * s},
            base + Vec3{0.85f * s, 0.6f * s, -0.55f * s},
            frameMat
        );
    }
}

} // namespace procedural::buildings
