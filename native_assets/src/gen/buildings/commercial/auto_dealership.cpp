#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateAutoDealership(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(38.0f, params_.footprintWidth);
    float d = std::max(24.0f, params_.footprintDepth);
    float h = std::max(1, params_.stories) * 7.2f; // Soaring double-height showroom ceiling
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t glassReflect = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t blackAlum = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t champAlum = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t signRed = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t doorMetal = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);

    // 1. Vehicle Display Forecourt & Lot Apron
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.3f, -hd - 4.0f},
        Vec3{ hw + 6.0f,  0.0f,  hd + 10.0f},
        tarmacMat);

    // Vehicle display parking spots in front lot
    for (float px = -hw + 3.0f; px < hw - 3.0f; px += 3.8f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px, 0.01f, hd + 3.5f},
            Vec3{px + 0.12f, 0.03f, hd + 8.5f},
            silverAlum);
    }

    // 2. Foundation Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.3f, 0.0f, -hd - 0.3f},
        Vec3{ hw + 0.3f, 0.45f, hd + 0.3f},
        foundMat);

    // Division: Left 62% is Showroom Pavilion, Right 38% is Service Workshop
    float splitX = -hw + w * 0.62f;

    // 3. Ultra-Modern Glass Showroom Pavilion (Left Section)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.45f, -hd},
        Vec3{ splitX, h,  hd},
        glassTinted);

    // Showroom Structural Rear & Side Wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.45f, -hd},
        Vec3{ splitX, h, -hd + 0.6f},
        wallConcrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.45f, -hd},
        Vec3{-hw + 0.6f, h,  hd},
        blackAlum);

    // Showroom Slender Structural Steel Perimeter Columns
    for (float cx = -hw + 1.0f; cx < splitX - 1.0f; cx += 4.5f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.15f, 0.45f, hd - 0.1f},
            Vec3{cx + 0.15f, h,      hd + 0.2f},
            blackAlum);
    }

    // Showroom Aerodynamic Cantilevered Roof Canopy Overhang
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.0f, h - 0.4f, -hd - 0.5f},
        Vec3{ splitX + 0.8f, h + 0.5f,  hd + 2.5f},
        silverAlum);

    // Showroom Brand Fascia Header (Illuminated Red Trim Band)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.8f, h - 0.3f, hd + 2.52f},
        Vec3{ splitX + 0.6f, h + 0.4f, hd + 2.7f},
        signRed);

    // Showroom Vehicle Display Turntables / Raised Ramps inside glass
    for (float tx : {-hw + 5.5f, -hw + 14.5f}) {
        // Octagonal/Circular Display Pedestal
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx - 2.8f, 0.45f, -1.0f},
            Vec3{tx + 2.8f, 0.85f,  4.5f},
            champAlum);
        // Pedestal Accent Perimeter Glow Strip
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx - 2.85f, 0.75f, -1.05f},
            Vec3{tx + 2.85f, 0.82f,  4.55f},
            silverAlum);
    }

    // Showroom Customer Reception / Hospitality Mezzanine at Rear
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.0f, 3.6f, -hd + 0.6f},
        Vec3{ splitX - 1.0f, 3.85f, -hd + 6.0f},
        silverAlum);
    // Mezzanine Glass Balustrade Handrail
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.0f, 3.85f, -hd + 5.9f},
        Vec3{ splitX - 1.0f, 4.8f,  -hd + 6.0f},
        glassMat);

    // Showroom Main Entrance Glass Double Doors
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{-hw + 10.0f, 0.45f, hd},
        3.8f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);

    // 4. Attached 4-Bay Service & Repair Workshop (Right Section)
    float shopH = h * 0.82f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX, 0.45f, -hd},
        Vec3{hw,     shopH,  hd},
        silverAlum);

    // Workshop Dividing Demising Wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX - 0.2f, 0.45f, -hd},
        Vec3{splitX + 0.2f, shopH,  hd},
        wallConcrete);

    // 4 Service Bay Glazed Overhead Sectional Doors
    float bayWidth = (hw - splitX - 3.0f) / 4.0f;
    for (int i = 0; i < 4; ++i) {
        float bx = splitX + 1.5f + i * (bayWidth + 0.5f);
        // Door Opening with glazed sectional panels
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx, 0.45f, hd - 0.1f},
            Vec3{bx + bayWidth - 0.4f, 4.4f, hd + 0.1f},
            glassTinted);
        // Sectional Aluminum Horizontal Slats
        for (float sy = 1.4f; sy < 4.4f; sy += 1.0f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx, sy - 0.08f, hd - 0.12f},
                Vec3{bx + bayWidth - 0.4f, sy + 0.08f, hd + 0.12f},
                blackAlum);
        }
        // Safety Yellow Vehicle Alignment Guide Marks on apron
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx + 0.1f, 0.01f, hd + 0.2f},
            Vec3{bx + 0.35f, 0.03f, hd + 4.0f},
            safetyYellow);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx + bayWidth - 0.75f, 0.01f, hd + 0.2f},
            Vec3{bx + bayWidth - 0.5f,  0.03f, hd + 4.0f},
            safetyYellow);
    }

    // Workshop Parapet Wall & Header
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX, shopH, -hd},
        Vec3{hw,     shopH + 0.8f, hd + 0.2f},
        blackAlum);

    // 5. Exterior Featured Vehicle Outdoor Ramp (Showcasing 1 Car at 20° incline)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.0f, 0.0f, hd + 4.5f},
        Vec3{-hw + 4.5f, 1.4f, hd + 9.5f},
        champAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.1f, 1.35f, hd + 4.6f},
        Vec3{-hw + 4.4f, 1.42f, hd + 9.4f},
        silverAlum);

    // 6. Slender Roadside Brand Pylon Totem
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 1.5f, 0.0f, hd + 6.5f},
        Vec3{hw + 2.5f, 8.5f, hd + 7.3f},
        blackAlum);
    // Totem Illuminated Brand Sign Panel
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 1.3f, 6.5f, hd + 6.35f},
        Vec3{hw + 2.7f, 8.2f, hd + 7.45f},
        signRed);

    // 7. Rooftop HVAC Chiller Packages
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX + 2.0f, shopH, -hd + 3.0f},
        Vec3{hw - 2.0f,     shopH + 1.8f, -hd + 8.0f},
        hvacMat);
}

} // namespace procedural::buildings
