#include "ExpandableBarricadeGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

ExpandableBarricadeGen::ExpandableBarricadeGen() = default;

bool ExpandableBarricadeGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void ExpandableBarricadeGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload ExpandableBarricadeGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float L = std::clamp(params_.span, 1.5f, 6.0f);
    float H = std::clamp(params_.height, 0.8f, 1.8f);
    float footW = std::clamp(params_.footWidth, 0.35f, 0.80f);

    uint32_t baseCol = (params_.colorScheme == 1) ? 0xFFFACC15 : // Traffic Yellow
                       (params_.colorScheme == 2) ? 0xFFCBD5E1 : // Galvanized Silver
                       (params_.colorScheme == 3) ? 0xFFDC2626 : // Traffic Red
                                                    0xFFEA580C;  // Safety Orange

    if (params_.barricadeStyle == 0) {
        // Steel Crowd Control Bike Rack
        uint32_t galvCol = (params_.colorScheme == 0 || params_.colorScheme == 1) ? baseCol : 0xFF94A3B8;
        float tubeR = 0.022f;

        // Perimeter frame
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H, 0.0f}, Vec3{L, tubeR * 2, tubeR * 2}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.15f, 0.0f}, Vec3{L, tubeR * 2, tubeR * 2}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f + tubeR, H * 0.5f + 0.075f, 0.0f}, Vec3{tubeR * 2, H - 0.15f, tubeR * 2}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{ L * 0.5f - tubeR, H * 0.5f + 0.075f, 0.0f}, Vec3{tubeR * 2, H - 0.15f, tubeR * 2}, galvCol);

        // Bridge feet
        for (float fx : {-L * 0.35f, L * 0.35f}) {
            PropMeshHelper::appendBox(payload, Vec3{fx, 0.08f, 0.0f}, Vec3{0.045f, 0.16f, 0.045f}, galvCol);
            PropMeshHelper::appendBox(payload, Vec3{fx, 0.02f, 0.0f}, Vec3{0.06f, 0.04f, footW}, galvCol);
            PropMeshHelper::appendBox(payload, Vec3{fx, 0.005f, -footW * 0.45f}, Vec3{0.08f, 0.015f, 0.08f}, 0xFF1E293B);
            PropMeshHelper::appendBox(payload, Vec3{fx, 0.005f,  footW * 0.45f}, Vec3{0.08f, 0.015f, 0.08f}, 0xFF1E293B);
        }

        // Pickets
        int nPickets = static_cast<int>(L / 0.12f);
        float innerH = H - 0.20f;
        for (int i = 1; i < nPickets; ++i) {
            float px = -L * 0.5f + i * (L / nPickets);
            PropMeshHelper::appendBox(payload, Vec3{px, 0.15f + innerH * 0.5f, 0.0f}, Vec3{0.014f, innerH, 0.014f}, galvCol);
        }

        // Hooks and loops
        PropMeshHelper::appendBox(payload, Vec3{L * 0.5f + 0.04f, H * 0.75f, 0.0f}, Vec3{0.08f, 0.02f, 0.02f}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{L * 0.5f + 0.07f, H * 0.70f, 0.0f}, Vec3{0.02f, 0.10f, 0.02f}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{L * 0.5f + 0.04f, H * 0.35f, 0.0f}, Vec3{0.08f, 0.02f, 0.02f}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{L * 0.5f + 0.07f, H * 0.30f, 0.0f}, Vec3{0.02f, 0.10f, 0.02f}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f - 0.03f, H * 0.75f, 0.0f}, Vec3{0.06f, 0.035f, 0.05f}, galvCol);
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f - 0.03f, H * 0.35f, 0.0f}, Vec3{0.06f, 0.035f, 0.05f}, galvCol);

        if (params_.hasFlashers) {
            PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.10f, 0.0f}, Vec3{0.12f, 0.16f, 0.08f}, 0xFFF59E0B);
            PropMeshHelper::appendSphere(payload, Vec3{0.0f, H + 0.20f, 0.0f}, 0.06f, 6, 8, 0xFFFEF08A);
        }
    } else if (params_.barricadeStyle == 1) {
        // Scissor Accordion Gate
        for (float side : {-1.0f, 1.0f}) {
            float px = side * (L * 0.5f - 0.06f);
            PropMeshHelper::appendBox(payload, Vec3{px, H * 0.5f, 0.0f}, Vec3{0.08f, H, 0.08f}, baseCol);
            PropMeshHelper::appendBox(payload, Vec3{px, 0.06f, 0.0f}, Vec3{0.06f, 0.06f, footW}, baseCol);
            PropMeshHelper::appendCylinder(payload, Vec3{px, 0.03f, -footW * 0.4f}, 0.035f, 0.035f, 0.03f, 8, 0xFF334155);
            PropMeshHelper::appendCylinder(payload, Vec3{px, 0.03f,  footW * 0.4f}, 0.035f, 0.035f, 0.03f, 8, 0xFF334155);
            PropMeshHelper::appendBox(payload, Vec3{px + side * 0.05f, H * 0.75f, 0.0f}, Vec3{0.08f, 0.025f, 0.025f}, 0xFF1E293B);
        }

        int numDiamonds = std::max(3, static_cast<int>(L / 0.35f));
        float diamondW = (L - 0.2f) / numDiamonds;
        for (int d = 0; d < numDiamonds; ++d) {
            float x0 = -L * 0.5f + 0.1f + d * diamondW;
            float x1 = x0 + diamondW;
            PropMeshHelper::appendStrut(payload, Vec3{x0, 0.15f, -0.01f}, Vec3{x1, H * 0.95f, -0.01f}, 0.024f, baseCol);
            PropMeshHelper::appendStrut(payload, Vec3{x0, H * 0.95f,  0.01f}, Vec3{x1, 0.15f,  0.01f}, 0.024f, 0xFF1E293B);
            float midX = (x0 + x1) * 0.5f;
            float midY = (0.15f + H * 0.95f) * 0.5f;
            PropMeshHelper::appendBox(payload, Vec3{midX, midY, 0.0f}, Vec3{0.025f, 0.025f, 0.035f}, 0xFFF1F5F9);
        }

        if (params_.hasFlashers) {
            for (float fx : {-L * 0.3f, L * 0.3f}) {
                PropMeshHelper::appendBox(payload, Vec3{fx, H + 0.08f, 0.0f}, Vec3{0.12f, 0.14f, 0.06f}, 0xFFEA580C);
                PropMeshHelper::appendSphere(payload, Vec3{fx, H + 0.16f, 0.0f}, 0.05f, 6, 8, 0xFFF59E0B);
            }
        }
    } else if (params_.barricadeStyle == 2) {
        // Water-Filled Plastic Barrier
        int numModules = std::max(1, static_cast<int>(L / 1.4f));
        float modL = L / numModules;
        for (int m = 0; m < numModules; ++m) {
            float mx = -L * 0.5f + (m + 0.5f) * modL;
            uint32_t mCol = (m % 2 == 0) ? baseCol : 0xFFF8FAFC;
            float bH0 = H * 0.25f; float bH1 = H * 0.65f; float bH2 = H * 0.10f;
            PropMeshHelper::appendBox(payload, Vec3{mx, bH0 * 0.5f, 0.0f}, Vec3{modL - 0.04f, bH0, footW}, mCol);
            PropMeshHelper::appendBox(payload, Vec3{mx, bH0 + bH1 * 0.5f, 0.0f}, Vec3{modL - 0.06f, bH1, footW * 0.60f}, mCol);
            PropMeshHelper::appendBox(payload, Vec3{mx, bH0 + bH1 + bH2 * 0.5f, 0.0f}, Vec3{modL - 0.08f, bH2, footW * 0.40f}, mCol);
            PropMeshHelper::appendCylinder(payload, Vec3{mx, H, 0.0f}, 0.06f, 0.06f, 0.03f, 8, 0xFF1E293B);
            PropMeshHelper::appendBox(payload, Vec3{mx, H * 0.60f, footW * 0.31f}, Vec3{modL * 0.6f, 0.08f, 0.01f}, 0xFFFEF08A);
            PropMeshHelper::appendCylinder(payload, Vec3{mx + modL * 0.48f, 0.05f, 0.0f}, 0.035f, 0.035f, H * 0.8f, 8, 0xFF475569);
        }
    } else {
        // A-Frame Traffic Barricade
        for (float side : {-1.0f, 1.0f}) {
            float lx = side * (L * 0.4f);
            PropMeshHelper::appendStrut(payload, Vec3{lx, 0.0f, -footW * 0.45f}, Vec3{lx, H, 0.0f}, 0.045f, 0xFFFFFFFF);
            PropMeshHelper::appendStrut(payload, Vec3{lx, 0.0f,  footW * 0.45f}, Vec3{lx, H, 0.0f}, 0.045f, 0xFFFFFFFF);
            PropMeshHelper::appendBox(payload, Vec3{lx, H * 0.25f, 0.0f}, Vec3{0.045f, 0.045f, footW * 0.55f}, 0xFFFFFFFF);
        }
        for (float by : {H * 0.45f, H * 0.80f}) {
            PropMeshHelper::appendBox(payload, Vec3{0.0f, by, 0.0f}, Vec3{L, 0.20f, 0.035f}, baseCol);
        }
    }

    return payload;
}

} // namespace procedural::props
