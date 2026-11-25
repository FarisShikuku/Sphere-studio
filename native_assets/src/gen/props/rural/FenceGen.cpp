#include "FenceGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

FenceGen::FenceGen() = default;

bool FenceGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void FenceGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload FenceGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    uint32_t woodCol = (params_.woodTint == 0) ? 0xFF8E7D6A : // Weathered Cedar
                       (params_.woodTint == 1) ? 0xFFF8FAFC : // Painted White
                       (params_.woodTint == 2) ? 0xFF3D2E22 : // Dark Creosote
                       (params_.woodTint == 3) ? 0xFFB45309 : // Warm Natural Oak
                       (params_.woodTint == 5) ? 0xFFD2B48C : // Warm Desert Tan
                                                 0xFF64748B;  // Slate Grey

    float L = std::max(params_.length, 1.5f);
    float H = std::max(params_.height, 0.7f);
    float spacing = std::max(params_.postSpacing, 0.8f);
    int postCount = static_cast<int>(std::ceil(L / spacing)) + 1;
    if (postCount < 2) postCount = 2;

    float actualSpacing = L / (postCount - 1);
    float startX = -L * 0.5f;

    if (params_.fenceType == 0) {
        // ---------------- 0: SPLIT-RAIL RANCH FENCE ----------------
        // Rough-hewn cedar posts with mortise holes
        float postW = 0.13f;
        for (int i = 0; i < postCount; ++i) {
            float x = startX + i * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{x, H * 0.5f, 0.0f}, Vec3{postW, H, postW}, woodCol);
            PropMeshHelper::appendBox(payload, Vec3{x, H + 0.05f, 0.0f}, Vec3{postW * 0.8f, 0.10f, postW * 0.8f}, woodCol);
        }

        // Heavy split timber rails that pass through posts
        int rails = std::clamp(params_.railCount, 2, 4);
        for (int r = 0; r < rails; ++r) {
            float y = H * (0.22f + 0.68f * (static_cast<float>(r) / (rails - 1)));
            for (int i = 0; i < postCount - 1; ++i) {
                float xMid = startX + (i + 0.5f) * actualSpacing;
                float zOffset = (r % 2 == 0) ? 0.03f : -0.03f;
                PropMeshHelper::appendBox(payload, Vec3{xMid, y, zOffset}, Vec3{actualSpacing + 0.18f, 0.10f, 0.07f}, woodCol);
            }
        }

        // Diagonal ground anchor stakes (if enabled)
        if (params_.hasAccentDetail) {
            PropMeshHelper::appendBox(payload, Vec3{startX - 0.25f, H * 0.45f, 0.0f}, Vec3{0.08f, H * 0.9f, 0.08f}, woodCol);
            PropMeshHelper::appendBox(payload, Vec3{startX + L + 0.25f, H * 0.45f, 0.0f}, Vec3{0.08f, H * 0.9f, 0.08f}, woodCol);
        }

    } else if (params_.fenceType == 1) {
        // ---------------- 1: TRADITIONAL WHITE PICKET FENCE ----------------
        uint32_t picketCol = (params_.woodTint == 0) ? 0xFFFFFFFF : woodCol;
        float postW = 0.09f;

        // Posts with pyramidal decorative caps
        for (int i = 0; i < postCount; ++i) {
            float x = startX + i * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{x, H * 0.52f, 0.0f}, Vec3{postW, H * 1.04f, postW}, picketCol);
            PropMeshHelper::appendBox(payload, Vec3{x, H * 1.05f + 0.03f, 0.0f}, Vec3{postW * 1.25f, 0.05f, postW * 1.25f}, picketCol);
            if (params_.hasAccentDetail) {
                PropMeshHelper::appendSphere(payload, Vec3{x, H * 1.05f + 0.08f, 0.0f}, 0.045f, 6, 8, picketCol);
            }
        }

        // Top & bottom horizontal runner rails
        float railY1 = H * 0.26f;
        float railY2 = H * 0.74f;
        for (int i = 0; i < postCount - 1; ++i) {
            float xMid = startX + (i + 0.5f) * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{xMid, railY1, -0.03f}, Vec3{actualSpacing, 0.045f, 0.045f}, picketCol);
            PropMeshHelper::appendBox(payload, Vec3{xMid, railY2, -0.03f}, Vec3{actualSpacing, 0.045f, 0.045f}, picketCol);
        }

        // Pickets spaced ~11cm apart with gothic spear tops
        float picketSpacing = 0.11f;
        int picketCount = static_cast<int>(L / picketSpacing);
        float pW = 0.075f;
        float pThick = 0.02f;

        for (int p = 0; p <= picketCount; ++p) {
            float px = startX + p * picketSpacing;
            float pH = H * 0.90f;
            PropMeshHelper::appendBox(payload, Vec3{px, pH * 0.5f, 0.03f}, Vec3{pW, pH, pThick}, picketCol);
            // Gothic pointed spearhead
            PropMeshHelper::appendBox(payload, Vec3{px, pH + 0.035f, 0.03f}, Vec3{pW * 0.6f, 0.07f, pThick}, picketCol);
        }

    } else if (params_.fenceType == 2) {
        // ---------------- 2: BARBED-WIRE AGRICULTURAL CATTLE FENCE ----------------
        // Heavy wooden corner posts and metal T-posts
        uint32_t tPostCol = 0xFF15803D; // Green metal T-post
        uint32_t wireCol = 0xFF94A3B8;

        for (int i = 0; i < postCount; ++i) {
            float x = startX + i * actualSpacing;
            bool isEndPost = (i == 0 || i == postCount - 1);
            if (isEndPost) {
                // Round cedar corner post
                PropMeshHelper::appendCylinder(payload, Vec3{x, 0.0f, 0.0f}, 0.10f, 0.09f, H * 1.05f, 12, woodCol);
            } else {
                // Steel T-post with white safety cap
                PropMeshHelper::appendBox(payload, Vec3{x, H * 0.5f, 0.0f}, Vec3{0.04f, H, 0.03f}, tPostCol);
                PropMeshHelper::appendBox(payload, Vec3{x, H + 0.02f, 0.0f}, Vec3{0.045f, 0.04f, 0.045f}, 0xFFFFFFFF);
            }
        }

        // 4 Strands of tensioned barbed wire
        int strands = std::clamp(params_.railCount, 3, 5);
        for (int s = 0; s < strands; ++s) {
            float y = H * (0.20f + 0.70f * (static_cast<float>(s) / (strands - 1)));
            PropMeshHelper::appendBox(payload, Vec3{0.0f, y, 0.02f}, Vec3{L, 0.012f, 0.012f}, wireCol);
            // Barbs along the wire
            int barbCount = static_cast<int>(L * 5.0f);
            for (int b = 0; b < barbCount; ++b) {
                float bx = startX + (static_cast<float>(b) / barbCount) * L;
                PropMeshHelper::appendBox(payload, Vec3{bx, y, 0.02f}, Vec3{0.02f, 0.025f, 0.025f}, 0xFF64748B);
            }
        }

        // Corner H-brace assembly (diagonal tension wire and horizontal compression strut)
        if (params_.hasAccentDetail) {
            float x1 = startX;
            float x2 = startX + actualSpacing;
            float strutY = H * 0.85f;
            PropMeshHelper::appendCylinder(payload, Vec3{x2, 0.0f, 0.0f}, 0.09f, 0.08f, H * 1.02f, 12, woodCol);
            // Horizontal strut
            PropMeshHelper::appendBox(payload, Vec3{(x1 + x2) * 0.5f, strutY, 0.0f}, Vec3{actualSpacing, 0.08f, 0.08f}, woodCol);
            // Diagonal wire brace
            PropMeshHelper::appendBox(payload, Vec3{(x1 + x2) * 0.5f, strutY * 0.5f, 0.0f}, Vec3{actualSpacing, 0.015f, 0.015f}, wireCol);
        }

    } else if (params_.fenceType == 3) {
        // ---------------- 3: DRY-STACK STONE FIELD WALL ----------------
        uint32_t stoneDark = (params_.woodTint == 5) ? 0xFFB08968 : 0xFF475569;
        uint32_t stoneMid  = (params_.woodTint == 5) ? 0xFFD2B48C : 0xFF64748B;
        uint32_t stoneLight= (params_.woodTint == 5) ? 0xFFE6CCB2 : 0xFF94A3B8;

        float wallW = 0.50f;
        int courses = std::max(4, static_cast<int>(H / 0.16f));
        float courseH = (H - 0.12f) / courses;

        for (int c = 0; c < courses; ++c) {
            float cy = c * courseH + courseH * 0.5f;
            float taperW = wallW * (1.0f - 0.15f * (static_cast<float>(c) / courses));
            int blockCount = static_cast<int>(L / 0.45f) + 1;
            float blockL = L / blockCount;

            for (int b = 0; b < blockCount; ++b) {
                float bx = startX + (b + 0.5f) * blockL + ((c % 2 == 1) ? (blockL * 0.25f) : 0.0f);
                if (bx < -L * 0.5f || bx > L * 0.5f) continue;
                uint32_t cCol = ((b + c) % 3 == 0) ? stoneDark : (((b + c) % 3 == 1) ? stoneMid : stoneLight);
                PropMeshHelper::appendBox(payload, Vec3{bx, cy, 0.0f}, Vec3{blockL * 0.95f, courseH * 0.94f, taperW}, cCol);
            }
        }

        // Heavy vertical capstones (coping stones) along the top
        float capstoneW = wallW * 0.90f;
        int capCount = static_cast<int>(L / 0.28f);
        float capL = L / capCount;
        for (int i = 0; i < capCount; ++i) {
            float cx = startX + (i + 0.5f) * capL;
            uint32_t capCol = (i % 2 == 0) ? stoneMid : stoneLight;
            PropMeshHelper::appendBox(payload, Vec3{cx, H - 0.04f, 0.0f}, Vec3{capL * 0.92f, 0.14f, capstoneW}, capCol);
        }

    } else if (params_.fenceType == 4) {
        // ---------------- 4: POST-AND-BOARD HORSE FENCE ----------------
        // Clean square timber posts with 3 wide running boards
        float postW = 0.12f;
        for (int i = 0; i < postCount; ++i) {
            float x = startX + i * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{x, H * 0.52f, 0.0f}, Vec3{postW, H * 1.04f, postW}, woodCol);
            PropMeshHelper::appendBox(payload, Vec3{x, H * 1.05f, 0.0f}, Vec3{postW * 1.1f, 0.04f, postW * 1.1f}, woodCol);
        }

        // Wide timber boards (1x6 inch planks)
        int boards = std::clamp(params_.railCount, 3, 4);
        for (int r = 0; r < boards; ++r) {
            float y = H * (0.22f + 0.70f * (static_cast<float>(r) / (boards - 1)));
            for (int i = 0; i < postCount - 1; ++i) {
                float xMid = startX + (i + 0.5f) * actualSpacing;
                PropMeshHelper::appendBox(payload, Vec3{xMid, y, 0.065f}, Vec3{actualSpacing + 0.02f, 0.14f, 0.03f}, woodCol);
            }
        }

        // Vertical face trim boards over post joints
        for (int i = 0; i < postCount; ++i) {
            float x = startX + i * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{x, H * 0.55f, 0.082f}, Vec3{0.10f, H * 0.85f, 0.02f}, woodCol);
        }

        // Top protective capping board (prevent horse chewing)
        if (params_.hasAccentDetail) {
            PropMeshHelper::appendBox(payload, Vec3{0.0f, H * 1.04f, 0.0f}, Vec3{L + 0.15f, 0.035f, 0.16f}, woodCol);
        }

    } else {
        // ---------------- 5: HIGH-TENSILE ELECTRIC LIVESTOCK FENCE ----------------
        uint32_t postCol = 0xFF3D2E22; // Treated round timber or fiberglass
        uint32_t ribbonCol = 0xFFF8FAFC; // High-vis white polytape ribbon
        uint32_t insulatorCol = 0xFFEAB308; // Bright yellow plastic standoff insulators

        for (int i = 0; i < postCount; ++i) {
            float x = startX + i * actualSpacing;
            PropMeshHelper::appendCylinder(payload, Vec3{x, 0.0f, 0.0f}, 0.06f, 0.055f, H * 1.05f, 10, postCol);
        }

        // 3 Electric polytape ribbons with yellow standoffs
        for (int s = 0; s < 3; ++s) {
            float y = H * (0.28f + 0.32f * s);
            // Insulator standoffs on each post
            for (int i = 0; i < postCount; ++i) {
                float x = startX + i * actualSpacing;
                PropMeshHelper::appendBox(payload, Vec3{x, y, 0.06f}, Vec3{0.035f, 0.035f, 0.08f}, insulatorCol);
            }
            // Running white conductive ribbon tape
            PropMeshHelper::appendBox(payload, Vec3{0.0f, y, 0.10f}, Vec3{L, 0.04f, 0.008f}, ribbonCol);
        }

        // Electric fence warning hazard sign
        if (params_.hasAccentDetail) {
            float signX = startX + actualSpacing * 0.5f;
            PropMeshHelper::appendBox(payload, Vec3{signX, H * 0.60f, 0.105f}, Vec3{0.22f, 0.12f, 0.01f}, 0xFFFACC15); // Yellow warning sign
            PropMeshHelper::appendBox(payload, Vec3{signX, H * 0.60f, 0.111f}, Vec3{0.08f, 0.06f, 0.005f}, 0xFF000000); // Lightning flash
        }
    }

    return payload;
}

} // namespace procedural::props
