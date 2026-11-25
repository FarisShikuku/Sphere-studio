#include "FarmWagonGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::props {

FarmWagonGen::FarmWagonGen() = default;

bool FarmWagonGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void FarmWagonGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload FarmWagonGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    payload.vertexStride = PropMeshHelper::kVertexStrideFloats * sizeof(float);

    float L = std::max(params_.length, 1.8f);
    float W = std::max(params_.width, 0.9f);
    float wheelD = std::max(params_.wheelDiameter, 0.6f);
    float wheelR = wheelD * 0.5f;

    uint32_t woodCol = (params_.woodFinish == 1) ? 0xFFB91C1C : // Barn Red
                       (params_.woodFinish == 2) ? 0xFF15803D : // Harvest Green
                                                   0xFF8E7D6A;  // Weathered Oak
    uint32_t ironCol = 0xFF1F242C;
    uint32_t bedCol  = 0xFFB45309;

    float axleY = wheelR;
    float bedY = axleY + 0.22f;

    // 1. Two Axles (Front & Rear)
    float axleSpanX = L * 0.65f;
    float frontAxleX = axleSpanX * 0.5f;
    float rearAxleX = -axleSpanX * 0.5f;

    for (float ax : {frontAxleX, rearAxleX}) {
        // Heavy square timber axle beam with iron spindle ends
        PropMeshHelper::appendBox(payload, Vec3{ax, axleY, 0.0f}, Vec3{0.14f, 0.14f, W + 0.15f}, ironCol);
        PropMeshHelper::appendBox(payload, Vec3{ax, axleY + 0.12f, 0.0f}, Vec3{0.12f, 0.12f, W * 0.85f}, woodCol); // Bolster
    }

    // Reach pole (long timber coupling front and rear gear undercarriage)
    PropMeshHelper::appendBox(payload, Vec3{0.0f, axleY + 0.06f, 0.0f}, Vec3{L * 0.85f, 0.10f, 0.10f}, woodCol);

    // 2. Four Spoked Wooden Wheels with Iron Rim Tires
    for (float ax : {frontAxleX, rearAxleX}) {
        for (float sz : {-1.0f, 1.0f}) {
            float wz = sz * (W * 0.5f + 0.12f);
            Vec3 wheelCenter{ax, axleY, wz};

            // Iron hub with axle lynchpin cap
            PropMeshHelper::appendCylinder(payload, Vec3{ax, axleY, wz - 0.06f}, 0.11f, 0.11f, 0.12f, 12, ironCol);

            // Outer wooden felloes & iron tire hoop
            PropMeshHelper::appendCylinder(payload, Vec3{ax, axleY, wz - 0.04f}, wheelR, wheelR, 0.08f, 16, ironCol);
            PropMeshHelper::appendCylinder(payload, Vec3{ax, axleY, wz - 0.03f}, wheelR * 0.94f, wheelR * 0.94f, 0.06f, 16, woodCol);

            // 10 radial wooden spokes
            for (int s = 0; s < 10; ++s) {
                float ang = (static_cast<float>(s) / 10.0f) * 2.0f * PropMeshHelper::kPi;
                float cosA = std::cos(ang);
                float sinA = std::sin(ang);
                Vec3 spokeCenter{ax + (wheelR * 0.50f) * cosA, axleY + (wheelR * 0.50f) * sinA, wz};
                PropMeshHelper::appendBox(payload, spokeCenter, Vec3{wheelR * 0.85f, 0.04f, 0.04f}, woodCol);
            }
        }
    }

    // 3. Wagon Timber Bed & Longitudinal Stringer Sills
    PropMeshHelper::appendBox(payload, Vec3{0.0f, bedY - 0.05f, -W * 0.35f}, Vec3{L, 0.10f, 0.10f}, woodCol);
    PropMeshHelper::appendBox(payload, Vec3{0.0f, bedY - 0.05f,  W * 0.35f}, Vec3{L, 0.10f, 0.10f}, woodCol);
    // Planks floor
    PropMeshHelper::appendBox(payload, Vec3{0.0f, bedY, 0.0f}, Vec3{L, 0.045f, W}, bedCol);

    // 4. Sides based on Wagon Style
    if (params_.wagonStyle == 0) {
        // Style 0: High Stake Hay Rack
        float stakeH = 0.95f;
        int stakes = std::max(4, static_cast<int>(L / 0.65f));
        for (int i = 0; i < stakes; ++i) {
            float sx = -L * 0.45f + i * (L * 0.90f / (stakes - 1));
            // Vertical side stakes
            PropMeshHelper::appendBox(payload, Vec3{sx, bedY + stakeH * 0.5f, -W * 0.5f}, Vec3{0.06f, stakeH, 0.05f}, woodCol);
            PropMeshHelper::appendBox(payload, Vec3{sx, bedY + stakeH * 0.5f,  W * 0.5f}, Vec3{0.06f, stakeH, 0.05f}, woodCol);
        }
        // Horizontal slats along the stakes
        for (int r = 1; r <= 3; ++r) {
            float ry = bedY + r * (stakeH * 0.28f);
            PropMeshHelper::appendBox(payload, Vec3{0.0f, ry, -W * 0.51f}, Vec3{L, 0.06f, 0.025f}, woodCol);
            PropMeshHelper::appendBox(payload, Vec3{0.0f, ry,  W * 0.51f}, Vec3{L, 0.06f, 0.025f}, woodCol);
        }
        // Front and rear endgate racks
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f, bedY + stakeH * 0.5f, 0.0f}, Vec3{0.04f, stakeH * 0.8f, W}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{ L * 0.5f, bedY + stakeH * 0.5f, 0.0f}, Vec3{0.04f, stakeH * 0.8f, W}, woodCol);

    } else if (params_.wagonStyle == 2) {
        // Style 2: Low Solid Grain Box
        float boxH = 0.45f;
        PropMeshHelper::appendBox(payload, Vec3{0.0f, bedY + boxH * 0.5f, -W * 0.5f}, Vec3{L, boxH, 0.04f}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{0.0f, bedY + boxH * 0.5f,  W * 0.5f}, Vec3{L, boxH, 0.04f}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{-L * 0.5f, bedY + boxH * 0.5f, 0.0f}, Vec3{0.04f, boxH, W}, woodCol);
        PropMeshHelper::appendBox(payload, Vec3{ L * 0.5f, bedY + boxH * 0.5f, 0.0f}, Vec3{0.04f, boxH, W}, woodCol);
    }

    // 5. Front Drawbar Tongue & Driver Bench Seat (if enabled)
    if (params_.hasDrawbarSeat) {
        // Front hitch tongue pole
        float tongueL = 1.8f;
        PropMeshHelper::appendBox(payload, Vec3{frontAxleX + tongueL * 0.5f, axleY, 0.0f}, Vec3{tongueL, 0.07f, 0.07f}, woodCol);
        // Doubletree hitch crossbar
        PropMeshHelper::appendBox(payload, Vec3{frontAxleX + 0.6f, axleY + 0.05f, 0.0f}, Vec3{0.08f, 0.06f, 0.90f}, ironCol);

        // Spring-mounted driver bench seat at front of bed
        float seatX = frontAxleX * 0.7f;
        float seatY = bedY + 0.40f;
        // Iron leaf springs
        PropMeshHelper::appendBox(payload, Vec3{seatX, bedY + 0.20f, -W * 0.35f}, Vec3{0.04f, 0.35f, 0.04f}, ironCol);
        PropMeshHelper::appendBox(payload, Vec3{seatX, bedY + 0.20f,  W * 0.35f}, Vec3{0.04f, 0.35f, 0.04f}, ironCol);
        // Wooden bench and backrest
        PropMeshHelper::appendBox(payload, Vec3{seatX, seatY, 0.0f}, Vec3{0.32f, 0.04f, W * 0.85f}, bedCol);
        PropMeshHelper::appendBox(payload, Vec3{seatX - 0.12f, seatY + 0.20f, 0.0f}, Vec3{0.04f, 0.28f, W * 0.85f}, bedCol);
    }

    return payload;
}

} // namespace procedural::props
