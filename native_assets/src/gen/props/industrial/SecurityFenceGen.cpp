#include "SecurityFenceGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

SecurityFenceGen::SecurityFenceGen() = default;

bool SecurityFenceGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void SecurityFenceGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload SecurityFenceGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float L = std::clamp(params_.length, 2.0f, 15.0f);
    float H = std::clamp(params_.height, 1.8f, 5.0f);
    float spacing = std::clamp(params_.postSpacing, 1.5f, 3.2f);

    uint32_t metalCol = (params_.colorScheme == 1) ? 0xFF1E2229 : // Security Black
                        (params_.colorScheme == 2) ? 0xFF3F4E3A : // Military Olive
                        (params_.colorScheme == 3) ? 0xFFEAB308 : // Safety Yellow
                        (params_.colorScheme == 4) ? 0xFFD2B48C : // Desert Tan
                                                     0xFF94A3B8;  // Galvanized Zinc

    int nPosts = static_cast<int>(L / spacing) + 1;
    if (nPosts < 2) nPosts = 2;
    float actualSpacing = L / (nPosts - 1);
    float startX = -L * 0.5f;

    if (params_.fenceStyle == 0) {
        // 358 Anti-Climb Welded Wire Mesh
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.12f, 0.0f}, Vec3{L + 0.4f, 0.24f, 0.28f}, 0xFF64748B);

        for (int p = 0; p < nPosts; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.24f + (H - 0.24f) * 0.5f, 0.0f}, Vec3{0.12f, H - 0.24f, 0.12f}, metalCol);
            PropMeshHelper::appendBox(payload, Vec3{px, 0.25f, 0.0f}, Vec3{0.20f, 0.03f, 0.20f}, metalCol);
            PropMeshHelper::appendBox(payload, Vec3{px, 0.24f + (H - 0.24f) * 0.5f, 0.065f}, Vec3{0.045f, H - 0.24f, 0.015f}, metalCol);

            PropMeshHelper::appendStrut(payload, Vec3{px, H, 0.0f}, Vec3{px, H + 0.42f, -0.32f}, 0.035f, metalCol);
            PropMeshHelper::appendStrut(payload, Vec3{px, H, 0.0f}, Vec3{px, H + 0.42f,  0.32f}, 0.035f, metalCol);
        }

        float meshH = H - 0.26f;
        float meshMidY = 0.24f + meshH * 0.5f;
        int vertWires = static_cast<int>(L / 0.08f);
        for (int v = 0; v <= vertWires; ++v) {
            float vx = startX + v * 0.08f;
            PropMeshHelper::appendBox(payload, Vec3{vx, meshMidY, 0.0f}, Vec3{0.008f, meshH, 0.008f}, metalCol);
        }
        int horizWires = static_cast<int>(meshH / 0.04f);
        for (int hz = 0; hz <= horizWires; ++hz) {
            float hy = 0.24f + hz * 0.04f;
            PropMeshHelper::appendBox(payload, Vec3{0.0f, hy, 0.0f}, Vec3{L, 0.008f, 0.008f}, metalCol);
        }

        if (params_.hasRazorWire) {
            int coilDivs = static_cast<int>(L / 0.30f);
            float coilR = 0.24f;
            for (int c = 0; c < coilDivs; ++c) {
                float cx = startX + (c + 0.5f) * (L / coilDivs);
                PropMeshHelper::appendCylinder(payload, Vec3{cx, H + 0.22f, 0.0f}, coilR, coilR, 0.02f, 10, 0xFFE2E8F0);
                PropMeshHelper::appendBox(payload, Vec3{cx, H + 0.22f + coilR, 0.0f}, Vec3{0.04f, 0.01f, 0.03f}, 0xFFF1F5F9);
            }
        }
    } else if (params_.fenceStyle == 1) {
        // Heavy Palisade Steel Trident Fence
        for (int p = 0; p < nPosts; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.02f, 0.0f}, Vec3{0.24f, 0.04f, 0.24f}, 0xFF334155);
            PropMeshHelper::appendBox(payload, Vec3{px, H * 0.5f, 0.0f}, Vec3{0.14f, H, 0.08f}, metalCol);
        }
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H * 0.22f, 0.0f}, Vec3{L, 0.06f, 0.06f}, metalCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H * 0.82f, 0.0f}, Vec3{L, 0.06f, 0.06f}, metalCol);

        int paleCount = static_cast<int>(L / 0.15f);
        float paleH = H - 0.18f;
        for (int i = 0; i <= paleCount; ++i) {
            float px = startX + i * 0.15f;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.08f + paleH * 0.5f, 0.035f}, Vec3{0.075f, paleH, 0.02f}, metalCol);
            PropMeshHelper::appendBox(payload, Vec3{px, 0.08f + paleH + 0.10f, 0.035f}, Vec3{0.025f, 0.20f, 0.02f}, metalCol);
            PropMeshHelper::appendBox(payload, Vec3{px - 0.035f, 0.08f + paleH + 0.05f, 0.035f}, Vec3{0.018f, 0.12f, 0.02f}, metalCol);
            PropMeshHelper::appendBox(payload, Vec3{px + 0.035f, 0.08f + paleH + 0.05f, 0.035f}, Vec3{0.018f, 0.12f, 0.02f}, metalCol);
        }
    } else {
        // High-Voltage Electric Perimeter Array
        for (int p = 0; p < nPosts; ++p) {
            float px = startX + p * actualSpacing;
            PropMeshHelper::appendBox(payload, Vec3{px, 0.06f, 0.0f}, Vec3{0.25f, 0.12f, 0.25f}, 0xFF475569);
            PropMeshHelper::appendBox(payload, Vec3{px, H * 0.5f, 0.0f}, Vec3{0.08f, H, 0.08f}, metalCol);
        }
        for (int w = 0; w < 8; ++w) {
            float wy = 0.20f + w * ((H - 0.30f) / 7.0f);
            PropMeshHelper::appendBox(payload, Vec3{0.0f, wy, 0.19f}, Vec3{L, 0.008f, 0.008f}, 0xFFF1F5F9);
        }
    }

    return payload;
}

} // namespace procedural::props
