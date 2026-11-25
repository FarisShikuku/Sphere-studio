#include "BenchGen.h"

namespace procedural::props {

BenchGen::BenchGen() = default;

bool BenchGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void BenchGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload BenchGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t woodCol = (params_.woodTint == 0) ? 0xFFC8823E :  // Teak
                       (params_.woodTint == 1) ? 0xFFDDB477 :  // Oak
                       (params_.woodTint == 2) ? 0xFF654321 :  // Walnut
                                                 0xFF2B5B37;   // Park Green

    uint32_t frameCol = (params_.frameStyle == 0) ? 0xFF22262B : // Cast Iron
                        (params_.frameStyle == 1) ? 0xFF888888 : // Concrete/Steel
                                                    0xFF7A4A28;  // Rustic Timber

    float halfLen = params_.length * 0.5f;
    float seatY = 0.45f;
    float seatDepth = 0.48f;

    // Two side supports (legs/frames)
    float frameX[2] = {-halfLen * 0.85f, halfLen * 0.85f};
    for (float fx : frameX) {
        // Front & Rear legs
        PropMeshHelper::appendBox(payload, Vec3{fx, seatY * 0.5f, -seatDepth * 0.4f}, Vec3{0.06f, seatY, 0.06f}, frameCol);
        PropMeshHelper::appendBox(payload, Vec3{fx, seatY * 0.5f,  seatDepth * 0.4f}, Vec3{0.06f, seatY, 0.06f}, frameCol);
        // Base ground rail
        PropMeshHelper::appendBox(payload, Vec3{fx, 0.04f, 0.0f}, Vec3{0.06f, 0.08f, seatDepth}, frameCol);
        // Seat under-bearer
        PropMeshHelper::appendBox(payload, Vec3{fx, seatY - 0.04f, 0.0f}, Vec3{0.07f, 0.06f, seatDepth * 0.95f}, frameCol);

        if (params_.hasBackrest) {
            // Upright backrest bracket
            PropMeshHelper::appendBox(payload, Vec3{fx, seatY + 0.28f, -seatDepth * 0.45f}, Vec3{0.06f, 0.58f, 0.06f}, frameCol);
        }

        if (params_.hasArmrests) {
            // Armrest loop
            PropMeshHelper::appendBox(payload, Vec3{fx, seatY + 0.20f, 0.0f}, Vec3{0.06f, 0.05f, seatDepth * 0.7f}, frameCol);
            PropMeshHelper::appendBox(payload, Vec3{fx, seatY + 0.10f, seatDepth * 0.32f}, Vec3{0.05f, 0.20f, 0.05f}, frameCol);
        }
    }

    // Longitudinal wooden seat slats
    int seatSlats = params_.slatCount / 2 + 1;
    float slatW = (seatDepth * 0.85f) / static_cast<float>(seatSlats);
    float slatThick = 0.035f;

    for (int i = 0; i < seatSlats; ++i) {
        float z = -seatDepth * 0.4f + i * (slatW + 0.015f);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, seatY + slatThick * 0.5f, z}, Vec3{params_.length, slatThick, slatW * 0.85f}, woodCol);
    }

    // Backrest slats
    if (params_.hasBackrest) {
        int backSlats = params_.slatCount - seatSlats;
        if (backSlats < 2) backSlats = 2;
        float backH = 0.45f;
        float stepY = backH / static_cast<float>(backSlats);

        for (int i = 0; i < backSlats; ++i) {
            float y = seatY + 0.12f + i * stepY;
            float z = -seatDepth * 0.45f - i * 0.02f; // Slight ergonomic tilt
            PropMeshHelper::appendBox(payload, Vec3{0.0f, y, z}, Vec3{params_.length, stepY * 0.75f, slatThick}, woodCol);
        }
    }

    return payload;
}

} // namespace procedural::props
