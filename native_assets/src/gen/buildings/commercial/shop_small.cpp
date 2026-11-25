#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateSmallShop(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 10.0f, 5.0f, 30.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 8.0f, 5.0f, 25.0f);
    int stories = std::clamp(params_.stories, 1, 3);
    float flH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.8f, 2.8f, 6.0f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt = params_.useExtendedParams ? params_.windowType : WindowType::PictureGlass;
    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::ClearFloat;
    DoorType dt = params_.useExtendedParams ? params_.doorType : DoorType::DoubleGlassCommercial;
    bool canopy = params_.useExtendedParams ? params_.hasEntranceCanopy : false;

    // Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.2f, -0.3f, -hd - 0.2f},
        Vec3{hw + 0.2f, 0.0f, hd + 0.2f}, foundMat);

    // Main store box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{hw, h, hd}, wallMat);

    // Central commercial entry door clamped to floor height and facade width
    float doorW = std::clamp(params_.useExtendedParams ? params_.doorWidth : 1.6f, 0.9f, std::max(1.2f, w * 0.35f));
    float doorH = std::clamp(params_.useExtendedParams ? params_.doorHeight : 2.4f, 2.0f, flH - 0.25f);
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{0.0f, 0.0f, hd}, doorW, doorH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, dt, canopy);
    }

    // Storefront display windows flanking the central door
    float sideSpan = std::max(1.2f, hw - doorW * 0.5f - 0.6f);
    float winW = std::clamp(params_.useExtendedParams ? params_.windowWidth : 2.4f, 0.7f, std::max(0.8f, sideSpan - 0.3f));
    float winH = std::clamp(params_.useExtendedParams ? params_.windowHeight : 2.2f, 0.8f, std::max(1.0f, flH - 0.9f));
    float sill = std::clamp(params_.useExtendedParams ? params_.windowSillHeight : 0.45f, 0.20f, std::max(0.25f, flH - winH - 0.25f));
    float sideCenterX = doorW * 0.5f + 0.35f + sideSpan * 0.5f;
    if (sideSpan >= 1.2f) {
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{-sideCenterX, sill + winH * 0.5f, hd}, winW, winH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{sideCenterX, sill + winH * 0.5f, hd}, winW, winH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
    }

    // Upper floor shop-living windows when stories >= 2
    for (int s = 1; s < stories; ++s) {
        float fy = s * flH;
        int cols = std::max(2, params_.useExtendedParams ? params_.windowsPerFacade : 3);
        float uw = std::clamp(params_.useExtendedParams ? params_.windowWidth : 1.4f, 0.6f, 2.2f);
        float uh = std::clamp(params_.useExtendedParams ? params_.windowHeight : 1.6f, 0.6f, flH - 0.8f);
        float uSill = std::clamp(params_.useExtendedParams ? params_.windowSillHeight : 0.75f, 0.3f, flH - uh - 0.2f);
        float span = std::max(1.0f, w - 1.2f);
        for (int i = 0; i < cols; ++i) {
            float wx = -span * 0.5f + (i + 0.5f) * (span / cols);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{wx, fy + uSill + uh * 0.5f, hd},
                std::min(uw, (span / cols) - 0.3f), uh,
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
        }
    }

    // Shop signage fascia board & awning
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, flH - 0.65f, hd},
        Vec3{hw, flH - 0.10f, hd + 0.38f}, frameMat);

    // Adaptive roof
    RoofType rt = params_.useExtendedParams ? params_.roofStyle : RoofType::FlatParapet;
    if (rt == RoofType::NoneFlat) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.1f, h - 0.05f, -hd - 0.1f},
            Vec3{hw + 0.1f, h + 0.22f, hd + 0.1f},
            static_cast<uint32_t>(MaterialSlot::RoofSurface));
    } else {
        RoofGenParams rParams;
        rParams.type = rt;
        rParams.baseWidth = w;
        rParams.baseDepth = d;
        rParams.baseTopY = h;
        rParams.roofHeight = params_.useExtendedParams ? params_.roofHeight : 2.0f;
        rParams.overhang = params_.useExtendedParams ? params_.roofOverhang : 0.45f;
        rParams.showRooftopClutter = params_.useExtendedParams ? params_.showRooftopClutter : false;
        RoofGen::generate(outVerts, outIndices, rParams);
    }
}

} // namespace procedural::buildings
