#include "TollGateGen.h"
#include <algorithm>

namespace procedural::props {

TollGateGen::TollGateGen() = default;

bool TollGateGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void TollGateGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload TollGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();

    float w = (ctx.customParam0 > 0.0f) ? ctx.customParam0 : params_.laneWidth;
    float h = (ctx.customParam1 > 0.0f) ? ctx.customParam1 : params_.kioskHeight;
    int style = (ctx.customParam3 >= 0) ? ctx.customParam3 : params_.style;
    bool hasCanopy = (ctx.customParam5 >= 0.0f) ? (ctx.customParam5 > 0.5f) : params_.hasCanopy;

    w = std::clamp(w, 3.0f, 6.0f);
    h = std::clamp(h, 2.4f, 4.5f);

    float armLen = w * 0.92f;

    if (style == 0) { // Highway Toll Plaza with Attendant Kiosk
        // Concrete traffic divider island curb
        PropMeshHelper::appendBox(payload, {-w * 0.5f - 0.65f, 0.15f, 0.0f}, {1.10f, 0.30f, 4.2f}, 0xFF94A3B8);
        PropMeshHelper::appendBox(payload, { w * 0.5f + 0.30f, 0.15f, 0.0f}, {0.60f, 0.30f, 3.8f}, 0xFF94A3B8);

        // Cashier attendant kiosk booth
        float kioskX = -w * 0.5f - 0.65f;
        PropMeshHelper::appendBox(payload, {kioskX, h * 0.45f + 0.15f, 0.0f}, {0.95f, h * 0.85f, 2.2f}, 0xFFE2E8F0);
        PropMeshHelper::appendBox(payload, {kioskX, 1.55f, 0.0f}, {0.98f, 0.85f, 2.05f}, 0xFF38BDF8);
        PropMeshHelper::appendBox(payload, {-w * 0.5f - 0.14f, 1.15f, 0.0f}, {0.25f, 0.06f, 0.65f}, 0xFF475569);

        // Motorized toll barrier pedestal (safety orange)
        float pedX = -w * 0.5f + 0.05f;
        PropMeshHelper::appendBox(payload, {pedX, 0.55f, 0.70f}, {0.35f, 1.05f, 0.35f}, 0xFFF97316);
        PropMeshHelper::appendBox(payload, {pedX + 0.10f, 0.95f, 0.70f}, {0.12f, 0.16f, 0.16f}, 0xFF1E293B);

        // High-speed barrier boom arm
        float armCenter = pedX + 0.15f + armLen * 0.5f;
        PropMeshHelper::appendBox(payload, {armCenter, 0.95f, 0.70f}, {armLen, 0.08f, 0.04f}, 0xFFF8FAFC);
        int stripes = 6;
        for (int s = 0; s < stripes; ++s) {
            float sx = pedX + 0.30f + s * ((armLen - 0.50f) / (stripes - 1));
            PropMeshHelper::appendBox(payload, {sx, 0.95f, 0.70f}, {0.20f, 0.082f, 0.042f}, 0xFFDC2626);
        }

        if (hasCanopy) {
            PropMeshHelper::appendBox(payload, {kioskX, h * 0.5f, -1.8f}, {0.20f, h, 0.20f}, 0xFF334155);
            PropMeshHelper::appendBox(payload, {kioskX, h * 0.5f,  1.8f}, {0.20f, h, 0.20f}, 0xFF334155);
            PropMeshHelper::appendBox(payload, {w * 0.5f + 0.30f, h * 0.5f, -1.8f}, {0.20f, h, 0.20f}, 0xFF334155);
            PropMeshHelper::appendBox(payload, {w * 0.5f + 0.30f, h * 0.5f,  1.8f}, {0.20f, h, 0.20f}, 0xFF334155);

            PropMeshHelper::appendBox(payload, {0.0f, h + 0.15f, 0.0f}, {w + 2.2f, 0.25f, 4.4f}, 0xFF0F172A);
            PropMeshHelper::appendBox(payload, {0.0f, h - 0.18f, 0.0f}, {0.50f, 0.50f, 0.22f}, 0xFF1E293B);
            PropMeshHelper::appendBox(payload, {0.0f, h - 0.18f, 0.12f}, {0.38f, 0.38f, 0.02f}, 0xFF22C55E);
        }
    } else if (style == 1) { // Automated Smart Toll ETC
        float gantryH = h + 0.8f;
        PropMeshHelper::appendBox(payload, {-w * 0.5f - 0.40f, gantryH * 0.5f, 0.0f}, {0.28f, gantryH, 0.28f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, { w * 0.5f + 0.40f, gantryH * 0.5f, 0.0f}, {0.28f, gantryH, 0.28f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, {0.0f, gantryH, 0.0f}, {w + 1.2f, 0.45f, 0.70f}, 0xFF334155);

        PropMeshHelper::appendBox(payload, {-0.45f, gantryH - 0.32f, 0.0f}, {0.40f, 0.16f, 0.35f}, 0xFFE2E8F0);
        PropMeshHelper::appendBox(payload, { 0.45f, gantryH - 0.32f, 0.0f}, {0.40f, 0.16f, 0.35f}, 0xFFE2E8F0);
        PropMeshHelper::appendBox(payload, {-w * 0.5f - 0.15f, 0.90f, 0.0f}, {0.12f, 1.40f, 0.12f}, 0xFF0F172A);
        PropMeshHelper::appendBox(payload, { w * 0.5f + 0.15f, 0.90f, 0.0f}, {0.12f, 1.40f, 0.12f}, 0xFF0F172A);

        PropMeshHelper::appendBox(payload, {-w * 0.5f + 0.05f, 0.50f, 0.50f}, {0.30f, 1.0f, 0.30f}, 0xFF2563EB);
        PropMeshHelper::appendBox(payload, {0.0f, 0.95f, 0.50f}, {w - 0.20f, 0.07f, 0.04f}, 0xFFF8FAFC);

        if (hasCanopy) {
            PropMeshHelper::appendBox(payload, {0.0f, gantryH - 0.22f, 0.40f}, {0.45f, 0.45f, 0.15f}, 0xFF1E293B);
            PropMeshHelper::appendBox(payload, {0.0f, gantryH - 0.22f, 0.48f}, {0.32f, 0.32f, 0.02f}, 0xFF22C55E);
        }
    } else { // Parking Access Totem
        PropMeshHelper::appendBox(payload, {-w * 0.5f - 0.35f, 0.10f, 0.0f}, {0.65f, 0.20f, 2.4f}, 0xFF94A3B8);

        float totemX = -w * 0.5f - 0.35f;
        PropMeshHelper::appendBox(payload, {totemX, 0.70f, -0.30f}, {0.36f, 1.25f, 0.40f}, 0xFF1E293B);
        PropMeshHelper::appendBox(payload, {totemX + 0.17f, 0.95f, -0.30f}, {0.04f, 0.28f, 0.22f}, 0xFF38BDF8);
        PropMeshHelper::appendBox(payload, {totemX + 0.17f, 0.75f, -0.30f}, {0.04f, 0.04f, 0.16f}, 0xFFFACC15);

        PropMeshHelper::appendBox(payload, {totemX, 0.50f, 0.50f}, {0.32f, 0.95f, 0.32f}, 0xFFF59E0B);
        PropMeshHelper::appendBox(payload, {totemX, 1.02f, 0.50f}, {0.12f, 0.10f, 0.12f}, 0xFFEF4444);

        float armC = totemX + 0.15f + armLen * 0.5f;
        PropMeshHelper::appendBox(payload, {armC, 0.90f, 0.50f}, {armLen, 0.07f, 0.04f}, 0xFFF8FAFC);
        PropMeshHelper::appendBox(payload, {armC, 0.90f, 0.50f}, {armLen - 0.20f, 0.02f, 0.042f}, 0xFFEF4444);

        if (hasCanopy) {
            PropMeshHelper::appendBox(payload, {totemX, 1.65f, 0.10f}, {0.85f, 0.08f, 1.8f}, 0xFF0F172A);
        }
    }

    return payload;
}

} // namespace procedural::props
