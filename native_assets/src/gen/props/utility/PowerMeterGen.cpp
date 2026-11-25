#include "PowerMeterGen.h"

namespace procedural::props {

PowerMeterGen::PowerMeterGen() {
    params_ = PowerMeterParams{};
}

bool PowerMeterGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void PowerMeterGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload PowerMeterGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t boxColor = 0xFF8F949D; // Galvanized electric panel gray
    uint32_t glassColor = 0xFFD2EBF7; // Transparent glass meter dome
    uint32_t conduitColor = 0xFF585D64;

    float h = params_.height;
    float w = params_.width;
    float d = params_.depth;

    // Wall mounting box
    PropMeshHelper::appendBox(payload, Vec3{0.0f, h * 0.5f, 0.0f}, Vec3{w, h, d}, boxColor);

    // Round meter socket base ring
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, h * 0.5f, d * 0.5f}, 0.11f, 0.11f, 0.03f, 16, boxColor);

    // Glass meter dome
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, h * 0.5f, d * 0.5f + 0.03f}, 0.10f, 0.09f, 0.08f, 16, glassColor);

    // Digital readout / dial face inside glass
    PropMeshHelper::appendBox(payload, Vec3{0.0f, h * 0.5f, d * 0.5f + 0.05f}, Vec3{0.12f, 0.06f, 0.01f}, 0xFF223322);

    // Vertical conduit pipes top and bottom
    if (params_.hasConduit) {
        // Upper conduit
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, h, 0.0f}, 0.03f, 0.03f, 0.6f, 12, conduitColor);
        // Lower conduit
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, -0.6f, 0.0f}, 0.03f, 0.03f, 0.6f, 12, conduitColor);
    }

    return payload;
}

} // namespace procedural::props
