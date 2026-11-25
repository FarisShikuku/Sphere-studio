#include "WaterTroughGen.h"

namespace procedural::props {

WaterTroughGen::WaterTroughGen() = default;

bool WaterTroughGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void WaterTroughGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload WaterTroughGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t basinCol = (params_.troughMaterial == 0) ? 0xFF8A939C : // Galvanized Steel
                        (params_.troughMaterial == 1) ? 0xFF726E69 : // Carved Stone
                                                        0xFF8C5C38;  // Timber

    uint32_t rimCol   = (params_.troughMaterial == 0) ? 0xFF586068 :
                        (params_.troughMaterial == 1) ? 0xFF5A5652 :
                                                        0xFF6A4022;

    uint32_t waterCol = 0xAA2B7BB8; // Semi-transparent blue water
    uint32_t brassCol = 0xFFD4AF37; // Brass valve

    float L = params_.length;
    float W = params_.width;
    float H = params_.height;
    float wallThick = 0.05f;

    // 1. Skid rails / base feet
    PropMeshHelper::appendBox(payload, Vec3{-L * 0.35f, 0.04f, 0.0f}, Vec3{0.12f, 0.08f, W * 1.05f}, 0xFF3A3D42);
    PropMeshHelper::appendBox(payload, Vec3{ L * 0.35f, 0.04f, 0.0f}, Vec3{0.12f, 0.08f, W * 1.05f}, 0xFF3A3D42);

    // 2. Bottom basin floor
    float floorY = 0.08f;
    PropMeshHelper::appendBox(payload, Vec3{0.0f, floorY + wallThick * 0.5f, 0.0f}, Vec3{L, wallThick, W}, basinCol);

    // 3. Four perimeter walls (North, South, East, West)
    float wallH = H - floorY;
    float wallMidY = floorY + wallH * 0.5f;

    // Longitudinal walls
    PropMeshHelper::appendBox(payload, Vec3{0.0f, wallMidY,  W * 0.5f - wallThick * 0.5f}, Vec3{L, wallH, wallThick}, basinCol);
    PropMeshHelper::appendBox(payload, Vec3{0.0f, wallMidY, -W * 0.5f + wallThick * 0.5f}, Vec3{L, wallH, wallThick}, basinCol);
    // End walls
    PropMeshHelper::appendBox(payload, Vec3{ L * 0.5f - wallThick * 0.5f, wallMidY, 0.0f}, Vec3{wallThick, wallH, W - wallThick * 2.0f}, basinCol);
    PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f + wallThick * 0.5f, wallMidY, 0.0f}, Vec3{wallThick, wallH, W - wallThick * 2.0f}, basinCol);

    // Rolled protective top safety rim
    PropMeshHelper::appendBox(payload, Vec3{0.0f, H,  W * 0.5f}, Vec3{L + 0.04f, 0.04f, 0.06f}, rimCol);
    PropMeshHelper::appendBox(payload, Vec3{0.0f, H, -W * 0.5f}, Vec3{L + 0.04f, 0.04f, 0.06f}, rimCol);
    PropMeshHelper::appendBox(payload, Vec3{ L * 0.5f, H, 0.0f}, Vec3{0.06f, 0.04f, W + 0.04f}, rimCol);
    PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f, H, 0.0f}, Vec3{0.06f, 0.04f, W + 0.04f}, rimCol);

    // 4. Water plane inside trough
    if (params_.waterLevel > 0.05f) {
        float waterH = floorY + wallThick + (wallH - wallThick) * params_.waterLevel * 0.92f;
        float innerL = L - wallThick * 2.0f;
        float innerW = W - wallThick * 2.0f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, waterH, 0.0f}, Vec3{innerL, 0.015f, innerW}, waterCol);
    }

    // 5. Automatic Float Valve Box & arm
    if (params_.hasFloatValve) {
        Vec3 valvePos{L * 0.42f, H - 0.06f, 0.0f};
        // Hood cage
        PropMeshHelper::appendBox(payload, valvePos, Vec3{0.18f, 0.16f, 0.20f}, 0xFF4A4E54);
        // Supply pipe riser
        PropMeshHelper::appendCylinder(payload, valvePos + Vec3{0.0f, 0.0f, 0.0f}, 0.02f, 0.02f, 0.22f, 8, brassCol);
        // Copper/brass float ball
        PropMeshHelper::appendSphere(payload, valvePos + Vec3{-0.15f, -0.08f, 0.0f}, 0.06f, 6, 8, brassCol);
    }

    return payload;
}

} // namespace procedural::props
