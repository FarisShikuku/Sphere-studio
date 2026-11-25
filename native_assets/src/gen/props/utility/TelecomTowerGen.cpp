#include "TelecomTowerGen.h"

namespace procedural::props {

TelecomTowerGen::TelecomTowerGen() {
    params_ = TelecomTowerParams{};
}

bool TelecomTowerGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void TelecomTowerGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload TelecomTowerGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t steelWhite = 0xFFD8DDE6;
    uint32_t steelRed = 0xFFC0392B; // Aviation warning orange-red
    uint32_t drumColor = 0xFF4B5563;
    uint32_t panelColor = 0xFFEEEEEE;

    float totalH = params_.height;
    float bw = params_.baseWidth;
    float tw = params_.topWidth;
    int sections = std::max(3, std::min(10, params_.sectionCount));
    float secH = totalH / sections;

    // Triangular lattice tower: 3 main tubular leg uprights
    const float kSqrt3 = 1.7320508f;
    float bRadius = bw / kSqrt3;
    float tRadius = tw / kSqrt3;

    for (int s = 0; s < sections; ++s) {
        float y0 = s * secH;
        float y1 = (s + 1) * secH;
        float t0 = static_cast<float>(s) / sections;
        float t1 = static_cast<float>(s + 1) / sections;
        float r0 = bRadius * (1.0f - t0) + tRadius * t0;
        float r1 = bRadius * (1.0f - t1) + tRadius * t1;

        // Alternate red/white aviation bands
        uint32_t bandColor = (s % 2 == 0) ? steelWhite : steelRed;

        // 3 corner columns for this section as continuous oriented tubular legs
        for (int c = 0; c < 3; ++c) {
            float ang = c * (2.0f * PropMeshHelper::kPi / 3.0f);
            float x0 = std::cos(ang) * r0;
            float z0 = std::sin(ang) * r0;
            float x1 = std::cos(ang) * r1;
            float z1 = std::sin(ang) * r1;

            PropMeshHelper::appendOrientedCylinder(payload, Vec3{x0, y0, z0}, Vec3{x1, y1, z1}, 0.08f, 0.07f, 8, bandColor, true, true);
        }

        // Horizontal perimeter girts and diagonal lattice X-bracing between legs
        for (int c = 0; c < 3; ++c) {
            float a0 = c * (2.0f * PropMeshHelper::kPi / 3.0f);
            float a1 = ((c + 1) % 3) * (2.0f * PropMeshHelper::kPi / 3.0f);
            float bx0 = std::cos(a0) * r0; float bz0 = std::sin(a0) * r0;
            float bx1 = std::cos(a1) * r0; float bz1 = std::sin(a1) * r0;
            float px0 = std::cos(a0) * r1; float pz0 = std::sin(a0) * r1;
            float px1 = std::cos(a1) * r1; float pz1 = std::sin(a1) * r1;

            // Continuous horizontal girt beam
            PropMeshHelper::appendOrientedCylinder(payload, Vec3{px0, y1, pz0}, Vec3{px1, y1, pz1}, 0.045f, 0.045f, 6, bandColor, true, true);

            // Diagonal lattice X-bracing
            PropMeshHelper::appendOrientedCylinder(payload, Vec3{bx0, y0, bz0}, Vec3{px1, y1, pz1}, 0.032f, 0.032f, 6, bandColor, false, false);
            PropMeshHelper::appendOrientedCylinder(payload, Vec3{bx1, y0, bz1}, Vec3{px0, y1, pz0}, 0.032f, 0.032f, 6, bandColor, false, false);
        }
    }

    // Top platforms & cellular sector antennas
    float platY = totalH;
    PropMeshHelper::appendBox(payload, Vec3{0.0f, platY, 0.0f}, Vec3{tw * 1.5f, 0.10f, tw * 1.5f}, steelWhite);

    // 3 Sector antennas (tri-sector cellular array)
    for (int i = 0; i < 3; ++i) {
        float ang = i * (2.0f * PropMeshHelper::kPi / 3.0f);
        float ax = std::cos(ang) * (tw * 0.9f);
        float az = std::sin(ang) * (tw * 0.9f);
        // Vertical cellular antenna panel
        PropMeshHelper::appendBox(payload, Vec3{ax, platY + 1.0f, az}, Vec3{0.22f, 1.8f, 0.12f}, panelColor, -ang);
    }

    // Microwave parabolic drum antenna on mid-tier
    float dishY = totalH * 0.75f;
    PropMeshHelper::appendCylinder(payload, Vec3{tw * 0.8f, dishY, 0.0f}, 0.55f, 0.55f, 0.35f, 16, drumColor);

    // Aviation flashing beacon on top tip
    if (params_.hasAviationBeacon) {
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, platY + 2.0f, 0.0f}, 0.04f, 0.04f, 0.8f, 8, steelWhite);
        PropMeshHelper::appendSphere(payload, Vec3{0.0f, platY + 2.85f, 0.0f}, 0.12f, 8, 10, 0xFFFF2222);
    }

    return payload;
}

} // namespace procedural::props
