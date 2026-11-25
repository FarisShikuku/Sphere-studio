#include "ModernFencesGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

namespace {

ModernFenceParams mergeFenceParams(const ModernFenceParams& base, const GeneratorContext& ctx) {
    ModernFenceParams p = base;
    if (ctx.customParam0 > 0.0f) p.length = ctx.customParam0;
    if (ctx.customParam1 > 0.0f) p.height = ctx.customParam1;
    if (ctx.customParam2 > 0.0f) p.postSpacing = ctx.customParam2;
    if (ctx.customParam3 >= 0) p.style = ctx.customParam3;
    if (ctx.customParam4 >= 0) p.colorScheme = ctx.customParam4;
    if (ctx.customParam5 >= 0.0f) p.hasAccentDetail = (ctx.customParam5 > 0.5f);

    p.length = std::clamp(p.length, 2.0f, 12.0f);
    p.height = std::clamp(p.height, 0.9f, 3.6f);
    p.postSpacing = std::clamp(p.postSpacing, 0.9f, 3.0f);
    return p;
}

int computePostCount(float L, float spacing) {
    return std::max(2, static_cast<int>(std::ceil(L / spacing)) + 1);
}

uint32_t resolveModernMetalColor(int colorScheme) {
    switch (colorScheme) {
        case 1: return 0xFFCBD5E1; // Brushed Silver
        case 2: return 0xFF78350F; // Architectural Bronze
        case 3: return 0xFFD2B48C; // Warm Champagne / Sandstone
        case 4: return 0xFF9A3412; // Weathering Corten Rust
        default: return 0xFF18181B; // Matte Obsidian Black
    }
}

uint32_t resolveTimberColor(int colorScheme) {
    switch (colorScheme) {
        case 1: return 0xFF78350F; // Dark Walnut
        case 2: return 0xFFD2B48C; // Nordic Pale Oak
        case 3: return 0xFF475569; // Charred Shou Sugi Ban
        default: return 0xFFB45309; // Warm Western Red Cedar
    }
}

} // namespace

#define IMPL_MODERN_FENCE_BOILERPLATE(ClassName)                                     \
    ClassName::ClassName() = default;                                                \
    bool ClassName::initialize(const GeneratorContext& /*ctx*/) { return true; }     \
    void ClassName::update(float /*timeSeconds*/, float /*dt*/) {}                   \
    ModernFenceParams ClassName::resolveParams(const GeneratorContext& ctx) const {  \
        return mergeFenceParams(params_, ctx);                                       \
    }

// ============================================================================
// 1. Modern Glass Balustrade Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(ModernGlassFenceGen)

GeneratedMeshPayload ModernGlassFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t postCol = resolveModernMetalColor(p.colorScheme);
    uint32_t glassCol = (p.style == 1) ? 0xFF475569 : (p.style == 2) ? 0xFFE0F2FE : 0xFF7DD3FC;
    uint32_t curbCol = 0xFF64748B;

    // Clean architectural concrete sill curb
    PropMeshHelper::appendBox(payload, {0.0f, 0.08f, 0.0f}, {L + 0.25f, 0.16f, 0.24f}, curbCol);

    // Base spigots and sleek slotted posts
    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        if (p.style != 2) { // Framed / post-supported glass
            PropMeshHelper::appendBox(payload, {px, H * 0.5f + 0.08f, 0.0f}, {0.08f, H, 0.08f}, postCol);
            PropMeshHelper::appendBox(payload, {px, 0.175f, 0.0f}, {0.14f, 0.03f, 0.14f}, postCol);
            PropMeshHelper::appendBox(payload, {px, H + 0.09f, 0.0f}, {0.10f, 0.025f, 0.10f}, postCol);
        } else { // Frameless stainless spigot mounts
            PropMeshHelper::appendBox(payload, {px, 0.23f, 0.0f}, {0.07f, 0.16f, 0.09f}, 0xFFE2E8F0);
        }
    }

    // Tempered safety glass bays
    for (int i = 0; i < nPosts - 1; ++i) {
        float midX = startX + (i + 0.5f) * actualSpacing;
        float bayW = actualSpacing - (p.style == 2 ? 0.03f : 0.10f);
        float panelH = H - 0.14f;
        PropMeshHelper::appendBox(payload, {midX, 0.22f + panelH * 0.5f, 0.0f}, {bayW, panelH, 0.022f}, glassCol);

        if (p.style == 2) {
            // Intermediate stainless spigot clamps per bay
            PropMeshHelper::appendBox(payload, {midX - bayW * 0.28f, 0.23f, 0.0f}, {0.055f, 0.14f, 0.08f}, 0xFFE2E8F0);
            PropMeshHelper::appendBox(payload, {midX + bayW * 0.28f, 0.23f, 0.0f}, {0.055f, 0.14f, 0.08f}, 0xFFE2E8F0);
        } else {
            // Horizontal glass clamp brackets
            PropMeshHelper::appendBox(payload, {midX - bayW * 0.45f, 0.35f, 0.0f}, {0.06f, 0.05f, 0.05f}, 0xFFCBD5E1);
            PropMeshHelper::appendBox(payload, {midX + bayW * 0.45f, 0.35f, 0.0f}, {0.06f, 0.05f, 0.05f}, 0xFFCBD5E1);
            PropMeshHelper::appendBox(payload, {midX - bayW * 0.45f, H - 0.10f, 0.0f}, {0.06f, 0.05f, 0.05f}, 0xFFCBD5E1);
            PropMeshHelper::appendBox(payload, {midX + bayW * 0.45f, H - 0.10f, 0.0f}, {0.06f, 0.05f, 0.05f}, 0xFFCBD5E1);
        }
    }

    // Offset stainless slotted handrail + optional warm LED underglow strip
    PropMeshHelper::appendBox(payload, {0.0f, H + 0.11f, 0.0f}, {L + 0.12f, 0.045f, 0.075f}, 0xFFCBD5E1);
    if (p.hasAccentDetail) {
        PropMeshHelper::appendBox(payload, {0.0f, H + 0.08f, 0.0f}, {L, 0.015f, 0.03f}, 0xFFFDE047);
    }

    return payload;
}

// ============================================================================
// 2. Modern Vertical Aluminum Fin Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(AluminumFinFenceGen)

GeneratedMeshPayload AluminumFinFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t finCol = resolveModernMetalColor(p.colorScheme);
    uint32_t frameCol = (p.colorScheme == 0) ? 0xFF334155 : 0xFF0F172A;

    // Architectural concrete plinth + steel bottom C-channel
    PropMeshHelper::appendBox(payload, {0.0f, 0.09f, 0.0f}, {L + 0.20f, 0.18f, 0.22f}, 0xFF475569);
    PropMeshHelper::appendBox(payload, {0.0f, 0.20f, 0.0f}, {L, 0.04f, 0.12f}, frameCol);
    PropMeshHelper::appendBox(payload, {0.0f, H + 0.08f, 0.0f}, {L, 0.05f, 0.12f}, frameCol);

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, H * 0.5f + 0.08f, 0.0f}, {0.10f, H, 0.12f}, frameCol);
    }

    // Angled / twisted vertical aluminum fins
    float finPitch = (p.style == 1) ? 0.16f : 0.11f;
    int numFins = std::max(6, static_cast<int>(L / finPitch));
    float angleRad = (p.style == 0) ? 0.55f : (p.style == 1 ? 0.95f : 0.22f);
    for (int f = 0; f <= numFins; ++f) {
        float fx = startX + (static_cast<float>(f) / numFins) * L;
        float finAngle = (p.style == 2) ? angleRad * ((f % 2 == 0) ? 1.0f : -1.0f) : angleRad;
        float finH = H - 0.16f;
        PropMeshHelper::appendBox(payload, {fx, 0.22f + finH * 0.5f, 0.0f}, {0.032f, finH, 0.11f}, finCol, finAngle);
    }

    if (p.hasAccentDetail) {
        for (int i = 0; i < nPosts; ++i) {
            float px = startX + i * actualSpacing;
            PropMeshHelper::appendBox(payload, {px, H * 0.72f, 0.065f}, {0.025f, H * 0.35f, 0.015f}, 0xFFFACC15);
        }
    }

    return payload;
}

// ============================================================================
// 3. Laser-Cut Perforated Architectural Art Screen Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(LaserCutFenceGen)

GeneratedMeshPayload LaserCutFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t plateCol = (p.colorScheme == 0) ? 0xFFB45309 : resolveModernMetalColor(p.colorScheme);
    uint32_t postCol = 0xFF1E293B;

    PropMeshHelper::appendBox(payload, {0.0f, 0.06f, 0.0f}, {L + 0.16f, 0.12f, 0.20f}, 0xFF334155);

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, H * 0.5f + 0.06f, 0.0f}, {0.11f, H, 0.11f}, postCol);
    }

    for (int b = 0; b < nPosts - 1; ++b) {
        float midX = startX + (b + 0.5f) * actualSpacing;
        float bayW = actualSpacing - 0.14f;
        float panelH = H - 0.18f;
        float baseY = 0.16f;

        // Shadow-gap steel perimeter frame
        PropMeshHelper::appendBox(payload, {midX, baseY, 0.0f}, {bayW, 0.045f, 0.05f}, postCol);
        PropMeshHelper::appendBox(payload, {midX, baseY + panelH, 0.0f}, {bayW, 0.045f, 0.05f}, postCol);
        PropMeshHelper::appendBox(payload, {midX - bayW * 0.5f, baseY + panelH * 0.5f, 0.0f}, {0.045f, panelH, 0.05f}, postCol);
        PropMeshHelper::appendBox(payload, {midX + bayW * 0.5f, baseY + panelH * 0.5f, 0.0f}, {0.045f, panelH, 0.05f}, postCol);

        // Lower solid dado kickplate
        float solidH = panelH * 0.25f;
        PropMeshHelper::appendBox(payload, {midX, baseY + solidH * 0.5f, 0.0f}, {bayW - 0.06f, solidH, 0.018f}, plateCol);

        // Geometric laser-cut lattice screen zone (diagonal diamond / chevron / cellular ribs)
        float artBaseY = baseY + solidH;
        float artH = panelH - solidH;
        int cols = 4;
        int rows = 4;
        float cellW = (bayW - 0.08f) / cols;
        float cellH = artH / rows;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                float x0 = midX - (bayW - 0.08f) * 0.5f + c * cellW;
                float y0 = artBaseY + r * cellH;
                float x1 = x0 + cellW;
                float y1 = y0 + cellH;
                if (p.style == 1) { // Botanical / Vertical Voronoi ribs
                    PropMeshHelper::appendStrut(payload, {x0 + cellW * 0.5f, y0, 0.0f}, {x1, y0 + cellH * 0.5f, 0.0f}, 0.022f, plateCol);
                    PropMeshHelper::appendStrut(payload, {x1, y0 + cellH * 0.5f, 0.0f}, {x0 + cellW * 0.5f, y1, 0.0f}, 0.022f, plateCol);
                    PropMeshHelper::appendStrut(payload, {x0 + cellW * 0.5f, y1, 0.0f}, {x0, y0 + cellH * 0.5f, 0.0f}, 0.022f, plateCol);
                    PropMeshHelper::appendStrut(payload, {x0, y0 + cellH * 0.5f, 0.0f}, {x0 + cellW * 0.5f, y0, 0.0f}, 0.022f, plateCol);
                } else { // Modern Geometric Chevron & X-lattice
                    PropMeshHelper::appendStrut(payload, {x0, y0, 0.0f}, {x1, y1, 0.0f}, 0.024f, plateCol);
                    PropMeshHelper::appendStrut(payload, {x0, y1, 0.0f}, {x1, y0, 0.0f}, 0.024f, plateCol);
                }
            }
        }

        if (p.hasAccentDetail) {
            // Integrated warm architectural light bar along top channel
            PropMeshHelper::appendBox(payload, {midX, baseY + panelH - 0.04f, 0.022f}, {bayW * 0.78f, 0.02f, 0.015f}, 0xFFFDE047);
        }
    }

    return payload;
}

// ============================================================================
// 4. Gabion Basket & Floating Cedar Slats Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(GabionStoneFenceGen)

GeneratedMeshPayload GabionStoneFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPiers = computePostCount(L, std::max(p.postSpacing, 1.8f));
    float actualSpacing = L / (nPiers - 1);
    float startX = -L * 0.5f;

    uint32_t cageCol = 0xFF94A3B8;
    uint32_t woodCol = resolveTimberColor(p.colorScheme);
    uint32_t stonePalette[3] = {0xFF475569, 0xFF64748B, 0xFF94A3B8};
    if (p.colorScheme == 2) {
        stonePalette[0] = 0xFF8C6D46;
        stonePalette[1] = 0xFFB08968;
        stonePalette[2] = 0xFFD2B48C;
    }

    float pierW = 0.42f;
    float pierD = 0.36f;

    // Gabion rock-filled wire piers
    for (int i = 0; i < nPiers; ++i) {
        float px = startX + i * actualSpacing;
        int courses = std::max(4, static_cast<int>(H / 0.22f));
        float rowH = H / courses;
        for (int r = 0; r < courses; ++r) {
            float ry = r * rowH + rowH * 0.5f;
            for (int bx = 0; bx < 2; ++bx) {
                float sx = px + (bx == 0 ? -0.095f : 0.095f);
                uint32_t sc = stonePalette[(r * 2 + bx + i) % 3];
                PropMeshHelper::appendBox(payload, {sx, ry, 0.0f}, {0.17f, rowH * 0.90f, pierD * 0.88f}, sc);
            }
        }
        // Welded gabion wire mesh cage corners & horizontal rings
        for (float cx : {-pierW * 0.5f, pierW * 0.5f}) {
            for (float cz : {-pierD * 0.5f, pierD * 0.5f}) {
                PropMeshHelper::appendBox(payload, {px + cx, H * 0.5f, cz}, {0.016f, H, 0.016f}, cageCol);
            }
        }
        int rings = std::max(3, static_cast<int>(H / 0.32f));
        for (int r = 0; r <= rings; ++r) {
            float ry = (static_cast<float>(r) / rings) * H;
            PropMeshHelper::appendBox(payload, {px, ry, pierD * 0.5f}, {pierW, 0.014f, 0.014f}, cageCol);
            PropMeshHelper::appendBox(payload, {px, ry, -pierD * 0.5f}, {pierW, 0.014f, 0.014f}, cageCol);
            PropMeshHelper::appendBox(payload, {px - pierW * 0.5f, ry, 0.0f}, {0.014f, 0.014f, pierD}, cageCol);
            PropMeshHelper::appendBox(payload, {px + pierW * 0.5f, ry, 0.0f}, {0.014f, 0.014f, pierD}, cageCol);
        }
        if (p.hasAccentDetail) {
            PropMeshHelper::appendBox(payload, {px, H + 0.03f, 0.0f}, {pierW + 0.06f, 0.05f, pierD + 0.06f}, 0xFF1E293B);
        }
    }

    // Timber / Metal Infill bays between gabion piers
    for (int b = 0; b < nPiers - 1; ++b) {
        float midX = startX + (b + 0.5f) * actualSpacing;
        float bayW = std::max(0.4f, actualSpacing - pierW);
        if (p.style == 1) { // Low stone gabion + wood upper
            PropMeshHelper::appendBox(payload, {midX, H * 0.20f, 0.0f}, {bayW, H * 0.40f, pierD * 0.85f}, stonePalette[1]);
        }
        float slatStartY = (p.style == 1) ? H * 0.44f : 0.12f;
        int slats = std::max(3, static_cast<int>((H - slatStartY) / 0.14f));
        for (int s = 0; s < slats; ++s) {
            float sy = slatStartY + s * ((H - slatStartY - 0.08f) / std::max(1, slats - 1));
            PropMeshHelper::appendBox(payload, {midX, sy, 0.0f}, {bayW, 0.095f, 0.04f}, woodCol);
        }
    }

    return payload;
}

// ============================================================================
// 5. Minimalist Precast Concrete & Steel Reveal Rib Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(ConcreteRibFenceGen)

GeneratedMeshPayload ConcreteRibFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t concCol = (p.colorScheme == 1) ? 0xFF475569 : (p.colorScheme == 2) ? 0xFFD2B48C : 0xFFCBD5E1;
    uint32_t trimCol = 0xFF18181B;

    PropMeshHelper::appendBox(payload, {0.0f, 0.10f, 0.0f}, {L + 0.3f, 0.20f, 0.34f}, 0xFF64748B);

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, H * 0.5f + 0.10f, 0.0f}, {0.22f, H, 0.26f}, concCol);
        PropMeshHelper::appendBox(payload, {px, H + 0.12f, 0.0f}, {0.28f, 0.05f, 0.32f}, trimCol);
        if (p.hasAccentDetail) {
            PropMeshHelper::appendBox(payload, {px, 0.45f, 0.135f}, {0.10f, 0.035f, 0.015f}, 0xFFFDE047);
        }
    }

    for (int b = 0; b < nPosts - 1; ++b) {
        float midX = startX + (b + 0.5f) * actualSpacing;
        float bayW = actualSpacing - 0.24f;
        int tiers = (p.style == 1) ? 4 : 3;
        float slabH = (H - 0.15f) / tiers * 0.82f;
        for (int t = 0; t < tiers; ++t) {
            float ty = 0.22f + t * ((H - 0.15f) / tiers) + slabH * 0.5f;
            PropMeshHelper::appendBox(payload, {midX, ty, 0.0f}, {bayW, slabH, 0.14f}, concCol);
            // Architectural reveal fin strip
            PropMeshHelper::appendBox(payload, {midX, ty + slabH * 0.5f + 0.015f, 0.0f}, {bayW, 0.025f, 0.04f}, trimCol);
        }
        // Top concrete coping cantilever
        PropMeshHelper::appendBox(payload, {midX, H + 0.10f, 0.0f}, {bayW + 0.06f, 0.06f, 0.22f}, concCol);
    }

    return payload;
}

// ============================================================================
// 6. Modern Tilted Aluminum Louver Blade Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(LouverBladeFenceGen)

GeneratedMeshPayload LouverBladeFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t postCol = 0xFF1E293B;
    uint32_t bladeCol = (p.colorScheme == 0) ? 0xFF334155 :
                        (p.colorScheme == 1) ? 0xFFB45309 :
                        (p.colorScheme == 2) ? 0xFFD2B48C : 0xFFCBD5E1;

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, H * 0.5f, 0.0f}, {0.10f, H, 0.10f}, postCol);
        if (p.hasAccentDetail) {
            PropMeshHelper::appendBox(payload, {px, H + 0.025f, 0.0f}, {0.13f, 0.03f, 0.13f}, bladeCol);
        }
    }

    PropMeshHelper::appendBox(payload, {0.0f, 0.06f, 0.0f}, {L, 0.05f, 0.08f}, postCol);
    PropMeshHelper::appendBox(payload, {0.0f, H - 0.03f, 0.0f}, {L, 0.06f, 0.09f}, postCol);

    float pitch = 0.095f;
    int blades = std::max(4, static_cast<int>((H - 0.16f) / pitch));
    float tilt = (p.style == 1) ? 0.65f : 0.35f;
    for (int b = 0; b < nPosts - 1; ++b) {
        float midX = startX + (b + 0.5f) * actualSpacing;
        float bayW = actualSpacing - 0.11f;
        for (int k = 0; k < blades; ++k) {
            float ky = 0.12f + k * pitch;
            // Airfoil tilted horizontal blade
            PropMeshHelper::appendBox(payload, {midX, ky, 0.0f}, {bayW, 0.022f, 0.085f}, bladeCol);
            PropMeshHelper::appendBox(payload, {midX, ky + 0.015f, tilt * 0.025f}, {bayW, 0.014f, 0.045f}, bladeCol);
        }
    }

    return payload;
}

// ============================================================================
// 7. Green Living Planter Trellis Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(LivingWallFenceGen)

GeneratedMeshPayload LivingWallFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t planterCol = (p.colorScheme == 1) ? 0xFF9A3412 : 0xFF475569;
    uint32_t frameCol = 0xFF1E293B;
    uint32_t woodCol = resolveTimberColor(p.colorScheme);
    uint32_t foliageA = 0xFF15803D;
    uint32_t foliageB = 0xFF22C55E;

    // Continuous planter base box
    float planterH = 0.42f;
    PropMeshHelper::appendBox(payload, {0.0f, planterH * 0.5f, 0.0f}, {L + 0.16f, planterH, 0.38f}, planterCol);
    PropMeshHelper::appendBox(payload, {0.0f, planterH + 0.01f, 0.0f}, {L + 0.04f, 0.03f, 0.30f}, 0xFF451A03); // Soil bed

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, planterH + (H - planterH) * 0.5f, 0.0f}, {0.09f, H - planterH, 0.09f}, frameCol);
    }

    // Back cedar screen + steel climbing trellis grid
    int backSlats = 8;
    for (int s = 0; s < backSlats; ++s) {
        float sy = planterH + 0.10f + s * ((H - planterH - 0.14f) / (backSlats - 1));
        PropMeshHelper::appendBox(payload, {0.0f, sy, -0.02f}, {L, 0.06f, 0.025f}, woodCol);
    }

    // Organic foliage tufts growing up the trellis
    if (p.hasAccentDetail) {
        int plantCols = std::max(6, static_cast<int>(L * 3.5f));
        for (int c = 0; c < plantCols; ++c) {
            float fx = startX + (c + 0.5f) * (L / plantCols);
            // Planter shrubs
            PropMeshHelper::appendSphere(payload, {fx, planterH + 0.12f, 0.04f}, 0.14f, 5, 6, (c % 2 == 0) ? foliageA : foliageB);
            if (p.style == 0 || (c % 2 == 0)) {
                float vineY = planterH + 0.35f + ((c * 37) % 100) * 0.008f * (H - planterH);
                vineY = std::min(vineY, H - 0.10f);
                PropMeshHelper::appendSphere(payload, {fx, vineY, 0.05f}, 0.12f, 5, 6, (c % 3 == 0) ? foliageB : foliageA);
            }
        }
    }

    PropMeshHelper::appendBox(payload, {0.0f, H + 0.02f, 0.0f}, {L + 0.10f, 0.05f, 0.12f}, frameCol);
    return payload;
}

// ============================================================================
// 8. Contemporary Bamboo & Blackened Steel Courtyard Screen Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(BambooSteelFenceGen)

GeneratedMeshPayload BambooSteelFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t steelCol = resolveModernMetalColor(p.colorScheme);
    uint32_t bambooCol = (p.style == 1) ? 0xFF78350F : (p.style == 2 ? 0xFF65A30D : 0xFFCA8A04);
    uint32_t nodeCol = 0xFFA16207;

    PropMeshHelper::appendBox(payload, {0.0f, 0.07f, 0.0f}, {L + 0.2f, 0.14f, 0.24f}, 0xFF334155);
    PropMeshHelper::appendBox(payload, {0.0f, 0.16f, 0.0f}, {L, 0.05f, 0.08f}, steelCol);
    PropMeshHelper::appendBox(payload, {0.0f, H + 0.06f, 0.0f}, {L + 0.08f, 0.06f, 0.09f}, steelCol);

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, H * 0.5f + 0.07f, 0.0f}, {0.08f, H, 0.09f}, steelCol);
    }

    // Tightly bundled bamboo culms with horizontal node rings
    int culms = std::max(10, static_cast<int>(L / 0.085f));
    float culmH = H - 0.12f;
    for (int c = 0; c <= culms; ++c) {
        float cx = startX + 0.06f + (static_cast<float>(c) / culms) * (L - 0.12f);
        PropMeshHelper::appendCylinder(payload, {cx, 0.18f, 0.0f}, 0.032f, 0.030f, culmH, 8, bambooCol);
        if (p.hasAccentDetail && (c % 2 == 0)) {
            for (float frac : {0.33f, 0.68f}) {
                PropMeshHelper::appendCylinder(payload, {cx, 0.18f + culmH * frac, 0.0f}, 0.036f, 0.036f, 0.02f, 8, nodeCol);
            }
        }
    }

    return payload;
}

// ============================================================================
// 9. Modern Stainless Cable-Rail Deck & Promenade Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(CableRailFenceGen)

GeneratedMeshPayload CableRailFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = std::clamp(p.height, 0.9f, 2.0f);
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t postCol = resolveModernMetalColor(p.colorScheme);
    uint32_t cableCol = 0xFFE2E8F0;
    uint32_t railCol = (p.style == 1) ? postCol : 0xFFB45309; // Teak wood top rail or metal cap

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, 0.015f, 0.0f}, {0.14f, 0.03f, 0.14f}, postCol);
        PropMeshHelper::appendBox(payload, {px, H * 0.5f, 0.0f}, {0.065f, H, 0.065f}, postCol);
    }

    // Wide drink/handrail top cap
    PropMeshHelper::appendBox(payload, {0.0f, H + 0.025f, 0.0f}, {L + 0.14f, 0.05f, 0.13f}, railCol);

    // 8 High-tension stainless steel aircraft cables
    int cables = 8;
    for (int c = 0; c < cables; ++c) {
        float cy = 0.12f + c * ((H - 0.20f) / (cables - 1));
        PropMeshHelper::appendOrientedCylinder(payload, {-L * 0.5f, cy, 0.0f}, {L * 0.5f, cy, 0.0f}, 0.008f, 0.008f, 8, cableCol);
        if (p.hasAccentDetail) {
            // Turnbuckle swages near end posts
            PropMeshHelper::appendCylinder(payload, {-L * 0.5f + 0.12f, cy, 0.0f}, 0.016f, 0.016f, 0.02f, 8, 0xFF94A3B8);
            PropMeshHelper::appendOrientedCylinder(payload, {L * 0.5f - 0.18f, cy, 0.0f}, {L * 0.5f - 0.04f, cy, 0.0f}, 0.015f, 0.015f, 8, 0xFF94A3B8);
        }
    }

    return payload;
}

// ============================================================================
// 10. Corrugated Corten Steel & Timber Architectural Fence
// ============================================================================
IMPL_MODERN_FENCE_BOILERPLATE(CorrugatedCortenFenceGen)

GeneratedMeshPayload CorrugatedCortenFenceGen::generateMesh(const GeneratorContext& ctx) {
    GeneratedMeshPayload payload;
    payload.generatorName = getName();
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    ModernFenceParams p = resolveParams(ctx);
    float L = p.length;
    float H = p.height;
    int nPosts = computePostCount(L, p.postSpacing);
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    uint32_t sheetCol = (p.colorScheme == 1) ? 0xFF475569 : (p.colorScheme == 2 ? 0xFF94A3B8 : 0xFF9A3412);
    uint32_t trimCol = 0xFFB45309;
    uint32_t postCol = 0xFF1E293B;

    for (int i = 0; i < nPosts; ++i) {
        float px = startX + i * actualSpacing;
        PropMeshHelper::appendBox(payload, {px, H * 0.5f, 0.0f}, {0.11f, H, 0.11f}, postCol);
        if (p.hasAccentDetail) {
            PropMeshHelper::appendBox(payload, {px, H + 0.04f, 0.0f}, {0.14f, 0.05f, 0.14f}, 0xFF0F172A);
            PropMeshHelper::appendBox(payload, {px, H + 0.07f, 0.0f}, {0.10f, 0.02f, 0.10f}, 0xFF38BDF8); // Solar cap
        }
    }

    // Timber top & bottom architectural fascia rails
    PropMeshHelper::appendBox(payload, {0.0f, 0.12f, 0.0f}, {L, 0.18f, 0.07f}, trimCol);
    PropMeshHelper::appendBox(payload, {0.0f, H - 0.08f, 0.0f}, {L, 0.16f, 0.07f}, trimCol);

    // Corrugated steel ridges
    float ribStep = 0.12f;
    int numRibs = std::max(6, static_cast<int>(L / ribStep));
    float sheetH = H - 0.34f;
    for (int r = 0; r < numRibs; ++r) {
        float rx = startX + (r + 0.5f) * (L / numRibs);
        float zOff = (r % 2 == 0) ? 0.025f : -0.025f;
        if (p.style == 1) { // Horizontal corrugation ribs
            PropMeshHelper::appendBox(payload, {rx, 0.21f + sheetH * 0.5f, zOff * 0.6f}, {(L / numRibs) * 1.05f, sheetH, 0.03f}, sheetCol);
        } else {
            PropMeshHelper::appendBox(payload, {rx, 0.21f + sheetH * 0.5f, zOff}, {(L / numRibs) * 1.02f, sheetH, 0.03f}, sheetCol);
        }
    }

    return payload;
}

#undef IMPL_MODERN_FENCE_BOILERPLATE

} // namespace procedural::props
