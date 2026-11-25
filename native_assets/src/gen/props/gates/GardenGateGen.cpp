#include "GardenGateGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

GardenGateGen::GardenGateGen() = default;

bool GardenGateGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void GardenGateGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload GardenGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();

    float w = (ctx.customParam0 > 0.0f) ? ctx.customParam0 : params_.arborWidth;
    float h = (ctx.customParam1 > 0.0f) ? ctx.customParam1 : params_.arborHeight;
    int style = (ctx.customParam3 >= 0) ? ctx.customParam3 : params_.style;
    int colIdx = (ctx.customParam4 >= 0) ? ctx.customParam4 : params_.colorScheme;
    bool hasLantern = (ctx.customParam5 >= 0.0f) ? (ctx.customParam5 > 0.5f) : params_.hasLantern;

    w = std::clamp(w, 1.0f, 2.4f);
    h = std::clamp(h, 1.8f, 3.2f);
    float d = 0.85f;

    uint32_t woodCol = 0xFF8D6E63; // Cedar
    if (colIdx == 1) woodCol = 0xFFF8FAFC; // White
    else if (colIdx == 2) woodCol = 0xFF334155; // Charcoal
    else if (colIdx == 3) woodCol = 0xFF0F172A; // Black

    if (style == 0) { // Rose Arbor Pergola
        float postThick = 0.09f;
        for (float sx : {-1.0f, 1.0f}) {
            for (float sz : {-1.0f, 1.0f}) {
                PropMeshHelper::appendBox(
                    payload,
                    {sx * (w * 0.5f), h * 0.5f, sz * (d * 0.5f)},
                    {postThick, h, postThick},
                    woodCol
                );
            }
            float sideX = sx * (w * 0.5f);
            for (int r = 1; r <= 4; ++r) {
                float ry = r * (h * 0.20f);
                PropMeshHelper::appendBox(payload, {sideX, ry, 0.0f}, {postThick * 0.6f, 0.04f, d}, woodCol);
            }
            PropMeshHelper::appendStrut(payload, {sideX, 0.2f, -d * 0.5f}, {sideX, h * 0.8f, d * 0.5f}, 0.025f, woodCol);
            PropMeshHelper::appendStrut(payload, {sideX, 0.2f, d * 0.5f}, {sideX, h * 0.8f, -d * 0.5f}, 0.025f, woodCol);
        }

        PropMeshHelper::appendBox(payload, {-w * 0.5f, h + 0.06f, 0.0f}, {0.08f, 0.14f, d + 0.35f}, woodCol);
        PropMeshHelper::appendBox(payload, { w * 0.5f, h + 0.06f, 0.0f}, {0.08f, 0.14f, d + 0.35f}, woodCol);

        int numRafters = 5;
        for (int r = 0; r < numRafters; ++r) {
            float rz = -d * 0.5f + r * (d / (numRafters - 1));
            PropMeshHelper::appendBox(payload, {0.0f, h + 0.16f, rz}, {w + 0.36f, 0.07f, 0.05f}, woodCol);
        }

        float doorW = w - 0.20f;
        float doorH = h * 0.60f;
        PropMeshHelper::appendBox(payload, {0.0f, doorH * 0.5f + 0.05f, 0.0f}, {doorW, doorH, 0.035f}, woodCol);
        PropMeshHelper::appendBox(payload, {0.0f, doorH + 0.06f, 0.0f}, {doorW * 0.70f, 0.08f, 0.045f}, woodCol);

        PropMeshHelper::appendBox(payload, {-doorW * 0.30f, doorH * 0.80f, 0.022f}, {doorW * 0.40f, 0.04f, 0.015f}, 0xFF1E293B);
        PropMeshHelper::appendBox(payload, {-doorW * 0.30f, doorH * 0.20f, 0.022f}, {doorW * 0.40f, 0.04f, 0.015f}, 0xFF1E293B);
        PropMeshHelper::appendBox(payload, {doorW * 0.40f, doorH * 0.52f, 0.025f}, {0.06f, 0.10f, 0.03f}, 0xFFD97706);

        if (hasLantern) {
            PropMeshHelper::appendBox(payload, {0.0f, h + 0.05f, 0.0f}, {0.015f, 0.15f, 0.015f}, 0xFF0F172A);
            PropMeshHelper::appendBox(payload, {0.0f, h - 0.12f, 0.0f}, {0.14f, 0.18f, 0.14f}, 0xFF0F172A);
            PropMeshHelper::appendBox(payload, {0.0f, h - 0.12f, 0.0f}, {0.10f, 0.14f, 0.10f}, 0xFFFBBF24);
        }
    } else if (style == 1) { // Cottage Picket Wicket
        float postH = h * 0.75f;
        float postThick = 0.10f;
        for (float side : {-1.0f, 1.0f}) {
            float px = side * (w * 0.5f);
            PropMeshHelper::appendBox(payload, {px, postH * 0.5f, 0.0f}, {postThick, postH, postThick}, woodCol);
            PropMeshHelper::appendCone(payload, {px, postH, 0.0f}, postThick * 0.6f, 0.10f, woodCol, 4);
        }

        float gateW = w - postThick * 2.0f - 0.04f;
        int numPickets = std::max(5, static_cast<int>(gateW / 0.12f));
        for (int p = 0; p < numPickets; ++p) {
            float t = static_cast<float>(p) / (numPickets - 1);
            float px = -gateW * 0.5f + t * gateW;
            float pickH = postH * (0.80f + 0.15f * std::sin(t * 3.14159f));
            PropMeshHelper::appendBox(payload, {px, pickH * 0.5f, 0.0f}, {0.07f, pickH, 0.025f}, woodCol);
            PropMeshHelper::appendCone(payload, {px, pickH, 0.0f}, 0.035f, 0.07f, woodCol, 4);
        }
        PropMeshHelper::appendBox(payload, {0.0f, postH * 0.75f, 0.015f}, {gateW, 0.06f, 0.03f}, woodCol);
        PropMeshHelper::appendBox(payload, {0.0f, postH * 0.25f, 0.015f}, {gateW, 0.06f, 0.03f}, woodCol);

        if (hasLantern) {
            PropMeshHelper::appendBox(payload, {w * 0.5f, postH + 0.18f, 0.0f}, {0.12f, 0.16f, 0.12f}, 0xFF0F172A);
            PropMeshHelper::appendBox(payload, {w * 0.5f, postH + 0.18f, 0.0f}, {0.08f, 0.12f, 0.08f}, 0xFFFBBF24);
        }
    } else if (style == 2) { // Zen Moon Gate
        PropMeshHelper::appendBox(payload, {0.0f, 0.12f, 0.0f}, {w + 0.8f, 0.24f, 0.40f}, 0xFF475569);
        float wallW = (w + 0.6f) * 0.5f;
        PropMeshHelper::appendBox(payload, {-wallW * 0.5f - 0.2f, h * 0.5f, 0.0f}, {wallW * 0.45f, h, 0.15f}, 0xFFF1F5F9);
        PropMeshHelper::appendBox(payload, { wallW * 0.5f + 0.2f, h * 0.5f, 0.0f}, {wallW * 0.45f, h, 0.15f}, 0xFFF1F5F9);

        float portalR = w * 0.48f;
        int segs = 12;
        for (int s = 0; s < segs; ++s) {
            float angle0 = s * (3.14159f * 2.0f / segs);
            float angle1 = (s + 1) * (3.14159f * 2.0f / segs);
            Vec3 p0{portalR * std::cos(angle0), h * 0.5f + portalR * std::sin(angle0), 0.0f};
            Vec3 p1{portalR * std::cos(angle1), h * 0.5f + portalR * std::sin(angle1), 0.0f};
            PropMeshHelper::appendStrut(payload, p0, p1, 0.08f, woodCol);
        }

        float gateH = h * 0.45f;
        int slats = 8;
        for (int s = 0; s < slats; ++s) {
            float sx = -w * 0.35f + s * (w * 0.70f / (slats - 1));
            PropMeshHelper::appendBox(payload, {sx, gateH * 0.5f, 0.0f}, {0.04f, gateH, 0.04f}, 0xFFD97706);
        }

        if (hasLantern) {
            PropMeshHelper::appendBox(payload, {w * 0.5f + 0.25f, 0.35f, 0.25f}, {0.22f, 0.35f, 0.22f}, 0xFF64748B);
            PropMeshHelper::appendBox(payload, {w * 0.5f + 0.25f, 0.55f, 0.25f}, {0.18f, 0.14f, 0.18f}, 0xFFFEF08A);
        }
    } else { // Victorian Floral Arch
        for (float side : {-1.0f, 1.0f}) {
            float px = side * (w * 0.5f);
            PropMeshHelper::appendBox(payload, {px, h * 0.5f, 0.0f}, {0.05f, h, 0.05f}, 0xFF111827);
        }
        PropMeshHelper::appendBox(payload, {0.0f, h + 0.15f, 0.0f}, {w + 0.10f, 0.06f, 0.06f}, 0xFF111827);
        PropMeshHelper::appendStrut(payload, {-w * 0.5f, h * 0.85f, 0.0f}, {0.0f, h + 0.15f, 0.0f}, 0.04f, 0xFF111827);
        PropMeshHelper::appendStrut(payload, { w * 0.5f, h * 0.85f, 0.0f}, {0.0f, h + 0.15f, 0.0f}, 0.04f, 0xFF111827);

        for (float side : {-1.0f, 1.0f}) {
            float leafW = (w - 0.08f) * 0.5f;
            float leafH = h * 0.65f;
            float cx = side * (leafW * 0.5f + 0.02f);
            PropMeshHelper::appendBox(payload, {cx, leafH * 0.5f, 0.0f}, {leafW, leafH, 0.025f}, 0xFF111827);
            PropMeshHelper::appendBox(payload, {cx, leafH * 0.5f, 0.015f}, {0.14f, 0.14f, 0.02f}, 0xFFF59E0B);
        }

        if (hasLantern) {
            PropMeshHelper::appendBox(payload, {0.0f, h + 0.02f, 0.0f}, {0.14f, 0.18f, 0.14f}, 0xFF111827);
            PropMeshHelper::appendBox(payload, {0.0f, h + 0.02f, 0.0f}, {0.10f, 0.12f, 0.10f}, 0xFFFBBF24);
        }
    }

    return payload;
}

} // namespace procedural::props
