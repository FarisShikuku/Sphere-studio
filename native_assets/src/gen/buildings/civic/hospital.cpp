#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void CivicGen::generateHospital(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int stories = std::clamp(params_.stories, 3, 20);
    float w = std::clamp(params_.mainWidth > 0.5f ? params_.mainWidth : 30.0f, 16.0f, 64.0f);
    float d = std::clamp(params_.mainDepth > 0.5f ? params_.mainDepth : 22.0f, 12.0f, 50.0f);
    float flH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.6f, 2.8f, 5.5f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    WindowType wt = params_.useExtendedParams ? params_.windowType : WindowType::RibbonStrip;
    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::ClearFloat;
    DoorType dt = params_.useExtendedParams ? params_.doorType : DoorType::IndustrialRollUp;
    bool canopy = params_.useExtendedParams ? params_.hasEntranceCanopy : true;

    // Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, -0.8f, -hd - 0.6f},
        Vec3{hw + 0.6f, 0.0f, hd + 0.6f}, foundMat);

    // Ground 2-floor diagnostic / emergency base plinth
    float podiumH = flH * 2.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, podiumH, hd}, wallMat);

    // Emergency Department Trauma Bay Canopy scaled to width
    if (canopy) {
        float canX0 = -hw * 0.65f;
        float canX1 = hw * 0.15f;
        float canD = std::min(5.5f, hd * 0.5f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{canX0, podiumH * 0.45f, hd},
            Vec3{canX1, podiumH * 0.50f, hd + canD}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{canX0 + 0.2f, 0.0f, hd + canD - 0.35f},
            Vec3{canX0 + 0.55f, podiumH * 0.45f, hd + canD}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{canX1 - 0.55f, 0.0f, hd + canD - 0.35f},
            Vec3{canX1 - 0.2f, podiumH * 0.45f, hd + canD}, frameMat);
    }

    // Emergency ambulance entrance roll-up & personnel doors clamped safely
    float doorW = std::clamp(params_.useExtendedParams ? params_.doorWidth : 3.6f, 1.4f, w * 0.35f);
    float doorH = std::clamp(params_.useExtendedParams ? params_.doorHeight : 3.1f, 2.2f, flH - 0.25f);
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{-hw * 0.25f, 0.0f, hd + 0.02f}, doorW, doorH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, dt, false);
    }

    // Multi-storey inpatient ward tower rising above podium
    float towerHw = hw * 0.68f;
    float towerHd = hd * 0.68f;
    float towerW = towerHw * 2.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-towerHw, podiumH, -towerHd}, Vec3{towerHw, h, towerHd}, wallMat);

    // Clamped inpatient ward windows (never wider than wall span!)
    float winH = std::clamp(params_.useExtendedParams ? params_.windowHeight : 1.45f, 0.6f, flH - 0.8f);
    float sill = std::clamp(params_.useExtendedParams ? params_.windowSillHeight : 0.85f, 0.30f, flH - winH - 0.25f);
    if (wt == WindowType::RibbonStrip) {
        float ribW = std::max(1.5f, towerW - 1.8f);
        for (int story = 2; story < stories; ++story) {
            float winY = story * flH + sill + winH * 0.5f;
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{0.0f, winY, towerHd + 0.02f}, ribW, winH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{0.0f, winY, -towerHd - 0.02f}, ribW, winH,
                Vec3{0, 0, -1}, Vec3{-1, 0, 0}, wt, gt);
        }
    } else {
        int cols = std::max(2, params_.useExtendedParams ? params_.windowsPerFacade : 5);
        float span = std::max(2.0f, towerW - 1.6f);
        float winW = std::clamp(params_.useExtendedParams ? params_.windowWidth : 1.5f, 0.6f, (span / cols) - 0.35f);
        for (int story = 2; story < stories; ++story) {
            float winY = story * flH + sill + winH * 0.5f;
            for (int i = 0; i < cols; ++i) {
                float wx = -span * 0.5f + (i + 0.5f) * (span / cols);
                BuildingComponents::generateWindow(outVerts, outIndices,
                    Vec3{wx, winY, towerHd + 0.02f}, winW, winH,
                    Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
                BuildingComponents::generateWindow(outVerts, outIndices,
                    Vec3{wx, winY, -towerHd - 0.02f}, winW, winH,
                    Vec3{0, 0, -1}, Vec3{-1, 0, 0}, wt, gt);
            }
        }
    }

    if (params_.useExtendedParams && params_.hasCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, towerW, towerHd * 2.0f, h, 0.35f);
    }

    // Adaptive rooftop
    RoofType rt = params_.useExtendedParams ? params_.roofStyle : RoofType::FlatParapet;
    if (rt == RoofType::NoneFlat) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-towerHw - 0.1f, h - 0.05f, -towerHd - 0.1f},
            Vec3{towerHw + 0.1f, h + 0.24f, towerHd + 0.1f},
            static_cast<uint32_t>(MaterialSlot::RoofSurface));
    } else {
        RoofGenParams rParams;
        rParams.type = rt;
        rParams.baseWidth = towerW;
        rParams.baseDepth = towerHd * 2.0f;
        rParams.baseTopY = h;
        rParams.roofHeight = params_.useExtendedParams ? params_.roofHeight : 2.2f;
        rParams.overhang = params_.useExtendedParams ? params_.roofOverhang : 0.4f;
        rParams.showRooftopClutter = params_.useExtendedParams ? params_.showRooftopClutter : true;
        RoofGen::generate(outVerts, outIndices, rParams);
    }

    // Rooftop Emergency Helipad deck clamped safely inside tower roof span
    if (params_.hasHelipad && (rt == RoofType::FlatParapet || rt == RoofType::NoneFlat)) {
        float padSize = std::min(10.0f, std::min(towerHw, towerHd) * 1.4f);
        float padY = h + 1.2f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-padSize * 0.5f, padY, -padSize * 0.5f},
            Vec3{padSize * 0.5f, padY + 0.28f, padSize * 0.5f}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-padSize * 0.42f, h, -padSize * 0.42f},
            Vec3{-padSize * 0.32f, padY, -padSize * 0.32f}, hvacMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{ padSize * 0.32f, h,  padSize * 0.32f},
            Vec3{ padSize * 0.42f, padY,  padSize * 0.42f}, hvacMat);
    }
}

} // namespace procedural::buildings
