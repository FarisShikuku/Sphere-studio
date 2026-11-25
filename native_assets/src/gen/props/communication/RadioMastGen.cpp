#include "RadioMastGen.h"

namespace procedural::props {

RadioMastGen::RadioMastGen() {
    params_ = RadioMastParams{};
}

bool RadioMastGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void RadioMastGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload RadioMastGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t steelWhite = 0xFFF8FAFC;
    uint32_t steelRed = 0xFFDC2626;   // ICAO safety red alternating bands
    uint32_t foundation = 0xFF555555;
    uint32_t darkDetail = 0xFF1E293B;
    uint32_t radomeWhite = 0xFFF1F5F9;

    float h = std::max(15.0f, params_.height);
    float bw = std::max(1.8f, params_.baseWidth);
    float tw = std::max(0.4f, params_.topWidth);
    int sections = std::max(3, std::min(8, params_.sectionCount));
    float secH = (h - 0.6f) / sections;

    float bRadius = bw * 0.577f; // Equilateral circumradius
    float tRadius = tw * 0.577f;

    // 1. Reinforced concrete foundation anchor pads for 3 corner legs
    float legAngles[3] = {0.0f, 2.094395f, 4.18879f}; // 0, 120, 240 deg
    for (int i = 0; i < 3; ++i) {
        float ang = legAngles[i];
        Vec3 fPos{std::cos(ang) * bRadius, 0.3f, std::sin(ang) * bRadius};
        PropMeshHelper::appendBox(payload, fPos, Vec3{0.9f, 0.6f, 0.9f}, foundation);
    }

    // 2. Triangular steel lattice tower with alternating ICAO red/white sections & X-braces
    for (int s = 0; s < sections; ++s) {
        float y0 = 0.6f + s * secH;
        float y1 = 0.6f + (s + 1) * secH;
        float f0 = static_cast<float>(s) / sections;
        float f1 = static_cast<float>(s + 1) / sections;
        float r0 = bRadius * (1.0f - f0) + tRadius * f0;
        float r1 = bRadius * (1.0f - f1) + tRadius * f1;

        uint32_t col = (s % 2 == 0) ? steelRed : steelWhite;

        // 3 Corner legs
        for (int i = 0; i < 3; ++i) {
            float ang = legAngles[i];
            Vec3 p0{std::cos(ang) * r0, y0, std::sin(ang) * r0};
            Vec3 p1{std::cos(ang) * r1, y1, std::sin(ang) * r1};
            PropMeshHelper::appendStrut(payload, p0, p1, 0.08f, col);
        }

        // Horizontal girt struts at top of this tier
        for (int i = 0; i < 3; ++i) {
            int iNext = (i + 1) % 3;
            Vec3 p0{std::cos(legAngles[i]) * r1, y1, std::sin(legAngles[i]) * r1};
            Vec3 p1{std::cos(legAngles[iNext]) * r1, y1, std::sin(legAngles[iNext]) * r1};
            PropMeshHelper::appendStrut(payload, p0, p1, 0.05f, col);
        }

        // Diagonal X-bracing on all 3 faces
        for (int i = 0; i < 3; ++i) {
            int iNext = (i + 1) % 3;
            Vec3 b0{std::cos(legAngles[i]) * r0, y0, std::sin(legAngles[i]) * r0};
            Vec3 b1{std::cos(legAngles[iNext]) * r0, y0, std::sin(legAngles[iNext]) * r0};
            Vec3 t0{std::cos(legAngles[i]) * r1, y1, std::sin(legAngles[i]) * r1};
            Vec3 t1{std::cos(legAngles[iNext]) * r1, y1, std::sin(legAngles[iNext]) * r1};

            PropMeshHelper::appendStrut(payload, b0, t1, 0.04f, col);
            PropMeshHelper::appendStrut(payload, b1, t0, 0.04f, col);
        }
    }

    // 3. Central coaxial cable transmission ladder running up tower
    PropMeshHelper::appendBox(payload, Vec3{0.0f, h * 0.5f, 0.0f}, Vec3{0.12f, h, 0.04f}, darkDetail);

    // 4. Parabolic microwave drum dishes & sector antenna panels
    for (int a = 0; a < 2; ++a) {
        float dishY = h * (0.60f + a * 0.15f);
        float ang = a * 1.57f + 0.4f;
        float dishR = bRadius * 0.45f + 0.6f;
        Vec3 dishCenter{std::cos(ang) * dishR, dishY, std::sin(ang) * dishR};

        // Outrigger stand-off mount
        PropMeshHelper::appendStrut(payload, Vec3{0.0f, dishY, 0.0f}, dishCenter, 0.07f, steelWhite);
        // Microwave drum shroud
        PropMeshHelper::appendCylinder(payload, dishCenter - Vec3{0.0f, 0.4f, 0.0f}, 0.55f, 0.55f, 0.35f, 16, darkDetail);
        // Radome cover
        PropMeshHelper::appendSphere(payload, dishCenter + Vec3{0.0f, 0.0f, 0.22f}, 0.54f, 8, 12, radomeWhite);

        // Sector panel antenna on opposite side
        float pAng = ang + 2.0f;
        Vec3 pPos{std::cos(pAng) * (dishR * 0.8f), dishY, std::sin(pAng) * (dishR * 0.8f)};
        PropMeshHelper::appendBox(payload, pPos, Vec3{0.25f, 1.4f, 0.12f}, steelWhite, pAng);
    }

    // 5. Crown platform, lightning arrestor & obstruction beacon
    float mastTopY = 0.6f + (sections * secH);
    // Triangular crown maintenance platform
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, mastTopY, 0.0f}, tRadius * 1.5f, tRadius * 1.5f, 0.08f, 3, darkDetail);
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, mastTopY + 0.08f, 0.0f}, tRadius * 1.45f, tRadius * 1.45f, 0.55f, 3, steelWhite);
    // Lightning arrestor rod
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, mastTopY, 0.0f}, 0.04f, 0.01f, 2.2f, 8, steelWhite);

    if (params_.hasAviationLight) {
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, mastTopY + 2.2f, 0.0f}, 0.08f, 0.08f, 0.35f, 8, darkDetail);
        PropMeshHelper::appendSphere(payload, Vec3{0.0f, mastTopY + 2.6f, 0.0f}, 0.18f, 8, 10, 0xFFFF1111);
    }

    return payload;
}

} // namespace procedural::props
