#include "SubstationGen.h"

namespace procedural::props {

SubstationGen::SubstationGen() {
    params_ = SubstationParams{};
}

bool SubstationGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void SubstationGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload SubstationGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t gravelColor = 0xFF78716C;      // Crushed aggregate substation ground
    uint32_t roadwayDark = 0xFF334155;
    uint32_t transformerColor = 0xFF475569; // Heavy power transformer tank
    uint32_t radiatorSteel = 0xFF334155;
    uint32_t gantrySteel = 0xFF94A3B8;      // Structural steel gantry
    uint32_t insulatorColor = 0xFF8D4024;   // Brown porcelain HV bushings
    uint32_t busbarColor = 0xFFD97706;      // Copper busbars
    uint32_t concretePad = 0xFF555555;

    float l = std::max(15.0f, params_.yardLength);
    float w = std::max(12.0f, params_.yardWidth);
    float gh = std::max(6.0f, params_.gantryHeight);

    // 1. Crushed gravel yard foundation & service roadway
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.06f, 0.0f}, Vec3{l, 0.12f, w}, gravelColor);
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.08f, 0.0f}, Vec3{3.2f, 0.02f, w * 0.9f}, roadwayDark);

    // 2. Large step-up power transformers with radiator banks, conservator & porcelain bushings
    int nTrans = std::max(1, std::min(3, params_.mainTransformerCount));
    for (int t = 0; t < nTrans; ++t) {
        float tx = (t - (nTrans - 1) * 0.5f) * (l * 0.38f);
        float tz = -w * 0.24f;

        // Concrete containment bund / oil pit
        PropMeshHelper::appendBox(payload, Vec3{tx, 0.25f, tz}, Vec3{4.8f, 0.5f, 4.0f}, concretePad);

        // Main transformer tank
        float tCenterY = 2.1f;
        Vec3 tPos{tx, tCenterY, tz};
        PropMeshHelper::appendBox(payload, tPos, Vec3{3.4f, 3.2f, 2.6f}, transformerColor);

        // Radiator banks with cooling fans
        float radZ[2] = {-1.65f, 1.65f};
        for (int rz = 0; rz < 2; ++rz) {
            PropMeshHelper::appendBox(payload, tPos + Vec3{0.0f, 0.0f, radZ[rz]}, Vec3{3.2f, 2.7f, 0.45f}, radiatorSteel);
            PropMeshHelper::appendCylinder(payload, tPos + Vec3{-0.9f, 0.0f, radZ[rz] + (radZ[rz] > 0 ? 0.25f : -0.25f)}, 0.45f, 0.45f, 0.12f, 12, 0xFF1E293B);
            PropMeshHelper::appendCylinder(payload, tPos + Vec3{ 0.9f, 0.0f, radZ[rz] + (radZ[rz] > 0 ? 0.25f : -0.25f)}, 0.45f, 0.45f, 0.12f, 12, 0xFF1E293B);
        }

        // Overhead oil conservator drum tank
        PropMeshHelper::appendBox(payload, Vec3{tx, tCenterY + 1.9f, tz}, Vec3{2.8f, 0.7f, 0.7f}, transformerColor);
        PropMeshHelper::appendStrut(payload, Vec3{tx + 1.2f, tCenterY + 1.9f, tz}, Vec3{tx + 1.2f, tCenterY + 0.6f, tz}, 0.06f, busbarColor);

        // High-voltage porcelain turret bushings on top
        for (int b = -1; b <= 1; ++b) {
            float bx = tx + b * 0.95f;
            float bBaseY = tCenterY + 1.6f;
            PropMeshHelper::appendCylinder(payload, Vec3{bx, bBaseY, tz}, 0.22f, 0.10f, 1.5f, 12, insulatorColor);
            PropMeshHelper::appendSphere(payload, Vec3{bx, bBaseY + 1.62f, tz}, 0.08f, 6, 8, busbarColor);
        }
    }

    // 3. High-voltage structural gantry portal towers & overhead busbars
    float gantryZ = w * 0.26f;
    float gantrySpan = l * 0.78f;
    float sides[2] = {-gantrySpan * 0.5f, gantrySpan * 0.5f};

    for (int s = 0; s < 2; ++s) {
        float side = sides[s];
        PropMeshHelper::appendBox(payload, Vec3{side, 0.25f, gantryZ}, Vec3{1.2f, 0.5f, 1.2f}, concretePad);

        float signsX[2] = {-0.35f, 0.35f};
        float signsZ[2] = {-0.35f, 0.35f};
        for (int ix = 0; ix < 2; ++ix) {
            for (int iz = 0; iz < 2; ++iz) {
                PropMeshHelper::appendStrut(payload, Vec3{side + signsX[ix] * 1.5f, 0.5f, gantryZ + signsZ[iz] * 1.5f},
                                            Vec3{side + signsX[ix] * 0.7f, gh,   gantryZ + signsZ[iz] * 0.7f}, 0.08f, gantrySteel);
            }
        }
    }

    // Horizontal lattice cross-gantry truss
    PropMeshHelper::appendBox(payload, Vec3{0.0f, gh, gantryZ}, Vec3{gantrySpan + 0.8f, 0.7f, 0.7f}, gantrySteel);

    // 3 Parallel rigid copper busbar conductor tubes spanning the yard
    float busOffsets[3] = {-0.25f, 0.0f, 0.25f};
    for (int b = 0; b < 3; ++b) {
        PropMeshHelper::appendBox(payload, Vec3{0.0f, gh - 1.25f, gantryZ + busOffsets[b]}, Vec3{gantrySpan + 0.6f, 0.05f, 0.05f}, busbarColor);
    }

    // 4. Relay control house kiosk
    Vec3 housePos{l * 0.35f, 1.4f, 0.0f};
    PropMeshHelper::appendBox(payload, housePos, Vec3{3.6f, 2.6f, 2.6f}, 0xFFCBD5E1);
    PropMeshHelper::appendBox(payload, housePos + Vec3{-1.82f, -0.3f, 0.0f}, Vec3{0.04f, 2.0f, 0.9f}, 0xFF334155);

    // 5. Perimeter Security Fence & Warning Signs
    if (params_.hasSecurityFence) {
        float fenceH = 2.4f;
        float halfL = l * 0.48f;
        float halfW = w * 0.48f;
        int xPosts = static_cast<int>(l / 3.2f);
        for (int i = 0; i <= xPosts; ++i) {
            float px = -halfL + i * (l * 0.96f / xPosts);
            PropMeshHelper::appendStrut(payload, Vec3{px, 0.1f, -halfW}, Vec3{px, fenceH, -halfW}, 0.06f, gantrySteel);
            PropMeshHelper::appendStrut(payload, Vec3{px, 0.1f,  halfW}, Vec3{px, fenceH,  halfW}, 0.06f, gantrySteel);
            PropMeshHelper::appendStrut(payload, Vec3{px, fenceH, -halfW}, Vec3{px, fenceH + 0.35f, -halfW - 0.25f}, 0.035f, gantrySteel);
            PropMeshHelper::appendStrut(payload, Vec3{px, fenceH,  halfW}, Vec3{px, fenceH + 0.35f,  halfW + 0.25f}, 0.035f, gantrySteel);
        }
        PropMeshHelper::appendBox(payload, Vec3{0.0f, fenceH, -halfW}, Vec3{l * 0.96f, 0.04f, 0.04f}, gantrySteel);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, fenceH,  halfW}, Vec3{l * 0.96f, 0.04f, 0.04f}, gantrySteel);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 1.4f, halfW + 0.03f}, Vec3{0.6f, 0.4f, 0.02f}, 0xFFF59E0B);
    }

    return payload;
}

} // namespace procedural::props
