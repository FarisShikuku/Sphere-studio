#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateSelfStorage(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(24.0f, params_.footprintWidth);
    float d = std::max(16.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    int32_t stories = std::clamp(params_.stories, 2, 6);
    float flH = std::max(3.0f, params_.storyHeight);
    float mainH = stories * flH;

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat     = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t gunmetal    = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t signOrange  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t hvacMat     = static_cast<uint32_t>(MaterialSlot::HvacMetal);

    // 1. Paved Drive-Up Apron
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, -0.35f, -hd - 2.5f},
        Vec3{ hw + 3.5f,  0.00f,  hd + 5.5f},
        tarmacMat);

    // 2. Main Multi-Storey Climate-Controlled Storage Building (Rear Section)
    float mainZ0 = -hd;
    float mainZ1 = hd - 4.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.2f, 0.0f, mainZ0 - 0.2f},
        Vec3{ hw + 0.2f, 0.30f, mainZ1 + 0.2f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.30f, mainZ0},
        Vec3{ hw, mainH, mainZ1},
        gunmetal);

    // Vibrant Orange Horizontal Architectural Branding Ribbons at each floor line
    for (int f = 1; f <= stories; ++f) {
        float y = f * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.06f, y - 0.22f, mainZ0 - 0.06f},
            Vec3{ hw + 0.06f, y + 0.08f, mainZ1 + 0.06f},
            signOrange);
    }

    // Glazed Corner Customer Office & Freight Elevator Showcase Tower (Front-Left)
    float officeX0 = -hw;
    float officeX1 = -hw + 6.5f;
    float officeZ0 = mainZ1 - 0.2f;
    float officeZ1 = hd;
    float officeTop = mainH + 1.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{officeX0, 0.0f, officeZ0},
        Vec3{officeX1, 0.30f, officeZ1},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{officeX0, 0.30f, officeZ0},
        Vec3{officeX1, officeTop, officeZ1},
        wallMat);
    // Vertical glass showcase curtain strip on corner tower
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{officeX0 + 0.8f, 0.5f, officeZ1 - 0.1f},
        Vec3{officeX1 - 0.8f, officeTop - 0.6f, officeZ1 + 0.08f},
        glassTinted);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{(officeX0 + officeX1) * 0.5f, 0.30f, officeZ1},
        2.0f, 2.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);
    // Prominent orange crown band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{officeX0 - 0.12f, officeTop - 0.55f, officeZ0 - 0.12f},
        Vec3{officeX1 + 0.12f, officeTop + 0.20f, officeZ1 + 0.12f},
        signOrange);

    // 3. Drive-Up Exterior Mini-Garage Row (Front-Right Single-Storey Wing)
    float wingX0 = officeX1 + 0.5f;
    float wingX1 = hw;
    float wingZ0 = mainZ1;
    float wingZ1 = hd - 0.4f;
    float wingH  = 3.4f;
    if (wingX1 > wingX0 + 6.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wingX0, 0.0f, wingZ0},
            Vec3{wingX1, 0.25f, wingZ1},
            foundMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wingX0, 0.25f, wingZ0},
            Vec3{wingX1, wingH, wingZ1},
            silverAlum);
        // Sloped corrugated roof canopy over drive-up garage doors
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wingX0 - 0.2f, wingH, wingZ0},
            Vec3{wingX1 + 0.2f, wingH + 0.35f, wingZ1 + 0.85f},
            roofMat);

        // Series of individual orange & red roll-up storage unit doors facing +Z
        int numUnits = std::clamp(static_cast<int>((wingX1 - wingX0) / 3.2f), 3, 8);
        float unitSpan = (wingX1 - wingX0 - 0.8f) / static_cast<float>(numUnits);
        for (int u = 0; u < numUnits; ++u) {
            float ux0 = wingX0 + 0.4f + u * unitSpan + 0.2f;
            float ux1 = ux0 + unitSpan - 0.4f;
            uint32_t doorCol = (u % 2 == 0) ? signOrange : signRed;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{ux0 - 0.08f, 0.25f, wingZ1 - 0.05f},
                Vec3{ux1 + 0.08f, 2.70f, wingZ1 + 0.06f},
                frameMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{ux0, 0.25f, wingZ1 - 0.02f},
                Vec3{ux1, 2.60f, wingZ1 + 0.08f},
                doorCol);
        }
    }

    // 4. Main Roof Cap & HVAC Chiller Units
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.2f, mainH, mainZ0 - 0.2f},
        Vec3{ hw + 0.2f, mainH + 0.45f, mainZ1 + 0.2f},
        roofMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{0.0f, mainH + 0.45f, (mainZ0 + mainZ1) * 0.5f - 1.2f},
        Vec3{3.5f, mainH + 1.45f, (mainZ0 + mainZ1) * 0.5f + 1.2f},
        hvacMat);
}

} // namespace procedural::buildings
