#include "UrbanFenceGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

UrbanFenceGen::UrbanFenceGen() = default;

bool UrbanFenceGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void UrbanFenceGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload UrbanFenceGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float L = std::max(params_.length, 1.5f);
    float H = std::max(params_.height, 0.8f);
    float spacing = std::max(params_.postSpacing, 1.0f);
    int postCount = static_cast<int>(std::ceil(L / spacing)) + 1;
    if (postCount < 2) postCount = 2;
    float actualSpacing = L / (postCount - 1);
    float startX = -L * 0.5f;

    uint32_t steelCol = (params_.colorScheme == 1) ? 0xFF22262B : // Jet Black
                        (params_.colorScheme == 2) ? 0xFFEAB308 : // Safety Yellow
                        (params_.colorScheme == 3) ? 0xFF374151 : // Dark Charcoal
                        (params_.colorScheme == 4) ? 0xFFD2B48C : // Sandstone Tan
                                                     0xFF9CA3AF;  // Galvanized Grey

    if (params_.fenceStyle == 0) {
        // ----------------- STYLE 0: CHAIN-LINK SECURITY FENCE -----------------
        // Tubular posts
        for (int p = 0; p < postCount; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendCylinder(payload, Vec3{px, 0.0f, 0.0f}, 0.045f, 0.045f, H, 10, steelCol);
            // Post cap
            PropMeshHelper::appendBox(payload, Vec3{px, H + 0.02f, 0.0f}, Vec3{0.10f, 0.04f, 0.10f}, steelCol);

            // Optional 45-deg angled barbed wire Y-arm
            if (params_.hasSecurityTop) {
                PropMeshHelper::appendBox(payload, Vec3{px, H + 0.18f, 0.12f}, Vec3{0.03f, 0.35f, 0.03f}, steelCol, 0.0f);
                PropMeshHelper::appendBox(payload, Vec3{px, H + 0.18f, -0.12f}, Vec3{0.03f, 0.35f, 0.03f}, steelCol, 0.0f);
            }
        }

        // Horizontal top rail and bottom tension pipe
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H - 0.03f, 0.0f}, Vec3{L, 0.05f, 0.05f}, steelCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.08f, 0.0f}, Vec3{L, 0.03f, 0.03f}, steelCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H * 0.5f, 0.0f}, Vec3{L, 0.025f, 0.025f}, steelCol);

        // Chain link wire diamond lattice panels
        uint32_t wireCol = 0xFFCBD5E1;
        float meshH = H - 0.12f;
        int gridDivs = static_cast<int>(L * 8.0f);
        for (int i = 0; i <= gridDivs; ++i) {
            float gx = startX + (static_cast<float>(i) / gridDivs) * L;
            PropMeshHelper::appendBox(payload, Vec3{gx, 0.08f + meshH * 0.5f, 0.0f}, Vec3{0.012f, meshH, 0.012f}, wireCol);
        }

        // Barbed wire strands along Y-arms
        if (params_.hasSecurityTop) {
            for (float barbZ : {0.15f, -0.15f}) {
                PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.22f, barbZ}, Vec3{L, 0.012f, 0.012f}, 0xFF94A3B8);
                PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.32f, barbZ * 1.2f}, Vec3{L, 0.012f, 0.012f}, 0xFF94A3B8);
            }
        }
    } else if (params_.fenceStyle == 1) {
        // ----------------- STYLE 1: WROUGHT-IRON DECORATIVE SPEAR FENCE -----------------
        uint32_t ironCol = (params_.colorScheme == 1 || params_.colorScheme == 0) ? 0xFF1E2229 : steelCol;
        uint32_t plinthCol = 0xFF64748B;

        // Base concrete plinth curb
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.12f, 0.0f}, Vec3{L + 0.3f, 0.24f, 0.24f}, plinthCol);

        // Heavy square posts with pyramid caps
        for (int p = 0; p < postCount; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.24f + (H - 0.24f) * 0.5f, 0.0f}, Vec3{0.14f, H - 0.24f, 0.14f}, ironCol);
            PropMeshHelper::appendBox(payload, Vec3{px, H + 0.05f, 0.0f}, Vec3{0.18f, 0.08f, 0.18f}, ironCol);
            PropMeshHelper::appendBox(payload, Vec3{px, H + 0.12f, 0.0f}, Vec3{0.10f, 0.06f, 0.10f}, 0xFFF59E0B); // Brass or gold accent
        }

        // Horizontal stringers (lower, mid, upper)
        float s1 = 0.32f;
        float s2 = H - 0.18f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, s1, 0.0f}, Vec3{L, 0.04f, 0.04f}, ironCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, s2, 0.0f}, Vec3{L, 0.04f, 0.04f}, ironCol);

        // Vertical pickets with spear finials
        float picketSpacing = 0.12f;
        int picketCount = static_cast<int>(L / picketSpacing);
        for (int i = 0; i <= picketCount; ++i) {
            float px = startX + i * picketSpacing;
            float pH = H - 0.30f;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.24f + pH * 0.5f, 0.0f}, Vec3{0.025f, pH, 0.025f}, ironCol);
            // Spearhead finial top
            PropMeshHelper::appendBox(payload, Vec3{px, 0.24f + pH + 0.06f, 0.0f}, Vec3{0.045f, 0.12f, 0.015f}, (params_.hasSecurityTop ? 0xFFF59E0B : ironCol));
            PropMeshHelper::appendBox(payload, Vec3{px, 0.24f + pH + 0.14f, 0.0f}, Vec3{0.015f, 0.05f, 0.015f}, ironCol);
        }
    } else if (params_.fenceStyle == 2) {
        // ----------------- STYLE 2: PEDESTRIAN SAFETY GUARDRAIL -----------------
        uint32_t railCol = (params_.colorScheme == 2) ? 0xFFFACC15 : (params_.colorScheme == 4) ? 0xFFD2B48C : (params_.colorScheme == 1 ? 0xFF1F2937 : 0xFFE2E8F0);

        // Mounting base flanges & vertical posts
        for (int p = 0; p < postCount; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.02f, 0.0f}, Vec3{0.18f, 0.04f, 0.18f}, 0xFF334155);
            PropMeshHelper::appendCylinder(payload, Vec3{px, 0.04f, 0.0f}, 0.04f, 0.04f, H * 0.95f, 10, railCol);
        }

        // Top continuous smooth bullnose handrail
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H, 0.0f}, Vec3{L + 0.1f, 0.08f, 0.08f}, railCol);
        // Bottom stringer
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.15f, 0.0f}, Vec3{L, 0.05f, 0.05f}, railCol);

        // Vertical infill safety loops
        float barSpacing = 0.14f;
        int barCount = static_cast<int>(L / barSpacing);
        for (int i = 0; i <= barCount; ++i) {
            float bx = startX + i * barSpacing;
            PropMeshHelper::appendBox(payload, Vec3{bx, 0.15f + (H - 0.25f) * 0.5f, 0.0f}, Vec3{0.02f, H - 0.25f, 0.02f}, railCol);
        }

        // Reflective warning marker bands
        if (params_.hasSecurityTop) {
            for (int p = 0; p < postCount; ++p) {
                float px = startX + p * actualSpacing;
                PropMeshHelper::appendBox(payload, Vec3{px, H * 0.75f, 0.0f}, Vec3{0.09f, 0.08f, 0.09f}, 0xFFEF4444); // Red/white retro reflector
            }
        }
    } else if (params_.fenceStyle == 3) {
        // ----------------- STYLE 3: MODERN ARCHITECTURAL SLAT PRIVACY FENCE -----------------
        uint32_t postCol = (params_.colorScheme == 4) ? 0xFF785A3C : 0xFF1E293B; // Charcoal / Warm Bronze
        uint32_t slatCol = (params_.colorScheme == 4) ? 0xFFD2B48C : (params_.colorScheme == 3) ? 0xFFB45309 : (params_.colorScheme == 1 ? 0xFF0F172A : 0xFFD97706); // Tan / Cedar / Composite

        // Aluminum posts
        for (int p = 0; p < postCount; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, H * 0.5f, 0.0f}, Vec3{0.10f, H, 0.10f}, postCol);
            PropMeshHelper::appendBox(payload, Vec3{px, H + 0.02f, 0.0f}, Vec3{0.12f, 0.04f, 0.12f}, postCol);
        }

        // Horizontal slats with crisp architectural shadow gaps
        float slatH = 0.07f;
        float gap = 0.018f;
        int slatCount = static_cast<int>((H - 0.10f) / (slatH + gap));
        for (int s = 0; s < slatCount; ++s) {
            float sy = 0.08f + s * (slatH + gap) + slatH * 0.5f;
            PropMeshHelper::appendBox(payload, Vec3{0.0f, sy, 0.0f}, Vec3{L, slatH, 0.035f}, slatCol);
        }

        // Top architectural capping channel
        if (params_.hasSecurityTop) {
            PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.03f, 0.0f}, Vec3{L + 0.15f, 0.04f, 0.09f}, postCol);
        }
    } else {
        // ----------------- STYLE 4: TEMPORARY CONSTRUCTION HOARDING BARRIER -----------------
        uint32_t shoeCol = 0xFFEA580C; // High-Vis Orange weighted counterweight base
        uint32_t meshCol = 0xFF94A3B8; // Galvanized wire mesh

        // Concrete ballast footing blocks
        for (int p = 0; p < postCount; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.10f, 0.0f}, Vec3{0.22f, 0.20f, 0.55f}, shoeCol);
            // Tubular vertical frame leg
            PropMeshHelper::appendCylinder(payload, Vec3{px, 0.20f, 0.0f}, 0.035f, 0.035f, H - 0.20f, 8, 0xFF64748B);
        }

        // Outer rectangular frame perimeter for each panel
        for (int p = 0; p < postCount - 1; ++p) {
            float pMid = startX + (p + 0.5f) * actualSpacing;
            // Top and bottom tubular rails
            PropMeshHelper::appendBox(payload, Vec3{pMid, H, 0.0f}, Vec3{actualSpacing - 0.04f, 0.04f, 0.04f}, 0xFF64748B);
            PropMeshHelper::appendBox(payload, Vec3{pMid, 0.22f, 0.0f}, Vec3{actualSpacing - 0.04f, 0.04f, 0.04f}, 0xFF64748B);
            // Welded anti-climb mesh infill
            int gridLines = static_cast<int>(actualSpacing * 6.0f);
            float innerH = H - 0.24f;
            for (int g = 0; g <= gridLines; ++g) {
                float gx = pMid - (actualSpacing * 0.5f) + (static_cast<float>(g) / gridLines) * actualSpacing;
                PropMeshHelper::appendBox(payload, Vec3{gx, 0.22f + innerH * 0.5f, 0.0f}, Vec3{0.012f, innerH, 0.012f}, meshCol);
            }
        }

        // Diagonal wind brace ground kickers
        if (params_.hasSecurityTop) {
            for (int p = 0; p < postCount; ++p) {
                float px = startX + p * actualSpacing;
                PropMeshHelper::appendBox(payload, Vec3{px, H * 0.45f, 0.25f}, Vec3{0.03f, H * 0.75f, 0.03f}, 0xFFCBD5E1, 0.0f);
            }
        }
    }

    return payload;
}

} // namespace procedural::props
