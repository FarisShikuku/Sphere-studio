#include "TrashBinGen.h"

namespace procedural::props {

TrashBinGen::TrashBinGen() = default;

bool TrashBinGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void TrashBinGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload TrashBinGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t col = (params_.bodyColor == 0) ? 0xFF2A5532 : // Forest Green
                   (params_.bodyColor == 1) ? 0xFF2F3238 : // Anthracite Grey
                                              0xFF1D5A9C;  // Cobalt Blue

    if (params_.binType == 0) {
        // Cylindrical Municipal Bin
        // Concrete ground plinth
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, params_.radius * 1.15f, params_.radius * 1.1f, 0.08f, 16, 0xFF666666);
        // Main container body
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.08f, 0.0f}, params_.radius, params_.radius * 1.05f, params_.height * 0.75f, 16, col);
        // Metal rim collar
        float rimY = 0.08f + params_.height * 0.75f;
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, rimY, 0.0f}, params_.radius * 1.08f, params_.radius * 1.08f, 0.04f, 16, 0xFF222222);

        // Rain dome hood
        if (params_.hasRainCover) {
            // 2 support posts
            float rSupp = params_.radius * 0.95f;
            PropMeshHelper::appendCylinder(payload, Vec3{-rSupp, rimY, 0.0f}, 0.02f, 0.02f, 0.18f, 8, 0xFF222222);
            PropMeshHelper::appendCylinder(payload, Vec3{ rSupp, rimY, 0.0f}, 0.02f, 0.02f, 0.18f, 8, 0xFF222222);
            // Curved arched canopy
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, rimY + 0.18f, 0.0f}, params_.radius * 1.12f, params_.radius * 0.6f, 0.10f, 16, col);
            // Center ashtray stub
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, rimY + 0.28f, 0.0f}, 0.06f, 0.06f, 0.02f, 8, 0xFF888888);
        }
    } else if (params_.binType == 1) {
        // Square Wooden Slatted Bin
        float w = params_.radius * 1.8f;
        // Steel internal frame
        PropMeshHelper::appendBox(payload, Vec3{0.0f, params_.height * 0.45f, 0.0f}, Vec3{w * 0.92f, params_.height * 0.85f, w * 0.92f}, 0xFF1E2024);

        // Exterior timber slats
        int slatCount = 5;
        float slatH = (params_.height * 0.82f) / slatCount;
        uint32_t woodCol = 0xFFBA8748;

        for (int i = 0; i < slatCount; ++i) {
            float y = 0.06f + i * (slatH + 0.01f) + slatH * 0.5f;
            // North & South sides
            PropMeshHelper::appendBox(payload, Vec3{0.0f, y,  w * 0.5f}, Vec3{w, slatH * 0.88f, 0.03f}, woodCol);
            PropMeshHelper::appendBox(payload, Vec3{0.0f, y, -w * 0.5f}, Vec3{w, slatH * 0.88f, 0.03f}, woodCol);
            // East & West sides
            PropMeshHelper::appendBox(payload, Vec3{ w * 0.5f, y, 0.0f}, Vec3{0.03f, slatH * 0.88f, w}, woodCol);
            PropMeshHelper::appendBox(payload, Vec3{-w * 0.5f, y, 0.0f}, Vec3{0.03f, slatH * 0.88f, w}, woodCol);
        }

        // Metal Pyramid / Flat lid
        float lidY = params_.height * 0.88f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, lidY, 0.0f}, Vec3{w * 1.05f, 0.05f, w * 1.05f}, 0xFF2A2D33);
        if (params_.hasRainCover) {
            PropMeshHelper::appendBox(payload, Vec3{0.0f, lidY + 0.12f, 0.0f}, Vec3{w * 0.95f, 0.04f, w * 0.95f}, 0xFF2A2D33);
        }
    } else {
        // Dual Recycling Station (Trash + Recyclables)
        float w = params_.radius * 2.8f;
        float d = params_.radius * 1.6f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, params_.height * 0.45f, 0.0f}, Vec3{w, params_.height * 0.9f, d}, 0xFF2A2E35);

        // Left door (General Waste - Grey/Green)
        PropMeshHelper::appendBox(payload, Vec3{-w * 0.25f, params_.height * 0.45f, d * 0.51f}, Vec3{w * 0.45f, params_.height * 0.75f, 0.02f}, 0xFF35443B);
        // Right door (Recycle - Blue/Yellow)
        PropMeshHelper::appendBox(payload, Vec3{ w * 0.25f, params_.height * 0.45f, d * 0.51f}, Vec3{w * 0.45f, params_.height * 0.75f, 0.02f}, col);

        // Aperture slots
        PropMeshHelper::appendBox(payload, Vec3{-w * 0.25f, params_.height * 0.78f, d * 0.52f}, Vec3{0.25f, 0.12f, 0.02f}, 0xFF111111);
        PropMeshHelper::appendBox(payload, Vec3{ w * 0.25f, params_.height * 0.78f, d * 0.52f}, Vec3{0.25f, 0.12f, 0.02f}, 0xFF111111);
    }

    return payload;
}

} // namespace procedural::props
