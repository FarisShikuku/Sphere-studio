#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateGtcOfficeTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // GTC Office Tower — Westlands, Nairobi (184m architectural landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 36.0f, 24.0f, 56.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 30.0f, 20.0f, 48.0f);
    int32_t stories = std::clamp(params_.stories, 18, 60);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.9f, 3.0f, 5.2f);
    float totalTowerH = std::max(stories * storyH + 16.3f, 184.0f);

    uint32_t crispWhite  = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t glassBlue   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t darkGlass   = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t frameDark   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t goldLogo    = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Wide Plaza Podium & Adjacent GTC Hotel Skybridge Wing (matching uploaded photo)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 22.0f, -0.25f, -hd - 10.0f},
        Vec3{ hw + 10.0f,  0.02f,  hd + 12.0f},
        tarmacMat);

    // Main GTC Retail & Lobby Podium
    float podiumH = storyH * 3.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.0f, 0.02f, -hd - 2.0f},
        Vec3{ hw + 2.5f, podiumH, hd + 2.0f},
        darkGlass);
    for (int pf = 1; pf <= 3; ++pf) {
        float py = pf * (podiumH / 3.2f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 3.15f, py - 0.16f, -hd - 2.15f},
            Vec3{ hw + 2.65f, py + 0.16f,  hd + 2.15f},
            crispWhite);
    }

    // Elevated GTC Skybridge & Flanking Hotel Companion Mass (-X side)
    float wingH = totalTowerH * 0.68f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 20.0f, 0.02f, -hd * 0.95f},
        Vec3{-hw - 4.5f,  wingH,  hd * 0.45f},
        darkGlass);
    // White U-frame border around companion wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 20.3f, 0.02f, hd * 0.44f},
        Vec3{-hw - 19.2f, wingH + 0.6f, hd * 0.52f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 5.4f, 0.02f, hd * 0.44f},
        Vec3{-hw - 4.2f, wingH + 0.6f, hd * 0.52f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 20.3f, wingH - 2.8f, -hd * 0.96f},
        Vec3{-hw - 4.2f,  wingH + 0.8f,  hd * 0.52f},
        crispWhite);
    // Elevated Enclosed Pedestrian Skybridge linking podiums
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.6f, podiumH * 0.58f, -3.5f},
        Vec3{-hw + 1.0f, podiumH * 0.94f,  3.5f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.5f, podiumH * 0.65f, -3.6f},
        Vec3{-hw + 0.9f, podiumH * 0.88f,  3.6f},
        glassBlue);

    // 2. Signature Projecting White Fin Entrance Marquee Canopy (Front +Z)
    float canopyY = podiumH * 0.72f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.48f, canopyY, hd + 1.8f},
        Vec3{ hw * 0.55f, canopyY + 0.42f, hd + 6.8f},
        crispWhite);
    for (int f = 0; f <= 8; ++f) {
        float fx = -hw * 0.46f + f * ((hw * 1.00f) / 8.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.14f, canopyY + 0.40f, hd + 1.8f},
            Vec3{fx + 0.14f, canopyY + 0.95f, hd + 6.95f},
            silverAlum);
    }
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.05f},
        4.2f, 3.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::RevolvingDoor, false
    );

    // 3. Main Full-Height Shaft (East/Right Core rising full 184m) + Stepped Left Shoulder (rising ~78%)
    float mainShaftX0 = -hw * 0.35f;
    float mainShaftX1 =  hw * 0.92f;
    float shoulderX0  = -hw * 0.96f;
    float shoulderX1  = -hw * 0.25f;
    float shoulderTopY = totalTowerH * 0.78f;

    // Dark blue-slate glazed curtain wall core for Main Crown Shaft
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{mainShaftX0, podiumH, -hd * 0.85f},
        Vec3{mainShaftX1, totalTowerH - 3.5f, hd * 0.85f},
        glassBlue);

    // Dark blue-slate glazed core for Stepped Left Shoulder
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{shoulderX0, podiumH, -hd * 0.65f},
        Vec3{shoulderX1, shoulderTopY, hd * 0.96f},
        darkGlass);

    // White Crisp Vertical Blade Fin Ribs wrapping Main Shaft (Front, Rear, Sides)
    int mainCols = 11;
    for (int c = 0; c <= mainCols; ++c) {
        float fx = mainShaftX0 + c * ((mainShaftX1 - mainShaftX0) / static_cast<float>(mainCols));
        bool isOuterPortal = (c == 0 || c == mainCols);
        float finTop = totalTowerH;
        // Central bay leaves clean executive penthouse glass bay near top (82%..95%)
        bool inCrownGlassZone = (c >= 2 && c <= mainCols - 2);
        if (inCrownGlassZone) {
            // Lower fin below crown executive window recess
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx - 0.20f, podiumH, -hd * 0.88f},
                Vec3{fx + 0.20f, totalTowerH * 0.82f, hd * 0.88f},
                crispWhite);
            // Upper short fin above crown recess
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx - 0.20f, totalTowerH * 0.95f, -hd * 0.88f},
                Vec3{fx + 0.20f, finTop, hd * 0.88f},
                crispWhite);
        } else {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx - (isOuterPortal ? 0.55f : 0.24f), podiumH, -hd * 0.90f},
                Vec3{fx + (isOuterPortal ? 0.55f : 0.24f), finTop,  hd * 0.90f},
                crispWhite);
        }
    }

    // White Vertical Blade Fins along Shoulder Block
    int shoulderCols = 7;
    for (int c = 0; c <= shoulderCols; ++c) {
        float fx = shoulderX0 + c * ((shoulderX1 - shoulderX0) / static_cast<float>(shoulderCols));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.18f, podiumH, -hd * 0.68f},
            Vec3{fx + 0.18f, shoulderTopY + 0.4f, hd * 1.01f},
            crispWhite);
    }

    // Side Face White Fin Ribs (+X and -X)
    int sideCols = 8;
    for (int c = 0; c <= sideCols; ++c) {
        float fz = -hd * 0.84f + c * ((hd * 1.68f) / static_cast<float>(sideCols));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mainShaftX1 - 0.20f, podiumH, fz - 0.18f},
            Vec3{mainShaftX1 + 0.35f, totalTowerH, fz + 0.18f},
            crispWhite);
    }

    // 4. Recessed Dark Executive Sky-Lobby Window Bay & Thick White Crown Portal at 184m
    float execBotY = totalTowerH * 0.82f;
    float execTopY = totalTowerH * 0.95f;
    // Left & Right White Portal Frames around Executive Sky-Lobby
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{mainShaftX0 - 0.1f, execBotY, hd * 0.82f},
        Vec3{mainShaftX0 + 1.4f, totalTowerH, hd * 0.92f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{mainShaftX1 - 1.4f, execBotY, hd * 0.82f},
        Vec3{mainShaftX1 + 0.1f, totalTowerH, hd * 0.92f},
        crispWhite);
    // Horizontal floor spandrel lines inside tower
    for (int f = 1; f < stories; ++f) {
        float fy = podiumH + f * ((totalTowerH - podiumH - 8.0f) / static_cast<float>(stories));
        if (fy < execBotY || fy > execTopY) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{mainShaftX0 - 0.05f, fy - 0.14f, -hd * 0.86f},
                Vec3{mainShaftX1 + 0.05f, fy + 0.14f,  hd * 0.86f},
                frameDark);
        }
    }

    // Signature Top White Crown Fascia + Illuminated Gold GTC Emblem Plaque at 184m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{mainShaftX0 - 0.2f, execTopY, -hd * 0.90f},
        Vec3{mainShaftX1 + 0.2f, totalTowerH, hd * 0.92f},
        limestone);
    float plaqueCx = 0.5f * (mainShaftX0 + mainShaftX1);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{plaqueCx - 2.6f, totalTowerH - 3.6f, hd * 0.92f},
        Vec3{plaqueCx + 2.6f, totalTowerH - 1.0f, hd * 0.96f},
        goldLogo);
}

} // namespace procedural::buildings
