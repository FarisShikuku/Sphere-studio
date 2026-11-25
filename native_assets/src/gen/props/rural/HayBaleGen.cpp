#include "HayBaleGen.h"

namespace procedural::props {

HayBaleGen::HayBaleGen() = default;

bool HayBaleGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void HayBaleGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload HayBaleGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t strawCol = params_.wrappedPlastic ? 0xFFF0F5F2 : 0xFFD8B962; // Silage film or Golden Straw
    uint32_t coreCol  = 0xFFBFA04A;
    uint32_t twineCol = 0xFF2A5BB5; // Blue baler twine

    if (params_.baleType == 0) {
        // Giant Round Cylindrical Bale lying horizontally
        float R = params_.radiusOrWidth;
        float L = params_.length;

        // Main cylindrical roll (centered along Z axis)
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, R, -L * 0.5f}, R, R, L, 24, strawCol);

        // Circular spiral end faces detail
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, R, -L * 0.505f}, R * 0.65f, R * 0.65f, 0.01f, 16, coreCol);
        PropMeshHelper::appendCylinder(payload, Vec3{0.0f, R,  L * 0.5f}, R * 0.65f, R * 0.65f, 0.01f, 16, coreCol);

        // Circumferential twine rings
        if (!params_.wrappedPlastic) {
            int twines = std::max(params_.twineCount, 2);
            for (int t = 0; t < twines; ++t) {
                float z = -L * 0.4f + t * (L * 0.8f / (twines - 1));
                PropMeshHelper::appendCylinder(payload, Vec3{0.0f, R, z - 0.01f}, R * 1.02f, R * 1.02f, 0.02f, 24, twineCol, false, false);
            }
        }
    } else {
        // Compact Rectangular Square Bale
        float W = params_.radiusOrWidth;
        float H = params_.radiusOrWidth * 0.75f;
        float L = params_.length;

        // Core block
        PropMeshHelper::appendBox(payload, Vec3{0.0f, H * 0.5f, 0.0f}, Vec3{W, H, L}, strawCol);

        // Longitudinal twine wrap bands (encircling the length)
        if (!params_.wrappedPlastic) {
            int twines = std::max(params_.twineCount, 2);
            for (int t = 0; t < twines; ++t) {
                float x = -W * 0.35f + t * (W * 0.7f / (twines - 1));
                // Top twine
                PropMeshHelper::appendBox(payload, Vec3{x, H + 0.005f, 0.0f}, Vec3{0.015f, 0.01f, L * 1.01f}, twineCol);
                // Bottom twine
                PropMeshHelper::appendBox(payload, Vec3{x, 0.005f, 0.0f}, Vec3{0.015f, 0.01f, L * 1.01f}, twineCol);
                // Front and back vertical drops
                PropMeshHelper::appendBox(payload, Vec3{x, H * 0.5f,  L * 0.505f}, Vec3{0.015f, H, 0.01f}, twineCol);
                PropMeshHelper::appendBox(payload, Vec3{x, H * 0.5f, -L * 0.505f}, Vec3{0.015f, H, 0.01f}, twineCol);
            }
        }
    }

    return payload;
}

} // namespace procedural::props
