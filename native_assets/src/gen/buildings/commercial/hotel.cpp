#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateHotel(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 32.0f, 16.0f, 64.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 22.0f, 12.0f, 50.0f);
    int32_t stories = std::clamp(params_.stories, 4, 30);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.4f, 2.8f, 5.5f);
    float h = stories * storyH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallLimestone = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallLimestone);
    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::BronzeArchitectural;
    uint32_t glassMat = BuildingComponents::getGlassMaterial(gt);
    uint32_t glassGold = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t champAlum = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t blackAlum = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t greenTurf = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    WindowType wt = params_.useExtendedParams ? params_.windowType : WindowType::PictureGlass;
    DoorType dt = params_.useExtendedParams ? params_.doorType : DoorType::DoubleGlassCommercial;
    bool canopy = params_.useExtendedParams ? params_.hasEntranceCanopy : true;

    // 1. Arrival Driveway & Plaza Apron
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 5.0f, -0.3f, -hd - 3.5f},
        Vec3{ hw + 5.0f,  0.0f,  hd + 9.0f},
        tarmacMat);

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-3.2f, 0.0f, hd + 4.2f},
        Vec3{ 3.2f, 0.42f, hd + 7.8f},
        wallLimestone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.9f, 0.42f, hd + 4.5f},
        Vec3{ 2.9f, 0.47f, hd + 7.5f},
        greenTurf);

    // 2. Foundation Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, 0.0f, -hd - 0.5f},
        Vec3{ hw + 0.5f, 0.55f,  hd + 0.5f},
        foundMat);

    // 3. Ground & Second Floor Grand Podium / Lobby Base
    float podiumH = std::min(h * 0.5f, 2 * storyH + 0.6f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.55f, -hd},
        Vec3{ hw, podiumH, hd},
        wallLimestone);

    float lobbyGlassW = std::min(10.0f, hw * 0.7f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-lobbyGlassW, 0.55f, hd - 0.1f},
        Vec3{ lobbyGlassW, podiumH - 0.55f, hd + 0.30f},
        glassGold);

    // Attached Conference Wing (+X side, scaled to width)
    float confW = std::clamp(w * 0.28f, 5.0f, 10.0f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw, 0.55f, -hd},
        Vec3{hw + confW, podiumH, hd * 0.78f},
        champAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 0.8f, 1.1f, hd * 0.78f - 0.1f},
        Vec3{hw + confW - 0.8f, podiumH - 0.9f, hd * 0.78f + 0.25f},
        glassMat);

    // 4. Porte-Cochère Arrival Canopy
    if (canopy) {
        float canHalfW = std::min(8.5f, hw * 0.65f);
        float canY = std::min(4.6f, podiumH - 0.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-canHalfW, canY, hd},
            Vec3{ canHalfW, canY + 0.55f, hd + 6.0f},
            champAlum);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-canHalfW + 0.4f, canY - 0.15f, hd + 0.2f},
            Vec3{ canHalfW - 0.4f, canY, hd + 5.8f},
            silverAlum);
        for (float cx : {-canHalfW * 0.82f, canHalfW * 0.82f}) {
            for (float cz : {hd + 1.2f, hd + 5.5f}) {
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{cx - 0.40f, 0.0f, cz - 0.40f},
                    Vec3{cx + 0.40f, canY, cz + 0.40f},
                    wallLimestone);
            }
        }
    }

    // Grand Lobby Entrance Door (scaled to floor height)
    float doorW = std::clamp(params_.useExtendedParams ? params_.doorWidth : 4.2f, 1.4f, std::min(6.0f, w * 0.4f));
    float doorH = std::clamp(params_.useExtendedParams ? params_.doorHeight : 3.2f, 2.2f, podiumH - 1.0f);
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{0.0f, 0.55f, hd},
            doorW, doorH, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            dt, false);
    }

    // 5. Multi-Story Guest Tower Massing with Setbacks
    float towerW = w * 0.84f;
    float towerD = d * 0.82f;
    int lowerStoriesEnd = std::max(3, (stories * 3) / 4);
    float hMid = params_.hasSetbacks ? std::min(h, lowerStoriesEnd * storyH) : h;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-towerW * 0.5f, podiumH, -towerD * 0.5f},
        Vec3{ towerW * 0.5f, hMid,     towerD * 0.5f},
        wallLimestone);

    // Guest room windows on true floor increments with collision safety
    int cols = std::max(3, params_.useExtendedParams ? params_.windowsPerFacade : static_cast<int>(towerW / 3.2f));
    float span = std::max(4.0f, towerW - 2.0f);
    float winW = std::clamp(params_.useExtendedParams ? params_.windowWidth : 2.1f, 0.7f, (span / cols) - 0.4f);
    float winH = std::clamp(params_.useExtendedParams ? params_.windowHeight : 2.0f, 0.7f, storyH - 0.8f);
    float sill = std::clamp(params_.useExtendedParams ? params_.windowSillHeight : 0.75f, 0.25f, std::max(0.3f, storyH - winH - 0.25f));

    for (float fy = podiumH; fy + storyH <= hMid + 0.05f; fy += storyH) {
        for (int i = 0; i < cols; ++i) {
            float wx = -span * 0.5f + (i + 0.5f) * (span / cols);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{wx, fy + sill + winH * 0.5f, towerD * 0.5f + 0.02f},
                winW, winH, Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{wx, fy + sill + winH * 0.5f, -towerD * 0.5f - 0.02f},
                winW, winH, Vec3{0, 0, -1}, Vec3{-1, 0, 0}, wt, gt);
        }
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-towerW * 0.5f - 0.12f, fy + storyH - 0.22f, -towerD * 0.5f - 0.12f},
            Vec3{ towerW * 0.5f + 0.12f, fy + storyH + 0.06f, towerD * 0.5f + 0.12f},
            champAlum);
    }

    // 6. Upper Stepped Penthouse Tower
    float topW = towerW;
    float topD = towerD;
    if (params_.hasSetbacks && h > hMid + 0.5f) {
        float pentW = towerW * 0.75f;
        float pentD = towerD * 0.75f;
        topW = pentW;
        topD = pentD;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-pentW * 0.5f, hMid, -pentD * 0.5f},
            Vec3{ pentW * 0.5f, h,     pentD * 0.5f},
            wallLimestone);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-towerW * 0.5f, hMid, towerD * 0.5f - 0.1f},
            Vec3{ towerW * 0.5f, hMid + 1.1f, towerD * 0.5f + 0.05f},
            glassMat);

        int pCols = std::max(2, cols - 1);
        float pSpan = std::max(3.0f, pentW - 1.6f);
        float pWinW = std::min(winW, (pSpan / pCols) - 0.35f);
        for (float fy = hMid; fy + storyH <= h + 0.05f; fy += storyH) {
            for (int i = 0; i < pCols; ++i) {
                float wx = -pSpan * 0.5f + (i + 0.5f) * (pSpan / pCols);
                BuildingComponents::generateWindow(outVerts, outIndices,
                    Vec3{wx, fy + sill + winH * 0.5f, pentD * 0.5f + 0.02f},
                    pWinW, winH, Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
            }
        }
    }

    // 7. Rooftop Sky Lounge Pavilion & Adaptive Crown
    RoofType rt = params_.useExtendedParams ? params_.roofStyle : RoofType::FlatParapet;
    if (rt == RoofType::FlatParapet || rt == RoofType::NoneFlat) {
        float loungeH = 3.8f;
        float loungeW = topW * 0.55f;
        float loungeD = topD * 0.55f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-loungeW * 0.5f, h, -loungeD * 0.5f},
            Vec3{ loungeW * 0.5f, h + loungeH, loungeD * 0.5f},
            glassGold);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-loungeW * 0.5f - 0.35f, h + loungeH - 0.45f, -loungeD * 0.5f - 0.35f},
            Vec3{ loungeW * 0.5f + 0.35f, h + loungeH + 0.35f,  loungeD * 0.5f + 0.35f},
            champAlum);
        if (!params_.useExtendedParams || params_.showRooftopClutter) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-2.8f, h + loungeH, -2.8f},
                Vec3{ 2.8f, h + loungeH + 2.8f, 2.8f},
                blackAlum);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{loungeW * 0.5f + 0.4f, h, -2.4f},
                Vec3{std::min(topW * 0.5f - 0.4f, loungeW * 0.5f + 3.2f), h + 2.0f, 2.4f},
                hvacMat);
        }
    } else {
        RoofGenParams rParams;
        rParams.type = rt;
        rParams.baseWidth = topW;
        rParams.baseDepth = topD;
        rParams.baseTopY = h;
        rParams.roofHeight = params_.useExtendedParams ? params_.roofHeight : 2.8f;
        rParams.overhang = params_.useExtendedParams ? params_.roofOverhang : 0.6f;
        rParams.showRooftopClutter = params_.useExtendedParams ? params_.showRooftopClutter : false;
        RoofGen::generate(outVerts, outIndices, rParams);
    }
}

} // namespace procedural::buildings
