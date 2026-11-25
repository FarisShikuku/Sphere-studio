#include "WaterTankGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

namespace {
inline float clampF(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

constexpr uint32_t kConcretePad  = 0xFF64748B;
constexpr uint32_t kSteelFrame   = 0xFF334155;
constexpr uint32_t kPipePVC      = 0xFFCBD5E1;
constexpr uint32_t kBrassValve   = 0xFFD97706;
constexpr uint32_t kLadderSteel  = 0xFF94A3B8;
} // namespace

WaterTankGen::WaterTankGen() = default;

bool WaterTankGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void WaterTankGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload WaterTankGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    const float H = clampF(params_.tankHeight, 1.0f, 5.5f);
    const float D = clampF(params_.tankDiameter, 0.8f, 4.0f);
    const float R = D * 0.5f;
    const float standH = clampF(params_.standHeight, 1.0f, 5.0f);
    const int style = std::max(0, std::min(3, params_.tankStyle));

    uint32_t bodyCol = 0xFF1A1D22; // 0: TopTank Matte Black
    uint32_t ribCol  = 0xFF2B3038;
    uint32_t capCol  = 0xFF111317;
    if (params_.colorScheme == 1) {
        bodyCol = 0xFF166534; // Forest Green Poly
        ribCol  = 0xFF14532D;
        capCol  = 0xFF0F3F22;
    } else if (params_.colorScheme == 2) {
        bodyCol = 0xFF1D4ED8; // Ocean Blue Poly
        ribCol  = 0xFF1E40AF;
        capCol  = 0xFF172554;
    } else if (params_.colorScheme == 3) {
        bodyCol = 0xFFD2B48C; // Desert Tan / Sandstone
        ribCol  = 0xFFB8976E;
        capCol  = 0xFF8C6D46;
    } else if (params_.colorScheme == 4 || style == 3) {
        bodyCol = (params_.colorScheme == 3) ? 0xFFD2B48C : 0xFF94A3B8; // Galvanized Steel (unless explicitly Tan)
        ribCol  = (params_.colorScheme == 3) ? 0xFFB8976E : 0xFF64748B;
        capCol  = (params_.colorScheme == 3) ? 0xFF8C6D46 : 0xFF475569;
    }

    auto appendVerticalRibbedTank = [&](float baseY, float bodyH, float radius, bool polyDome) {
        const float wallH = bodyH * (polyDome ? 0.78f : 0.86f);
        const float domeH = bodyH - wallH;

        // Main cylindrical shell
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, baseY, 0.0f}, radius, radius, wallH, 32, bodyCol);

        // Structural circumferential hoop ribs (corrugated rings)
        const int ribCount = std::max(3, static_cast<int>(wallH / 0.38f));
        for (int i = 1; i < ribCount; ++i) {
            float ry = baseY + wallH * (static_cast<float>(i) / ribCount);
            PropMeshHelper::appendCylinder(
                payload,
                Vec3{0.0f, ry - 0.032f, 0.0f},
                radius * 1.032f, radius * 1.032f, 0.064f, 32, ribCol
            );
        }

        // Bottom reinforced base skirt
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, baseY, 0.0f}, radius * 1.028f, radius * 1.028f, 0.09f, 32, ribCol);

        if (polyDome) {
            // Stepped domed shoulder + collar + threaded manhole lid
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, baseY + wallH, 0.0f}, radius, radius * 0.72f, domeH * 0.65f, 32, bodyCol);
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, baseY + wallH + domeH * 0.65f, 0.0f}, radius * 0.72f, radius * 0.42f, domeH * 0.25f, 28, ribCol);
            // Screw-on top inspection lid
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, baseY + bodyH - 0.05f, 0.0f}, radius * 0.34f, radius * 0.34f, 0.09f, 24, capCol);
            // Brand collar band around mid-belly
            PropMeshHelper::appendCylinder(
                payload,
                Vec3{0.0f, baseY + wallH * 0.46f, 0.0f},
                radius * 1.016f, radius * 1.016f, wallH * 0.12f, 32, ribCol
            );
        } else {
            // Corrugated farm cistern conical pitched roof + vent cap
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, baseY + wallH, 0.0f}, radius * 1.04f, 0.08f, domeH, 32, capCol);
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, baseY + wallH + domeH, 0.0f}, 0.09f, 0.09f, 0.12f, 16, kLadderSteel);
            // Offset hinged inspection hatch
            PropMeshHelper::appendBox(payload, Vec3{radius * 0.45f, baseY + wallH + domeH * 0.45f, 0.0f}, Vec3{0.32f, 0.08f, 0.32f}, ribCol);
        }

        // Plumbing fittings & optional access ladder
        if (params_.hasPlumbingAndLadder) {
            // Lower outlet pipe + brass gate valve
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{radius * 0.96f, baseY + 0.18f, 0.0f},
                Vec3{radius + 0.36f, baseY + 0.18f, 0.0f},
                0.032f, 0.032f, 12, kPipePVC
            );
            PropMeshHelper::appendCylinder(payload, Vec3{radius + 0.20f, baseY + 0.14f, 0.0f}, 0.048f, 0.048f, 0.12f, 12, kBrassValve);
            // Upper overflow / inlet line
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{0.0f, baseY + wallH - 0.10f, radius * 0.96f},
                Vec3{0.0f, baseY + wallH - 0.10f, radius + 0.24f},
                0.025f, 0.025f, 10, kPipePVC
            );
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{0.0f, baseY + wallH - 0.10f, radius + 0.22f},
                Vec3{0.0f, baseY + 0.08f, radius + 0.22f},
                0.025f, 0.025f, 10, kPipePVC
            );

            // Exterior vertical steel access ladder along -X side
            const float ladX = -(radius + 0.14f);
            const float ladTop = baseY + wallH + 0.15f;
            for (float zOff : {-0.18f, 0.18f}) {
                PropMeshHelper::appendOrientedCylinder(
                    payload,
                    Vec3{ladX, 0.0f, zOff},
                    Vec3{ladX, ladTop, zOff},
                    0.018f, 0.018f, 8, kLadderSteel
                );
            }
            const int rungs = std::max(4, static_cast<int>(ladTop / 0.32f));
            for (int r = 1; r <= rungs; ++r) {
                float ry = (static_cast<float>(r) / (rungs + 1)) * ladTop;
                PropMeshHelper::appendOrientedCylinder(
                    payload,
                    Vec3{ladX, ry, -0.18f},
                    Vec3{ladX, ry, +0.18f},
                    0.013f, 0.013f, 8, kLadderSteel
                );
            }
        }
    };

    if (style == 0) {
        // ----------------- STYLE 0: RIBBED POLY TOPTANK (ON CONCRETE PAD) -----------------
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, R * 1.18f, R * 1.16f, 0.14f, 32, kConcretePad);
        appendVerticalRibbedTank(0.14f, H, R, true);
    } else if (style == 1) {
        // ----------------- STYLE 1: ELEVATED STEEL-STAND WATER TANK -----------------
        const float frameSpan = R * 1.55f;
        const float footPad = 0.28f;
        const float legTopY = standH;

        // Four corner foundation footings + angled structural legs
        const Vec2 corners[4] = {
            {-frameSpan * 0.5f, -frameSpan * 0.5f},
            { frameSpan * 0.5f, -frameSpan * 0.5f},
            { frameSpan * 0.5f,  frameSpan * 0.5f},
            {-frameSpan * 0.5f,  frameSpan * 0.5f}
        };
        const float topScale = 0.86f;
        for (int i = 0; i < 4; ++i) {
            PropMeshHelper::appendBox(payload, Vec3{corners[i].x, 0.08f, corners[i].y}, Vec3{footPad, 0.16f, footPad}, kConcretePad);
            Vec3 pBot{corners[i].x, 0.16f, corners[i].y};
            Vec3 pTop{corners[i].x * topScale, legTopY, corners[i].y * topScale};
            PropMeshHelper::appendOrientedCylinder(payload, pBot, pTop, 0.055f, 0.050f, 12, kSteelFrame);
        }

        // Horizontal perimeter girts & X cross-bracing
        const int tiers = std::max(1, static_cast<int>(standH / 1.4f));
        for (int t = 0; t < tiers; ++t) {
            float f0 = static_cast<float>(t) / tiers;
            float f1 = static_cast<float>(t + 1) / tiers;
            float y0 = 0.16f + (legTopY - 0.16f) * f0;
            float y1 = 0.16f + (legTopY - 0.16f) * f1;
            float s0 = 1.0f - f0 * (1.0f - topScale);
            float s1 = 1.0f - f1 * (1.0f - topScale);

            for (int e = 0; e < 4; ++e) {
                int nextE = (e + 1) % 4;
                Vec3 a0{corners[e].x * s0, y0, corners[e].y * s0};
                Vec3 b0{corners[nextE].x * s0, y0, corners[nextE].y * s0};
                Vec3 a1{corners[e].x * s1, y1, corners[e].y * s1};
                Vec3 b1{corners[nextE].x * s1, y1, corners[nextE].y * s1};

                PropMeshHelper::appendStrut(payload, a1, b1, 0.055f, kSteelFrame);
                PropMeshHelper::appendStrut(payload, a0, b1, 0.028f, kSteelFrame);
                PropMeshHelper::appendStrut(payload, b0, a1, 0.028f, kSteelFrame);
            }
        }

        // Steel I-beam platform deck + tank
        const float deckSize = R * 2.15f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, legTopY + 0.06f, 0.0f}, Vec3{deckSize, 0.12f, deckSize}, kSteelFrame);
        appendVerticalRibbedTank(legTopY + 0.12f, H, R, true);
    } else if (style == 2) {
        // ----------------- STYLE 2: HORIZONTAL SADDLE STORAGE CISTERN -----------------
        const float cylLen = std::max(1.5f, H);
        const float axisY = R + 0.36f;

        // Concrete ground slab + twin curved saddle cradles
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.06f, 0.0f}, Vec3{cylLen + 0.45f, 0.12f, D + 0.45f}, kConcretePad);
        for (float sx : {-cylLen * 0.28f, cylLen * 0.28f}) {
            PropMeshHelper::appendBox(payload, Vec3{sx, 0.28f, 0.0f}, Vec3{0.22f, 0.34f, D * 0.82f}, kConcretePad);
        }

        // Horizontal main tank cylinder along X axis
        PropMeshHelper::appendOrientedCylinder(
            payload,
            Vec3{-cylLen * 0.5f, axisY, 0.0f},
            Vec3{ cylLen * 0.5f, axisY, 0.0f},
            R, R, 32, bodyCol, true, true
        );

        // End dome caps + circumferential steel straps
        for (float endX : {-cylLen * 0.5f, cylLen * 0.5f}) {
            PropMeshHelper::appendSphere(payload, Vec3{endX, axisY, 0.0f}, R * 0.96f, 16, 24, bodyCol);
        }
        for (float rx : {-cylLen * 0.28f, 0.0f, cylLen * 0.28f}) {
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{rx - 0.04f, axisY, 0.0f},
                Vec3{rx + 0.04f, axisY, 0.0f},
                R * 1.028f, R * 1.028f, 32, ribCol
            );
        }

        // Top manhole dome & lid
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, axisY + R * 0.85f, 0.0f}, R * 0.32f, R * 0.30f, 0.22f, 20, ribCol);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, axisY + R * 0.85f + 0.22f, 0.0f}, R * 0.34f, R * 0.34f, 0.05f, 20, capCol);

        if (params_.hasPlumbingAndLadder) {
            PropMeshHelper::appendOrientedCylinder(
                payload,
                Vec3{cylLen * 0.5f, axisY - R * 0.45f, 0.0f},
                Vec3{cylLen * 0.5f + R + 0.25f, axisY - R * 0.45f, 0.0f},
                0.035f, 0.035f, 12, kPipePVC
            );
            PropMeshHelper::appendCylinder(
                payload,
                Vec3{cylLen * 0.5f + R + 0.14f, axisY - R * 0.45f - 0.04f, 0.0f},
                0.055f, 0.055f, 0.12f, 12, kBrassValve
            );
        }
    } else {
        // ----------------- STYLE 3: GALVANIZED CORRUGATED FARM CISTERN -----------------
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, R * 1.15f, R * 1.15f, 0.12f, 32, kConcretePad);
        appendVerticalRibbedTank(0.12f, H, R, false);
    }

    return payload;
}

} // namespace procedural::props
