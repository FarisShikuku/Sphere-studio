#include "RailwayGateGen.h"
#include <algorithm>

namespace procedural::props {

RailwayGateGen::RailwayGateGen() = default;

bool RailwayGateGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void RailwayGateGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload RailwayGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();

    float armLen = (ctx.customParam0 > 0.0f) ? ctx.customParam0 : params_.armLength;
    float mastH = (ctx.customParam1 > 0.0f) ? ctx.customParam1 : params_.mastHeight;
    int style = (ctx.customParam3 >= 0) ? ctx.customParam3 : params_.style;
    bool hasBellAndLights = (ctx.customParam5 >= 0.0f) ? (ctx.customParam5 > 0.5f) : params_.hasBellAndLights;

    armLen = std::clamp(armLen, 4.0f, 12.0f);
    mastH = std::clamp(mastH, 3.0f, 5.5f);

    if (style == 0) { // Dual-Track Grade Crossing Mast
        // Concrete foundation pedestal
        PropMeshHelper::appendBox(payload, {0.0f, 0.15f, 0.0f}, {0.65f, 0.30f, 0.65f}, 0xFF64748B);
        // Galvanized steel mast pipe
        PropMeshHelper::appendCylinder(payload, {0.0f, 0.30f, 0.0f}, 0.10f, mastH - 0.30f, 0xFFCBD5E1, 8);

        // Crossbuck "X" Sign Blades
        float cbY = mastH * 0.78f;
        PropMeshHelper::appendBox(payload, {0.0f, cbY, 0.12f}, {1.25f, 0.18f, 0.02f}, 0xFFF8FAFC);
        PropMeshHelper::appendBox(payload, {0.0f, cbY, 0.14f}, {1.25f, 0.18f, 0.02f}, 0xFFF8FAFC);
        PropMeshHelper::appendBox(payload, {0.0f, cbY, 0.15f}, {0.20f, 0.20f, 0.03f}, 0xFF0F172A);

        // Dual Alternating Red Flashing Lamp Assembly
        float lampY = mastH * 0.62f;
        PropMeshHelper::appendBox(payload, {0.0f, lampY, 0.10f}, {0.85f, 0.06f, 0.06f}, 0xFF334155);
        for (float side : {-1.0f, 1.0f}) {
            float lx = side * 0.38f;
            PropMeshHelper::appendBox(payload, {lx, lampY, 0.14f}, {0.26f, 0.26f, 0.12f}, 0xFF0F172A);
            PropMeshHelper::appendCylinder(payload, {lx, lampY, 0.20f}, 0.09f, 0.025f, 0xFFEF4444, 8);
        }

        // Motorized Gate Operator Mechanism Box
        PropMeshHelper::appendBox(payload, {0.0f, 1.05f, 0.0f}, {0.50f, 0.58f, 0.45f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, {-0.55f, 1.05f, 0.0f}, {0.65f, 0.10f, 0.10f}, 0xFF334155);
        PropMeshHelper::appendBox(payload, {-0.85f, 1.05f, 0.0f}, {0.30f, 0.38f, 0.25f}, 0xFF1E293B);

        // Road-spanning striped barrier arm
        float armY = 1.05f;
        float armCenter = armLen * 0.5f;
        PropMeshHelper::appendBox(payload, {armCenter, armY, 0.0f}, {armLen, 0.14f, 0.035f}, 0xFFF8FAFC);
        int stripes = std::max(4, static_cast<int>(armLen / 0.8f));
        for (int i = 0; i < stripes; ++i) {
            float sx = 0.50f + i * ((armLen - 0.70f) / (stripes - 1));
            PropMeshHelper::appendBox(payload, {sx, armY, 0.0f}, {0.36f, 0.142f, 0.038f}, 0xFFDC2626);
        }

        if (hasBellAndLights) {
            PropMeshHelper::appendCone(payload, {0.0f, mastH + 0.12f, 0.0f}, 0.14f, 0.18f, 0xFFD97706, 8);
            for (float pos : {armLen * 0.35f, armLen * 0.65f, armLen - 0.15f}) {
                PropMeshHelper::appendBox(payload, {pos, armY + 0.12f, 0.0f}, {0.08f, 0.10f, 0.08f}, 0xFF0F172A);
                PropMeshHelper::appendCylinder(payload, {pos, armY + 0.16f, 0.0f}, 0.035f, 0.04f, 0xFFEF4444, 6);
            }
        }
    } else if (style == 1) { // Cantilever Overhead Truss Crossing
        float trussSpan = 4.5f;
        PropMeshHelper::appendBox(payload, {-trussSpan * 0.5f, mastH * 0.5f, 0.0f}, {0.40f, mastH, 0.40f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, {0.0f, mastH, 0.0f}, {trussSpan + 0.8f, 0.45f, 0.45f}, 0xFF334155);

        PropMeshHelper::appendBox(payload, {0.0f, mastH * 0.90f, 0.30f}, {1.1f, 0.16f, 0.02f}, 0xFFF8FAFC);
        PropMeshHelper::appendBox(payload, {0.0f, mastH * 0.90f, 0.32f}, {1.1f, 0.16f, 0.02f}, 0xFFF8FAFC);
        PropMeshHelper::appendBox(payload, {-0.35f, mastH * 0.72f, 0.30f}, {0.24f, 0.24f, 0.12f}, 0xFFEF4444);
        PropMeshHelper::appendBox(payload, { 0.35f, mastH * 0.72f, 0.30f}, {0.24f, 0.24f, 0.12f}, 0xFFEF4444);

        float gateX = -trussSpan * 0.5f;
        PropMeshHelper::appendBox(payload, {gateX, 1.05f, 0.0f}, {0.50f, 0.55f, 0.45f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, {gateX + armLen * 0.5f, 1.05f, 0.0f}, {armLen, 0.14f, 0.035f}, 0xFFF8FAFC);

        if (hasBellAndLights) {
            PropMeshHelper::appendCone(payload, {gateX, mastH + 0.15f, 0.0f}, 0.14f, 0.18f, 0xFFD97706, 8);
        }
    } else { // Pedestrian Maze & Gate
        float mazeW = 3.2f;
        float fenceH = 1.2f;
        PropMeshHelper::appendBox(payload, {-mazeW * 0.5f, fenceH * 0.5f, -0.6f}, {0.06f, fenceH, 1.2f}, 0xFFEAB308);
        PropMeshHelper::appendBox(payload, {0.0f, fenceH * 0.5f, 0.0f}, {mazeW * 0.6f, fenceH, 0.06f}, 0xFFEAB308);
        PropMeshHelper::appendBox(payload, {mazeW * 0.5f, fenceH * 0.5f, 0.6f}, {0.06f, fenceH, 1.2f}, 0xFFEAB308);

        PropMeshHelper::appendBox(payload, {-0.4f, fenceH * 0.5f, 0.6f}, {0.90f, fenceH * 0.85f, 0.04f}, 0xFFDC2626);
        PropMeshHelper::appendBox(payload, {mazeW * 0.5f, mastH * 0.45f, 0.0f}, {0.10f, mastH * 0.9f, 0.10f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, {mazeW * 0.5f, mastH * 0.85f, 0.12f}, {0.22f, 0.22f, 0.18f}, 0xFFEF4444);

        if (hasBellAndLights) {
            PropMeshHelper::appendBox(payload, {mazeW * 0.5f, mastH * 0.95f, 0.0f}, {0.18f, 0.18f, 0.18f}, 0xFFFACC15);
        }
    }

    return payload;
}

} // namespace procedural::props
