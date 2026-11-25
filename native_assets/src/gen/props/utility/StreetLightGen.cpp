#include "StreetLightGen.h"

namespace procedural::props {

StreetLightGen::StreetLightGen() {
    params_ = StreetLightParams{};
}

bool StreetLightGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void StreetLightGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload StreetLightGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t mastColor = 0xFF4E5560; // Dark grey powder-coat steel
    uint32_t headColor = 0xFF2D3238;
    uint32_t lensColor = 0xFFFFFBE8; // Warm LED glow
    uint32_t baseColor = 0xFF3A3F45;

    float h = params_.height;
    float reach = params_.armReach;

    // Anchor base plate & anchor bolts
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.15f, 0.0f}, Vec3{0.5f, 0.3f, 0.5f}, baseColor);

    // Tapered vertical mast
    float poleH = h * 0.85f;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.3f, 0.0f}, 0.13f, 0.075f, poleH, 16, mastColor);

    // Curved mast arm (modeled with segmented cantilever arm)
    float mastTopY = 0.3f + poleH;
    int armSegs = 4;
    Vec3 prevPt{0.0f, mastTopY, 0.0f};

    for (int i = 1; i <= armSegs; ++i) {
        float t = static_cast<float>(i) / armSegs;
        float curX = reach * t;
        float curY = mastTopY + std::sin(t * 1.57f) * 0.6f;
        Vec3 curPt{curX, curY, 0.0f};

        PropMeshHelper::appendOrientedCylinder(payload, prevPt, curPt, 0.075f, 0.055f, 12, mastColor, true, true);
        prevPt = curPt;
    }

    // Luminaire head (Cobra-head style)
    Vec3 headPos = prevPt + Vec3{0.3f, -0.05f, 0.0f};
    PropMeshHelper::appendBox(payload, headPos, Vec3{0.65f, 0.14f, 0.28f}, headColor);

    // Underside optical lens
    PropMeshHelper::appendBox(payload, Vec3{headPos.x, headPos.y - 0.08f, headPos.z}, Vec3{0.50f, 0.02f, 0.22f}, lensColor);

    // Photocell sensor on top
    if (params_.hasPhotocell) {
        PropMeshHelper::appendCylinder(payload, Vec3{headPos.x - 0.12f, headPos.y + 0.07f, headPos.z}, 0.025f, 0.025f, 0.04f, 8, 0xFF1E3A8A);
    }

    return payload;
}

} // namespace procedural::props
