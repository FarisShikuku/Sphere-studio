#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

void WarehouseGen::generateCleanroomFab(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(46.0f, std::max(params_.width, params_.length));
    float d = std::max(32.0f, std::min(params_.width, params_.length));
    float h = std::max(10.0f, params_.height * 1.25f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallWhite = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMetal = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t cyanSpandrel = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t doorMetal = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);

    // 1. Pristine Concrete Foundation & Utility Forecourt
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.3f, -hd - 4.0f},
        Vec3{ hw + 6.0f,  0.0f,  hd + 6.0f},
        tarmacMat);

    // 2. High-Tech Semiconductor Cleanroom Main Envelope
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{ hw, h,     hd},
        wallWhite);

    // Horizontal Vibrant Cyan Accent Reveal Channel around building
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.1f, h * 0.45f, -hd - 0.1f},
        Vec3{ hw + 0.1f, h * 0.45f + 0.4f, hd + 0.1f},
        cyanSpandrel);

    // Brushed Silver Aluminum Accent Pilasters
    for (float px = -hw + 4.0f; px < hw - 4.0f; px += 6.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.2f, 0.0f, hd},
            Vec3{px + 0.2f, h,    hd + 0.25f},
            silverAlum);
    }

    // 3. Massive Rooftop Sub-Fab Air Filtration Penthouse
    float pentW = w * 0.72f;
    float pentD = d * 0.65f;
    float pentH = 4.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-pentW * 0.5f, h, -pentD * 0.5f},
        Vec3{ pentW * 0.5f, h + pentH, pentD * 0.5f},
        silverAlum);

    // 6 Industrial Chemical Scrubber Exhaust Stacks atop Penthouse
    for (int i = 0; i < 6; ++i) {
        float sx = -pentW * 0.38f + i * (pentW * 0.76f / 5.0f);
        float sz = 0.0f;
        float stackH = 6.2f;
        // Stack cylinder
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - 0.65f, h + pentH, sz - 0.65f},
            Vec3{sx + 0.65f, h + pentH + stackH, sz + 0.65f},
            darkMetal);
        // Conical rain cap
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - 1.1f, h + pentH + stackH, sz - 1.1f},
            Vec3{sx + 1.1f, h + pentH + stackH + 0.8f, sz + 1.1f},
            silverAlum);
    }

    // 4. Cryogenic Liquid Nitrogen (LN2) & Argon Storage Tank Farm (Right Yard)
    for (float tx : {hw + 1.8f, hw + 4.8f}) {
        float tz = -hd + 8.0f;
        float tankH = 12.0f;
        // Cryogenic vertical vacuum vessel
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx - 1.2f, 0.0f, tz - 1.2f},
            Vec3{tx + 1.2f, tankH, tz + 1.2f},
            silverAlum);
        // Top dome cap
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx - 1.25f, tankH, tz - 1.25f},
            Vec3{tx + 1.25f, tankH + 1.0f, tz + 1.25f},
            cyanSpandrel);
    }

    // 5. High-Tech Glass Entrance & Administrative Office Pavilion (Front Left)
    float entryW = 16.0f;
    float entryD = 6.0f;
    float entryH = 7.0f;
    float entryX = -hw + 12.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{entryX - entryW * 0.5f, 0.0f, hd},
        Vec3{entryX + entryW * 0.5f, entryH, hd + entryD},
        glassMat);

    // Minimalist Cantilevered Steel Entry Canopy
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{entryX - entryW * 0.5f - 1.0f, 4.0f, hd + entryD},
        Vec3{entryX + entryW * 0.5f + 1.0f, 4.35f, hd + entryD + 3.2f},
        silverAlum);

    // Glass Double Entrance Doors
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{entryX, 0.0f, hd + entryD},
        3.6f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);

    // Corporate Technology Header Sign Band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{entryX - entryW * 0.5f, entryH - 1.2f, hd + entryD + 0.05f},
        Vec3{entryX + entryW * 0.5f, entryH - 0.2f, hd + entryD + 0.25f},
        cyanSpandrel);

    // 6. Bulk Chemical Delivery Dock (Rear)
    float dockX = hw - 10.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{dockX - 3.2f, 0.0f, -hd - 0.1f},
        Vec3{dockX + 3.2f, 4.2f, -hd + 0.2f},
        doorMetal);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{dockX - 3.8f, 4.2f, -hd - 1.8f},
        Vec3{dockX + 3.8f, 4.5f, -hd},
        silverAlum);
}

} // namespace procedural::buildings
