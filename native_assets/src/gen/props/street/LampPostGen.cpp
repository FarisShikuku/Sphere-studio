#include "LampPostGen.h"

namespace procedural::props {

LampPostGen::LampPostGen() = default;

bool LampPostGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void LampPostGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload LampPostGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    // Concrete base pedestal
    PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.15f, 0.0f}, Vec3{0.6f, 0.3f, 0.6f}, 0xFF666666);

    // Steel base plate & anchor flange
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.3f, 0.0f}, params_.poleRadius * 1.8f, params_.poleRadius * 1.5f, 0.15f, 16, 0xFF444444);

    // Main vertical tapered mast
    float poleH = params_.height * 0.85f;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.45f, 0.0f}, params_.poleRadius, params_.poleRadius * 0.55f, poleH, 16, 0xFF555A60);

    // Curved arm outreach segment(s)
    float mastTopY = 0.45f + poleH;
    int arms = params_.doubleArm ? 2 : 1;

    for (int armIdx = 0; armIdx < arms; ++armIdx) {
        float dirSign = (armIdx == 0) ? 1.0f : -1.0f;
        float armR = params_.poleRadius * 0.45f;
        int segments = 6;
        Vec3 prevPt{0.0f, mastTopY, 0.0f};

        for (int i = 1; i <= segments; ++i) {
            float t = static_cast<float>(i) / segments;
            float px = dirSign * params_.armReach * t;
            // Arc curve
            float py = mastTopY + (params_.height * 0.15f) * std::sin(t * 1.4f);
            Vec3 currPt{px, py, 0.0f};

            PropMeshHelper::appendOrientedCylinder(payload, prevPt, currPt, armR, armR * 0.95f, 12, 0xFF555A60, true, true);
            prevPt = currPt;
        }

        // Luminaire Head at the end of the arm
        Vec3 headPos = prevPt;
        if (params_.luminaireStyle == 0) {
            // Modern Cobra-head steel fixture
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.35f, -0.05f, 0.0f}, Vec3{0.8f, 0.18f, 0.35f}, 0xFF3A3F45);
            // Emissive lens
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.35f, -0.15f, 0.0f}, Vec3{0.65f, 0.04f, 0.25f}, 0xFFFFFFAA);
        } else if (params_.luminaireStyle == 1) {
            // Vintage / Victorian Heritage Lantern
            PropMeshHelper::appendCylinder(payload, headPos + Vec3{0.0f, -0.6f, 0.0f}, 0.22f, 0.16f, 0.45f, 6, 0xFF222222);
            PropMeshHelper::appendCylinder(payload, headPos + Vec3{0.0f, -0.15f, 0.0f}, 0.16f, 0.28f, 0.15f, 6, 0xFF1A1A1A);
            // Glowing core
            PropMeshHelper::appendSphere(payload, headPos + Vec3{0.0f, -0.38f, 0.0f}, 0.12f, 6, 8, 0xFFFFEE99);
        } else if (params_.luminaireStyle == 2) {
            // Minimalist Modern LED Bar
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.5f, 0.0f, 0.0f}, Vec3{1.1f, 0.08f, 0.15f}, 0xFF282828);
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.5f, -0.05f, 0.0f}, Vec3{1.0f, 0.02f, 0.10f}, 0xFFDDFFFF);
        } else if (params_.luminaireStyle == 3) {
            // Acorn Globe Boulevard Lamp (cast iron fitter + frosted glass globe + brass finial)
            PropMeshHelper::appendCylinder(payload, headPos + Vec3{0.0f, 0.0f, 0.0f}, 0.16f, 0.22f, 0.18f, 10, 0xFF1F2429);
            PropMeshHelper::appendSphere(payload, headPos + Vec3{0.0f, 0.34f, 0.0f}, 0.22f, 10, 12, 0xFFFFF8DC);
            PropMeshHelper::appendCylinder(payload, headPos + Vec3{0.0f, 0.54f, 0.0f}, 0.08f, 0.01f, 0.14f, 8, 0xFFB8860B);
        } else if (params_.luminaireStyle == 4) {
            // Solar Smart LED Street Lamp (angled PV panel + battery box + slim LED head)
            if (armIdx == 0) {
                PropMeshHelper::appendBox(payload, Vec3{0.0f, mastTopY + 0.35f, 0.0f}, Vec3{0.95f, 0.06f, 0.65f}, 0xFF102A43);
                PropMeshHelper::appendBox(payload, Vec3{0.0f, mastTopY + 0.39f, 0.0f}, Vec3{0.88f, 0.02f, 0.58f}, 0xFF1D4ED8);
                PropMeshHelper::appendBox(payload, Vec3{0.0f, mastTopY - 0.35f, 0.18f}, Vec3{0.30f, 0.40f, 0.22f}, 0xFF334155);
            }
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.35f, -0.04f, 0.0f}, Vec3{0.75f, 0.10f, 0.30f}, 0xFF1E293B);
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.35f, -0.10f, 0.0f}, Vec3{0.68f, 0.03f, 0.24f}, 0xFFE0F2FE);
        } else {
            // High-Mast Stadium / Plaza Floodlight Bar
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.15f, 0.0f, 0.0f}, Vec3{0.18f, 0.14f, 0.90f}, 0xFF374151);
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.30f, -0.04f, -0.28f}, Vec3{0.28f, 0.22f, 0.28f}, 0xFF1F2937);
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.30f, -0.04f, 0.28f}, Vec3{0.28f, 0.22f, 0.28f}, 0xFF1F2937);
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.44f, -0.06f, -0.28f}, Vec3{0.04f, 0.16f, 0.22f}, 0xFFFFFFFF);
            PropMeshHelper::appendBox(payload, headPos + Vec3{dirSign * 0.44f, -0.06f, 0.28f}, Vec3{0.04f, 0.16f, 0.22f}, 0xFFFFFFFF);
        }
    }

    return payload;
}

} // namespace procedural::props
