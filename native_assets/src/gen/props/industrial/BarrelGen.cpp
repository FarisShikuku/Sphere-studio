#include "BarrelGen.h"

namespace procedural::props {

BarrelGen::BarrelGen() = default;

bool BarrelGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void BarrelGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload BarrelGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    if (params_.barrelType == 1) {
        // Wooden Cask with bulging belly staves and iron hoops
        uint32_t woodCol = 0xFF8A5A36;
        uint32_t hoopCol = 0xFF2A2D30;

        int slices = 6;
        float sliceH = params_.height / slices;
        float rEnd = params_.radius * 0.88f;
        float rMid = params_.radius * 1.12f;

        for (int i = 0; i < slices; ++i) {
            float t0 = static_cast<float>(i) / slices;
            float t1 = static_cast<float>(i + 1) / slices;
            float y0 = i * sliceH;

            // Parabolic belly curve
            float curve0 = 1.0f - 4.0f * (t0 - 0.5f) * (t0 - 0.5f);
            float curve1 = 1.0f - 4.0f * (t1 - 0.5f) * (t1 - 0.5f);
            float r0 = rEnd + (rMid - rEnd) * curve0;
            float r1 = rEnd + (rMid - rEnd) * curve1;

            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, y0, 0.0f}, r0, r1, sliceH, 18, woodCol, (i == 0), (i == slices - 1));
        }

        // 4 Steel hoops binding the barrel
        float hoopYs[4] = {params_.height * 0.12f, params_.height * 0.32f, params_.height * 0.68f, params_.height * 0.88f};
        for (float hy : hoopYs) {
            float t = hy / params_.height;
            float curve = 1.0f - 4.0f * (t - 0.5f) * (t - 0.5f);
            float rH = (rEnd + (rMid - rEnd) * curve) * 1.03f;
            PropMeshHelper::appendCylinder(payload, Vec3{0.0f, hy - 0.015f, 0.0f}, rH, rH, 0.03f, 18, hoopCol, false, false);
        }

        // Top wooden bung tap
        PropMeshHelper::appendCylinder(payload, Vec3{rMid * 0.98f, params_.height * 0.5f, 0.0f}, 0.03f, 0.02f, 0.04f, 8, hoopCol);
    } else {
        // 55-Gallon Standard Industrial Drum with rolling chimes / ribs
        uint32_t drumCol = (params_.drumColor == 0) ? 0xFF1853A3 : // Blue
                           (params_.drumColor == 1) ? 0xFFBA281C : // Red
                           (params_.drumColor == 2) ? 0xFFD8B018 : // Yellow
                                                      0xFF1C1E20;  // Black

        // Main cylindrical barrel
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.02f, 0.0f}, params_.radius, params_.radius, params_.height * 0.96f, 20, drumCol);

        // Top & Bottom rolled rim chimes
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, params_.radius * 1.04f, params_.radius * 1.04f, 0.035f, 20, 0xFF333333);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, params_.height * 0.965f, 0.0f}, params_.radius * 1.04f, params_.radius * 1.04f, 0.035f, 20, 0xFF333333);

        // 2 Center rolled expansion ribs
        float rib1Y = params_.height * 0.33f;
        float rib2Y = params_.height * 0.66f;
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, rib1Y, 0.0f}, params_.radius * 1.05f, params_.radius * 1.05f, 0.03f, 20, drumCol, false, false);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, rib2Y, 0.0f}, params_.radius * 1.05f, params_.radius * 1.05f, 0.03f, 20, drumCol, false, false);

        // Top Bung holes & threaded plugs (2-inch large bung + 3/4-inch vent bung)
        if (params_.hasBungPlug) {
            float topY = params_.height;
            PropMeshHelper::appendCylinder(payload, Vec3{params_.radius * 0.55f, topY - 0.01f, 0.0f}, 0.035f, 0.035f, 0.025f, 10, 0xFFEEEEEE);
            PropMeshHelper::appendCylinder(payload, Vec3{-params_.radius * 0.50f, topY - 0.01f, 0.0f}, 0.022f, 0.022f, 0.025f, 10, 0xFFEEEEEE);
        }

        // Hazardous diamond decal (for type 2)
        if (params_.barrelType == 2) {
            Vec3 decCenter{0.0f, params_.height * 0.5f, params_.radius * 1.01f};
            PropMeshHelper::appendBox(payload, decCenter, Vec3{0.22f, 0.22f, 0.01f}, 0xFFFF5500, 0.785f); // 45 deg rotated square
        }
    }

    return payload;
}

} // namespace procedural::props
