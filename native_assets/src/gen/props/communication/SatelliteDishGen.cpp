#include "SatelliteDishGen.h"

namespace procedural::props {

SatelliteDishGen::SatelliteDishGen() {
    params_ = SatelliteDishParams{};
}

bool SatelliteDishGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void SatelliteDishGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload SatelliteDishGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t dishWhite = 0xFFF8FAFC;
    uint32_t mountSteel = 0xFF334155;
    uint32_t yokeSteel = 0xFF475569;
    uint32_t feedhornColor = 0xFF0F172A;
    uint32_t rimColor = 0xFFCBD5E1;
    uint32_t copperWaveguide = 0xFFD97706;

    float d = std::max(1.2f, params_.diameter);
    float r = d * 0.5f;
    float f = std::max(0.25f, params_.focalDepth);
    float elRad = params_.elevationAngleDeg * (PropMeshHelper::kPi / 180.0f);

    // 1. Reinforced octagonal concrete ground pad
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, 1.2f, 1.1f, 0.35f, 8, 0xFF555555);

    // 2. Heavy steel kingpost pedestal column with 4 gusset stiffeners
    float pedH = 1.3f;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.35f, 0.0f}, 0.22f, 0.20f, pedH, 16, mountSteel);
    float gussetAngles[4] = {0.0f, 1.57f, 3.14f, 4.71f};
    for (int g = 0; g < 4; ++g) {
        float ga = gussetAngles[g];
        Vec3 g0{std::cos(ga) * 0.4f, 0.35f, std::sin(ga) * 0.4f};
        Vec3 g1{std::cos(ga) * 0.12f, 0.35f + pedH * 0.5f, std::sin(ga) * 0.12f};
        PropMeshHelper::appendStrut(payload, g0, g1, 0.05f, mountSteel);
    }

    // 3. Az-El turntable ring & elevation pivot yoke
    float yokeBaseY = 0.35f + pedH;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, yokeBaseY, 0.0f}, 0.32f, 0.32f, 0.15f, 16, 0xFF1E293B);

    Vec3 gimbalPos{0.0f, yokeBaseY + 0.45f, 0.0f};
    // Elevation yoke arms
    PropMeshHelper::appendBox(payload, gimbalPos + Vec3{-0.35f, 0.0f, 0.0f}, Vec3{0.12f, 0.70f, 0.28f}, yokeSteel);
    PropMeshHelper::appendBox(payload, gimbalPos + Vec3{ 0.35f, 0.0f, 0.0f}, Vec3{0.12f, 0.70f, 0.28f}, yokeSteel);
    // Jack-screw actuator
    PropMeshHelper::appendStrut(payload, Vec3{0.0f, yokeBaseY + 0.15f, -0.25f}, gimbalPos + Vec3{0.0f, 0.15f, -0.45f}, 0.08f, 0xFF94A3B8);
    // Counterweights
    PropMeshHelper::appendBox(payload, gimbalPos + Vec3{0.0f, -0.20f, -0.65f}, Vec3{0.70f, 0.25f, 0.35f}, 0xFF1E293B);

    // 4. Parabolic reflector bowl (concentric parabolic tiers)
    int tiers = 5;
    Vec3 dishCenter = gimbalPos + Vec3{0.0f, 0.15f, 0.2f};

    for (int t = 0; t < tiers; ++t) {
        float f0 = static_cast<float>(t) / tiers;
        float f1 = static_cast<float>(t + 1) / tiers;
        float r0 = r * f0;
        float r1 = r * f1;
        float z0 = (r0 * r0) / (4.0f * f);
        float z1 = (r1 * r1) / (4.0f * f);

        Vec3 ringPos = dishCenter + Vec3{0.0f, (z0 + z1) * 0.5f * std::sin(elRad), (z0 + z1) * 0.5f * std::cos(elRad)};
        PropMeshHelper::appendCylinder(payload, ringPos, r0, r1, std::max(0.04f, z1 - z0), 20, dishWhite);
    }

    // Outer rim flange
    float rimZ = (r * r) / (4.0f * f);
    Vec3 rimPos = dishCenter + Vec3{0.0f, rimZ * std::sin(elRad), rimZ * std::cos(elRad)};
    PropMeshHelper::appendCylinder(payload, rimPos, r * 1.02f, r * 1.02f, 0.05f, 20, rimColor);

    // Rear radial stiffening ribs (8 ribs)
    for (int rb = 0; rb < 8; ++rb) {
        float ang = rb * (PropMeshHelper::kPi / 4.0f);
        float rx = std::cos(ang) * (r * 0.85f);
        float ry = std::sin(ang) * (r * 0.85f);
        Vec3 ribEnd = dishCenter + Vec3{rx, ry * std::cos(elRad) + rimZ * 0.8f * std::sin(elRad), -ry * std::sin(elRad) + rimZ * 0.8f * std::cos(elRad)};
        PropMeshHelper::appendStrut(payload, dishCenter, ribEnd, 0.04f, mountSteel);
    }

    // 5. Focal point feedhorn & tripod support struts
    float feedDist = f * 1.6f;
    Vec3 feedPos = dishCenter + Vec3{0.0f, feedDist * std::sin(elRad), feedDist * std::cos(elRad)};

    // Prime-focus feedhorn package & LNB
    PropMeshHelper::appendCylinder(payload, feedPos, 0.12f, 0.08f, 0.25f, 14, feedhornColor);
    PropMeshHelper::appendSphere(payload, feedPos + Vec3{0.0f, 0.25f * std::sin(elRad), 0.25f * std::cos(elRad)}, 0.09f, 8, 10, rimColor);

    // 3 Struts connecting rim to feedhorn
    for (int i = 0; i < 3; ++i) {
        float ang = i * (2.0f * PropMeshHelper::kPi / 3.0f);
        float sx = std::cos(ang) * (r * 0.92f);
        float sy = std::sin(ang) * (r * 0.92f);
        Vec3 rimAnchor = dishCenter + Vec3{sx, sy * std::cos(elRad) + rimZ * std::sin(elRad), -sy * std::sin(elRad) + rimZ * std::cos(elRad)};
        PropMeshHelper::appendStrut(payload, rimAnchor, feedPos, 0.035f, 0xFF94A3B8);
    }

    // Flexible waveguide
    PropMeshHelper::appendStrut(payload, dishCenter, feedPos, 0.025f, copperWaveguide);

    // De-icer heating element when toggled
    if (params_.hasDeicerHeater) {
        PropMeshHelper::appendCylinder(payload, feedPos, 0.14f, 0.14f, 0.10f, 12, 0xFFDC2626);
    }

    return payload;
}

} // namespace procedural::props

