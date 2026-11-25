#include "FarmGateGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

FarmGateGen::FarmGateGen() = default;

bool FarmGateGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void FarmGateGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload FarmGateGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float W = std::clamp(params_.width, 2.0f, 6.0f);
    float H = std::clamp(params_.height, 1.0f, 2.0f);
    float postThick = std::clamp(params_.postThickness, 0.12f, 0.28f);

    uint32_t gateCol = (params_.colorScheme == 1) ? 0xFFF8FAFC : // Painted White
                       (params_.colorScheme == 2) ? 0xFFB91C1C : // Barn Red
                       (params_.colorScheme == 3) ? 0xFF166534 : // Forest Green
                                                    0xFFCBD5E1;  // Galvanized Steel / Natural

    if (params_.gateStyle == 0) {
        // 5-Bar Galvanized Tubular Steel Pasture Gate
        float postH = H * 1.35f;
        uint32_t postWood = 0xFF5D4037;
        PropMeshHelper::appendCylinder(payload, Vec3{-W * 0.5f - postThick * 0.5f, 0.0f, 0.0f}, postThick * 0.5f, postThick * 0.45f, postH, 10, postWood);
        PropMeshHelper::appendCylinder(payload, Vec3{ W * 0.5f + postThick * 0.5f, 0.0f, 0.0f}, postThick * 0.5f, postThick * 0.45f, postH, 10, postWood);

        float tubeR = 0.028f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.10f, 0.0f}, Vec3{W, tubeR * 2, tubeR * 2}, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H, 0.0f}, Vec3{W, tubeR * 2, tubeR * 2}, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{-W * 0.5f + tubeR, (H + 0.10f) * 0.5f, 0.0f}, Vec3{tubeR * 2, H - 0.10f, tubeR * 2}, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{ W * 0.5f - tubeR, (H + 0.10f) * 0.5f, 0.0f}, Vec3{tubeR * 2, H - 0.10f, tubeR * 2}, gateCol);

        for (int r = 1; r <= 3; ++r) {
            float ry = 0.10f + r * ((H - 0.10f) / 4.0f);
            PropMeshHelper::appendBox(payload, Vec3{0.0f, ry, 0.0f}, Vec3{W - tubeR * 4, tubeR * 2, tubeR * 2}, gateCol);
        }
        PropMeshHelper::appendBox(payload, Vec3{0.0f, (H + 0.10f) * 0.5f, 0.0f}, Vec3{tubeR * 2, H - 0.10f, tubeR * 2}, gateCol);

        if (params_.hasLockChain) {
            PropMeshHelper::appendBox(payload, Vec3{W * 0.5f + 0.04f, H * 0.65f, 0.0f}, Vec3{0.14f, 0.04f, 0.04f}, 0xFF334155);
        }
    } else if (params_.gateStyle == 1) {
        // Solid Timber 5-Bar Paddock Gate with Diagonal Brace
        uint32_t woodCol = (params_.colorScheme == 1) ? 0xFFF8FAFC : (params_.colorScheme == 2) ? 0xFFB91C1C : 0xFF8D6E63;
        float postH = H * 1.30f;
        PropMeshHelper::appendBox(payload, Vec3{-W * 0.5f - postThick * 0.5f, postH * 0.5f, 0.0f}, Vec3{postThick, postH, postThick}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{ W * 0.5f + postThick * 0.5f, postH * 0.5f, 0.0f}, Vec3{postThick, postH, postThick}, woodCol);

        PropMeshHelper::appendBox(payload, Vec3{0.0f, H, 0.0f}, Vec3{W, 0.12f, 0.07f}, woodCol);
        for (int r = 0; r <= 3; ++r) {
            float ry = 0.08f + r * ((H - 0.16f) / 3.0f);
            PropMeshHelper::appendBox(payload, Vec3{0.0f, ry, 0.0f}, Vec3{W, 0.08f, 0.04f}, woodCol);
        }
        PropMeshHelper::appendStrut(payload, Vec3{-W * 0.5f + 0.06f, 0.12f, 0.025f}, Vec3{W * 0.5f - 0.06f, H - 0.06f, 0.025f}, 0.06f, woodCol);
    } else if (params_.gateStyle == 2) {
        // Arched Bow Gate
        float archH = H * 1.6f;
        float tubeR = 0.035f;
        PropMeshHelper::appendBox(payload, Vec3{-W * 0.5f - 0.04f, archH * 0.5f, 0.0f}, Vec3{tubeR * 2, archH, tubeR * 2}, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{ W * 0.5f + 0.04f, archH * 0.5f, 0.0f}, Vec3{tubeR * 2, archH, tubeR * 2}, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, archH, 0.0f}, Vec3{W + 0.16f, tubeR * 2, tubeR * 2}, gateCol);

        for (int r = 0; r < 6; ++r) {
            float ry = 0.12f + r * ((H - 0.12f) / 5.0f);
            PropMeshHelper::appendBox(payload, Vec3{0.0f, ry, 0.0f}, Vec3{W - 0.08f, 0.045f, 0.045f}, gateCol);
        }
    } else {
        // Texas Cattle Guard
        float pitW = 2.4f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.08f, -pitW * 0.5f}, Vec3{W, 0.20f, 0.20f}, 0xFF64748B);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.08f,  pitW * 0.5f}, Vec3{W, 0.20f, 0.20f}, 0xFF64748B);

        int numPipes = 14;
        for (int p = 0; p < numPipes; ++p) {
            float pz = -pitW * 0.45f + p * (pitW * 0.90f / (numPipes - 1));
            PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.18f, pz}, Vec3{W, 0.08f, 0.08f}, 0xFF94A3B8);
        }
    }

    return payload;
}

} // namespace procedural::props
