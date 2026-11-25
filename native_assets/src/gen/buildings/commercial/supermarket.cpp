#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateSupermarket(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(42.0f, params_.footprintWidth);
    float d = std::max(30.0f, params_.footprintDepth);
    float h = std::max(1, params_.stories) * 6.5f; // High-clearance supermarket volume
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t wallLimestone = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t blackAlum = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t signRed = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t signOrange = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t doorMetal = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);

    // 1. Customer Parking Apron & Delivery Yard Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.3f, -hd - 8.0f},
        Vec3{ hw + 6.0f,  0.0f,  hd + 10.0f},
        tarmacMat);

    // Parking Bay Stalls in front of store
    for (float px = -hw + 4.0f; px < hw - 4.0f; px += 3.6f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px, 0.01f, hd + 3.0f},
            Vec3{px + 0.15f, 0.03f, hd + 8.5f},
            silverAlum);
    }

    // 2. Concrete Base Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.4f, 0.0f, -hd - 0.4f},
        Vec3{ hw + 0.4f, 0.5f,  hd + 0.4f},
        foundMat);

    // 3. Main Sprawling Supermarket Building Volume
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.5f, -hd},
        Vec3{ hw, h,     hd},
        wallConcrete);

    // 4. Raised Front Architectural Parapet Facade
    float parapetExtraH = 2.4f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.2f, h, hd - 0.8f},
        Vec3{ hw + 0.2f, h + parapetExtraH, hd + 0.3f},
        wallLimestone);

    // High-Visibility Branded Supermarket Signage Fascia Band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 4.0f, h - 0.5f, hd + 0.32f},
        Vec3{ hw - 4.0f, h + parapetExtraH - 0.4f, hd + 0.55f},
        signRed);

    // Secondary Accent Orange Lighting Stripe
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 3.5f, h - 0.8f, hd + 0.32f},
        Vec3{ hw - 3.5f, h - 0.5f, hd + 0.52f},
        signOrange);

    // 5. Protruding Glass Cart Lobby Vestibule
    float vestW = 16.0f;
    float vestH = 4.2f;
    float vestD = 3.6f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-vestW * 0.5f, 0.5f, hd},
        Vec3{ vestW * 0.5f, vestH, hd + vestD},
        glassTinted);

    // Vestibule Structural Framing & Cantilevered Entrance Canopy
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-vestW * 0.5f - 0.5f, vestH - 0.3f, hd - 0.2f},
        Vec3{ vestW * 0.5f + 0.5f, vestH + 0.2f, hd + vestD + 1.2f},
        silverAlum);

    // Automatic Sliding Entry Doors & Exit Doors
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{-4.2f, 0.5f, hd + vestD},
        3.6f, 3.0f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{ 4.2f, 0.5f, hd + vestD},
        3.6f, 3.0f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);

    // 6. Flanking Front Picture Display Windows
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{-hw + 7.0f, 2.2f, hd},
        8.0f, 2.6f, Vec3{0, 0, 1}, Vec3{1, 0, 0}, WindowType::PictureGlass);
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{ hw - 7.0f, 2.2f, hd},
        8.0f, 2.6f, Vec3{0, 0, 1}, Vec3{1, 0, 0}, WindowType::PictureGlass);

    // 7. Covered Shopping Cart Return Corrals in Parking Forecourt
    for (float cx : {-hw + 8.0f, hw - 10.0f}) {
        // Canopy Roof
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx, 2.2f, hd + 4.5f},
            Vec3{cx + 2.4f, 2.4f, hd + 8.5f},
            silverAlum);
        // Steel Posts
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx, 0.0f, hd + 4.5f},
            Vec3{cx + 0.12f, 2.2f, hd + 4.62f},
            frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx + 2.28f, 0.0f, hd + 4.5f},
            Vec3{cx + 2.4f, 2.2f, hd + 4.62f},
            frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx, 0.0f, hd + 8.38f},
            Vec3{cx + 0.12f, 2.2f, hd + 8.5f},
            frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx + 2.28f, 0.0f, hd + 8.38f},
            Vec3{cx + 2.4f, 2.2f, hd + 8.5f},
            frameMat);
        // Shopping cart row inside corral
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx + 0.4f, 0.0f, hd + 4.8f},
            Vec3{cx + 2.0f, 0.95f, hd + 8.0f},
            frameMat);
    }

    // 8. Rear Logistics Delivery Dock Bays (3 Docks)
    float dockSpacing = 5.2f;
    float startDockX = -dockSpacing;
    for (int i = 0; i < 3; ++i) {
        float dx = startDockX + i * dockSpacing;
        // Sunken dock pit aperture / Roll-up door
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx - 1.8f, 1.2f, -hd - 0.15f},
            Vec3{dx + 1.8f, 4.4f, -hd + 0.05f},
            doorMetal);
        // Weather seal dock shelter hood
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx - 2.1f, 1.0f, -hd - 0.7f},
            Vec3{dx + 2.1f, 4.7f, -hd - 0.1f},
            blackAlum);
        // Heavy rubber dock bumpers on foundation
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx - 1.9f, 0.4f, -hd - 0.35f},
            Vec3{dx - 1.5f, 1.3f, -hd},
            blackAlum);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx + 1.5f, 0.4f, -hd - 0.35f},
            Vec3{dx + 1.9f, 1.3f, -hd},
            blackAlum);
        // Yellow safety truck wheel guide lines
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx - 1.6f, 0.01f, -hd - 6.0f},
            Vec3{dx - 1.4f, 0.03f, -hd - 0.4f},
            safetyYellow);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx + 1.4f, 0.01f, -hd - 6.0f},
            Vec3{dx + 1.6f, 0.03f, -hd - 0.4f},
            safetyYellow);
    }

    // Waste Recycling Compactor unit at rear corner
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 7.0f, 0.0f, -hd - 4.5f},
        Vec3{hw - 2.0f, 2.6f, -hd - 0.5f},
        frameMat);

    // 9. Rooftop Commercial Refrigeration Compressor & HVAC Chiller Plant
    // Left Refrigeration Condenser Bank
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 6.0f, h, -4.0f},
        Vec3{-hw + 16.0f, h + 1.8f, 4.0f},
        hvacMat);
    // Right HVAC Air Handling Unit
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 16.0f, h, -4.0f},
        Vec3{hw - 6.0f, h + 1.8f, 4.0f},
        hvacMat);

    // 10. Roadside Brand Totem Pylon Sign
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.5f, 0.0f, hd + 6.5f},
        Vec3{-hw - 3.2f, 7.5f, hd + 7.3f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.8f, 5.8f, hd + 6.4f},
        Vec3{-hw - 2.9f, 7.3f, hd + 7.4f},
        signRed);
}

} // namespace procedural::buildings
