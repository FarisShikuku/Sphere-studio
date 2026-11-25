#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateOfficeTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int totalStories = std::clamp(params_.stories, 3, 40);
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 24.0f, 12.0f, 60.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 18.0f, 10.0f, 50.0f);
    float flH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.5f, 2.8f, 6.0f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::ClearFloat;
    uint32_t glassMat = BuildingComponents::getGlassMaterial(gt);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t wallMat = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : glassMat;
    DoorType dt = params_.useExtendedParams ? params_.doorType : DoorType::DoubleGlassCommercial;
    bool canopy = params_.useExtendedParams ? params_.hasEntranceCanopy : true;

    // Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, -0.8f, -hd - 0.5f},
        Vec3{hw + 0.5f, 0.0f, hd + 0.5f}, foundMat);

    // Ground Floor Double-Height Grand Lobby
    float lobbyH = flH * 1.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, lobbyH, hd}, glassMat);

    float doorW = std::clamp(params_.useExtendedParams ? params_.doorWidth : 3.2f, 1.2f, std::min(6.0f, w * 0.45f));
    float doorH = std::clamp(params_.useExtendedParams ? params_.doorHeight : 3.2f, 2.1f, lobbyH - 0.3f);
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{0.0f, 0.0f, hd}, doorW, doorH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, dt, canopy);
    }

    // Architectural Tower Body with optional vertical setbacks
    float currY = lobbyH;
    int remainingFloors = totalStories - 1;
    int tier1Floors = params_.hasSetbacks ? std::max(1, remainingFloors * 2 / 3) : remainingFloors;
    int tier2Floors = remainingFloors - tier1Floors;

    // Tier 1
    float tier1H = tier1Floors * flH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, currY, -hd}, Vec3{hw, currY + tier1H, hd}, wallMat);

    // Structural spandrel horizontal bands & vertical mullions
    for (int fl = 0; fl <= tier1Floors; ++fl) {
        float fY = currY + fl * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.1f, fY - 0.14f, -hd - 0.1f},
            Vec3{hw + 0.1f, fY + 0.14f, hd + 0.1f}, frameMat);
    }
    int mullionCount = std::max(3, params_.useExtendedParams ? params_.windowsPerFacade : 6);
    for (int m = 0; m <= mullionCount; ++m) {
        float mX = -hw + m * (w / mullionCount);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mX - 0.08f, currY, hd - 0.05f},
            Vec3{mX + 0.08f, currY + tier1H, hd + 0.15f}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mX - 0.08f, currY, -hd - 0.15f},
            Vec3{mX + 0.08f, currY + tier1H, -hd + 0.05f}, frameMat);
    }

    // Glazed curtain window ribbons if wallMaterial isn't pure curtain glass
    if (params_.useExtendedParams && !BuildingComponents::isCurtainWallMaterial(params_.wallMaterial)) {
        for (int fl = 0; fl < tier1Floors; ++fl) {
            float winY = currY + fl * flH + flH * 0.5f;
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{0.0f, winY, hd + 0.02f}, w * 0.86f, std::min(params_.windowHeight, flH * 0.68f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, WindowType::RibbonStrip, gt);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{0.0f, winY, -hd - 0.02f}, w * 0.86f, std::min(params_.windowHeight, flH * 0.68f),
                Vec3{0, 0, -1}, Vec3{-1, 0, 0}, WindowType::RibbonStrip, gt);
        }
    }

    currY += tier1H;

    // Tier 2 (Setback crown)
    if (tier2Floors > 0) {
        float sHw = hw * 0.82f;
        float sHd = hd * 0.82f;
        float tier2H = tier2Floors * flH;

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-sHw, currY, -sHd}, Vec3{sHw, currY + tier2H, sHd}, wallMat);

        for (int fl = 0; fl <= tier2Floors; ++fl) {
            float fY = currY + fl * flH;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-sHw - 0.1f, fY - 0.12f, -sHd - 0.1f},
                Vec3{sHw + 0.1f, fY + 0.12f, sHd + 0.1f}, frameMat);
        }
        currY += tier2H;
        hw = sHw;
        hd = sHd;
    }

    if (params_.useExtendedParams && params_.hasCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, hw * 2.0f, hd * 2.0f, currY, 0.35f);
    }

    // Adaptive rooftop
    RoofType rt = params_.useExtendedParams ? params_.roofStyle : RoofType::FlatParapet;
    if (rt == RoofType::NoneFlat) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.1f, currY - 0.05f, -hd - 0.1f},
            Vec3{hw + 0.1f, currY + 0.25f, hd + 0.1f},
            static_cast<uint32_t>(MaterialSlot::RoofSurface));
    } else {
        RoofGenParams rParams;
        rParams.type = rt;
        rParams.baseWidth = hw * 2.0f;
        rParams.baseDepth = hd * 2.0f;
        rParams.baseTopY = currY;
        rParams.roofHeight = params_.useExtendedParams ? params_.roofHeight : 2.5f;
        rParams.overhang = params_.useExtendedParams ? params_.roofOverhang : 0.4f;
        rParams.showRooftopClutter = params_.useExtendedParams ? params_.showRooftopClutter : true;
        RoofGen::generate(outVerts, outIndices, rParams);
    }
}

} // namespace procedural::buildings
