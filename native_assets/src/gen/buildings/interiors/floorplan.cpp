#include "RoomGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void RoomGen::generateFloorplan(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = params_.width;
    float l = params_.length;
    float h = params_.ceilingHeight;
    float t = params_.wallThickness;
    float hw = w * 0.5f;
    float hl = l * 0.5f;

    uint32_t floorMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t woodMat = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);

    // Floor and Ceiling slabs
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, -0.1f, -hl}, Vec3{hw, 0.0f, hl}, floorMat);
    if (!params_.showInside) {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, h, -hl}, Vec3{hw, h + 0.1f, hl}, wallMat);
    }

    // Exterior perimeter envelope
    float frontWallH = params_.showInside ? 0.45f : h;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, hl - t}, Vec3{hw, frontWallH, hl}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, -hl}, Vec3{hw, h, -hl + t}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, -hl}, Vec3{-hw + t, h, hl}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - t, 0.0f, -hl}, Vec3{hw, frontWallH, hl}, wallMat);

    // Central Circulation Hallway (Running along Z axis from -hl to hl)
    float hallW = 1.8f;
    float hallHalfW = hallW * 0.5f;

    // West hallway partition wall with 2 interior doorways
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hallHalfW - t, 0.0f, -hl}, Vec3{-hallHalfW, h, -hl * 0.5f}, wallMat);
    // Doorway 1
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hallHalfW - t, 2.1f, -hl * 0.5f}, Vec3{-hallHalfW, h, -hl * 0.5f + 1.0f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hallHalfW - t, 0.0f, -hl * 0.5f + 1.0f}, Vec3{-hallHalfW, h, hl * 0.2f}, wallMat);
    // Doorway 2
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hallHalfW - t, 2.1f, hl * 0.2f}, Vec3{-hallHalfW, h, hl * 0.2f + 1.0f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hallHalfW - t, 0.0f, hl * 0.2f + 1.0f}, Vec3{-hallHalfW, h, hl}, wallMat);

    // East hallway partition wall with 2 interior doorways
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hallHalfW, 0.0f, -hl}, Vec3{hallHalfW + t, h, -hl * 0.3f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hallHalfW, 2.1f, -hl * 0.3f}, Vec3{hallHalfW + t, h, -hl * 0.3f + 1.0f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hallHalfW, 0.0f, -hl * 0.3f + 1.0f}, Vec3{hallHalfW + t, h, hl * 0.4f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hallHalfW, 2.1f, hl * 0.4f}, Vec3{hallHalfW + t, h, hl * 0.4f + 1.0f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hallHalfW, 0.0f, hl * 0.4f + 1.0f}, Vec3{hallHalfW + t, h, hl}, wallMat);

    // Transverse room divider walls (West wing: 2 rooms)
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, 0.0f}, Vec3{-hallHalfW, h, t}, wallMat);

    // Transverse room divider walls (East wing: 2 rooms)
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{hallHalfW, 0.0f, 0.5f}, Vec3{hw, h, 0.5f + t}, wallMat);

    // Door frames
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hallHalfW - t * 1.2f, 0.0f, -hl * 0.5f - 0.02f}, Vec3{-hallHalfW + 0.02f, 2.12f, -hl * 0.5f + 0.06f}, woodMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hallHalfW - t * 1.2f, 0.0f, -hl * 0.5f + 0.94f}, Vec3{-hallHalfW + 0.02f, 2.12f, -hl * 0.5f + 1.02f}, woodMat);

    // Physical 3D Interior Bulbs across Hallway and Partitioned Rooms
    float sconceY = std::min(2.05f, h * 0.72f);
    BuildingComponents::generateWallSconceLamp(outVerts, outIndices, Vec3{-hallHalfW, sconceY, 0.0f}, Vec3{1.0f, 0.0f, 0.0f}, false);
    BuildingComponents::generateFloorStandingLamp(outVerts, outIndices, Vec3{-hw * 0.62f, 0.0f, -hl * 0.52f}, std::min(1.75f, h - 0.55f), false);
    BuildingComponents::generateFloorStandingLamp(outVerts, outIndices, Vec3{ hw * 0.58f, 0.0f,  hl * 0.55f}, std::min(1.75f, h - 0.55f), false);
    if (!params_.showInside) {
        BuildingComponents::generateCeilingPendantBulb(outVerts, outIndices, Vec3{0.0f, h, 0.0f}, 0.45f, 0.13f, false);
    }
}

} // namespace procedural::buildings
