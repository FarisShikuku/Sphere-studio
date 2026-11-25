#include "ModernGatesGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

namespace {

ModernGateParams mergeGateParams(const ModernGateParams& base, const GeneratorContext& ctx) {
    ModernGateParams p = base;
    if (ctx.customParam0 > 0.0f) p.width = ctx.customParam0;
    if (ctx.customParam1 > 0.0f) p.height = ctx.customParam1;
    if (ctx.customParam2 > 0.0f) p.postDepth = ctx.customParam2;
    if (ctx.customParam3 >= 0) p.style = ctx.customParam3;
    if (ctx.customParam4 >= 0) p.colorScheme = ctx.customParam4;
    if (ctx.customParam5 >= 0.0f) p.hasSmartHardware = (ctx.customParam5 > 0.5f);

    p.width = std::clamp(p.width, 1.4f, 10.0f);
    p.height = std::clamp(p.height, 1.1f, 3.8f);
    p.postDepth = std::clamp(p.postDepth, 0.15f, 1.0f);
    return p;
}

uint32_t resolveGateFrameColor(int colorScheme) {
    switch (colorScheme) {
        case 1: return 0xFFCBD5E1; // Anodized Silver
        case 2: return 0xFF78350F; // Architectural Bronze
        case 3: return 0xFFF8FAFC; // Matte White
        case 4: return 0xFF9A3412; // Weathering Corten
        default: return 0xFF111827; // Obsidian Charcoal
    }
}

uint32_t resolveGateWoodColor(int colorScheme) {
    switch (colorScheme) {
        case 1: return 0xFF78350F; // Dark Walnut
        case 2: return 0xFFD2B48C; // Blonde Oak
        case 3: return 0xFF334155; // Shou Sugi Ban Slate
        default: return 0xFFB45309; // Western Cedar
    }
}

} // namespace

#define IMPL_MODERN_GATE_BOILERPLATE(ClassName)                                     \
    ClassName::ClassName() = default;                                               \
    bool ClassName::initialize(const GeneratorContext& /*ctx*/) { return true; }    \
    void ClassName::update(float /*timeSeconds*/, float /*dt*/) {}                  \
    ModernGateParams ClassName::resolveParams(const GeneratorContext& ctx) const {  \
        return mergeGateParams(params_, ctx);                                       \
    }

// ============================================================================
// 1. Modern Cantilevered Cedar & Matte Black Steel Sliding Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(ModernSlatGateGen)

GeneratedMeshPayload ModernSlatGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 3.0f, 8.5f);
    float H = std::clamp(p.height, 1.5f, 3.2f);
    uint32_t frameCol = resolveGateFrameColor(p.colorScheme);
    uint32_t slatCol = resolveGateWoodColor(p.colorScheme);
    uint32_t pillarCol = 0xFF334155;

    // Monolithic asymmetric address pier (left) and catch post (right)
    float pierW = 0.68f;
    float leftX = -W * 0.5f - pierW * 0.55f;
    float rightX = W * 0.5f + 0.14f;

    PropMeshHelper::appendBox(payload, {leftX, H * 0.55f, 0.0f}, {pierW, H * 1.10f, p.postDepth}, pillarCol);
    PropMeshHelper::appendBox(payload, {leftX, H * 1.12f, 0.0f}, {pierW + 0.08f, 0.06f, p.postDepth + 0.08f}, frameCol);
    PropMeshHelper::appendBox(payload, {rightX, H * 0.52f, 0.0f}, {0.16f, H * 1.04f, 0.18f}, frameCol);

    // Floating sliding leaf with offset steel border & alternating timber slats
    float leafW = W + 0.15f;
    PropMeshHelper::appendBox(payload, {0.0f, 0.10f, 0.0f}, {leafW, 0.09f, 0.07f}, frameCol);
    PropMeshHelper::appendBox(payload, {0.0f, H, 0.0f}, {leafW, 0.09f, 0.07f}, frameCol);
    PropMeshHelper::appendBox(payload, {-leafW * 0.5f + 0.04f, H * 0.5f + 0.05f, 0.0f}, {0.09f, H - 0.05f, 0.07f}, frameCol);
    PropMeshHelper::appendBox(payload, { leafW * 0.5f - 0.04f, H * 0.5f + 0.05f, 0.0f}, {0.09f, H - 0.05f, 0.07f}, frameCol);

    // Track beam extension & drive housing
    PropMeshHelper::appendBox(payload, {W * 0.18f, 0.03f, -0.05f}, {W * 1.45f, 0.04f, 0.08f}, 0xFF64748B);
    int slats = std::max(6, static_cast<int>((H - 0.24f) / 0.14f));
    for (int s = 0; s < slats; ++s) {
        float sy = 0.20f + s * ((H - 0.28f) / std::max(1, slats - 1));
        float thickH = (p.style == 1 && (s % 2 == 0)) ? 0.12f : 0.085f;
        PropMeshHelper::appendBox(payload, {0.0f, sy, 0.0f}, {leafW - 0.14f, thickH, 0.035f}, slatCol);
    }

    if (p.hasSmartHardware) {
        // Vertical glowing address backlight bar & video intercom panel
        PropMeshHelper::appendBox(payload, {leftX + 0.12f, H * 0.65f, p.postDepth * 0.5f + 0.015f}, {0.10f, 0.55f, 0.02f}, 0xFFFDE047);
        PropMeshHelper::appendBox(payload, {leftX - 0.14f, 1.35f, p.postDepth * 0.5f + 0.02f}, {0.16f, 0.30f, 0.04f}, 0xFF0F172A);
        PropMeshHelper::appendBox(payload, {leftX - 0.14f, 1.42f, p.postDepth * 0.5f + 0.045f}, {0.08f, 0.08f, 0.01f}, 0xFF38BDF8);
        // Brushless motor housing
        PropMeshHelper::appendBox(payload, {rightX + 0.22f, 0.26f, -0.14f}, {0.36f, 0.42f, 0.24f}, 0xFF1E293B);
    }

    return payload;
}

// ============================================================================
// 2. Frosted Architectural Glass & Steel Pivot Entry Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(GlassPivotGateGen)

GeneratedMeshPayload GlassPivotGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 1.5f, 4.0f);
    float H = std::clamp(p.height, 2.0f, 3.6f);
    uint32_t frameCol = resolveGateFrameColor(p.colorScheme);
    uint32_t glassCol = (p.style == 1) ? 0xFF475569 : 0xFFBAE6FD; // Smoke glass vs Frosted sky glass

    // Monumental portal frame
    float portalH = H + 0.35f;
    PropMeshHelper::appendBox(payload, {-W * 0.5f - 0.12f, portalH * 0.5f, 0.0f}, {0.22f, portalH, p.postDepth}, frameCol);
    PropMeshHelper::appendBox(payload, { W * 0.5f + 0.12f, portalH * 0.5f, 0.0f}, {0.22f, portalH, p.postDepth}, frameCol);
    PropMeshHelper::appendBox(payload, {0.0f, portalH - 0.12f, 0.0f}, {W + 0.46f, 0.24f, p.postDepth + 0.06f}, frameCol);

    // Slightly pivoted open leaf (~14 deg rotated pivot effect)
    float leafW = W - 0.08f;
    float angle = (p.style == 2) ? 0.0f : 0.24f;
    PropMeshHelper::appendBox(payload, {0.0f, H * 0.5f + 0.04f, 0.0f}, {leafW, H - 0.06f, 0.045f}, glassCol, angle);
    // Top & bottom rail shoes on pivot door
    PropMeshHelper::appendBox(payload, {0.0f, 0.08f, 0.0f}, {leafW, 0.10f, 0.07f}, frameCol, angle);
    PropMeshHelper::appendBox(payload, {0.0f, H, 0.0f}, {leafW, 0.10f, 0.07f}, frameCol, angle);
    // Decorative steel transom mullions
    PropMeshHelper::appendBox(payload, {0.0f, H * 0.35f, 0.0f}, {leafW, 0.03f, 0.065f}, frameCol, angle);
    PropMeshHelper::appendBox(payload, {0.0f, H * 0.68f, 0.0f}, {leafW, 0.03f, 0.065f}, frameCol, angle);

    if (p.hasSmartHardware) {
        // Oversize brushed stainless architectural pull bar
        float handleX = leafW * 0.30f;
        PropMeshHelper::appendCylinder(payload, {handleX, H * 0.25f, 0.08f}, 0.024f, 0.024f, H * 0.55f, 10, 0xFFE2E8F0);
        PropMeshHelper::appendBox(payload, {handleX, H * 0.30f, 0.04f}, {0.03f, 0.03f, 0.08f}, 0xFFE2E8F0);
        PropMeshHelper::appendBox(payload, {handleX, H * 0.75f, 0.04f}, {0.03f, 0.03f, 0.08f}, 0xFFE2E8F0);
        // Header LED downlight strip
        PropMeshHelper::appendBox(payload, {0.0f, portalH - 0.25f, 0.0f}, {W * 0.8f, 0.02f, 0.05f}, 0xFFFDE047);
    }

    return payload;
}

// ============================================================================
// 3. Laser-Cut Corten Architectural Starburst Portal Double Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(LaserArtGateGen)

GeneratedMeshPayload LaserArtGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 3.0f, 8.0f);
    float H = std::clamp(p.height, 1.6f, 3.2f);
    uint32_t pierCol = 0xFF475569;
    uint32_t artCol = (p.colorScheme == 1) ? 0xFF1E293B : (p.colorScheme == 2 ? 0xFFCBD5E1 : 0xFFB45309);
    uint32_t rimCol = 0xFF0F172A;

    for (float side : {-1.0f, 1.0f}) {
        float px = side * (W * 0.5f + 0.32f);
        PropMeshHelper::appendBox(payload, {px, H * 0.55f, 0.0f}, {0.55f, H * 1.10f, 0.55f}, pierCol);
        PropMeshHelper::appendBox(payload, {px, H * 1.12f, 0.0f}, {0.62f, 0.07f, 0.62f}, rimCol);
        if (p.hasSmartHardware) {
            PropMeshHelper::appendBox(payload, {px, H * 0.65f, 0.285f}, {0.14f, 0.24f, 0.06f}, 0xFF1E293B);
            PropMeshHelper::appendBox(payload, {px, H * 0.65f, 0.305f}, {0.10f, 0.18f, 0.02f}, 0xFFFACC15);
        }
    }

    float leafW = (W - 0.10f) * 0.5f;
    for (float side : {-1.0f, 1.0f}) {
        float cx = side * (leafW * 0.5f + 0.02f);
        // Perimeter frame
        PropMeshHelper::appendBox(payload, {cx, 0.12f, 0.0f}, {leafW, 0.08f, 0.06f}, rimCol);
        PropMeshHelper::appendBox(payload, {cx, H, 0.0f}, {leafW, 0.08f, 0.06f}, rimCol);
        PropMeshHelper::appendBox(payload, {side * (W * 0.5f - 0.04f), H * 0.5f + 0.06f, 0.0f}, {0.08f, H - 0.04f, 0.06f}, rimCol);
        PropMeshHelper::appendBox(payload, {side * 0.04f, H * 0.5f + 0.06f, 0.0f}, {0.08f, H - 0.04f, 0.06f}, rimCol);

        // Radiating CNC laser-cut starburst ribs from bottom inner pivot
        Vec3 hub{side * 0.12f, H * 0.52f, 0.0f};
        int rays = 7;
        for (int r = 0; r < rays; ++r) {
            float t = static_cast<float>(r) / (rays - 1);
            float tx = side * (leafW - 0.10f);
            float ty = 0.20f + t * (H - 0.30f);
            PropMeshHelper::appendStrut(payload, hub, {tx, ty, 0.0f}, 0.032f, artCol);
        }
        PropMeshHelper::appendStrut(payload, {cx - leafW * 0.42f, 0.20f, 0.0f}, {cx + leafW * 0.42f, H - 0.10f, 0.0f}, 0.035f, artCol);
        PropMeshHelper::appendStrut(payload, {cx - leafW * 0.42f, H - 0.10f, 0.0f}, {cx + leafW * 0.42f, 0.20f, 0.0f}, 0.035f, artCol);
    }

    return payload;
}

// ============================================================================
// 4. High-Speed Trackless Bi-Fold Folding Security Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(BifoldSpeedGateGen)

GeneratedMeshPayload BifoldSpeedGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 3.2f, 9.0f);
    float H = std::clamp(p.height, 1.8f, 3.2f);
    uint32_t frameCol = (p.colorScheme == 1) ? 0xFFFACC15 : resolveGateFrameColor(p.colorScheme);
    uint32_t meshCol = 0xFF64748B;

    // Heavy structural drive towers
    for (float side : {-1.0f, 1.0f}) {
        float px = side * (W * 0.5f + 0.18f);
        PropMeshHelper::appendBox(payload, {px, H * 0.58f, 0.0f}, {0.28f, H * 1.16f, 0.28f}, frameCol);
        if (p.hasSmartHardware) {
            PropMeshHelper::appendCylinder(payload, {px, H * 1.16f, 0.0f}, 0.06f, 0.05f, 0.14f, 10, 0xFFF97316);
            // Yellow crash protection bollards
            PropMeshHelper::appendCylinder(payload, {px, 0.0f, 0.48f}, 0.065f, 0.065f, 1.0f, 10, 0xFFFACC15);
            PropMeshHelper::appendCylinder(payload, {px, 0.0f, -0.48f}, 0.065f, 0.065f, 1.0f, 10, 0xFFFACC15);
        }
    }

    // Dual bi-fold leaves angled in crisp folded relief
    float subLeafW = W * 0.24f;
    float foldAngle = (p.style == 1) ? 0.55f : 0.28f;
    for (float side : {-1.0f, 1.0f}) {
        float hingeX = side * (W * 0.5f);
        float c1X = hingeX - side * (subLeafW * 0.5f * std::cos(foldAngle));
        float c1Z = subLeafW * 0.5f * std::sin(foldAngle);
        PropMeshHelper::appendBox(payload, {c1X, H * 0.5f + 0.05f, c1Z}, {subLeafW, H - 0.12f, 0.06f}, frameCol, side * foldAngle);

        float midX = hingeX - side * (subLeafW * std::cos(foldAngle));
        float midZ = subLeafW * std::sin(foldAngle);
        float c2X = midX - side * (subLeafW * 0.5f * std::cos(foldAngle));
        float c2Z = midZ * 0.5f;
        PropMeshHelper::appendBox(payload, {c2X, H * 0.5f + 0.05f, c2Z}, {subLeafW, H - 0.12f, 0.06f}, meshCol, -side * foldAngle);

        // Overhead articulated folding arm linkage
        PropMeshHelper::appendStrut(payload, {hingeX, H + 0.12f, 0.0f}, {midX, H + 0.12f, midZ}, 0.045f, 0xFFCBD5E1);
    }

    return payload;
}

// ============================================================================
// 5. Modern Louvered Aluminum Privacy Driveway Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(AluminumLouverGateGen)

GeneratedMeshPayload AluminumLouverGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 3.0f, 7.5f);
    float H = std::clamp(p.height, 1.5f, 3.0f);
    uint32_t frameCol = 0xFF1E293B;
    uint32_t louverCol = resolveGateFrameColor(p.colorScheme);
    if (p.colorScheme == 0) louverCol = 0xFF475569;

    for (float side : {-1.0f, 1.0f}) {
        float px = side * (W * 0.5f + 0.24f);
        PropMeshHelper::appendBox(payload, {px, H * 0.52f, 0.0f}, {0.40f, H * 1.04f, p.postDepth}, 0xFF64748B);
        PropMeshHelper::appendBox(payload, {px, H * 1.06f, 0.0f}, {0.46f, 0.05f, p.postDepth + 0.06f}, frameCol);
    }

    float leafW = (W - 0.08f) * 0.5f;
    for (float side : {-1.0f, 1.0f}) {
        float cx = side * (leafW * 0.5f + 0.02f);
        PropMeshHelper::appendBox(payload, {cx, 0.10f, 0.0f}, {leafW, 0.08f, 0.08f}, frameCol);
        PropMeshHelper::appendBox(payload, {cx, H, 0.0f}, {leafW, 0.08f, 0.08f}, frameCol);
        PropMeshHelper::appendBox(payload, {side * (W * 0.5f - 0.03f), H * 0.52f, 0.0f}, {0.07f, H - 0.06f, 0.08f}, frameCol);
        PropMeshHelper::appendBox(payload, {side * 0.03f, H * 0.52f, 0.0f}, {0.07f, H - 0.06f, 0.08f}, frameCol);

        int nBlades = std::max(6, static_cast<int>((H - 0.22f) / 0.11f));
        for (int b = 0; b < nBlades; ++b) {
            float by = 0.18f + b * ((H - 0.26f) / std::max(1, nBlades - 1));
            PropMeshHelper::appendBox(payload, {cx, by, 0.0f}, {leafW - 0.10f, 0.024f, 0.09f}, louverCol);
            PropMeshHelper::appendBox(payload, {cx, by + 0.015f, 0.025f}, {leafW - 0.10f, 0.015f, 0.04f}, louverCol);
        }
    }

    if (p.hasSmartHardware) {
        // Parcel locker slot & glowing intercom on right pillar
        float px = W * 0.5f + 0.24f;
        PropMeshHelper::appendBox(payload, {px, 0.85f, p.postDepth * 0.5f + 0.01f}, {0.26f, 0.18f, 0.03f}, 0xFF0F172A);
        PropMeshHelper::appendBox(payload, {px, 1.38f, p.postDepth * 0.5f + 0.01f}, {0.12f, 0.22f, 0.03f}, 0xFF38BDF8);
    }

    return payload;
}

// ============================================================================
// 6. Brutalist Concrete & Steel Fin Overhead Portal Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(BrutalistPortalGateGen)

GeneratedMeshPayload BrutalistPortalGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 3.0f, 7.5f);
    float H = std::clamp(p.height, 2.2f, 3.8f);
    uint32_t concCol = (p.colorScheme == 1) ? 0xFFD2B48C : 0xFF64748B;
    uint32_t darkCol = 0xFF18181B;

    float portalH = H + 0.45f;
    // Dramatic board-formed concrete pylons & cantilevered canopy hood
    PropMeshHelper::appendBox(payload, {-W * 0.5f - 0.35f, portalH * 0.5f, 0.0f}, {0.65f, portalH, 0.85f}, concCol);
    PropMeshHelper::appendBox(payload, { W * 0.5f + 0.35f, portalH * 0.5f, 0.0f}, {0.65f, portalH, 0.85f}, concCol);
    PropMeshHelper::appendBox(payload, {0.0f, portalH + 0.18f, 0.12f}, {W + 1.8f, 0.36f, 1.35f}, concCol);
    PropMeshHelper::appendBox(payload, {0.0f, portalH - 0.01f, 0.12f}, {W + 1.72f, 0.05f, 1.25f}, darkCol);

    // Architectural blackened vertical fin leaf
    int fins = std::max(10, static_cast<int>(W / 0.14f));
    float leafH = H - 0.10f;
    PropMeshHelper::appendBox(payload, {0.0f, 0.12f, 0.0f}, {W, 0.08f, 0.08f}, darkCol);
    PropMeshHelper::appendBox(payload, {0.0f, leafH, 0.0f}, {W, 0.08f, 0.08f}, darkCol);
    for (int f = 0; f <= fins; ++f) {
        float fx = -W * 0.5f + 0.06f + (static_cast<float>(f) / fins) * (W - 0.12f);
        PropMeshHelper::appendBox(payload, {fx, 0.10f + leafH * 0.5f, 0.0f}, {0.03f, leafH, 0.10f}, darkCol);
    }

    if (p.hasSmartHardware) {
        // Recessed linear soffit LED strip
        PropMeshHelper::appendBox(payload, {0.0f, portalH - 0.03f, 0.35f}, {W + 0.6f, 0.025f, 0.05f}, 0xFFFDE047);
    }

    return payload;
}

// ============================================================================
// 7. Optical Glass Lobby Speedstile Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(GlassSpeedGateGen)

GeneratedMeshPayload GlassSpeedGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 1.4f, 4.5f);
    float H = std::clamp(p.height, 1.0f, 1.9f);
    uint32_t bodyCol = (p.colorScheme == 1) ? 0xFF1E293B : 0xFFCBD5E1;
    uint32_t glassCol = 0xFF38BDF8;

    int lanes = (p.style == 1 && W >= 2.2f) ? 2 : 1;
    float laneW = W / lanes;
    float pedLen = 1.35f;

    for (int pIdx = 0; pIdx <= lanes; ++pIdx) {
        float px = -W * 0.5f + pIdx * laneW;
        // Streamlined stainless housing pedestal
        PropMeshHelper::appendBox(payload, {px, 0.52f, 0.0f}, {0.18f, 0.96f, pedLen}, bodyCol);
        PropMeshHelper::appendBox(payload, {px, 1.02f, 0.0f}, {0.20f, 0.04f, pedLen + 0.04f}, 0xFF0F172A);
        if (p.hasSmartHardware) {
            // Top RFID reader tap glow + status LED
            PropMeshHelper::appendBox(payload, {px, 1.05f, pedLen * 0.36f}, {0.11f, 0.015f, 0.14f}, 0xFF22C55E);
            PropMeshHelper::appendBox(payload, {px, 0.85f, pedLen * 0.505f}, {0.12f, 0.05f, 0.02f}, 0xFF38BDF8);
        }
    }

    // Twin tempered glass swing paddles inside each lane
    float paddleH = std::max(0.85f, H * 0.82f);
    for (int l = 0; l < lanes; ++l) {
        float lx0 = -W * 0.5f + l * laneW + 0.10f;
        float lx1 = -W * 0.5f + (l + 1) * laneW - 0.10f;
        float pW = (lx1 - lx0) * 0.48f;
        PropMeshHelper::appendBox(payload, {lx0 + pW * 0.5f, 0.25f + paddleH * 0.5f, 0.0f}, {pW, paddleH, 0.02f}, glassCol);
        PropMeshHelper::appendBox(payload, {lx1 - pW * 0.5f, 0.25f + paddleH * 0.5f, 0.0f}, {pW, paddleH, 0.02f}, glassCol);
    }

    return payload;
}

// ============================================================================
// 8. Architectural Japanese Sukiya / Shou Sugi Ban Courtyard Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(JapaneseToriGateGen)

GeneratedMeshPayload JapaneseToriGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 1.6f, 3.8f);
    float H = std::clamp(p.height, 2.2f, 3.4f);
    uint32_t woodCol = resolveGateWoodColor(p.colorScheme);
    uint32_t roofCol = 0xFF1E293B;

    // Stepped basalt threshold
    PropMeshHelper::appendBox(payload, {0.0f, 0.05f, 0.0f}, {W + 0.6f, 0.10f, 0.85f}, 0xFF475569);

    // Timber posts and cross lintels
    PropMeshHelper::appendBox(payload, {-W * 0.5f, H * 0.5f, 0.0f}, {0.14f, H, 0.14f}, woodCol);
    PropMeshHelper::appendBox(payload, { W * 0.5f, H * 0.5f, 0.0f}, {0.14f, H, 0.14f}, woodCol);
    PropMeshHelper::appendBox(payload, {0.0f, H * 0.78f, 0.0f}, {W + 0.35f, 0.10f, 0.16f}, woodCol);
    PropMeshHelper::appendBox(payload, {0.0f, H + 0.06f, 0.0f}, {W + 0.70f, 0.12f, 0.20f}, woodCol);

    // Sloped standing-seam modern gable canopy roof
    float roofPeak = H + 0.48f;
    PropMeshHelper::appendBox(payload, {0.0f, H + 0.24f, -0.28f}, {W + 0.90f, 0.06f, 0.75f}, roofCol);
    PropMeshHelper::appendBox(payload, {0.0f, H + 0.24f,  0.28f}, {W + 0.90f, 0.06f, 0.75f}, roofCol);
    PropMeshHelper::appendBox(payload, {0.0f, roofPeak - 0.08f, 0.0f}, {W + 0.95f, 0.10f, 0.20f}, roofCol);

    // Kumiko vertical timber gate door leaves
    float leafW = (W - 0.16f) * 0.5f;
    float doorH = H * 0.72f;
    for (float side : {-1.0f, 1.0f}) {
        float cx = side * (leafW * 0.5f + 0.02f);
        PropMeshHelper::appendBox(payload, {cx, 0.14f, 0.0f}, {leafW, 0.08f, 0.05f}, woodCol);
        PropMeshHelper::appendBox(payload, {cx, doorH, 0.0f}, {leafW, 0.08f, 0.05f}, woodCol);
        int bars = 5;
        for (int b = 0; b < bars; ++b) {
            float bx = cx - leafW * 0.40f + b * (leafW * 0.80f / (bars - 1));
            PropMeshHelper::appendBox(payload, {bx, 0.14f + doorH * 0.48f, 0.0f}, {0.03f, doorH - 0.12f, 0.03f}, woodCol);
        }
    }

    if (p.hasSmartHardware) {
        // Hanging warm bronze courtyard lantern
        PropMeshHelper::appendBox(payload, {W * 0.5f + 0.18f, H * 0.68f, 0.18f}, {0.16f, 0.22f, 0.16f}, 0xFF78350F);
        PropMeshHelper::appendBox(payload, {W * 0.5f + 0.18f, H * 0.68f, 0.18f}, {0.11f, 0.16f, 0.11f}, 0xFFFDE047);
    }

    return payload;
}

// ============================================================================
// 9. Floating Gabion Rock Pier & Horizontal Oak Sliding Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(GabionTimberGateGen)

GeneratedMeshPayload GabionTimberGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 3.0f, 8.0f);
    float H = std::clamp(p.height, 1.4f, 2.8f);
    uint32_t woodCol = resolveGateWoodColor(p.colorScheme);
    uint32_t steelCol = 0xFF334155;
    uint32_t stoneCols[3] = {0xFF475569, 0xFF64748B, 0xFF94A3B8};

    // Twin rock-filled gabion piers
    float pierW = 0.52f;
    for (float side : {-1.0f, 1.0f}) {
        float px = side * (W * 0.5f + pierW * 0.6f);
        int courses = 5;
        float rowH = H / courses;
        for (int r = 0; r < courses; ++r) {
            PropMeshHelper::appendBox(payload, {px, (r + 0.5f) * rowH, 0.0f}, {pierW * 0.90f, rowH * 0.88f, 0.42f}, stoneCols[(r + (side > 0 ? 1 : 0)) % 3]);
        }
        // Gabion cage frame bars
        PropMeshHelper::appendBox(payload, {px, H * 0.5f, 0.23f}, {pierW, H, 0.02f}, 0xFFCBD5E1);
        PropMeshHelper::appendBox(payload, {px, H + 0.03f, 0.0f}, {pierW + 0.06f, 0.05f, 0.48f}, steelCol);
    }

    // Steel truss & horizontal oak board gate span
    PropMeshHelper::appendBox(payload, {0.0f, 0.14f, 0.0f}, {W + 0.10f, 0.08f, 0.07f}, steelCol);
    PropMeshHelper::appendBox(payload, {0.0f, H * 0.92f, 0.0f}, {W + 0.10f, 0.07f, 0.07f}, steelCol);
    int boards = 6;
    for (int b = 0; b < boards; ++b) {
        float by = 0.26f + b * ((H * 0.86f - 0.26f) / (boards - 1));
        PropMeshHelper::appendBox(payload, {0.0f, by, 0.0f}, {W, 0.12f, 0.04f}, woodCol);
    }

    if (p.hasSmartHardware) {
        PropMeshHelper::appendBox(payload, {W * 0.5f + 0.08f, 0.30f, -0.18f}, {0.32f, 0.38f, 0.24f}, 0xFF1E293B);
        PropMeshHelper::appendBox(payload, {-W * 0.5f, 0.55f, 0.12f}, {0.10f, 0.14f, 0.08f}, 0xFFEF4444); // Safety photocell
    }

    return payload;
}

// ============================================================================
// 10. Solar-Powered Autonomous Smart Tubular Gate
// ============================================================================
IMPL_MODERN_GATE_BOILERPLATE(SolarSmartGateGen)

GeneratedMeshPayload SolarSmartGateGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernGateParams p = resolveParams(ctx);
    float W = std::clamp(p.width, 3.0f, 7.5f);
    float H = std::clamp(p.height, 1.3f, 2.4f);
    uint32_t gateCol = resolveGateFrameColor(p.colorScheme);
    if (p.colorScheme == 0) gateCol = 0xFF1E293B;

    // Hinge & latch posts
    for (float side : {-1.0f, 1.0f}) {
        float px = side * (W * 0.5f + 0.10f);
        PropMeshHelper::appendCylinder(payload, {px, 0.0f, 0.0f}, 0.09f, 0.09f, H * 1.15f, 12, gateCol);
    }

    // Modern truss double-swing tubular aluminum leaves
    float leafW = (W - 0.12f) * 0.5f;
    for (float side : {-1.0f, 1.0f}) {
        float cx = side * (leafW * 0.5f + 0.03f);
        PropMeshHelper::appendBox(payload, {cx, 0.15f, 0.0f}, {leafW, 0.06f, 0.05f}, gateCol);
        PropMeshHelper::appendBox(payload, {cx, H * 0.95f, 0.0f}, {leafW, 0.06f, 0.05f}, gateCol);
        PropMeshHelper::appendBox(payload, {side * (W * 0.5f - 0.03f), H * 0.55f, 0.0f}, {0.06f, H * 0.88f, 0.06f}, gateCol);
        PropMeshHelper::appendBox(payload, {side * 0.04f, H * 0.55f, 0.0f}, {0.06f, H * 0.88f, 0.06f}, gateCol);

        int pick = std::max(5, static_cast<int>(leafW / 0.18f));
        for (int i = 0; i < pick; ++i) {
            float x = cx - leafW * 0.42f + i * (leafW * 0.84f / (pick - 1));
            PropMeshHelper::appendBox(payload, {x, H * 0.55f, 0.0f}, {0.025f, H * 0.82f, 0.025f}, gateCol);
        }

        if (p.hasSmartHardware) {
            // Dual telescoping linear actuator motor rams
            PropMeshHelper::appendOrientedCylinder(payload, {side * (W * 0.5f + 0.08f), 0.45f, -0.14f}, {side * (W * 0.30f), 0.45f, -0.04f}, 0.038f, 0.024f, 10, 0xFF94A3B8);
        }
    }

    // Off-grid Solar PV mast + tilted panel + NEMA battery control box + GSM antenna
    if (p.hasSmartHardware) {
        float mastX = -W * 0.5f - 0.10f;
        float mastTop = H * 1.15f + 0.85f;
        PropMeshHelper::appendCylinder(payload, {mastX, H * 1.15f, 0.0f}, 0.045f, 0.045f, 0.85f, 10, 0xFF64748B);
        // Battery control enclosure
        PropMeshHelper::appendBox(payload, {mastX - 0.20f, 1.10f, 0.0f}, {0.28f, 0.38f, 0.20f}, 0xFFCBD5E1);
        // Tilted monocrystalline PV solar panel array
        PropMeshHelper::appendBox(payload, {mastX, mastTop + 0.05f, 0.0f}, {0.85f, 0.04f, 0.65f}, 0xFF1E3A8A);
        PropMeshHelper::appendBox(payload, {mastX, mastTop + 0.05f, 0.0f}, {0.87f, 0.02f, 0.67f}, 0xFFE2E8F0);
        // Telemetry whip antenna
        PropMeshHelper::appendCylinder(payload, {mastX - 0.25f, 1.29f, 0.0f}, 0.01f, 0.006f, 0.35f, 6, 0xFF0F172A);
    }

    return payload;
}

#undef IMPL_MODERN_GATE_BOILERPLATE

} // namespace procedural::props
