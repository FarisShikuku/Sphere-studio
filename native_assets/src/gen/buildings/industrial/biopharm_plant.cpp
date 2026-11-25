#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void WarehouseGen::generateBiopharmPlant(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float l = std::max(36.0f, params_.length);
    float w = std::max(24.0f, params_.width);
    float h = 12.5f;
    float hl = l * 0.5f;
    float hw = w * 0.5f;

    uint32_t whiteCleanroom = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t stainlessSteel = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t cyanReveals    = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t foundMat       = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t glassMat       = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t hvacMetal      = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t pipeMat        = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // 1. Foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.5f, -0.4f, -hl - 1.5f},
        Vec3{hw + 1.5f, 0.0f, hl + 1.5f},
        foundMat);

    // 2. Main Pharmaceutical Production Hall (Cleanroom White Insulated Panels)
    float hallW = hw * 1.35f;
    float hallL = hl * 1.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hallW * 0.5f, 0.0f, -hallL * 0.5f},
        Vec3{hallW * 0.5f, h, hallL * 0.5f},
        whiteCleanroom);

    // High-tech turquoise/cyan accent reveal stripes
    for (float ry : {3.8f, 7.8f, 11.6f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hallW * 0.5f - 0.05f, ry, -hallL * 0.5f - 0.05f},
            Vec3{hallW * 0.5f + 0.05f, ry + 0.25f, hallL * 0.5f + 0.05f},
            cyanReveals);
    }

    // 3. Exterior Fermentation Bioreactor Tank Farm (Right side: +X)
    float farmX = hallW * 0.5f + 3.2f;
    float tankR = 2.0f;
    float tankH = 8.5f;

    for (int t = 0; t < 4; ++t) {
        float tz = -hallL * 0.35f + t * (hallL * 0.70f / 3.0f);

        // Stainless steel cylindrical fermentation tank
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{farmX, 0.4f, tz},
            Vec3{farmX, tankH, tz},
            tankR, tankR, 20, stainlessSteel, true, true);

        // Hemispherical dome cap
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{farmX, tankH, tz},
            Vec3{farmX, tankH + 1.2f, tz},
            tankR, 0.1f, 20, stainlessSteel, true, true);

        // Glass liquid level sight gauge tube
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{farmX + tankR * 0.95f, 1.2f, tz},
            Vec3{farmX + tankR * 0.95f, tankH - 0.5f, tz},
            0.08f, 0.08f, 8, cyanReveals, true, true);

        // Sanitary stainless pipe loops entering the building
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{farmX - tankR, tankH * 0.7f, tz},
            Vec3{hallW * 0.5f, tankH * 0.7f, tz},
            0.14f, 0.14f, 10, pipeMat, true, true);
    }

    // Clean-In-Place (CIP) Chemical Skid Enclosure beside the tanks
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{farmX - 2.0f, 0.0f, hallL * 0.42f},
        Vec3{farmX + 2.0f, 3.6f, hallL * 0.42f + 4.2f},
        stainlessSteel);

    // 4. Rooftop HEPA Air-Handling & Clean Steam Penthouse (height h to h + 4.2m)
    float pentW = hallW * 0.65f;
    float pentL = hallL * 0.75f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-pentW * 0.5f, h, -pentL * 0.5f},
        Vec3{pentW * 0.5f, h + 3.8f, pentL * 0.5f},
        hvacMetal);

    // 4 Clean steam exhaust vent stacks with conical rain caps
    for (float sx : {-pentW * 0.3f, pentW * 0.3f}) {
        for (float sz : {-pentL * 0.3f, pentL * 0.3f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{sx, h + 3.8f, sz},
                Vec3{sx, h + 6.2f, sz},
                0.28f, 0.28f, 12, stainlessSteel, true, true);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{sx, h + 6.2f, sz},
                Vec3{sx, h + 6.6f, sz},
                0.55f, 0.05f, 12, stainlessSteel, true, true);
        }
    }

    // 5. Clean-Load Logistics Bay & Personnel Airlock (Front, +Z)
    float dockW = 8.5f;
    float dockD = 3.6f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-dockW * 0.5f, 0.0f, hallL * 0.5f},
        Vec3{dockW * 0.5f, 4.6f, hallL * 0.5f + dockD},
        stainlessSteel);

    // Flush high-speed roll-up cleanroom door
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.0f, hallL * 0.5f + dockD + 0.02f},
        3.2f, 3.6f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::IndustrialRollUp, true);

    // Personnel airlock glazed entrance (left side)
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{-hallW * 0.35f, 0.0f, hallL * 0.5f + 0.02f},
        1.6f, 2.6f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);
}

} // namespace procedural::buildings
