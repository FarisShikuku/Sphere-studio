#include "EstateGateGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

EstateGateGen::EstateGateGen() = default;

bool EstateGateGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void EstateGateGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload EstateGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();

    float w = (ctx.customParam0 > 0.0f) ? ctx.customParam0 : params_.span;
    float h = (ctx.customParam1 > 0.0f) ? ctx.customParam1 : params_.pillarHeight;
    int style = (ctx.customParam3 >= 0) ? ctx.customParam3 : params_.style;
    int colIdx = (ctx.customParam4 >= 0) ? ctx.customParam4 : params_.colorScheme;
    bool hasLanterns = (ctx.customParam5 >= 0.0f) ? (ctx.customParam5 > 0.5f) : params_.hasLanterns;

    w = std::clamp(w, 3.0f, 9.0f);
    h = std::clamp(h, 1.8f, 3.8f);

    uint32_t gateCol = 0xFF111827; // Charcoal Black
    if (colIdx == 1) gateCol = 0xFFD97706; // Antique Gold
    else if (colIdx == 2) gateCol = 0xFF064E3B; // Heritage Green
    else if (colIdx == 3) gateCol = 0xFF78350F; // Bronze

    uint32_t stoneCol = 0xFFE2E8F0; // Ashlar Limestone
    if (colIdx == 1) stoneCol = 0xFFF1F5F9; // Carrara Marble
    else if (colIdx == 2) stoneCol = 0xFFCBD5E1; // Sandstone

    float pillarW = 0.55f;

    // Two stone pillars
    for (float side : {-1.0f, 1.0f}) {
        float px = side * (w * 0.5f + pillarW * 0.5f);
        // Plinth base
        PropMeshHelper::appendBox(payload, {px, 0.20f, 0.0f}, {pillarW + 0.16f, 0.40f, pillarW + 0.16f}, stoneCol);
        // Pillar shaft
        PropMeshHelper::appendBox(payload, {px, (h + 0.40f) * 0.5f, 0.0f}, {pillarW, h - 0.40f, pillarW}, stoneCol);
        // Capital cornice
        PropMeshHelper::appendBox(payload, {px, h + 0.08f, 0.0f}, {pillarW + 0.14f, 0.16f, pillarW + 0.14f}, stoneCol);
        // Pyramid capstone
        PropMeshHelper::appendBox(payload, {px, h + 0.22f, 0.0f}, {pillarW + 0.04f, 0.12f, pillarW + 0.04f}, stoneCol);

        if (hasLanterns) {
            PropMeshHelper::appendBox(payload, {px, h + 0.32f, 0.0f}, {0.12f, 0.08f, 0.12f}, 0xFF0F172A);
            PropMeshHelper::appendBox(payload, {px, h + 0.52f, 0.0f}, {0.24f, 0.32f, 0.24f}, 0xFF0F172A);
            PropMeshHelper::appendBox(payload, {px, h + 0.52f, 0.0f}, {0.18f, 0.26f, 0.18f}, 0xFFFBBF24);
            PropMeshHelper::appendCone(payload, {px, h + 0.68f, 0.0f}, 0.18f, 0.15f, 0xFF0F172A, 4);
        }
    }

    // Gate leaves
    float leafW = (w - 0.10f) * 0.5f;
    if (style == 0) { // Wrought iron double swing
        for (float side : {-1.0f, 1.0f}) {
            float cx = side * (leafW * 0.5f + 0.02f);
            float leafH = h * 0.88f;
            PropMeshHelper::appendBox(payload, {cx, 0.12f, 0.0f}, {leafW, 0.06f, 0.05f}, gateCol);
            PropMeshHelper::appendBox(payload, {cx, leafH, 0.0f}, {leafW, 0.06f, 0.05f}, gateCol);
            PropMeshHelper::appendBox(payload, {side * (w * 0.5f - 0.02f), leafH * 0.5f + 0.06f, 0.0f}, {0.06f, leafH, 0.06f}, gateCol);
            PropMeshHelper::appendBox(payload, {side * 0.03f, leafH * 0.5f + 0.06f, 0.0f}, {0.06f, leafH, 0.06f}, gateCol);

            int numPickets = std::max(4, static_cast<int>(leafW / 0.18f));
            for (int p = 0; p < numPickets; ++p) {
                float pickX = side * (0.08f + p * ((leafW - 0.12f) / (numPickets - 1)));
                PropMeshHelper::appendBox(payload, {pickX, leafH * 0.5f + 0.06f, 0.0f}, {0.025f, leafH - 0.06f, 0.025f}, gateCol);
                PropMeshHelper::appendCone(payload, {pickX, leafH + 0.03f, 0.0f}, 0.035f, 0.10f, (colIdx == 1 ? 0xFFF59E0B : gateCol), 4);
            }
            PropMeshHelper::appendBox(payload, {cx, leafH * 0.45f, 0.0f}, {leafW - 0.08f, 0.04f, 0.04f}, gateCol);
            PropMeshHelper::appendBox(payload, {cx, leafH * 0.45f, 0.0f}, {0.24f, 0.24f, 0.06f}, 0xFFF59E0B);
        }
    } else if (style == 1) { // Neoclassical Bronze
        for (float side : {-1.0f, 1.0f}) {
            float cx = side * (leafW * 0.5f + 0.02f);
            float leafH = h * 0.92f;
            PropMeshHelper::appendBox(payload, {cx, leafH * 0.20f, 0.0f}, {leafW, leafH * 0.36f, 0.06f}, gateCol);
            PropMeshHelper::appendBox(payload, {cx, leafH, 0.0f}, {leafW, 0.08f, 0.07f}, gateCol);
            PropMeshHelper::appendBox(payload, {side * (w * 0.5f - 0.02f), leafH * 0.55f, 0.0f}, {0.08f, leafH * 0.85f, 0.08f}, gateCol);
            PropMeshHelper::appendBox(payload, {side * 0.03f, leafH * 0.55f, 0.0f}, {0.08f, leafH * 0.85f, 0.08f}, gateCol);

            int numSpokes = 8;
            for (int s = 0; s < numSpokes; ++s) {
                float sx = side * (0.08f + s * ((leafW - 0.14f) / (numSpokes - 1)));
                PropMeshHelper::appendBox(payload, {sx, leafH * 0.65f, 0.0f}, {0.035f, leafH * 0.52f, 0.035f}, gateCol);
            }
        }
    } else if (style == 2) { // Modern architectural slats
        for (float side : {-1.0f, 1.0f}) {
            float cx = side * (leafW * 0.5f + 0.02f);
            float leafH = h * 0.85f;
            PropMeshHelper::appendBox(payload, {side * (w * 0.5f - 0.02f), leafH * 0.5f, 0.0f}, {0.07f, leafH, 0.07f}, 0xFF334155);
            PropMeshHelper::appendBox(payload, {side * 0.03f, leafH * 0.5f, 0.0f}, {0.07f, leafH, 0.07f}, 0xFF334155);

            int numSlats = 12;
            for (int s = 0; s < numSlats; ++s) {
                float sy = 0.10f + s * ((leafH - 0.12f) / (numSlats - 1));
                PropMeshHelper::appendBox(payload, {cx, sy, 0.0f}, {leafW - 0.04f, 0.07f, 0.03f}, gateCol);
            }
        }
        PropMeshHelper::appendBox(payload, {w * 0.5f + pillarW * 0.5f + 0.02f, 1.35f, pillarW * 0.5f + 0.02f}, {0.12f, 0.28f, 0.04f}, 0xFF0F172A);
        PropMeshHelper::appendBox(payload, {w * 0.5f + pillarW * 0.5f + 0.02f, 1.42f, pillarW * 0.5f + 0.03f}, {0.06f, 0.06f, 0.02f}, 0xFF38BDF8);
    } else { // Royal Gilded Palisade
        for (float side : {-1.0f, 1.0f}) {
            float cx = side * (leafW * 0.5f + 0.02f);
            float leafH = h * 0.95f;
            PropMeshHelper::appendBox(payload, {cx, 0.12f, 0.0f}, {leafW, 0.08f, 0.06f}, 0xFF111827);
            PropMeshHelper::appendBox(payload, {cx, leafH, 0.0f}, {leafW, 0.08f, 0.06f}, 0xFF111827);
            PropMeshHelper::appendBox(payload, {side * (w * 0.5f - 0.02f), leafH * 0.5f, 0.0f}, {0.08f, leafH, 0.08f}, 0xFF111827);
            PropMeshHelper::appendBox(payload, {side * 0.03f, leafH * 0.5f, 0.0f}, {0.08f, leafH, 0.08f}, 0xFF111827);

            int numPickets = std::max(5, static_cast<int>(leafW / 0.16f));
            for (int p = 0; p < numPickets; ++p) {
                float pickX = side * (0.08f + p * ((leafW - 0.12f) / (numPickets - 1)));
                PropMeshHelper::appendBox(payload, {pickX, leafH * 0.5f, 0.0f}, {0.03f, leafH - 0.08f, 0.03f}, 0xFF111827);
                PropMeshHelper::appendCone(payload, {pickX, leafH + 0.04f, 0.0f}, 0.04f, 0.14f, 0xFFF59E0B, 5);
            }
        }
    }

    return payload;
}

} // namespace procedural::props
