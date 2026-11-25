#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateEvChargingStation(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(26.0f, params_.footprintWidth);
    float d = std::max(18.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float canopyH = std::max(4.6f, params_.storyHeight);

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat     = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t gunmetal    = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t evEmerald   = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t evGreen     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t yellowMat   = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t hvacMat     = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t woodMat     = static_cast<uint32_t>(MaterialSlot::PergolaWood);

    // 1. Paved EV Charging Plaza Forecourt
    float apronMinX = -hw - 3.5f;
    float apronMaxX =  hw + 3.5f;
    float apronMinZ = -hd - 2.0f;
    float apronMaxZ =  hd + 3.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{apronMinX, -0.35f, apronMinZ},
        Vec3{apronMaxX,  0.00f, apronMaxZ},
        tarmacMat);

    // Raised concrete perimeter curb islands
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{apronMinX, 0.0f, apronMinZ},
        Vec3{apronMaxX, 0.16f, apronMinZ + 0.55f},
        foundMat);

    // 2. Driver Lounge & Micro-Cafe Pavilion (Rear-Left Zone)
    float loungeW = std::clamp(w * 0.36f, 8.5f, 13.0f);
    float loungeD = std::clamp(d * 0.30f, 4.5f, 6.2f);
    float loungeX0 = -hw + 0.5f;
    float loungeX1 = loungeX0 + loungeW;
    float loungeZ0 = -hd + 0.2f;
    float loungeZ1 = loungeZ0 + loungeD;
    float loungeH  = 3.6f;

    // Lounge foundation deck & sidewalk apron
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{loungeX0 - 0.6f, 0.0f, loungeZ0 - 0.3f},
        Vec3{loungeX1 + 0.8f, 0.18f, loungeZ1 + 1.4f},
        foundMat);

    // Lounge structural rear & side walls
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{loungeX0, 0.18f, loungeZ0},
        Vec3{loungeX1, loungeH, loungeZ0 + 0.45f},
        gunmetal);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{loungeX0, 0.18f, loungeZ0},
        Vec3{loungeX0 + 0.45f, loungeH, loungeZ1},
        gunmetal);

    // Lounge glazed storefront & sliding glass door facing +Z charging bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{loungeX0 + 0.45f, 0.18f, loungeZ0 + 0.45f},
        Vec3{loungeX1 - 0.15f, loungeH - 0.35f, loungeZ1 - 0.05f},
        glassTinted);
    // Vertical mullion fins on lounge curtain wall
    for (float mx = loungeX0 + 0.6f; mx <= loungeX1 + 0.05f; mx += 2.2f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.08f, 0.18f, loungeZ1 - 0.1f},
            Vec3{mx + 0.08f, loungeH - 0.35f, loungeZ1 + 0.08f},
            blackAlum);
    }
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{(loungeX0 + loungeX1) * 0.5f, 0.18f, loungeZ1},
        2.2f, 2.5f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true);

    // Lounge cantilevered roof + timber soffit pergola overhang
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{loungeX0 - 0.5f, loungeH - 0.35f, loungeZ0 - 0.4f},
        Vec3{loungeX1 + 1.0f, loungeH + 0.25f, loungeZ1 + 1.5f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{loungeX0 - 0.4f, loungeH - 0.42f, loungeZ1 + 0.05f},
        Vec3{loungeX1 + 0.9f, loungeH - 0.35f, loungeZ1 + 1.4f},
        woodMat);
    // EV Lounge illuminated emerald accent fascia
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{loungeX0 - 0.45f, loungeH - 0.18f, loungeZ1 + 1.50f},
        Vec3{loungeX1 + 0.95f, loungeH + 0.05f, loungeZ1 + 1.58f},
        evEmerald);

    // 3. Battery Energy Storage System (BESS) & Grid Transformer Yard (Rear-Right Zone)
    float bessX0 = loungeX1 + 2.2f;
    float bessX1 = std::min(hw - 0.6f, bessX0 + 8.5f);
    if (bessX1 > bessX0 + 3.5f) {
        float bessZ0 = -hd + 0.4f;
        float bessZ1 = bessZ0 + 3.2f;
        // Concrete equipment pad
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bessX0 - 0.4f, 0.0f, bessZ0 - 0.3f},
            Vec3{bessX1 + 0.4f, 0.22f, bessZ1 + 0.5f},
            foundMat);
        // Twin Megapack BESS Cabinets
        float midBess = (bessX0 + bessX1) * 0.5f;
        for (int b = 0; b < 2; ++b) {
            float bx0 = (b == 0) ? bessX0 : midBess + 0.25f;
            float bx1 = (b == 0) ? midBess - 0.25f : bessX1;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx0, 0.22f, bessZ0},
                Vec3{bx1, 2.75f, bessZ1},
                silverAlum);
            // Dark louver vents & top radiator hood
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx0 + 0.3f, 1.4f, bessZ1},
                Vec3{bx1 - 0.3f, 2.3f, bessZ1 + 0.06f},
                frameMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx0 + 0.2f, 2.75f, bessZ0 + 0.3f},
                Vec3{bx1 - 0.2f, 3.05f, bessZ1 - 0.3f},
                hvacMat);
        }
        // Protective safety yellow bollards in front of BESS
        for (float px = bessX0; px <= bessX1 + 0.01f; px += 1.8f) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{px, 0.22f, bessZ1 + 0.32f},
                Vec3{px, 1.15f, bessZ1 + 0.32f},
                0.09f, 0.09f, 8, yellowMat);
        }
    }

    // 4. Sculptural Solar Photovoltaic Canopy & EV Charging Islands (Front Forecourt)
    float islandZ = 2.2f;
    float canopyW = w * 0.92f;
    float canopyHalfW = canopyW * 0.5f;
    float canopyMinZ = islandZ - 4.8f;
    float canopyMaxZ = islandZ + 4.8f;
    float canopyTilt = 0.65f; // Tilted solar roof facing front (+Z) slightly lower

    // Central raised charging island curb along X
    float islandHalfL = canopyHalfW - 1.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-islandHalfL, 0.0f, islandZ - 0.85f},
        Vec3{ islandHalfL, 0.24f, islandZ + 0.85f},
        foundMat);
    // Emerald EV accent curb band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-islandHalfL - 0.03f, 0.16f, islandZ - 0.88f},
        Vec3{ islandHalfL + 0.03f, 0.25f, islandZ + 0.88f},
        evEmerald);

    // 6 Charging Bays (3 on front +Z lane, 3 on rear -Z lane of the island)
    int numChargers = (w >= 30.0f) ? 4 : 3;
    float baySpan = (islandHalfL * 1.8f) / static_cast<float>(numChargers);
    float startBayX = -baySpan * (numChargers - 1) * 0.5f;

    for (int i = 0; i < numChargers; ++i) {
        float cx = startBayX + i * baySpan;

        // Painted green EV parking bay border & wheel stops on both sides (+Z and -Z)
        for (int side : {-1, 1}) {
            float bayCenterZ = islandZ + side * 3.1f;
            float stallHalfW = 1.45f;
            float stallHalfD = 2.15f;
            // Side stripes
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - stallHalfW, 0.0f, bayCenterZ - stallHalfD},
                Vec3{cx - stallHalfW + 0.12f, 0.025f, bayCenterZ + stallHalfD},
                evGreen);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx + stallHalfW - 0.12f, 0.0f, bayCenterZ - stallHalfD},
                Vec3{cx + stallHalfW, 0.025f, bayCenterZ + stallHalfD},
                evGreen);
            // EV floor emblem pad
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - 0.55f, 0.0f, bayCenterZ - 0.55f},
                Vec3{cx + 0.55f, 0.02f, bayCenterZ + 0.55f},
                evEmerald);
            // Concrete wheel stop bumper near island
            float stopZ = islandZ + side * 1.45f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - 0.85f, 0.0f, stopZ - 0.12f},
                Vec3{cx + 0.85f, 0.14f, stopZ + 0.12f},
                yellowMat);
        }

        // DC Fast Charger Supercharger Pedestal on Island
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.48f, 0.24f, islandZ - 0.24f},
            Vec3{cx + 0.48f, 2.25f, islandZ + 0.24f},
            silverAlum);
        // Sleek dark inner arch trim & top glowing emerald halo
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.36f, 0.40f, islandZ - 0.26f},
            Vec3{cx + 0.36f, 2.10f, islandZ + 0.26f},
            blackAlum);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.46f, 2.15f, islandZ - 0.26f},
            Vec3{cx + 0.46f, 2.28f, islandZ + 0.26f},
            evEmerald);
        // Dual Touchscreen HMI displays (+Z and -Z)
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.26f, 1.25f, islandZ + 0.26f},
            Vec3{cx + 0.26f, 1.78f, islandZ + 0.29f},
            glassMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.26f, 1.25f, islandZ - 0.29f},
            Vec3{cx + 0.26f, 1.78f, islandZ - 0.26f},
            glassMat);

        // Curved liquid-cooled charging cables draped on left/right sides
        BuildingComponents::appendBentCylinderCatenary(outVerts, outIndices,
            Vec3{cx - 0.48f, 1.95f, islandZ + 0.10f},
            Vec3{cx - 0.48f, 0.95f, islandZ + 0.42f},
            0.35f, 0.032f, 8, 6, blackAlum);
        BuildingComponents::appendBentCylinderCatenary(outVerts, outIndices,
            Vec3{cx + 0.48f, 1.95f, islandZ - 0.10f},
            Vec3{cx + 0.48f, 0.95f, islandZ - 0.42f},
            0.35f, 0.032f, 8, 6, blackAlum);

        // Protective Yellow Steel Crash Bollards flanking charger
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{cx - 0.72f, 0.24f, islandZ},
            Vec3{cx - 0.72f, 1.15f, islandZ},
            0.08f, 0.08f, 8, yellowMat);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{cx + 0.72f, 0.24f, islandZ},
            Vec3{cx + 0.72f, 1.15f, islandZ},
            0.08f, 0.08f, 8, yellowMat);
    }

    // 5. Sculptural Y-Column Canopy Supports along island
    std::vector<float> colXs = {-canopyHalfW * 0.65f, 0.0f, canopyHalfW * 0.65f};
    for (float colX : colXs) {
        // Concrete pier footing
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{colX - 0.35f, 0.24f, islandZ - 0.35f},
            Vec3{colX + 0.35f, 0.90f, islandZ + 0.35f},
            foundMat);
        // Main central mast column
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{colX, 0.85f, islandZ},
            Vec3{colX, canopyH - 1.3f, islandZ},
            0.24f, 0.22f, 10, blackAlum);
        // Front & Rear angled V-struts supporting canopy deck
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{colX, canopyH - 1.4f, islandZ},
            Vec3{colX, canopyH + 0.10f, canopyMinZ + 1.8f},
            0.16f, 0.14f, 8, blackAlum);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{colX, canopyH - 1.4f, islandZ},
            Vec3{colX, canopyH - canopyTilt + 0.10f, canopyMaxZ - 1.8f},
            0.16f, 0.14f, 8, blackAlum);
    }

    // 6. Tilted Solar Photovoltaic Canopy Roof Slab (sloped slightly frontward)
    float yRearBot  = canopyH;
    float yRearTop  = canopyH + 0.42f;
    float yFrontBot = canopyH - canopyTilt;
    float yFrontTop = yFrontBot + 0.42f;

    // Top tilted solar deck quad & bottom soffit quad
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-canopyHalfW, yFrontTop, canopyMaxZ},
        Vec3{ canopyHalfW, yFrontTop, canopyMaxZ},
        Vec3{ canopyHalfW, yRearTop,  canopyMinZ},
        Vec3{-canopyHalfW, yRearTop,  canopyMinZ},
        Vec3{0.0f, 1.0f, 0.0f}, blackAlum);
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-canopyHalfW, yRearBot,  canopyMinZ},
        Vec3{ canopyHalfW, yRearBot,  canopyMinZ},
        Vec3{ canopyHalfW, yFrontBot, canopyMaxZ},
        Vec3{-canopyHalfW, yFrontBot, canopyMaxZ},
        Vec3{0.0f, -1.0f, 0.0f}, silverAlum);
    // Front & rear fascia edge faces
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-canopyHalfW, yFrontBot, canopyMaxZ},
        Vec3{ canopyHalfW, yFrontBot, canopyMaxZ},
        Vec3{ canopyHalfW, yFrontTop, canopyMaxZ},
        Vec3{-canopyHalfW, yFrontTop, canopyMaxZ},
        Vec3{0.0f, 0.0f, 1.0f}, evEmerald);
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{ canopyHalfW, yRearBot, canopyMinZ},
        Vec3{-canopyHalfW, yRearBot, canopyMinZ},
        Vec3{-canopyHalfW, yRearTop, canopyMinZ},
        Vec3{ canopyHalfW, yRearTop, canopyMinZ},
        Vec3{0.0f, 0.0f, -1.0f}, blackAlum);
    // Left & right canopy side caps
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-canopyHalfW, yRearBot,  canopyMinZ},
        Vec3{-canopyHalfW, yFrontBot, canopyMaxZ},
        Vec3{-canopyHalfW, yFrontTop, canopyMaxZ},
        Vec3{-canopyHalfW, yRearTop,  canopyMinZ},
        Vec3{-1.0f, 0.0f, 0.0f}, blackAlum);
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{ canopyHalfW, yFrontBot, canopyMaxZ},
        Vec3{ canopyHalfW, yRearBot,  canopyMinZ},
        Vec3{ canopyHalfW, yRearTop,  canopyMinZ},
        Vec3{ canopyHalfW, yFrontTop, canopyMaxZ},
        Vec3{1.0f, 0.0f, 0.0f}, blackAlum);

    // Solar PV panel array grid mounted cleanly atop the tilted canopy
    int pvCols = 6;
    int pvRows = 3;
    float pvPadX = 0.6f;
    float pvCellW = (canopyW - pvPadX * 2.0f) / static_cast<float>(pvCols);
    for (int r = 0; r < pvRows; ++r) {
        float t0 = 0.08f + (r / static_cast<float>(pvRows)) * 0.84f;
        float t1 = 0.08f + ((r + 0.88f) / static_cast<float>(pvRows)) * 0.84f;
        float z0 = canopyMinZ + t0 * (canopyMaxZ - canopyMinZ);
        float z1 = canopyMinZ + t1 * (canopyMaxZ - canopyMinZ);
        float y0 = yRearTop + (yFrontTop - yRearTop) * t0 + 0.06f;
        float y1 = yRearTop + (yFrontTop - yRearTop) * t1 + 0.06f;
        for (int c = 0; c < pvCols; ++c) {
            float x0 = -canopyHalfW + pvPadX + c * pvCellW + 0.12f;
            float x1 = x0 + pvCellW - 0.24f;
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{x0, y1, z1}, Vec3{x1, y1, z1},
                Vec3{x1, y0, z0}, Vec3{x0, y0, z0},
                Vec3{0.0f, 1.0f, 0.0f}, glassTinted);
        }
    }

    // 7. EV Supercharger Totem Pylon Sign at Plaza Entrance
    float totemX = apronMinX + 1.8f;
    float totemZ = canopyMaxZ + 1.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{totemX - 0.65f, 0.0f, totemZ - 0.35f},
        Vec3{totemX + 0.65f, 0.35f, totemZ + 0.35f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{totemX - 0.50f, 0.35f, totemZ - 0.22f},
        Vec3{totemX + 0.50f, 6.20f, totemZ + 0.22f},
        blackAlum);
    // Glowing emerald EV lightning strip along front & rear of totem
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{totemX - 0.36f, 4.40f, totemZ - 0.25f},
        Vec3{totemX + 0.36f, 5.90f, totemZ + 0.25f},
        evEmerald);
    // Live availability status bar slots
    for (int s = 0; s < 3; ++s) {
        float sy = 1.6f + s * 0.8f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{totemX - 0.36f, sy, totemZ + 0.23f},
            Vec3{totemX + 0.36f, sy + 0.35f, totemZ + 0.26f},
            glassMat);
    }
}

} // namespace procedural::buildings
