#include "FarmWindmillGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

FarmWindmillGen::FarmWindmillGen() = default;

bool FarmWindmillGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void FarmWindmillGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload FarmWindmillGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float H = std::max(params_.towerHeight, 5.0f);
    float D = std::max(params_.rotorDiameter, 1.2f);
    float rotorR = D * 0.5f;
    float baseW = std::max(params_.baseWidth, 1.0f);
    float topW = 0.55f;

    uint32_t steelCol = 0xFF94A3B8; // Galvanized steel
    uint32_t tailCol = (params_.tailStyle == 1) ? 0xFFDC2626 : // Farm Red
                       (params_.tailStyle == 2) ? 0xFF9A3412 : // Weathered Rust
                                                  0xFF64748B;  // Galvanized Grey
    uint32_t woodCol = 0xFF8E7D6A; // Weathered wood deck

    // 1. Concrete corner foundation piers
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-1.0f, 1.0f}) {
            Vec3 pierPos{sx * baseW * 0.5f, 0.15f, sz * baseW * 0.5f};
            PropMeshHelper::appendBox(payload, pierPos, Vec3{0.30f, 0.30f, 0.30f}, 0xFF64748B);
        }
    }

    // 2. 4 Tapered angle-iron lattice legs
    // 2. 4 Tapered tubular lattice legs with real continuous girts and X-braces
    int tiers = std::max(4, static_cast<int>(H / 1.8f));
    float tierH = H / tiers;

    for (int t = 0; t < tiers; ++t) {
        float y0 = t * tierH;
        float y1 = (t + 1) * tierH;
        float w0 = baseW + (topW - baseW) * (static_cast<float>(t) / tiers);
        float w1 = baseW + (topW - baseW) * (static_cast<float>(t + 1) / tiers);

        float hw0 = w0 * 0.5f;
        float hw1 = w1 * 0.5f;

        // 4 corner post continuous tapered segments
        Vec3 c00_0{-hw0, y0, -hw0}; Vec3 c00_1{-hw1, y1, -hw1};
        Vec3 c10_0{ hw0, y0, -hw0}; Vec3 c10_1{ hw1, y1, -hw1};
        Vec3 c11_0{ hw0, y0,  hw0}; Vec3 c11_1{ hw1, y1,  hw1};
        Vec3 c01_0{-hw0, y0,  hw0}; Vec3 c01_1{-hw1, y1,  hw1};

        PropMeshHelper::appendOrientedCylinder(payload, c00_0, c00_1, 0.035f, 0.032f, 8, steelCol, true, true);
        PropMeshHelper::appendOrientedCylinder(payload, c10_0, c10_1, 0.035f, 0.032f, 8, steelCol, true, true);
        PropMeshHelper::appendOrientedCylinder(payload, c11_0, c11_1, 0.035f, 0.032f, 8, steelCol, true, true);
        PropMeshHelper::appendOrientedCylinder(payload, c01_0, c01_1, 0.035f, 0.032f, 8, steelCol, true, true);

        // Horizontal girt struts at tier top level
        PropMeshHelper::appendOrientedCylinder(payload, c00_1, c10_1, 0.022f, 0.022f, 6, steelCol, true, true);
        PropMeshHelper::appendOrientedCylinder(payload, c10_1, c11_1, 0.022f, 0.022f, 6, steelCol, true, true);
        PropMeshHelper::appendOrientedCylinder(payload, c11_1, c01_1, 0.022f, 0.022f, 6, steelCol, true, true);
        PropMeshHelper::appendOrientedCylinder(payload, c01_1, c00_1, 0.022f, 0.022f, 6, steelCol, true, true);

        // Diagonal X-bracing on all 4 faces
        // Front (Z = -hw)
        PropMeshHelper::appendOrientedCylinder(payload, c00_0, c10_1, 0.015f, 0.015f, 6, steelCol, false, false);
        PropMeshHelper::appendOrientedCylinder(payload, c10_0, c00_1, 0.015f, 0.015f, 6, steelCol, false, false);
        // Right (X = +hw)
        PropMeshHelper::appendOrientedCylinder(payload, c10_0, c11_1, 0.015f, 0.015f, 6, steelCol, false, false);
        PropMeshHelper::appendOrientedCylinder(payload, c11_0, c10_1, 0.015f, 0.015f, 6, steelCol, false, false);
        // Back (Z = +hw)
        PropMeshHelper::appendOrientedCylinder(payload, c11_0, c01_1, 0.015f, 0.015f, 6, steelCol, false, false);
        PropMeshHelper::appendOrientedCylinder(payload, c01_0, c11_1, 0.015f, 0.015f, 6, steelCol, false, false);
        // Left (X = -hw)
        PropMeshHelper::appendOrientedCylinder(payload, c01_0, c00_1, 0.015f, 0.015f, 6, steelCol, false, false);
        PropMeshHelper::appendOrientedCylinder(payload, c00_0, c01_1, 0.015f, 0.015f, 6, steelCol, false, false);
    }

    // 3. Central vertical pump rod shaft
    PropMeshHelper::appendOrientedCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, Vec3{0.0f, H, 0.0f}, 0.025f, 0.025f, 8, 0xFF475569, true, true);

    // 4. Maintenance platform deck at top of tower
    if (params_.hasPlatformDeck) {
        float deckW = topW * 1.55f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H - 0.04f, 0.0f}, Vec3{deckW, 0.05f, deckW}, woodCol);
        // Safety handrail around platform
        float railH = 0.70f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H + railH, -deckW * 0.5f}, Vec3{deckW, 0.03f, 0.03f}, steelCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H + railH,  deckW * 0.5f}, Vec3{deckW, 0.03f, 0.03f}, steelCol);
    }

    // 5. Gearbox nacelle head
    float hubY = H + 0.35f;
    PropMeshHelper::appendBox(payload, Vec3{0.0f, hubY, 0.0f}, Vec3{0.32f, 0.35f, 0.55f}, 0xFF334155);

    // 6. Multi-blade wind wheel rotor (pointed along +Z)
    int blades = std::clamp(params_.bladeCount, 12, 24);
    float hubZ = 0.32f;
    // Central rotor hub cylinder oriented along +Z
    PropMeshHelper::appendOrientedCylinder(payload, Vec3{0.0f, hubY, hubZ - 0.06f}, Vec3{0.0f, hubY, hubZ + 0.08f}, 0.18f, 0.14f, 16, steelCol, true, true);

    float rInner = 0.18f;
    float rOuter = rotorR * 0.96f;
    for (int b = 0; b < blades; ++b) {
        float angle = (static_cast<float>(b) / blades) * 2.0f * PropMeshHelper::kPi;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        // Blade radial spoke rib
        Vec3 innerPt{rInner * cosA, hubY + rInner * sinA, hubZ + 0.02f};
        Vec3 outerPt{rOuter * cosA, hubY + rOuter * sinA, hubZ + 0.02f};
        PropMeshHelper::appendOrientedCylinder(payload, innerPt, outerPt, 0.014f, 0.010f, 6, steelCol, true, true);

        // Aerodynamic sheet-metal blade paddle
        Vec3 pPerp{-sinA * 0.06f, cosA * 0.06f, 0.0f};
        PropMeshHelper::appendQuad(payload, innerPt - pPerp, innerPt + pPerp, outerPt + pPerp, outerPt - pPerp, 0xFFCBD5E1);
        PropMeshHelper::appendQuad(payload, outerPt - pPerp, outerPt + pPerp, innerPt + pPerp, innerPt - pPerp, 0xFF94A3B8);
    }
    // Outer concentric continuous circular stabilizer ring
    const int kRingSegs = 24;
    float ringR = rotorR * 0.88f;
    for (int s = 0; s < kRingSegs; ++s) {
        float a0 = (static_cast<float>(s) / kRingSegs) * 2.0f * PropMeshHelper::kPi;
        float a1 = (static_cast<float>(s + 1) / kRingSegs) * 2.0f * PropMeshHelper::kPi;
        Vec3 p0{ringR * std::cos(a0), hubY + ringR * std::sin(a0), hubZ + 0.02f};
        Vec3 p1{ringR * std::cos(a1), hubY + ringR * std::sin(a1), hubZ + 0.02f};
        PropMeshHelper::appendOrientedCylinder(payload, p0, p1, 0.015f, 0.015f, 6, steelCol, false, false);
    }

    // 7. Aerodynamic directional tail vane (extending back along -Z)
    float tailL = rotorR * 1.35f;
    float tailZ = -0.30f - tailL * 0.5f;
    // Tail boom rod
    PropMeshHelper::appendBox(payload, Vec3{0.0f, hubY, -0.30f - tailL * 0.5f}, Vec3{0.04f, 0.04f, tailL}, steelCol);
    // Flat metal tail rudder vane with farm crest shape
    PropMeshHelper::appendBox(payload, Vec3{0.0f, hubY + 0.15f, -0.30f - tailL * 0.85f}, Vec3{0.02f, 0.65f, tailL * 0.45f}, tailCol);

    return payload;
}

} // namespace procedural::props
