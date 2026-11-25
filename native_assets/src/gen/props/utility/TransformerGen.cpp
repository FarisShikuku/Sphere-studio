#include "TransformerGen.h"

namespace procedural::props {

TransformerGen::TransformerGen() {
    params_ = TransformerParams{};
}

bool TransformerGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void TransformerGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload TransformerGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    if (params_.mountType == 1) {
        // Pad-mounted green enclosure
        uint32_t cabinetColor = 0xFF2A5034; // Utility dark green
        uint32_t padColor = 0xFF555555;
        uint32_t warningColor = 0xFFFFCC00;

        // Concrete pad
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.1f, 0.0f}, Vec3{1.4f, 0.2f, 1.2f}, padColor);
        // Steel cabinet body
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.7f, 0.0f}, Vec3{1.2f, 1.0f, 1.0f}, cabinetColor);
        // Peaked/sloped cabinet roof
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 1.25f, 0.0f}, Vec3{1.26f, 0.1f, 1.06f}, cabinetColor);
        // Warning decal
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.8f, 0.51f}, Vec3{0.25f, 0.25f, 0.01f}, warningColor);
    } else {
        // Pole-mounted transformer cylinder
        uint32_t tankColor = 0xFF656F7D;
        uint32_t bushingColor = 0xFF8B3A22; // Brown ceramic
        uint32_t lidColor = 0xFF4A525D;

        float h = params_.height;
        float r = params_.radius;

        // Main cylindrical oil tank
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, r, r, h, 20, tankColor);
        // Lid rim
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, h, 0.0f}, r * 1.05f, r * 1.05f, 0.06f, 20, lidColor);

        // Cooling fins / radiator ribs
        int fins = std::max(4, std::min(24, params_.finCount));
        float step = 2.0f * PropMeshHelper::kPi / fins;
        for (int i = 0; i < fins; ++i) {
            float ang = i * step;
            float fx = std::cos(ang) * (r + 0.05f);
            float fz = std::sin(ang) * (r + 0.05f);
            PropMeshHelper::appendBox(payload, Vec3{fx, h * 0.5f, fz}, Vec3{0.02f, h * 0.7f, 0.10f}, tankColor, -ang);
        }

        // Bushing insulators on top
        if (params_.hasBushingInsulators) {
            float bX[3] = {-r * 0.45f, 0.0f, r * 0.45f};
            for (float bx : bX) {
                PropMeshHelper::appendCylinder(payload, Vec3{bx, h + 0.06f, 0.0f}, 0.045f, 0.025f, 0.25f, 10, bushingColor);
                PropMeshHelper::appendSphere(payload, Vec3{bx, h + 0.31f, 0.0f}, 0.025f, 6, 8, 0xFFE0E0E0);
            }
        }
    }

    return payload;
}

} // namespace procedural::props
