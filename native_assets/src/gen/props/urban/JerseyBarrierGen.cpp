#include "JerseyBarrierGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

JerseyBarrierGen::JerseyBarrierGen() = default;

bool JerseyBarrierGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void JerseyBarrierGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload JerseyBarrierGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float L = std::max(params_.length, 1.0f);
    float H = std::max(params_.height, 0.5f);
    float W = std::max(params_.baseWidth, 0.35f);

    uint32_t bodyCol = (params_.colorScheme == 1) ? 0xFFEA580C : // Safety Orange
                       (params_.colorScheme == 2) ? 0xFFFACC15 : // Safety Yellow
                       (params_.barrierStyle == 1) ? 0xFFDC2626 : // Plastic Red/Orange
                                                     0xFF94A3B8;  // Concrete Grey

    // Profile of a standard Jersey Barrier:
    // 1. Lower vertical base curb (h = 0.08m, w = W)
    // 2. Lower sloped break (35-deg bevel to w = W * 0.70m, h = 0.25m)
    // 3. Upper stem vertical taper (w = W * 0.35m, h = H)
    float baseH = 0.08f;
    float bevelH = 0.25f;
    float stemH = H - (baseH + bevelH);

    // Base rectangular curb
    PropMeshHelper::appendBox(payload, Vec3{0.0f, baseH * 0.5f, 0.0f}, Vec3{L, baseH, W}, bodyCol);

    // Bevel transition tier
    PropMeshHelper::appendBox(payload, Vec3{0.0f, baseH + bevelH * 0.5f, 0.0f}, Vec3{L, bevelH, W * 0.70f}, bodyCol);

    // Upper vertical stem
    PropMeshHelper::appendBox(payload, Vec3{0.0f, baseH + bevelH + stemH * 0.5f, 0.0f}, Vec3{L, stemH, W * 0.35f}, bodyCol);

    // Rounded / beveled top crest
    PropMeshHelper::appendBox(payload, Vec3{0.0f, H + 0.015f, 0.0f}, Vec3{L, 0.03f, W * 0.30f}, bodyCol);

    // Pin connection loop ears at both ends (for linking modular barriers)
    uint32_t pinCol = 0xFF475569;
    PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f - 0.03f, H * 0.5f, 0.0f}, Vec3{0.06f, 0.12f, 0.08f}, pinCol);
    PropMeshHelper::appendBox(payload, Vec3{ L * 0.5f + 0.03f, H * 0.5f, 0.0f}, Vec3{0.06f, 0.12f, 0.08f}, pinCol);

    // Reflective delineators and forklift drainage scuppers
    if (params_.hasReflectors) {
        // High-vis delineator reflectors on top edge
        int delinCount = std::max(2, static_cast<int>(L / 1.0f));
        for (int i = 0; i < delinCount; ++i) {
            float dx = -L * 0.45f + i * (L * 0.90f / (delinCount - 1));
            // Amber/Red trapezoid reflective reflector tab
            PropMeshHelper::appendBox(payload, Vec3{dx, H + 0.07f, 0.0f}, Vec3{0.08f, 0.09f, 0.02f}, 0xFFF59E0B);
        }

        // Base forklift slot / drainage scallop cutouts
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.25f, 0.03f, 0.0f}, Vec3{0.25f, 0.06f, W * 1.02f}, 0xFF1E293B);
        PropMeshHelper::appendBox(payload, Vec3{ L * 0.25f, 0.03f, 0.0f}, Vec3{0.25f, 0.06f, W * 1.02f}, 0xFF1E293B);
    }

    // Optional Construction Mesh Topper (Style 2)
    if (params_.barrierStyle == 2) {
        uint32_t meshCol = 0xFFCBD5E1;
        float topperH = 0.90f;
        // Posts mounted on top of barrier
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.45f, H + topperH * 0.5f, 0.0f}, Vec3{0.04f, topperH, 0.04f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, Vec3{ 0.0f,       H + topperH * 0.5f, 0.0f}, Vec3{0.04f, topperH, 0.04f}, 0xFF475569);
        PropMeshHelper::appendBox(payload, Vec3{ L * 0.45f,  H + topperH * 0.5f, 0.0f}, Vec3{0.04f, topperH, 0.04f}, 0xFF475569);
        // Top rail
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H + topperH, 0.0f}, Vec3{L, 0.03f, 0.03f}, 0xFF475569);
        // Wire mesh screen
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H + topperH * 0.5f, 0.0f}, Vec3{L * 0.95f, topperH * 0.9f, 0.01f}, meshCol);
    }

    return payload;
}

} // namespace procedural::props
