#include "PrivacyFenceGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

PrivacyFenceGen::PrivacyFenceGen() = default;

bool PrivacyFenceGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void PrivacyFenceGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload PrivacyFenceGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float L = std::clamp(params_.length, 2.0f, 12.0f);
    float H = std::clamp(params_.height, 1.5f, 4.0f);
    float spacing = std::clamp(params_.postSpacing, 1.2f, 3.0f);

    int nPosts = static_cast<int>(L / spacing) + 1;
    if (nPosts < 2) nPosts = 2;
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    const bool isTan = (params_.colorScheme == 3);

    if (params_.fenceStyle == 0) {
        // Precast Concrete Highway / Sandstone Compound Wall
        uint32_t concCol = isTan ? 0xFFD2B48C : (params_.colorScheme == 1) ? 0xFFCBD5E1 : 0xFF94A3B8;
        uint32_t capCol  = isTan ? 0xFFE6CCB2 : concCol;
        uint32_t steelCol = isTan ? 0xFFB08968 : 0xFF334155;

        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.15f, 0.0f}, Vec3{L + 0.4f, 0.30f, 0.36f}, concCol);

        for (int p = 0; p < nPosts; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, H * 0.5f, 0.0f}, Vec3{0.20f, H, 0.16f}, steelCol);
            if (params_.hasDecorativeCaps) {
                PropMeshHelper::appendBox(payload, Vec3{px, H + 0.05f, 0.0f}, Vec3{0.26f, 0.07f, 0.22f}, capCol);
            }
        }

        for (int p = 0; p < nPosts - 1; ++p) {
            float pMid = startX + (p + 0.5f) * actualSpacing;
            float panelW = actualSpacing - 0.10f;
            float panelH = H - 0.30f;
            PropMeshHelper::appendBox(payload, Vec3{pMid, 0.30f + panelH * 0.5f, 0.0f}, Vec3{panelW, panelH, 0.12f}, concCol);
        }
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.04f, 0.0f}, Vec3{L + 0.2f, 0.08f, 0.22f}, capCol);
    } else if (params_.fenceStyle == 1) {
        // Cedar / Tan Tongue-and-Groove with Diamond Trellis
        uint32_t woodCol = isTan ? 0xFFD2B48C : (params_.colorScheme == 1) ? 0xFF94A3B8 : (params_.colorScheme == 2) ? 0xFF3E2723 : 0xFFB45309;
        uint32_t postCol = isTan ? 0xFFB08968 : (params_.colorScheme == 1) ? 0xFF64748B : (params_.colorScheme == 2) ? 0xFF1E293B : 0xFF78350F;

        for (int p = 0; p < nPosts; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, (H + 0.1f) * 0.5f, 0.0f}, Vec3{0.13f, H + 0.1f, 0.13f}, postCol);
            PropMeshHelper::appendBox(payload, Vec3{px, H + 0.07f, 0.0f}, Vec3{0.16f, 0.04f, 0.16f}, postCol);
            PropMeshHelper::appendBox(payload, Vec3{px, H + 0.11f, 0.0f}, Vec3{0.11f, 0.05f, 0.11f}, postCol);
        }

        float trellisH = 0.40f;
        float solidH = H - trellisH - 0.12f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.08f, 0.0f}, Vec3{L, 0.05f, 0.05f}, postCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.08f + solidH, 0.0f}, Vec3{L, 0.05f, 0.05f}, postCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H, 0.0f}, Vec3{L, 0.05f, 0.05f}, postCol);

        int nBoards = static_cast<int>(L / 0.13f);
        for (int b = 0; b <= nBoards; ++b) {
            float bx = startX + b * 0.13f;
            PropMeshHelper::appendBox(payload, Vec3{bx, 0.08f + solidH * 0.5f, 0.0f}, Vec3{0.125f, solidH, 0.025f}, woodCol);
        }

        int nLattice = static_cast<int>(L / 0.18f);
        for (int lt = 0; lt < nLattice; ++lt) {
            float lx = startX + (lt + 0.5f) * 0.18f;
            PropMeshHelper::appendStrut(payload, Vec3{lx - 0.09f, 0.08f + solidH, -0.008f}, Vec3{lx + 0.09f, H, -0.008f}, 0.015f, woodCol);
            PropMeshHelper::appendStrut(payload, Vec3{lx - 0.09f, H,  0.008f}, Vec3{lx + 0.09f, 0.08f + solidH,  0.008f}, 0.015f, woodCol);
        }
    } else {
        // Horizontal Slat / Louvered Blade Privacy
        uint32_t slatCol = isTan ? 0xFFD2B48C : (params_.colorScheme == 1) ? 0xFF475569 : 0xFF1E293B;
        uint32_t postCol = isTan ? 0xFF8C6D46 : 0xFF0F172A;
        for (int p = 0; p < nPosts; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, H * 0.5f, 0.0f}, Vec3{0.10f, H, 0.10f}, postCol);
            if (params_.hasDecorativeCaps) {
                PropMeshHelper::appendBox(payload, Vec3{px, H + 0.03f, 0.0f}, Vec3{0.13f, 0.04f, 0.13f}, slatCol);
            }
        }
        float slatH = 0.12f; float gap = 0.018f;
        int totalSlats = static_cast<int>((H - 0.08f) / (slatH + gap));
        for (int s = 0; s < totalSlats; ++s) {
            float sy = 0.06f + s * (slatH + gap) + slatH * 0.5f;
            PropMeshHelper::appendBox(payload, Vec3{0.0f, sy, 0.0f}, Vec3{L, slatH, 0.035f}, slatCol);
        }
    }

    return payload;
}

} // namespace procedural::props
