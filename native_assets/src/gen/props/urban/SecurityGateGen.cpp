#include "SecurityGateGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

SecurityGateGen::SecurityGateGen() = default;

bool SecurityGateGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void SecurityGateGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload SecurityGateGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float W = std::clamp(params_.width, 2.5f, 10.0f);
    float H = std::clamp(params_.height, 1.0f, 3.5f);
    float trackOff = std::clamp(params_.trackOffset, 0.05f, 0.30f);

    uint32_t gateCol = (params_.colorScheme == 1) ? 0xFF1E2229 : // Jet Black
                       (params_.colorScheme == 2) ? 0xFFFACC15 : // Safety Yellow
                       (params_.colorScheme == 3) ? 0xFFF8FAFC : // Traffic White
                                                    0xFF94A3B8;  // Galvanized Steel

    if (params_.gateStyle == 0) {
        // Motorized Cantilever Sliding Security Gate
        float totalLeafW = W * 1.35f;
        PropMeshHelper::appendBox(payload, Vec3{W * 0.45f, 0.08f, 0.0f}, Vec3{W * 0.70f, 0.16f, 0.40f}, 0xFF64748B);

        for (float gx : {W * 0.15f, W * 0.65f}) {
            PropMeshHelper::appendBox(payload, Vec3{gx, (H + 0.2f) * 0.5f, -0.15f}, Vec3{0.12f, H + 0.2f, 0.12f}, gateCol);
            PropMeshHelper::appendBox(payload, Vec3{gx, (H + 0.2f) * 0.5f,  0.15f}, Vec3{0.12f, H + 0.2f, 0.12f}, gateCol);
            PropMeshHelper::appendBox(payload, Vec3{gx, H + 0.2f, 0.0f}, Vec3{0.12f, 0.06f, 0.42f}, gateCol);
        }

        PropMeshHelper::appendBox(payload, Vec3{-W * 0.52f, (H + 0.1f) * 0.5f, 0.0f}, Vec3{0.14f, H + 0.1f, 0.14f}, gateCol);

        float leafCenterX = W * 0.10f;
        PropMeshHelper::appendBox(payload, Vec3{leafCenterX, trackOff + 0.06f, 0.0f}, Vec3{totalLeafW, 0.12f, 0.08f}, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{leafCenterX, H, 0.0f}, Vec3{totalLeafW, 0.06f, 0.06f}, gateCol);

        int pickets = static_cast<int>(W / 0.14f);
        float picketH = H - trackOff - 0.12f;
        for (int p = 0; p <= pickets; ++p) {
            float px = -W * 0.5f + p * (W / pickets);
            PropMeshHelper::appendBox(payload, Vec3{px, trackOff + 0.06f + picketH * 0.5f, 0.0f}, Vec3{0.025f, picketH, 0.025f}, gateCol);
        }

        PropMeshHelper::appendStrut(payload, Vec3{W * 0.05f, trackOff + 0.12f, 0.0f}, Vec3{W * 0.75f, H - 0.05f, 0.0f}, 0.035f, gateCol);

        if (params_.isMotorized) {
            PropMeshHelper::appendBox(payload, Vec3{W * 0.35f, 0.35f, 0.22f}, Vec3{0.38f, 0.55f, 0.25f}, 0xFF334155);
            PropMeshHelper::appendCylinder(payload, Vec3{-W * 0.65f, 0.0f, 0.65f}, 0.04f, 0.04f, 1.15f, 8, 0xFF1E293B);
            PropMeshHelper::appendBox(payload, Vec3{-W * 0.65f, 1.2f, 0.65f}, Vec3{0.18f, 0.24f, 0.10f}, 0xFF0F172A);
            PropMeshHelper::appendSphere(payload, Vec3{W * 0.15f, H + 0.30f, 0.0f}, 0.06f, 6, 8, 0xFFF59E0B);
        }
    } else if (params_.gateStyle == 1) {
        // Double-Swing Commercial Security Gate
        float halfW = W * 0.5f;
        for (float side : {-1.0f, 1.0f}) {
            float px = side * (halfW + 0.10f);
            PropMeshHelper::appendBox(payload, Vec3{px, 0.06f, 0.0f}, Vec3{0.25f, 0.12f, 0.25f}, 0xFF475569);
            PropMeshHelper::appendBox(payload, Vec3{px, H * 0.55f, 0.0f}, Vec3{0.16f, H * 1.1f, 0.16f}, gateCol);
        }
        for (float side : {-1.0f, 1.0f}) {
            float leafMidX = side * (halfW * 0.5f);
            float leafW = halfW - 0.06f;
            PropMeshHelper::appendBox(payload, Vec3{leafMidX, trackOff + 0.04f, 0.0f}, Vec3{leafW, 0.08f, 0.05f}, gateCol);
            PropMeshHelper::appendBox(payload, Vec3{leafMidX, H, 0.0f}, Vec3{leafW, 0.08f, 0.05f}, gateCol);
            PropMeshHelper::appendStrut(payload, Vec3{side * (halfW - 0.03f), trackOff + 0.08f, 0.0f}, Vec3{side * 0.04f, H - 0.04f, 0.0f}, 0.016f, 0xFFCBD5E1);
        }
    } else if (params_.gateStyle == 2) {
        // Traffic Boom Barrier Gate
        float cabW = 0.40f; float cabH = 1.15f; float cabD = 0.38f;
        PropMeshHelper::appendBox(payload, Vec3{-W * 0.45f, 0.05f, 0.0f}, Vec3{cabW + 0.1f, 0.10f, cabD + 0.1f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, Vec3{-W * 0.45f, 0.10f + cabH * 0.5f, 0.0f}, Vec3{cabW, cabH, cabD}, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{-W * 0.45f, 0.10f + cabH + 0.03f, 0.0f}, Vec3{cabW + 0.03f, 0.06f, cabD + 0.03f}, 0xFF1E293B);

        float armY = 0.95f;
        float armL = W * 0.88f;
        float armStartX = -W * 0.45f + cabW * 0.5f + 0.08f;
        float armMidX = armStartX + armL * 0.5f;
        PropMeshHelper::appendBox(payload, Vec3{armMidX, armY, 0.0f}, Vec3{armL, 0.10f, 0.045f}, 0xFFFFFFFF);
        if (params_.isMotorized) {
            PropMeshHelper::appendSphere(payload, Vec3{-W * 0.45f, 0.10f + cabH + 0.10f, 0.0f}, 0.065f, 8, 10, 0xFFF59E0B);
        }
    } else {
        // Full-Height Turnstile
        float turnR = W * 0.28f;
        float turnH = std::max(H, 2.2f);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.04f, 0.0f}, Vec3{W * 0.7f, 0.08f, turnR * 2.4f}, 0xFF64748B);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.08f, 0.0f}, 0.05f, 0.05f, turnH - 0.16f, 10, gateCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, turnH, 0.0f}, Vec3{W * 0.75f, 0.18f, turnR * 2.5f}, 0xFF1E293B);
    }

    return payload;
}

} // namespace procedural::props
