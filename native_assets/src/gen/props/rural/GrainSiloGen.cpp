#include "GrainSiloGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

GrainSiloGen::GrainSiloGen() = default;

bool GrainSiloGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void GrainSiloGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload GrainSiloGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float totalH = std::max(params_.totalHeight, 4.0f);
    float D = std::max(params_.diameter, 2.0f);
    float R = D * 0.5f;
    float wallH = std::clamp(params_.wallHeight, 2.5f, totalH - 1.2f);
    float roofH = totalH - wallH;

    uint32_t binCol = (params_.finishType == 1) ? 0xFFB91C1C : // Barn Red
                      (params_.finishType == 2) ? 0xFFF8FAFC : // Silo White
                                                  0xFFCBD5E1;  // Galvanized Zinc

    uint32_t seamCol = 0xFF94A3B8;
    uint32_t concreteCol = 0xFF64748B;

    // 1. Concrete foundation footing ring
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, R * 1.08f, R * 1.08f, 0.35f, 20, concreteCol);

    // 2. Corrugated cylindrical body with horizontal seam tiers
    int tiers = std::clamp(params_.ringTiers, 3, 10);
    float tierH = wallH / tiers;
    for (int t = 0; t < tiers; ++t) {
        float ty = 0.35f + t * tierH;
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, ty, 0.0f}, R, R, tierH * 0.95f, 20, binCol);
        // Horizontal corrugated overlap band
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, ty + tierH * 0.95f, 0.0f}, R * 1.02f, R * 1.02f, tierH * 0.05f, 20, seamCol);
    }

    // 3. Conical corrugated roof
    float roofBaseY = 0.35f + wallH;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, roofBaseY, 0.0f}, R * 1.04f, 0.30f, roofH - 0.30f, 20, binCol);
    // Peak ventilation cap & inspection hatch
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, roofBaseY + roofH - 0.30f, 0.0f}, 0.35f, 0.20f, 0.30f, 12, seamCol);
    PropMeshHelper::appendSphere(payload, Vec3{0.0f, roofBaseY + roofH, 0.0f}, 0.22f, 6, 8, seamCol);

    // 4. Exterior caged inspection ladder (if enabled)
    if (params_.hasCageLadder) {
        uint32_t ladderCol = 0xFF475569;
        float ladderAngle = 0.0f; // along +Z
        float lx = 0.0f;
        float lz = R + 0.12f;

        // Side rails
        PropMeshHelper::appendBox(payload, Vec3{lx - 0.20f, (roofBaseY + 0.35f) * 0.5f, lz}, Vec3{0.03f, roofBaseY, 0.04f}, ladderCol);
        PropMeshHelper::appendBox(payload, Vec3{lx + 0.20f, (roofBaseY + 0.35f) * 0.5f, lz}, Vec3{0.03f, roofBaseY, 0.04f}, ladderCol);

        // Ladder rungs every 30cm
        int rungs = static_cast<int>(roofBaseY / 0.30f);
        for (int r = 1; r <= rungs; ++r) {
            float ry = r * 0.30f;
            PropMeshHelper::appendBox(payload, Vec3{lx, ry, lz}, Vec3{0.40f, 0.02f, 0.02f}, ladderCol);
        }

        // Safety cage hoops above 2.2m
        int hoops = static_cast<int>((roofBaseY - 2.2f) / 0.70f);
        for (int h = 0; h < hoops; ++h) {
            float hy = 2.2f + h * 0.70f;
            PropMeshHelper::appendBox(payload, Vec3{lx, hy, lz + 0.35f}, Vec3{0.50f, 0.04f, 0.02f}, ladderCol);
            PropMeshHelper::appendBox(payload, Vec3{lx - 0.25f, hy, lz + 0.18f}, Vec3{0.02f, 0.04f, 0.35f}, ladderCol);
            PropMeshHelper::appendBox(payload, Vec3{lx + 0.25f, hy, lz + 0.18f}, Vec3{0.02f, 0.04f, 0.35f}, ladderCol);
        }

        // Ground discharge auger chute pipe
        PropMeshHelper::appendBox(payload, Vec3{R * 0.65f, 0.75f, -R * 0.65f}, Vec3{1.4f, 0.22f, 0.22f}, seamCol);
        // Ground access manhole door
        PropMeshHelper::appendBox(payload, Vec3{lx, 1.10f, R}, Vec3{0.55f, 0.85f, 0.06f}, seamCol);
    }

    return payload;
}

} // namespace procedural::props
