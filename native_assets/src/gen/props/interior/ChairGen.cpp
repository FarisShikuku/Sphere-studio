#include "ChairGen.h"

namespace procedural::props {

ChairGen::ChairGen() = default;

bool ChairGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void ChairGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload ChairGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t seatCol = (params_.upholsteryColor == 0) ? 0xFF202326 : // Stealth Black
                       (params_.upholsteryColor == 1) ? 0xFF5A626A : // Slate Grey
                       (params_.upholsteryColor == 2) ? 0xFF9E5B32 : // Cognac Leather
                                                        0xFF1A6B72;  // Ocean Teal

    uint32_t baseCol = 0xFF181A1C;
    uint32_t chromeCol = 0xFFD8DDE2;

    float sY = params_.seatHeight;

    if (params_.chairType == 0) {
        // Ergonomic 5-Star Caster Mesh Swivel Office Chair
        // 1. 5-Star Caster Base
        int starPoints = 5;
        float baseRadius = 0.32f;
        for (int i = 0; i < starPoints; ++i) {
            float a = i * (2.0f * 3.14159f / starPoints);
            float lx = std::cos(a) * baseRadius;
            float lz = std::sin(a) * baseRadius;

            // Leg spoke ray
            PropMeshHelper::appendBox(payload, Vec3{lx * 0.5f, 0.08f, lz * 0.5f}, Vec3{baseRadius * 0.9f, 0.035f, 0.04f}, baseCol, -a);
            // Caster wheel housing
            PropMeshHelper::appendSphere(payload, Vec3{lx, 0.04f, lz}, 0.035f, 6, 6, baseCol);
        }

        // 2. Central pneumatic gas lift cylinder
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.08f, 0.0f}, 0.035f, 0.030f, sY - 0.12f, 12, chromeCol);
        // Tilt mechanism housing
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY - 0.05f, 0.0f}, Vec3{0.22f, 0.06f, 0.22f}, baseCol);

        // 3. Ergonomic contoured seat pan
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY, 0.0f}, Vec3{0.48f, 0.06f, 0.46f}, seatCol);

        // 4. Mesh Backrest with lumbar support spine
        // Rear spine support bracket
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY + 0.25f, -0.22f}, Vec3{0.08f, 0.45f, 0.04f}, baseCol);
        // Lumbar cushion
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY + 0.18f, -0.19f}, Vec3{0.32f, 0.12f, 0.03f}, seatCol);
        // Curved backrest frame
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY + 0.35f, -0.21f}, Vec3{0.44f, 0.45f, 0.025f}, seatCol);

        // 5. 3D Adjustable T-Armrests
        if (params_.hasArmrests) {
            for (float side : {-1.0f, 1.0f}) {
                Vec3 stemPos{side * 0.26f, sY + 0.10f, 0.0f};
                PropMeshHelper::appendBox(payload, stemPos, Vec3{0.035f, 0.20f, 0.035f}, baseCol);
                // Arm pad
                PropMeshHelper::appendBox(payload, stemPos + Vec3{0.0f, 0.10f, 0.0f}, Vec3{0.08f, 0.025f, 0.22f}, baseCol);
            }
        }
    } else if (params_.chairType == 1) {
        // Classic Dining Chair: 4 wood legs, contoured seat, spindle/slat back
        uint32_t woodCol = 0xFF8A552C;
        float legThick = 0.04f;

        // 4 Splayed Legs
        float lx[2] = {-0.18f, 0.18f};
        float lz[2] = {-0.18f, 0.18f};
        for (float x : lx) {
            for (float z : lz) {
                PropMeshHelper::appendBox(payload, Vec3{x, sY * 0.5f, z}, Vec3{legThick, sY, legThick}, woodCol);
            }
        }

        // Padded / Solid Seat
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY, 0.0f}, Vec3{0.44f, 0.04f, 0.42f}, seatCol);

        // Backrest uprights & horizontal crest rail
        PropMeshHelper::appendBox(payload, Vec3{-0.18f, sY + 0.25f, -0.18f}, Vec3{0.04f, 0.50f, 0.04f}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{ 0.18f, sY + 0.25f, -0.18f}, Vec3{0.04f, 0.50f, 0.04f}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY + 0.48f, -0.18f}, Vec3{0.42f, 0.08f, 0.03f}, woodCol);
        // Center backrest slats
        PropMeshHelper::appendBox(payload, Vec3{-0.06f, sY + 0.25f, -0.18f}, Vec3{0.025f, 0.40f, 0.02f}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{ 0.06f, sY + 0.25f, -0.18f}, Vec3{0.025f, 0.40f, 0.02f}, woodCol);
    } else {
        // Lounge Club Armchair
        float w = 0.72f;
        float d = 0.68f;
        // Low plinth feet
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.04f, 0.0f}, Vec3{w * 0.9f, 0.08f, d * 0.9f}, 0xFF1C1E20);

        // Thick seat cushion
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY, 0.0f}, Vec3{w * 0.75f, 0.18f, d * 0.8f}, seatCol);

        // Heavy wrap-around upholstered armrests & backrest
        PropMeshHelper::appendBox(payload, Vec3{-w * 0.42f, sY + 0.12f, 0.0f}, Vec3{0.14f, 0.38f, d}, seatCol);
        PropMeshHelper::appendBox(payload, Vec3{ w * 0.42f, sY + 0.12f, 0.0f}, Vec3{0.14f, 0.38f, d}, seatCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, sY + 0.25f, -d * 0.42f}, Vec3{w, 0.50f, 0.14f}, seatCol);
    }

    return payload;
}

} // namespace procedural::props
