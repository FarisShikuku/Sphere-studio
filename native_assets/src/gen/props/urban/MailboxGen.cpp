#include "MailboxGen.h"

namespace procedural::props {

MailboxGen::MailboxGen() = default;

bool MailboxGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void MailboxGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload MailboxGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    if (params_.boxStyle == 0) {
        // Standard US Blue Postal Collection Drop Box
        uint32_t uspsBlue = 0xFF144D91;
        uint32_t handleCol = 0xFFD0D0D0;

        float w = 0.55f;
        float d = 0.55f;
        float boxH = 0.85f;
        float legH = 0.35f;

        // 4 Steel angle legs anchored to sidewalk
        float legX[2] = {-w * 0.42f, w * 0.42f};
        float legZ[2] = {-d * 0.42f, d * 0.42f};
        for (float lx : legX) {
            for (float lz : legZ) {
                PropMeshHelper::appendBox(payload, Vec3{lx, legH * 0.5f, lz}, Vec3{0.05f, legH, 0.05f}, uspsBlue);
            }
        }

        // Main collection container
        Vec3 boxCenter{0.0f, legH + boxH * 0.5f, 0.0f};
        PropMeshHelper::appendBox(payload, boxCenter, Vec3{w, boxH, d}, uspsBlue);

        // Curved / sloping top hopper chute
        float topY = legH + boxH;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, topY + 0.08f, 0.0f}, Vec3{w * 0.98f, 0.16f, d * 0.98f}, uspsBlue);

        // Pull handle chute
        PropMeshHelper::appendBox(payload, Vec3{0.0f, topY - 0.05f, d * 0.52f}, Vec3{0.32f, 0.06f, 0.08f}, handleCol);

        // White USPS Logo / collection schedule plate
        if (params_.hasCollectionPlate) {
            PropMeshHelper::appendBox(payload, Vec3{0.0f, legH + boxH * 0.6f, d * 0.51f}, Vec3{0.25f, 0.18f, 0.01f}, 0xFFFFFFFF);
            // Lock escutcheon
            PropMeshHelper::appendCylinder(payload, Vec3{w * 0.35f, legH + boxH * 0.3f, d * 0.51f}, 0.02f, 0.02f, 0.02f, 8, 0xFF888888);
        }
    } else if (params_.boxStyle == 1) {
        // British Royal Mail Red Cylindrical Pillar Box
        uint32_t postOfficeRed = 0xFFC51D24;
        uint32_t goldCipher = 0xFFDDAA22;

        float R = 0.28f;
        float H = params_.height;

        // Base plinth
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, R * 1.15f, R * 1.15f, 0.12f, 20, 0xFF222222);

        // Main cylindrical post
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.12f, 0.0f}, R, R, H * 0.72f, 20, postOfficeRed);

        // Domed top cap
        float domeY = 0.12f + H * 0.72f;
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, domeY, 0.0f}, R * 1.08f, R * 1.08f, 0.06f, 20, postOfficeRed);
        PropMeshHelper::appendSphere(payload, Vec3{0.0f, domeY + 0.06f, 0.0f}, R * 1.05f, 8, 16, postOfficeRed);

        // Letter posting aperture slit with hood
        PropMeshHelper::appendBox(payload, Vec3{0.0f, domeY - 0.12f, R * 0.98f}, Vec3{0.25f, 0.05f, 0.08f}, 0xFF151515);

        // Royal Cypher badge
        PropMeshHelper::appendBox(payload, Vec3{0.0f, domeY - 0.35f, R * 1.01f}, Vec3{0.14f, 0.14f, 0.01f}, goldCipher);
    } else {
        // Suburban Post-Mounted Mailbox
        uint32_t postCol = 0xFF5A3E26; // Timber post
        uint32_t boxCol  = 0xFF1E2124; // Black steel box

        // 4x4 Wood post
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.55f, 0.0f}, Vec3{0.10f, 1.10f, 0.10f}, postCol);
        // Cantilever support bracket
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 1.05f, 0.12f}, Vec3{0.10f, 0.04f, 0.35f}, postCol);

        // Half-cylinder arched mailbox tunnel
        Vec3 mCenter{0.0f, 1.18f, 0.15f};
        PropMeshHelper::appendBox(payload, mCenter, Vec3{0.22f, 0.20f, 0.48f}, boxCol);

        // Red outgoing mail flag
        PropMeshHelper::appendBox(payload, Vec3{0.12f, 1.25f, 0.30f}, Vec3{0.01f, 0.15f, 0.04f}, 0xFFFF2222);
    }

    return payload;
}

} // namespace procedural::props
