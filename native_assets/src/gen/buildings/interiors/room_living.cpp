#include "RoomGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void RoomGen::generateLivingRoom(
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
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // Floor slab
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, -0.1f, -hl}, Vec3{hw, 0.0f, hl}, floorMat);

    // Ceiling slab (hidden in Show Inside cutaway mode)
    if (!params_.showInside) {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, h, -hl}, Vec3{hw, h + 0.1f, hl}, wallMat);
    }

    // North wall (+Z): full wall with picture window when enclosed, low cutaway sill when Show Inside
    if (params_.showInside) {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, hl - t}, Vec3{hw, 0.45f, hl}, wallMat);
    } else {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, hl - t}, Vec3{-hw * 0.4f, h, hl}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw * 0.4f, 0.0f, hl - t}, Vec3{hw, h, hl}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.4f, 0.0f, hl - t}, Vec3{hw * 0.4f, 0.8f, hl}, wallMat); // sill
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.4f, h - 0.4f, hl - t}, Vec3{hw * 0.4f, h, hl}, wallMat); // lintel
        BuildingComponents::generateWindow(outVerts, outIndices, Vec3{0.0f, (0.8f + h - 0.4f) * 0.5f, hl - t * 0.5f}, hw * 0.75f, (h - 1.2f), Vec3{0, 0, -1}, Vec3{1, 0, 0}, WindowType::PictureGlass);
    }

    // South wall (-Z) with interior doorway & picture window
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, -hl}, Vec3{-1.0f, h, -hl + t}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{1.0f, 0.0f, -hl}, Vec3{hw, h, -hl + t}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-1.0f, 2.2f, -hl}, Vec3{1.0f, h, -hl + t}, wallMat);
    if (params_.hasInteriorDoors) {
        BuildingComponents::generateDoor(outVerts, outIndices, Vec3{0.0f, 0.0f, -hl + t}, 1.2f, 2.1f, Vec3{0, 0, 1}, Vec3{1, 0, 0}, DoorType::SingleModern, false);
    }

    // East wall (+X): cutaway sill when Show Inside, full wall otherwise
    if (params_.showInside) {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - t, 0.0f, -hl}, Vec3{hw, 0.45f, hl}, wallMat);
    } else {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - t, 0.0f, -hl}, Vec3{hw, h, hl}, wallMat);
    }

    // West wall (-X) with window view
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, -hl}, Vec3{-hw + t, h, hl}, wallMat);
    BuildingComponents::generateWindow(outVerts, outIndices, Vec3{-hw + t, h * 0.52f, 0.0f}, l * 0.45f, h * 0.45f, Vec3{1, 0, 0}, Vec3{0, 0, 1}, WindowType::PictureGlass);

    // Baseboard perimeter trim
    float trimH = 0.12f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw + t, 0.0f, hl - t - 0.04f}, Vec3{hw - t, trimH, hl - t}, woodMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw + t, 0.0f, -hl + t}, Vec3{hw - t, trimH, -hl + t + 0.04f}, woodMat);

    // Built-in furniture stubs (coffee table & media console)
    if (params_.hasFurnitureStubs) {
        // Media console against east wall
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - t - 0.6f, 0.0f, -2.0f}, Vec3{hw - t, 0.6f, 2.0f}, woodMat);
        // Central coffee table (casts crisp 3D shadow on floor from overhead & corner bulbs)
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-1.2f, 0.0f, -0.6f}, Vec3{1.2f, 0.45f, 0.6f}, frameMat);
    }

    // Physical 3D Interior Lighting Fixtures (2700K warm bulbs casting 1/d^2 light & BVH shadows)
    BuildingComponents::generateFloorStandingLamp(
        outVerts, outIndices,
        Vec3{-hw + t + 0.75f, 0.0f, -hl + t + 0.85f},
        std::min(1.85f, h - 0.55f),
        false
    );
    BuildingComponents::generateWallSconceLamp(
        outVerts, outIndices,
        Vec3{-hw + t, std::min(2.05f, h * 0.72f), -hl * 0.45f},
        Vec3{1.0f, 0.0f, 0.0f},
        false
    );
    BuildingComponents::generateWallSconceLamp(
        outVerts, outIndices,
        Vec3{-hw + t, std::min(2.05f, h * 0.72f), hl * 0.45f},
        Vec3{1.0f, 0.0f, 0.0f},
        false
    );
    if (!params_.showInside) {
        BuildingComponents::generateCeilingPendantBulb(
            outVerts, outIndices,
            Vec3{0.0f, h, 0.0f}, 0.55f, 0.14f, false
        );
    }
}

} // namespace procedural::buildings
