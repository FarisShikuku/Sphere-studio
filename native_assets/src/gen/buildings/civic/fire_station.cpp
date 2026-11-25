#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void CivicGen::generateFireStation(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(28.0f, params_.mainWidth);
    float d = std::max(18.0f, params_.mainDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float appH = std::max(5.8f, params_.storyHeight * 1.6f); // Tall apparatus bay for ladder trucks

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t brickMat    = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t wallMat     = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t yellowMat   = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t hvacMat     = static_cast<uint32_t>(MaterialSlot::HvacMetal);

    // 1. Concrete Emergency Engine Apron & Driveway Ramp (+Z front)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, -0.35f, -hd - 2.0f},
        Vec3{ hw + 3.5f,  0.00f,  hd + 9.0f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.35f, 0.0f, -hd - 0.35f},
        Vec3{ hw + 0.35f, 0.25f, hd + 0.35f},
        foundMat);

    // Layout: Left 62% = 3-Bay Apparatus Garage, Mid-Right = Hose-Drying Watch Tower, Far-Right = 2-Storey Living & Dispatch Wing
    float appX0 = -hw;
    float appX1 = -hw + w * 0.58f;
    float towerX0 = appX1;
    float towerX1 = towerX0 + 4.4f;
    float wingX0 = towerX1;
    float wingX1 = hw;

    // 2. 3-Bay Fire Engine Apparatus Garage Hall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{appX0, 0.25f, -hd},
        Vec3{appX1, appH,   hd},
        brickMat);
    // Concrete parapet cornice cap
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{appX0 - 0.2f, appH, -hd - 0.2f},
        Vec3{appX1 + 0.1f, appH + 0.45f, hd + 0.2f},
        wallMat);

    // 3 Tall Red-Framed Sectional Glass-Pane Apparatus Doors (+Z front)
    float appW = appX1 - appX0;
    int bays = 3;
    float baySpan = (appW - 1.2f) / static_cast<float>(bays);
    float doorW = baySpan - 0.85f;
    float doorH = appH - 1.25f;

    for (int b = 0; b < bays; ++b) {
        float cx = appX0 + 0.6f + (b + 0.5f) * baySpan;
        float dx0 = cx - doorW * 0.5f;
        float dx1 = cx + doorW * 0.5f;

        // Precast concrete lintel header & red emblem plaque above door
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx0 - 0.15f, 0.25f + doorH, hd - 0.05f},
            Vec3{dx1 + 0.15f, 0.25f + doorH + 0.45f, hd + 0.18f},
            wallMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.65f, 0.25f + doorH + 0.08f, hd + 0.18f},
            Vec3{cx + 0.65f, 0.25f + doorH + 0.38f, hd + 0.24f},
            signRed);

        // Red Sectional Apparatus Door Leaf
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx0, 0.25f, hd - 0.04f},
            Vec3{dx1, 0.25f + doorH, hd + 0.08f},
            signRed);
        // Horizontal safety glass vision windows across door
        for (float wy : {1.6f, 2.7f}) {
            if (wy + 0.65f < doorH) {
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{dx0 + 0.25f, wy, hd - 0.06f},
                    Vec3{dx1 - 0.25f, wy + 0.65f, hd + 0.11f},
                    glassMat);
            }
        }
        // Yellow striped apron approach bay markings in front of each door
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx0, 0.0f, hd + 0.5f},
            Vec3{dx0 + 0.16f, 0.03f, hd + 7.5f},
            yellowMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx1 - 0.16f, 0.0f, hd + 0.5f},
            Vec3{dx1, 0.03f, hd + 7.5f},
            yellowMat);

        // Emergency red beacon light beside each apparatus bay
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx1 + 0.18f, 3.2f, hd},
            Vec3{dx1 + 0.36f, 3.55f, hd + 0.20f},
            signRed);
    }

    // 3. Hose-Drying & Communications Watch Tower (Central Landmark)
    float towerH = appH + 6.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerX0, 0.25f, -hd + 2.0f},
        Vec3{towerX1, towerH, hd - 1.0f},
        brickMat);
    // Vertical curtain glass slit window running up tower face
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{(towerX0 + towerX1) * 0.5f - 0.65f, 1.5f, hd - 1.05f},
        Vec3{(towerX0 + towerX1) * 0.5f + 0.65f, towerH - 1.2f, hd - 0.88f},
        glassTinted);
    // Tower concrete observation cap & red siren/radio mast
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerX0 - 0.2f, towerH, -hd + 1.8f},
        Vec3{towerX1 + 0.2f, towerH + 0.55f, hd - 0.8f},
        wallMat);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{(towerX0 + towerX1) * 0.5f, towerH + 0.55f, 0.0f},
        Vec3{(towerX0 + towerX1) * 0.5f, towerH + 3.8f, 0.0f},
        0.12f, 0.05f, 8, frameMat);

    // 4. 2-Storey Firefighter Quarters & Dispatch Annex (Right Wing)
    float wingH = 6.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingX0, 0.25f, -hd + 0.8f},
        Vec3{wingX1, wingH,  hd - 0.8f},
        brickMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingX0 - 0.15f, wingH, -hd + 0.65f},
        Vec3{wingX1 + 0.15f, wingH + 0.45f, hd - 0.65f},
        roofMat);

    // Dispatch entrance door & upper/lower dormitory windows
    float wingCenterX = (wingX0 + wingX1) * 0.5f;
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{wingCenterX, 0.25f, hd - 0.8f},
        1.8f, 2.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{wingCenterX, 4.9f, hd - 0.8f},
        std::max(2.4f, (wingX1 - wingX0) * 0.62f), 1.5f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::RibbonStrip, GlassType::ClearFloat);

    // Rooftop HVAC unit on apparatus hall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{appX0 + 2.0f, appH + 0.45f, -2.0f},
        Vec3{appX0 + 4.8f, appH + 1.55f,  0.8f},
        hvacMat);
}

} // namespace procedural::buildings
