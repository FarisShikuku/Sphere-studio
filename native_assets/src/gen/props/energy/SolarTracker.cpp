#include "SolarTracker.h"

namespace procedural::props {

void SolarTracker::appendGimbalAndTorqueTube(
    GeneratedMeshPayload& payload,
    const Vec3& center,
    float tubeLength,
    float /*tiltAngleDeg*/,
    uint32_t color
) {
    // Torque tube running horizontally
    PropMeshHelper::appendBox(payload, center, Vec3{tubeLength, 0.12f, 0.12f}, color);

    // Motor drive / actuator gearbox box at midpoint
    PropMeshHelper::appendBox(payload, center + Vec3{0.0f, -0.15f, 0.0f}, Vec3{0.35f, 0.30f, 0.35f}, 0xFF333333);

    // Torque slewing drive bearing ring
    PropMeshHelper::appendCylinder(payload, center + Vec3{-0.15f, 0.0f, 0.0f}, 0.18f, 0.18f, 0.08f, 16, 0xFF777777);
}

} // namespace procedural::props
