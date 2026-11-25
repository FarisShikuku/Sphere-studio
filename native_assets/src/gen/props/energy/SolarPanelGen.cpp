#include "SolarPanelGen.h"

namespace procedural::props {

SolarPanelGen::SolarPanelGen() {
    params_ = SolarPanelParams{};
}

bool SolarPanelGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void SolarPanelGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload SolarPanelGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t frameColor = 0xFFCBD5E1; // Silver anodized aluminum frame
    uint32_t cellColor = 0xFF0D1E42;  // Monocrystalline deep dark blue PV silicon
    uint32_t busbarColor = 0xFFE2E8F0; // Silver conductive ribbons
    uint32_t mountColor = 0xFF475569; // Galvanized steel racking
    uint32_t concreteColor = 0xFF64748B;
    uint32_t darkJunction = 0xFF1E293B;

    float l = std::max(1.2f, params_.length);
    float w = std::max(0.6f, params_.width);
    float tiltRad = params_.tiltAngleDeg * (PropMeshHelper::kPi / 180.0f);
    float sinT = std::sin(tiltRad);

    if (params_.isSunTracking) {
        // Central heavy tubular pedestal with concrete pier
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, 0.30f, 0.28f, 0.40f, 16, concreteColor);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.4f, 0.0f}, 0.10f, 0.10f, 1.10f, 16, mountColor);
        // Slew drive gearbox & pivot yoke
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 1.50f, 0.0f}, Vec3{0.40f, 0.35f, 0.40f}, darkJunction);
        // Tilt hydraulic actuator cylinder
        PropMeshHelper::appendStrut(payload, Vec3{0.0f, 0.8f, -0.3f}, Vec3{0.0f, 1.50f + sinT * 0.4f, -std::cos(tiltRad) * 0.4f}, 0.06f, 0xFF94A3B8);
        // Cross torque beam
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 1.55f, 0.0f}, Vec3{w * 0.9f, 0.10f, 0.10f}, mountColor);
    } else {
        // Fixed ground-mount racking with front & rear steel legs on concrete ballast footings
        float halfW = w * 0.38f;
        float signs[2] = {-halfW, halfW};
        for (int i = 0; i < 2; ++i) {
            float sx = signs[i];
            PropMeshHelper::appendBox(payload, Vec3{sx, 0.15f, -l * 0.3f}, Vec3{0.35f, 0.30f, 0.35f}, concreteColor);
            PropMeshHelper::appendBox(payload, Vec3{sx, 0.15f,  l * 0.3f}, Vec3{0.35f, 0.30f, 0.35f}, concreteColor);

            PropMeshHelper::appendStrut(payload, Vec3{sx, 0.3f,  l * 0.3f}, Vec3{sx, 1.0f - sinT * l * 0.3f,  l * 0.3f}, 0.05f, mountColor);
            PropMeshHelper::appendStrut(payload, Vec3{sx, 0.3f, -l * 0.3f}, Vec3{sx, 1.0f + sinT * l * 0.3f, -l * 0.3f}, 0.05f, mountColor);
            PropMeshHelper::appendStrut(payload, Vec3{sx, 0.3f, -l * 0.3f}, Vec3{sx, 1.0f - sinT * l * 0.3f,  l * 0.3f}, 0.035f, frameColor);
        }
    }

    // Aluminum panel outer frame
    Vec3 panelCenter{0.0f, 1.15f, 0.0f};
    PropMeshHelper::appendBox(payload, panelCenter, Vec3{w, 0.05f, l}, frameColor);

    // Dark blue PV silicon cell face
    PropMeshHelper::appendBox(payload, panelCenter + Vec3{0.0f, 0.026f, 0.0f}, Vec3{w * 0.96f, 0.015f, l * 0.96f}, cellColor);

    // Silver busbars on PV cells
    PropMeshHelper::appendBox(payload, panelCenter + Vec3{-w * 0.28f, 0.035f, 0.0f}, Vec3{0.012f, 0.008f, l * 0.92f}, busbarColor);
    PropMeshHelper::appendBox(payload, panelCenter + Vec3{ 0.0f,      0.035f, 0.0f}, Vec3{0.012f, 0.008f, l * 0.92f}, busbarColor);
    PropMeshHelper::appendBox(payload, panelCenter + Vec3{ w * 0.28f, 0.035f, 0.0f}, Vec3{0.012f, 0.008f, l * 0.92f}, busbarColor);

    // Rear IP67 micro-inverter
    PropMeshHelper::appendBox(payload, panelCenter + Vec3{0.0f, -0.045f, 0.0f}, Vec3{w * 0.25f, 0.05f, l * 0.18f}, darkJunction);

    return payload;
}

} // namespace procedural::props

