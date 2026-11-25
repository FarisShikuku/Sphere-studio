#include "HydrantGen.h"

namespace procedural::props {

HydrantGen::HydrantGen() = default;

bool HydrantGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void HydrantGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload HydrantGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = 8 * sizeof(float);

    uint32_t bodyCol = (params_.colorScheme == 0) ? 0xFFC9281D : // Red
                       (params_.colorScheme == 1) ? 0xFFE5B518 : // Yellow
                                                    0xFFA5ADB8;  // Chrome/Silver

    uint32_t capCol = (params_.colorScheme == 0) ? 0xFFEEEEEE : 0xFF333333;

    float R = params_.barrelRadius;
    float H = params_.height;

    // Ground flange & bolts
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.0f, 0.0f}, R * 1.55f, R * 1.55f, 0.06f, 16, bodyCol);

    // Lower breakable spool barrel
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, 0.06f, 0.0f}, R * 1.05f, R * 0.98f, H * 0.25f, 16, bodyCol);

    // Mid barrel flange ring
    float midY = 0.06f + H * 0.25f;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, midY, 0.0f}, R * 1.25f, R * 1.25f, 0.04f, 16, bodyCol);

    // Main upper barrel with nozzles
    float upperH = H * 0.50f;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, midY + 0.04f, 0.0f}, R, R * 1.05f, upperH, 16, bodyCol);

    // Two 2.5" side hose nozzles (Left & Right) pointing horizontally along X
    float nozzleY = midY + 0.04f + upperH * 0.55f;
    for (float side : {-1.0f, 1.0f}) {
        Vec3 nozBase{side * R * 0.85f, nozzleY, 0.0f};
        Vec3 nozEnd{side * (R * 0.85f + R * 0.65f), nozzleY, 0.0f};
        PropMeshHelper::appendOrientedCylinder(payload, nozBase, nozEnd, R * 0.38f, R * 0.38f, 12, bodyCol, true, true);
        // Threaded cap
        Vec3 capEnd{side * (R * 0.85f + R * 0.65f + 0.06f), nozzleY, 0.0f};
        PropMeshHelper::appendOrientedCylinder(payload, nozEnd, capEnd, R * 0.45f, R * 0.42f, 12, capCol, true, true);
        // Pentagon lug on cap
        Vec3 lugEnd{side * (R * 0.85f + R * 0.65f + 0.06f + 0.04f), nozzleY, 0.0f};
        PropMeshHelper::appendOrientedCylinder(payload, capEnd, lugEnd, 0.035f, 0.035f, 6, capCol, true, true);
    }

    // Large front 4.5" Steamer Pumper Nozzle pointing horizontally along Z
    if (params_.outletCount >= 3) {
        Vec3 frontBase{0.0f, nozzleY, R * 0.85f};
        Vec3 frontEnd{0.0f, nozzleY, R * 0.85f + R * 0.70f};
        PropMeshHelper::appendOrientedCylinder(payload, frontBase, frontEnd, R * 0.55f, R * 0.55f, 16, bodyCol, true, true);
        // Large steamer cap
        Vec3 capEnd{0.0f, nozzleY, R * 0.85f + R * 0.70f + 0.07f};
        PropMeshHelper::appendOrientedCylinder(payload, frontEnd, capEnd, R * 0.62f, R * 0.58f, 16, capCol, true, true);
        Vec3 lugEnd{0.0f, nozzleY, R * 0.85f + R * 0.70f + 0.07f + 0.04f};
        PropMeshHelper::appendOrientedCylinder(payload, capEnd, lugEnd, 0.045f, 0.045f, 6, capCol, true, true);
    }

    // Top Dome Bonnet
    float bonnetBaseY = midY + 0.04f + upperH;
    PropMeshHelper::appendCylinder(payload, Vec3{0.0f, bonnetBaseY, 0.0f}, R * 1.15f, R * 0.40f, H * 0.15f, 16, bodyCol);
    PropMeshHelper::appendSphere(payload, Vec3{0.0f, bonnetBaseY + H * 0.12f, 0.0f}, R * 0.42f, 8, 12, bodyCol);

    // Operating pentagonal valve stem nut on top
    if (params_.hasPentagonNut) {
        Vec3 nutPos{0.0f, bonnetBaseY + H * 0.15f, 0.0f};
        PropMeshHelper::appendCylinder(payload, nutPos, 0.045f, 0.045f, 0.055f, 5, 0xFF2A2A2A);
    }

    return payload;
}

} // namespace procedural::props
