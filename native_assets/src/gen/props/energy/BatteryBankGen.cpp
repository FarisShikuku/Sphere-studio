#include "BatteryBankGen.h"

namespace procedural::props {

BatteryBankGen::BatteryBankGen() {
    params_ = BatteryBankParams{};
}

bool BatteryBankGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void BatteryBankGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload BatteryBankGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t containerWhite = 0xFFF0F3F7; // Grid-scale BESS white enclosure
    uint32_t frameDark = 0xFF3E454F;
    uint32_t hvacGray = 0xFF9CA3AF;
    uint32_t warningDiamond = 0xFFFFCC00;
    uint32_t pipeRed = 0xFFDC2626;

    float l = std::max(6.0f, params_.containerLength);
    float w = std::max(2.0f, params_.containerWidth);
    float h = std::max(2.2f, params_.containerHeight);
    int units = std::max(1, std::min(4, params_.unitCount));

    float halfL = l * 0.5f;
    float halfW = w * 0.5f;

    for (int u = 0; u < units; ++u) {
        float offsetZ = (u - units * 0.5f + 0.5f) * (w + 2.2f);
        float basePadY = 0.2f;
        float padH = 0.4f;

        // 1. Concrete equipment pad
        PropMeshHelper::appendBox(payload, Vec3{0.0f, basePadY, offsetZ}, Vec3{l + 1.2f, padH, w + 0.8f}, 0xFF555555);

        float contBottomY = padH;
        float contCenterY = contBottomY + h * 0.5f;
        Vec3 contMid{0.0f, contCenterY, offsetZ};

        // 2. Structural frame rails & 4 corner posts
        PropMeshHelper::appendBox(payload, Vec3{0.0f, contBottomY + 0.08f, offsetZ}, Vec3{l + 0.04f, 0.16f, w + 0.04f}, frameDark);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, contBottomY + h - 0.08f, offsetZ}, Vec3{l + 0.04f, 0.16f, w + 0.04f}, frameDark);

        float signsX[2] = {-halfL, halfL};
        float signsZ[2] = {-halfW, halfW};
        for (int ix = 0; ix < 2; ++ix) {
            for (int iz = 0; iz < 2; ++iz) {
                PropMeshHelper::appendBox(payload, Vec3{signsX[ix], contCenterY, offsetZ + signsZ[iz]}, Vec3{0.22f, h, 0.22f}, frameDark);
            }
        }

        // 3. Main container body
        PropMeshHelper::appendBox(payload, contMid, Vec3{l - 0.10f, h - 0.15f, w - 0.08f}, containerWhite);

        // 4. Corrugated side walls (vertical ribs)
        int ribCount = std::min(24, std::max(10, static_cast<int>(l * 2.5f)));
        float ribStep = (l - 0.8f) / ribCount;
        for (int r = 0; r <= ribCount; ++r) {
            float rx = -halfL + 0.4f + r * ribStep;
            PropMeshHelper::appendBox(payload, Vec3{rx, contCenterY, offsetZ + halfW + 0.015f}, Vec3{ribStep * 0.35f, h - 0.35f, 0.03f}, 0xFFE2E8F0);
            PropMeshHelper::appendBox(payload, Vec3{rx, contCenterY, offsetZ - halfW - 0.015f}, Vec3{ribStep * 0.35f, h - 0.35f, 0.03f}, 0xFFE2E8F0);
        }

        // 5. Personnel access double door on container end
        float doorX = halfL + 0.02f;
        PropMeshHelper::appendBox(payload, Vec3{doorX, contBottomY + h * 0.42f, offsetZ}, Vec3{0.04f, h * 0.78f, w * 0.75f}, 0xFFCBD5E1);
        PropMeshHelper::appendBox(payload, Vec3{doorX + 0.02f, contBottomY + h * 0.42f, offsetZ - 0.3f}, Vec3{0.04f, h * 0.74f, 0.04f}, frameDark);
        PropMeshHelper::appendBox(payload, Vec3{doorX + 0.02f, contBottomY + h * 0.42f, offsetZ + 0.3f}, Vec3{0.04f, h * 0.74f, 0.04f}, frameDark);

        // 6. Rooftop HVAC chiller packs
        int hvacs = std::max(1, params_.hvacUnitCount);
        float hvacSpacing = (l * 0.55f) / hvacs;
        for (int v = 0; v < hvacs; ++v) {
            float vx = (v - (hvacs - 1) * 0.5f) * hvacSpacing;
            float hvacY = contBottomY + h + 0.45f;
            Vec3 hvacPos{vx, hvacY, offsetZ};
            PropMeshHelper::appendBox(payload, hvacPos, Vec3{1.9f, 0.9f, w * 0.75f}, hvacGray);
            // Intake louvers
            PropMeshHelper::appendBox(payload, Vec3{vx, hvacY, offsetZ + w * 0.38f}, Vec3{1.6f, 0.6f, 0.02f}, frameDark);
            PropMeshHelper::appendBox(payload, Vec3{vx, hvacY, offsetZ - w * 0.38f}, Vec3{1.6f, 0.6f, 0.02f}, frameDark);
            // Dual fan cowlings
            PropMeshHelper::appendCylinder(payload, Vec3{vx - 0.45f, hvacY + 0.45f, offsetZ}, 0.35f, 0.35f, 0.12f, 14, 0xFF1E293B);
            PropMeshHelper::appendCylinder(payload, Vec3{vx + 0.45f, hvacY + 0.45f, offsetZ}, 0.35f, 0.35f, 0.12f, 14, 0xFF1E293B);
        }

        // 7. High-voltage isolation switchgear cabinet
        float swX = -halfL * 0.5f;
        PropMeshHelper::appendBox(payload, Vec3{swX, contBottomY + h * 0.45f, offsetZ + halfW + 0.14f}, Vec3{0.9f, 1.2f, 0.28f}, frameDark);
        PropMeshHelper::appendBox(payload, Vec3{swX, contBottomY + h * 0.5f, offsetZ + halfW + 0.29f}, Vec3{0.12f, 0.12f, 0.04f}, pipeRed);

        // 8. NFPA 704 Placard & Fire Suppression
        if (params_.hasFireSuppressionPiping) {
            PropMeshHelper::appendBox(payload, contMid + Vec3{halfL * 0.25f, 0.2f, halfW + 0.04f}, Vec3{0.45f, 0.45f, 0.02f}, warningDiamond);
            PropMeshHelper::appendBox(payload, contMid + Vec3{halfL * 0.25f - 0.7f, 0.2f, halfW + 0.04f}, Vec3{0.5f, 0.35f, 0.02f}, pipeRed);

            float pipeZ = offsetZ + halfW + 0.18f;
            PropMeshHelper::appendStrut(payload, Vec3{halfL * 0.85f, contBottomY + 0.2f, pipeZ}, Vec3{halfL * 0.85f, contBottomY + h * 0.6f, pipeZ}, 0.08f, pipeRed);
            PropMeshHelper::appendCylinder(payload, Vec3{halfL * 0.85f, contBottomY + 0.8f, pipeZ}, 0.08f, 0.06f, 0.22f, 10, 0xFFD97706);
        }
    }

    return payload;
}

} // namespace procedural::props
