#include "BollardGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

BollardGen::BollardGen() = default;

bool BollardGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void BollardGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload BollardGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float H = std::max(params_.height, 0.5f);
    float R = std::max(params_.radius, 0.05f);
    int count = std::clamp(params_.bollardCount, 1, 5);
    float spacing = std::max(params_.spacing, R * 3.0f);
    float startX = -((count - 1) * spacing) * 0.5f;

    for (int b = 0; b < count; ++b) {
        float bx = startX + b * spacing;

        if (params_.style == 0) {
            // Style 0: Stainless Steel Security Ram Bollard
            uint32_t steelCol = 0xFFD1D5DB;
            uint32_t ringCol = 0xFFDC2626; // Red retroreflective band
            // Inset pavement collar
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.0f, 0.0f}, R * 1.3f, R * 1.3f, 0.04f, 16, 0xFF4B5563);
            // Main cylinder
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.04f, 0.0f}, R, R, H * 0.85f, 16, steelCol);
            // Reflective tape ring
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.04f + H * 0.85f, 0.0f}, R * 1.01f, R * 1.01f, 0.06f, 16, ringCol);
            // Dome/flat top cap
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.10f + H * 0.85f, 0.0f}, R, R * 0.95f, 0.04f, 16, steelCol);
            PropMeshHelper::appendBox(payload, Vec3{bx, H + 0.02f, 0.0f}, Vec3{R * 1.4f, 0.02f, R * 1.4f}, steelCol);

        } else if (params_.style == 1) {
            // Style 1: Heritage Cast Iron Fluted Bollard
            uint32_t ironCol = 0xFF1F242C;
            uint32_t goldCol = 0xFFD97706;
            // Octagonal molded base
            PropMeshHelper::appendBox(payload, Vec3{bx, 0.08f, 0.0f}, Vec3{R * 2.5f, 0.16f, R * 2.5f}, ironCol);
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.16f, 0.0f}, R * 1.25f, R, 0.12f, 16, ironCol);
            // Fluted tapered shaft
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.28f, 0.0f}, R, R * 0.85f, H - 0.42f, 16, ironCol);
            // Molded collar rings & acorn / sphere finial
            PropMeshHelper::appendCylinder(payload, Vec3{bx, H - 0.14f, 0.0f}, R * 1.05f, R * 1.05f, 0.05f, 16, goldCol);
            PropMeshHelper::appendSphere(payload, Vec3{bx, H, 0.0f}, R * 0.75f, 8, 12, ironCol);

            // Side chain eyelets
            if (params_.hasConnectingChains) {
                PropMeshHelper::appendBox(payload, Vec3{bx + R * 0.95f, H - 0.18f, 0.0f}, Vec3{0.06f, 0.06f, 0.03f}, ironCol);
                PropMeshHelper::appendBox(payload, Vec3{bx - R * 0.95f, H - 0.18f, 0.0f}, Vec3{0.06f, 0.06f, 0.03f}, ironCol);
            }

        } else if (params_.style == 2) {
            // Style 2: Hydraulic Automatic Retractable Bollard
            uint32_t collarCol = 0xFF374151;
            uint32_t cylCol = 0xFF9CA3AF;
            // Flush road casing ring
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.0f, 0.0f}, R * 1.45f, R * 1.45f, 0.03f, 16, collarCol);
            // Rising stainless cylinder
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.03f, 0.0f}, R, R, H, 16, cylCol);
            // Top crown with amber LED warning ring
            PropMeshHelper::appendCylinder(payload, Vec3{bx, H, 0.0f}, R * 1.02f, R * 1.02f, 0.03f, 16, 0xFFF59E0B);
            PropMeshHelper::appendBox(payload, Vec3{bx, H + 0.02f, 0.0f}, Vec3{R * 1.5f, 0.015f, R * 1.5f}, collarCol);

        } else if (params_.style == 3) {
            // Style 3: Flexible Polyurethane Rebound Delineator
            uint32_t polyCol = 0xFFF97316; // Traffic Orange
            uint32_t stripeCol = 0xFFFFFFFF; // White retro-reflective
            // Flexible base foot
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.0f, 0.0f}, R * 1.6f, R * 1.4f, 0.05f, 14, 0xFF1F2937);
            // Cylindrical body
            PropMeshHelper::appendCylinder(payload, Vec3{bx, 0.05f, 0.0f}, R, R * 0.90f, H - 0.05f, 14, polyCol);
            // 2 Reflective bands
            PropMeshHelper::appendCylinder(payload, Vec3{bx, H * 0.65f, 0.0f}, R * 0.96f, R * 0.96f, 0.08f, 14, stripeCol);
            PropMeshHelper::appendCylinder(payload, Vec3{bx, H * 0.82f, 0.0f}, R * 0.93f, R * 0.93f, 0.08f, 14, stripeCol);

        } else {
            // Style 4: Architectural Illuminated Path Bollard
            uint32_t bodyCol = 0xFF1E293B; // Dark Graphite
            uint32_t lightCol = 0xFFFEF08A; // Warm LED glow
            // Square column
            PropMeshHelper::appendBox(payload, Vec3{bx, H * 0.42f, 0.0f}, Vec3{R * 1.8f, H * 0.84f, R * 1.8f}, bodyCol);
            // Louver recess window
            PropMeshHelper::appendBox(payload, Vec3{bx, H * 0.85f, 0.0f}, Vec3{R * 1.6f, 0.12f, R * 1.6f}, lightCol);
            // Hooded angled cap
            PropMeshHelper::appendBox(payload, Vec3{bx, H + 0.02f, 0.0f}, Vec3{R * 2.0f, 0.04f, R * 2.0f}, bodyCol);
        }
    }

    // Heavy connecting chains between bollards (if enabled and count > 1)
    if (params_.hasConnectingChains && count > 1) {
        uint32_t chainCol = 0xFF475569;
        for (int b = 0; b < count - 1; ++b) {
            float x1 = startX + b * spacing;
            float x2 = startX + (b + 1) * spacing;
            float midX = (x1 + x2) * 0.5f;
            float chainY = H * 0.68f;
            // Catenary dip chain segments
            PropMeshHelper::appendBox(payload, Vec3{midX, chainY - 0.10f, 0.0f}, Vec3{spacing * 0.92f, 0.025f, 0.025f}, chainCol);
        }
    }

    return payload;
}

} // namespace procedural::props
