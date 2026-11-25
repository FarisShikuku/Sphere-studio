#include "KilometreMarkerGen.h"

namespace procedural::props {

KilometreMarkerGen::KilometreMarkerGen() {
    params_ = KilometreMarkerParams{};
}

bool KilometreMarkerGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void KilometreMarkerGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload KilometreMarkerGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;

    float h = params_.height;
    float w = params_.width;
    float d = params_.depth;

    if (params_.markerStyle == 1) {
        // Modern highway guide post / delineator
        uint32_t plasticWhite = 0xFFFFFFFF;
        uint32_t capBlack = 0xFF1F2937;
        uint32_t amberReflector = 0xFFF59E0B;
        uint32_t redReflector = 0xFFEF4444;

        // Ground socket
        PropMeshHelper::appendBox(payload, Vec3{0.0f, 0.05f, 0.0f}, Vec3{w * 0.8f, 0.1f, d * 0.8f}, 0xFF444444);

        // White hollow plastic extrusion post
        PropMeshHelper::appendBox(payload, Vec3{0.0f, h * 0.5f, 0.0f}, Vec3{0.14f, h, 0.06f}, plasticWhite);

        // Sloped black top cap
        PropMeshHelper::appendBox(payload, Vec3{0.0f, h + 0.08f, 0.0f}, Vec3{0.15f, 0.16f, 0.07f}, capBlack);

        // Front & back retroreflective badges
        PropMeshHelper::appendBox(payload, Vec3{0.0f, h - 0.15f, 0.035f}, Vec3{0.06f, 0.18f, 0.01f}, amberReflector);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, h - 0.15f, -0.035f}, Vec3{0.06f, 0.18f, 0.01f}, redReflector);
    } else {
        // Historic arched carved stone milestone
        uint32_t stoneColor = 0xFFD1D5DB; // Weathered limestone
        uint32_t capColor = 0xFFDC2626;   // Highway route cap red
        uint32_t carvedText = 0xFF1F2937; // Carved dark numerals

        // Rectangular stone pillar body
        float bodyH = h * 0.72f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, bodyH * 0.5f, 0.0f}, Vec3{w, bodyH, d}, stoneColor);

        // Semicircular rounded / arched milestone dome crown
        float crownR = w * 0.5f;
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, bodyH, 0.0f}, crownR, crownR * 0.2f, h - bodyH, 16, capColor);

        // Carved distance numeral badge
        PropMeshHelper::appendBox(payload, Vec3{0.0f, bodyH * 0.55f, d * 0.5f + 0.005f}, Vec3{w * 0.65f, 0.22f, 0.01f}, carvedText);

        // Route indicator band
        PropMeshHelper::appendBox(payload, Vec3{0.0f, bodyH * 0.85f, d * 0.5f + 0.005f}, Vec3{w * 0.75f, 0.12f, 0.01f}, capColor);
    }

    return payload;
}

} // namespace procedural::props
