#include "WaterTowerGen.h"

namespace procedural::props {

WaterTowerGen::WaterTowerGen() {
    params_ = WaterTowerParams{};
}

bool WaterTowerGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void WaterTowerGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload WaterTowerGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    uint32_t tankColor = 0xFFC5CCD6;   // Sky-tinted galvanized municipal coating
    uint32_t legColor = 0xFF5A6370;    // Steel structure gray
    uint32_t pipeColor = 0xFF454B54;   // Central riser pipe
    uint32_t roofColor = 0xFF9DA7B5;

    float totalH = params_.totalHeight;
    float tankD = params_.tankDiameter;
    float tankH = params_.tankHeight;
    float legR = tankD * 0.45f;
    float towerLegsH = totalH - tankH;

    // Central water riser pipe
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, 0.6f, 0.6f, towerLegsH, 16, pipeColor);

    // Multi-leg structural steel tower
    int numLegs = std::max(4, std::min(8, params_.legCount));
    float legStep = 2.0f * PropMeshHelper::kPi / numLegs;

    for (int i = 0; i < numLegs; ++i) {
        float ang = i * legStep;
        float bx = std::cos(ang) * (legR * 1.35f); // Base spread
        float bz = std::sin(ang) * (legR * 1.35f);
        float tx = std::cos(ang) * legR;           // Top under tank
        float tz = std::sin(ang) * legR;

        // Concrete pier footing
        PropMeshHelper::appendBox(payload, Vec3{bx, 0.4f, bz}, Vec3{1.4f, 0.8f, 1.4f}, 0xFF666666);

        // Tubular steel leg column connecting from pier to tank
        PropMeshHelper::appendOrientedCylinder(payload, Vec3{bx, 0.4f, bz}, Vec3{tx, towerLegsH, tz}, 0.26f, 0.22f, 12, legColor, true, true);

        // Circumferential sway braces and diagonal cross lattice at height tiers
        for (int tier = 1; tier <= 3; ++tier) {
            float ty = towerLegsH * (static_cast<float>(tier) / 4.0f);
            float nAng = (i + 1) * legStep;
            float ntx = std::cos(nAng) * (legR * (1.35f - 0.35f * (ty / towerLegsH)));
            float ntz = std::sin(nAng) * (legR * (1.35f - 0.35f * (ty / towerLegsH)));
            float ctx = std::cos(ang) * (legR * (1.35f - 0.35f * (ty / towerLegsH)));
            float ctz = std::sin(ang) * (legR * (1.35f - 0.35f * (ty / towerLegsH)));

            // Horizontal continuous ring brace
            PropMeshHelper::appendOrientedCylinder(payload, Vec3{ctx, ty, ctz}, Vec3{ntx, ty, ntz}, 0.10f, 0.10f, 8, legColor, true, true);

            // Diagonal cross-bracing
            float prevTy = towerLegsH * (static_cast<float>(tier - 1) / 4.0f);
            float pctx = std::cos(ang) * (legR * (1.35f - 0.35f * (prevTy / towerLegsH)));
            float pctz = std::sin(ang) * (legR * (1.35f - 0.35f * (prevTy / towerLegsH)));
            float pntx = std::cos(nAng) * (legR * (1.35f - 0.35f * (prevTy / towerLegsH)));
            float pntz = std::sin(nAng) * (legR * (1.35f - 0.35f * (prevTy / towerLegsH)));
            PropMeshHelper::appendOrientedCylinder(payload, Vec3{pctx, prevTy, pctz}, Vec3{ntx, ty, ntz}, 0.05f, 0.05f, 6, legColor, false, false);
            PropMeshHelper::appendOrientedCylinder(payload, Vec3{pntx, prevTy, pntz}, Vec3{ctx, ty, ctz}, 0.05f, 0.05f, 6, legColor, false, false);
        }
    }

    // Elevated tank bottom bowl / hopper
    float tankBaseY = towerLegsH;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, tankBaseY, 0.0f}, legR, tankD * 0.5f, tankH * 0.35f, 24, tankColor);

    // Cylindrical tank barrel
    float barrelY = tankBaseY + tankH * 0.35f;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, barrelY, 0.0f}, tankD * 0.5f, tankD * 0.5f, tankH * 0.65f, 24, tankColor);

    // Conical roof cap
    float roofY = barrelY + tankH * 0.65f;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, roofY, 0.0f}, tankD * 0.5f, 0.3f, tankH * 0.3f, 24, roofColor);

    // Maintenance catwalk platform around tank
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, barrelY, 0.0f}, tankD * 0.53f, tankD * 0.53f, 0.12f, 24, legColor);

    return payload;
}

} // namespace procedural::props
