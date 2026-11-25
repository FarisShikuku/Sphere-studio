#include "PipeGen.h"

namespace procedural::props {

PipeGen::PipeGen() = default;

bool PipeGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void PipeGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload PipeGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t pipeCol = (params_.materialType == 0) ? 0xFF656D75 : // Steel
                       (params_.materialType == 1) ? 0xFFE0AE1B : // Gas Yellow
                                                     0xFF999A9C;  // Insulation

    uint32_t flangeCol = 0xFF35393E;
    uint32_t valveCol  = 0xFFB8281D; // Red valve handwheel

    float R = params_.radius;
    float L = params_.length;

    // Helper to append an oriented bolt circle flange
    auto appendFlange = [&](const Vec3& center, const Vec3& dir) {
        float fR = R * 1.55f;
        float fThick = 0.05f;
        Vec3 start = center - dir * (fThick * 0.5f);
        Vec3 end   = center + dir * (fThick * 0.5f);
        PropMeshHelper::appendOrientedCylinder(payload, start, end, fR, fR, 16, flangeCol, true, true);
    };

    if (params_.pipeConfig == 0) {
        // Straight horizontal spool on support cradles
        PropMeshHelper::appendOrientedCylinder(payload, Vec3{-L * 0.5f, R + 0.1f, 0.0f}, Vec3{L * 0.5f, R + 0.1f, 0.0f}, R, R, 16, pipeCol, true, true);

        // End flanges
        appendFlange(Vec3{-L * 0.5f, R + 0.1f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f});
        appendFlange(Vec3{ L * 0.5f, R + 0.1f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f});

        // Intermediate flange
        if (params_.flangeCount > 2) {
            appendFlange(Vec3{0.0f, R + 0.1f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f});
        }

        // Concrete support cradles
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.3f, 0.08f, 0.0f}, Vec3{0.25f, 0.16f, R * 2.8f}, 0xFF555555);
        PropMeshHelper::appendBox(payload, Vec3{ L * 0.3f, 0.08f, 0.0f}, Vec3{0.25f, 0.16f, R * 2.8f}, 0xFF555555);

        // Handwheel valve in the center
        if (params_.hasHandwheelValve) {
            Vec3 vCenter{0.15f, R + 0.1f, 0.0f};
            // Valve bonnet body
            PropMeshHelper::appendBox(payload, vCenter, Vec3{R * 2.0f, R * 2.2f, R * 2.0f}, flangeCol);
            // Stem riser
            PropMeshHelper::appendCylinder(payload, vCenter + Vec3{0.0f, R * 1.1f, 0.0f}, 0.025f, 0.025f, 0.35f, 8, 0xFF777777);
            // Handwheel rim & spokes
            Vec3 wheelCenter = vCenter + Vec3{0.0f, R * 1.1f + 0.35f, 0.0f};
            PropMeshHelper::appendCylinder(payload, wheelCenter, R * 1.1f, R * 1.1f, 0.03f, 16, valveCol);
            PropMeshHelper::appendCylinder(payload, wheelCenter + Vec3{0.0f, 0.015f, 0.0f}, 0.04f, 0.04f, 0.04f, 8, 0xFF333333);
        }
    } else if (params_.pipeConfig == 1) {
        // 90-Degree Elbow riser
        float arm1 = L * 0.55f;
        float arm2 = L * 0.45f;
        // Horizontal leg
        PropMeshHelper::appendOrientedCylinder(payload, Vec3{-arm1, R + 0.1f, 0.0f}, Vec3{0.0f, R + 0.1f, 0.0f}, R, R, 16, pipeCol, true, true);
        // Vertical riser leg
        PropMeshHelper::appendOrientedCylinder(payload, Vec3{0.0f, R + 0.1f, 0.0f}, Vec3{0.0f, R + 0.1f + arm2, 0.0f}, R, R, 16, pipeCol, true, true);

        // Elbow joint sphere/junction
        PropMeshHelper::appendSphere(payload, Vec3{0.0f, R + 0.1f, 0.0f}, R * 1.15f, 8, 12, pipeCol);

        // Flanges at endpoints
        appendFlange(Vec3{-arm1, R + 0.1f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f});
        appendFlange(Vec3{0.0f, R + 0.1f + arm2, 0.0f}, Vec3{0.0f, 1.0f, 0.0f});
    } else {
        // T-Junction with bypass valve
        PropMeshHelper::appendOrientedCylinder(payload, Vec3{-L * 0.5f, R + 0.15f, 0.0f}, Vec3{L * 0.5f, R + 0.15f, 0.0f}, R, R, 16, pipeCol, true, true);
        PropMeshHelper::appendOrientedCylinder(payload, Vec3{0.0f, R + 0.15f, 0.0f}, Vec3{0.0f, R + 0.15f, L * 0.4f}, R * 0.8f, R * 0.8f, 14, pipeCol, true, true);
        PropMeshHelper::appendSphere(payload, Vec3{0.0f, R + 0.15f, 0.0f}, R * 1.2f, 8, 12, flangeCol);

        appendFlange(Vec3{-L * 0.5f, R + 0.15f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f});
        appendFlange(Vec3{ L * 0.5f, R + 0.15f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f});
        appendFlange(Vec3{0.0f, R + 0.15f, L * 0.4f}, Vec3{0.0f, 0.0f, 1.0f});
    }

    return payload;
}

} // namespace procedural::props
