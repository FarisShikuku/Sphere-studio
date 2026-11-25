#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateFastFood(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(20.0f, params_.footprintWidth);
    float d = std::max(16.0f, params_.footprintDepth);
    float h = std::max(1, params_.stories) * 5.2f;
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
    uint32_t woodMat = static_cast<uint32_t>(MaterialSlot::PergolaWood);

    // 1. Site Foundation & Drive-Thru Apron
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.25f, -hd - 5.0f},
        Vec3{ hw + 6.0f,  0.0f,   hd + 7.0f},
        tarmacMat);

    // Parking spaces in front
    for (float px = -hw + 2.0f; px < hw - 7.0f; px += 3.5f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px, 0.01f, hd + 2.5f},
            Vec3{px + 0.12f, 0.03f, hd + 6.5f},
            silverAlum);
    }

    // 2. Concrete Building Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, 0.0f, -hd - 0.25f},
        Vec3{ hw + 0.25f, 0.45f, hd + 0.25f},
        foundMat);

    // 3. Main Restaurant Dining & Kitchen Volume
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.45f, -hd},
        Vec3{ hw, h,     hd},
        wallLimestone);

    // Front & Side Dining Room Panoramic Glass Facade
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.0f, 1.0f, hd - 0.05f},
        Vec3{ 1.0f,      h - 0.6f, hd + 0.15f},
        glassTinted);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.05f, 1.0f, -hd * 0.3f},
        Vec3{-hw + 0.15f, h - 0.6f, hd},
        glassTinted);

    // Dining Room Entrance Glass Door with Red Cantilevered Awning
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{-hw + 4.5f, 0.45f, hd},
        2.4f, 2.8f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, true);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 3.0f, 3.2f, hd},
        Vec3{-hw + 6.0f, 3.5f, hd + 2.2f},
        signRed);

    // 4. Iconic Brand Architectural Corner Tower / Pylon
    float towerW = 4.8f;
    float towerH = h + 3.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - towerW, 0.45f, hd - towerW},
        Vec3{hw + 0.3f,   towerH, hd + 0.3f},
        blackAlum);

    // Brand Accent Red Fascia Band running around tower and building parapet
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.1f, h - 1.2f, -hd - 0.1f},
        Vec3{ hw + 0.1f, h,        hd + 0.1f},
        signRed);
    // Tower Corporate Sign Header Box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - towerW - 0.1f, towerH - 2.2f, hd - 0.1f},
        Vec3{hw + 0.4f,          towerH - 0.4f, hd + 0.45f},
        signOrange);

    // 5. Drive-Thru Lane Infrastructure (Right Wall: X = hw)
    // Safety Curb Barrier along Drive-Thru Lane
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 4.5f, 0.0f, -hd - 4.0f},
        Vec3{hw + 4.8f, 0.35f, hd + 3.0f},
        safetyYellow);

    // Clearance Height Warning Bar Gantry at drive-thru entrance (rear right)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 0.2f, 0.0f, -hd - 3.8f},
        Vec3{hw + 0.4f, 3.8f, -hd - 3.6f},
        safetyYellow);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 4.5f, 0.0f, -hd - 3.8f},
        Vec3{hw + 4.7f, 3.8f, -hd - 3.6f},
        safetyYellow);
    // Overhead yellow crossbar
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 0.2f, 3.6f, -hd - 3.8f},
        Vec3{hw + 4.7f, 3.85f, -hd - 3.6f},
        safetyYellow);

    // Electronic Menu Board & Order Speaker Totem
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 4.6f, 0.0f, -hd + 1.0f},
        Vec3{hw + 4.8f, 2.6f, -hd + 4.0f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 4.5f, 0.8f, -hd + 1.2f},
        Vec3{hw + 4.58f, 2.4f, -hd + 3.8f},
        signOrange);

    // Drive-Thru Service Windows on right wall (X = hw)
    // Window 1: Cashier Booth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.1f, 1.2f, -hd * 0.2f - 0.8f},
        Vec3{hw + 0.2f, 2.6f, -hd * 0.2f + 0.8f},
        glassTinted);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw, 2.7f, -hd * 0.2f - 1.2f},
        Vec3{hw + 1.4f, 2.85f, -hd * 0.2f + 1.2f},
        silverAlum);

    // Window 2: Food Pickup Window
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.1f, 1.2f, hd * 0.4f - 0.8f},
        Vec3{hw + 0.2f, 2.6f, hd * 0.4f + 0.8f},
        glassTinted);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw, 2.7f, hd * 0.4f - 1.2f},
        Vec3{hw + 1.4f, 2.85f, hd * 0.4f + 1.2f},
        silverAlum);

    // 6. Outdoor Patio Dining Area with Wooden Pergola Trellis (Left Front)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.5f, 0.0f, hd - 6.0f},
        Vec3{-hw - 0.1f, 0.2f, hd + 2.5f},
        foundMat);

    // Pergola Posts & Overhead Beams
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.4f, 0.2f, hd - 5.9f},
        Vec3{-hw - 4.2f, 3.2f, hd - 5.7f},
        woodMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.4f, 0.2f, hd + 2.3f},
        Vec3{-hw - 4.2f, 3.2f, hd + 2.5f},
        woodMat);
    // Overhead horizontal wooden trellis slats
    for (float sl = hd - 5.5f; sl <= hd + 2.2f; sl += 1.4f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 4.5f, 3.2f, sl - 0.08f},
            Vec3{-hw,        3.35f, sl + 0.08f},
            woodMat);
    }

    // Outdoor Picnic Seating Tables inside patio
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.6f, 0.2f, hd - 4.2f},
        Vec3{-hw - 1.2f, 0.95f, hd - 2.4f},
        silverAlum);

    // 7. Rooftop Commercial Grease Exhaust & HVAC Condensers
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{0.0f, h, -hd + 3.0f},
        Vec3{5.0f, h + 1.8f, 1.0f},
        hvacMat);
    // Cylindrical Kitchen Grease Exhaust Hood
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{2.0f, h + 1.8f, -1.0f},
        Vec3{3.2f, h + 2.8f, 0.2f},
        frameMat);
}

} // namespace procedural::buildings
