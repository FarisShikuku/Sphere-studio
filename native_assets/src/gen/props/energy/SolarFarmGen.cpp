#include "SolarFarmGen.h"

namespace procedural::props {

SolarFarmGen::SolarFarmGen() {
    params_ = SolarFarmParams{};
}

bool SolarFarmGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void SolarFarmGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload SolarFarmGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t cellColor = 0xFF0F2552;
    uint32_t frameColor = 0xFFCBD5E1;
    uint32_t pierColor = 0xFF475569;
    uint32_t concreteColor = 0xFF64748B;
    uint32_t busbarColor = 0xFFE2E8F0;

    int nRows = std::max(2, std::min(5, params_.rows));
    int panelsPerRow = std::max(4, std::min(8, params_.panelsPerRow));
    float pitch = std::max(3.0f, params_.rowPitch);
    float panelW = 0.95f;
    float panelL = 2.1f;

    float farmWidth = panelsPerRow * (panelW + 0.12f);

    for (int r = 0; r < nRows; ++r) {
        float rowZ = (r - (nRows - 1) * 0.5f) * pitch;
        float tubeY = params_.groundClearance + 0.75f;

        // Continuous horizontal torque tube
        SolarTracker::appendGimbalAndTorqueTube(payload, Vec3{0.0f, tubeY, rowZ}, farmWidth, params_.tiltAngleDeg, pierColor);

        // Driven steel H-piles with foundation collars
        for (int p = 0; p <= panelsPerRow; p += 3) {
            float px = (p - panelsPerRow * 0.5f) * (panelW + 0.12f);
            PropMeshHelper::appendBox(payload, Vec3{px, tubeY * 0.5f, rowZ}, Vec3{0.12f, tubeY, 0.12f}, pierColor);
            PropMeshHelper::appendBox(payload, Vec3{px, 0.10f, rowZ}, Vec3{0.24f, 0.20f, 0.24f}, concreteColor);
        }

        // Row of PV solar modules
        for (int p = 0; p < panelsPerRow; ++p) {
            float px = (p - panelsPerRow * 0.5f + 0.5f) * (panelW + 0.12f);
            Vec3 pos{px, tubeY + 0.08f, rowZ};
            PropMeshHelper::appendBox(payload, pos, Vec3{panelW, 0.04f, panelL}, frameColor);
            PropMeshHelper::appendBox(payload, pos + Vec3{0.0f, 0.022f, 0.0f}, Vec3{panelW * 0.95f, 0.012f, panelL * 0.94f}, cellColor);
            // Center silver busbar
            PropMeshHelper::appendBox(payload, pos + Vec3{0.0f, 0.03f, 0.0f}, Vec3{0.01f, 0.006f, panelL * 0.90f}, busbarColor);
        }

        // String combiner box at row end
        PropMeshHelper::appendBox(payload, Vec3{farmWidth * 0.5f + 0.2f, tubeY * 0.6f, rowZ}, Vec3{0.22f, 0.35f, 0.18f}, concreteColor);
    }

    // Central skid inverter / transformer unit
    if (params_.hasInverterStation) {
        float kioskX = -farmWidth * 0.5f - 2.8f;
        // Concrete pad
        PropMeshHelper::appendBox(payload, Vec3{kioskX, 0.18f, 0.0f}, Vec3{3.2f, 0.36f, 2.2f}, concreteColor);
        // Central inverter enclosure
        Vec3 invPos{kioskX - 0.6f, 1.2f, 0.0f};
        PropMeshHelper::appendBox(payload, invPos, Vec3{1.6f, 1.7f, 1.4f}, 0xFFF8FAFC);
        // Ventilation louvers
        PropMeshHelper::appendBox(payload, invPos + Vec3{0.0f, 0.1f, 0.72f}, Vec3{1.2f, 0.8f, 0.02f}, 0xFF334155);

        // Step-up transformer
        PropMeshHelper::appendBox(payload, Vec3{kioskX + 0.75f, 1.0f, 0.0f}, Vec3{1.1f, 1.3f, 1.2f}, 0xFF334155);
        for (float b : {-0.3f, 0.0f, 0.3f}) {
            PropMeshHelper::appendCylinder(payload, Vec3{kioskX + 0.75f + b, 1.65f, 0.0f}, 0.06f, 0.03f, 0.4f, 8, 0xFF8D4024);
        }

        // Weather station mast
        PropMeshHelper::appendCylinder(payload, Vec3{kioskX, 0.36f, -0.9f}, 0.04f, 0.03f, 2.8f, 8, pierColor);
        PropMeshHelper::appendSphere(payload, Vec3{kioskX, 3.2f, -0.9f}, 0.12f, 6, 8, 0xFFCBD5E1);
    }

    return payload;
}

} // namespace procedural::props
