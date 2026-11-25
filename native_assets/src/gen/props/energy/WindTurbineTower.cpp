#include "WindTurbineTower.h"

namespace procedural::props {

void WindTurbineTower::generate(GeneratedMeshPayload& payload, const WindTurbineParams& params) {
    uint32_t towerColor = 0xFFF1F5F9;
    uint32_t flangeColor = 0xFFCBD5E1;
    uint32_t nacelleColor = 0xFFE2E8F0;
    uint32_t hubColor = 0xFFCBD5E1;
    uint32_t darkDetail = 0xFF334155;
    uint32_t concreteColor = 0xFF64748B;
    uint32_t beaconColor = 0xFFFF1111;

    float h = params.hubHeight;
    float rb = std::max(1.8f, params.towerBaseRadius);
    float rt = std::max(0.8f, params.towerTopRadius);

    // 1. Reinforced octagonal concrete foundation plinth & collar ring
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, rb * 1.6f, rb * 1.45f, 0.6f, 20, concreteColor);
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.6f, 0.0f}, rb * 1.25f, rb * 1.15f, 0.35f, 20, flangeColor);

    // 2. Base entrance security door & rain canopy
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 1.8f, rb * 1.02f}, Vec3{0.9f, 2.0f, 0.12f}, darkDetail);
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 2.85f, rb * 1.12f}, Vec3{1.2f, 0.08f, 0.35f}, flangeColor);

    // 3. Ground auxiliary step-up transformer cabinet with safety placard
    PropMeshHelper::appendBox(payload, Vec3{rb * 1.6f, 0.8f, 0.0f}, Vec3{1.8f, 1.6f, 1.4f}, 0xFF475569);
    PropMeshHelper::appendBox(payload, Vec3{rb * 1.6f, 1.2f, 0.72f}, Vec3{0.35f, 0.35f, 0.02f}, 0xFFF59E0B);

    // 4. Modular 3-section tapered tubular steel tower with flange rings
    float sec1H = (h - 0.95f) * 0.35f;
    float sec2H = (h - 0.95f) * 0.35f;
    float sec3H = (h - 0.95f) * 0.30f;
    float r1 = rb * 0.82f;
    float r2 = rb * 0.65f;
    float r3 = rt;

    // Section 1
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.95f, 0.0f}, rb, r1, sec1H, 24, towerColor);
    float yFlange1 = 0.95f + sec1H;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, yFlange1 - 0.15f, 0.0f}, r1 * 1.08f, r1 * 1.08f, 0.30f, 24, flangeColor);

    // Section 2
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, yFlange1, 0.0f}, r1, r2, sec2H, 24, towerColor);
    float yFlange2 = yFlange1 + sec2H;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, yFlange2 - 0.15f, 0.0f}, r2 * 1.08f, r2 * 1.08f, 0.30f, 24, flangeColor);

    // Section 3
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, yFlange2, 0.0f}, r2, r3, sec3H, 24, towerColor);

    // 5. Yaw mechanism ring collar
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, h, 0.0f}, r3 * 1.12f, r3 * 1.12f, 0.5f, 20, darkDetail);

    // 6. Aerodynamic nacelle fuselage atop tower
    float nacY = h + 1.6f;
    Vec3 nacellePos{0.0f, nacY, -0.6f};
    // Main body
    PropMeshHelper::appendBox(payload, nacellePos, Vec3{2.8f, 2.7f, 7.8f}, nacelleColor);
    // Beveled rear roof
    PropMeshHelper::appendBox(payload, Vec3{0.0f, nacY + 0.35f, -3.8f}, Vec3{2.5f, 1.8f, 1.8f}, nacelleColor);
    // Rear cooling radiator intake/exhaust louvers
    PropMeshHelper::appendBox(payload, Vec3{0.0f, nacY - 0.2f, -4.72f}, Vec3{2.0f, 1.4f, 0.08f}, darkDetail);
    // Roof safety railing
    PropMeshHelper::appendBox(payload, Vec3{0.0f, nacY + 1.45f, -1.0f}, Vec3{2.4f, 0.05f, 4.0f}, 0xFF94A3B8);
    PropMeshHelper::appendBox(payload, Vec3{-1.18f, nacY + 1.65f, -1.0f}, Vec3{0.04f, 0.4f, 4.0f}, 0xFF64748B);
    PropMeshHelper::appendBox(payload, Vec3{ 1.18f, nacY + 1.65f, -1.0f}, Vec3{0.04f, 0.4f, 4.0f}, 0xFF64748B);
    // Anemometer & wind vane sensor mast
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, nacY + 1.45f, -2.8f}, 0.06f, 0.04f, 1.2f, 8, darkDetail);
    PropMeshHelper::appendSphere(payload, Vec3{0.0f, nacY + 2.65f, -2.8f}, 0.14f, 6, 8, flangeColor);

    // 7. Flashing red aviation warning beacon atop nacelle
    if (params.hasAviationBeacon) {
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, nacY + 1.45f, -3.6f}, 0.08f, 0.08f, 0.5f, 8, darkDetail);
        PropMeshHelper::appendSphere(payload, Vec3{0.0f, nacY + 2.05f, -3.6f}, 0.22f, 8, 10, beaconColor);
    }

    // 8. Streamlined rotor spinner / hub cone
    Vec3 hubPos{0.0f, nacY, 3.3f};
    PropMeshHelper::appendCylinder(payload, hubPos, 1.8f, 1.2f, 1.4f, 20, hubColor);
    PropMeshHelper::appendSphere(payload, Vec3{0.0f, nacY, 4.6f}, 1.2f, 10, 14, towerColor);
}

} // namespace procedural::props

