#include "TrafficLightGen.h"

namespace procedural::props {

TrafficLightGen::TrafficLightGen() = default;

bool TrafficLightGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void TrafficLightGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload TrafficLightGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t bodyColor = (params_.housingColor == 0) ? 0xFFE5A912 :
                         (params_.housingColor == 1) ? 0xFF222222 : 0xFF1C3826;

    // Base foundation
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.2f, 0.0f}, Vec3{0.7f, 0.4f, 0.7f}, 0xFF666666);
    // Mast column
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.4f, 0.0f}, 0.16f, 0.10f, params_.mastHeight, 16, 0xFF454B52);

    // Mast arm extending horizontally over the roadway as a continuous cylinder
    float armY = 0.4f + params_.mastHeight * 0.95f;
    Vec3 armStart{0.0f, armY, 0.0f};
    Vec3 armEnd{params_.armLength, armY, 0.0f};
    PropMeshHelper::appendOrientedCylinder(payload, armStart, armEnd, 0.09f, 0.055f, 16, 0xFF454B52, true, true);

    // Diagonal support strut / gusset between vertical mast and horizontal arm
    float gussetSpan = std::min(0.90f, params_.armLength * 0.35f);
    PropMeshHelper::appendOrientedCylinder(payload,
        Vec3{0.0f, armY - gussetSpan, 0.0f},
        Vec3{gussetSpan, armY, 0.0f},
        0.045f, 0.045f, 10, 0xFF454B52, true, true);

    // Helper to build 3-aspect traffic light head
    auto appendSignalHead = [&](const Vec3& center, float scale) {
        Vec3 boxSize = Vec3{0.32f, 0.95f, 0.22f} * scale;
        PropMeshHelper::appendBox(payload, center, boxSize, bodyColor);

        // Backplate border
        PropMeshHelper::appendBox(payload, center + Vec3{0.0f, 0.0f, 0.02f * scale}, Vec3{0.48f, 1.15f, 0.02f} * scale, 0xFF151515);

        // 3 Lenses: Top (Red), Mid (Yellow), Bot (Green)
        float lensR = 0.10f * scale;
        float yOffsets[3] = {0.30f * scale, 0.0f, -0.30f * scale};
        uint32_t activeColors[3] = {0xFFFF2222, 0xFFFFCC00, 0xFF00FF66};
        uint32_t dimColors[3] = {0xFF551111, 0xFF554400, 0xFF00441A};

        for (int i = 0; i < 3; ++i) {
            Vec3 lensPos = center + Vec3{0.0f, yOffsets[i], 0.12f * scale};
            uint32_t col = (params_.activeLight == i) ? activeColors[i] : dimColors[i];
            PropMeshHelper::appendSphere(payload, lensPos, lensR, 6, 8, col);

            // Cowl / visor arch over lens
            Vec3 cowlPos = center + Vec3{0.0f, yOffsets[i] + 0.08f * scale, 0.16f * scale};
            PropMeshHelper::appendBox(payload, cowlPos, Vec3{0.24f * scale, 0.04f * scale, 0.14f * scale}, bodyColor);
        }
    };

    // Primary Overhead Signal Head (at end of arm)
    float overheadX = params_.armLength * 0.85f;
    Vec3 overheadHeadPos{overheadX, armY - 0.55f, 0.0f};

    // Continuous vertical hanger bracket connecting the horizontal arm to the overhead signal head
    PropMeshHelper::appendOrientedCylinder(payload,
        Vec3{overheadX, armY, 0.0f},
        Vec3{overheadX, overheadHeadPos.y + 0.48f, 0.0f},
        0.038f, 0.038f, 8, 0xFF2A2E33, true, true);
    appendSignalHead(overheadHeadPos, 1.0f);

    // Secondary Pole-Mounted Head (for cross-traffic or near vehicle view)
    if (params_.hasSecondaryHead) {
        Vec3 secPos{0.32f, 3.2f, 0.0f};
        // Continuous horizontal bracket connecting pole to secondary head
        PropMeshHelper::appendOrientedCylinder(payload,
            Vec3{0.08f, secPos.y, 0.0f},
            Vec3{secPos.x - 0.16f, secPos.y, 0.0f},
            0.035f, 0.035f, 8, 0xFF2A2E33, true, true);
        appendSignalHead(secPos, 0.85f);
    }

    // Pedestrian Signal Box
    if (params_.hasPedestrianBox) {
        Vec3 pedPos{0.28f, 2.1f, 0.0f};
        // Continuous horizontal bracket connecting pole to pedestrian box
        PropMeshHelper::appendOrientedCylinder(payload,
            Vec3{0.10f, pedPos.y, 0.0f},
            Vec3{pedPos.x - 0.13f, pedPos.y, 0.0f},
            0.032f, 0.032f, 8, 0xFF2A2E33, true, true);
        PropMeshHelper::appendBox(payload, pedPos, Vec3{0.26f, 0.32f, 0.18f}, 0xFF1A1A1A);
        // Pedestrian symbol display
        PropMeshHelper::appendBox(payload, pedPos + Vec3{0.0f, 0.0f, 0.10f}, Vec3{0.20f, 0.25f, 0.02f}, 0xFFFF5500);
    }

    return payload;
}

} // namespace procedural::props
