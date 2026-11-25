#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void CivicGen::generateSchool(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int stories = std::clamp(params_.stories, 1, 6);
    float w = std::clamp(params_.mainWidth > 0.5f ? params_.mainWidth : 28.0f, 14.0f, 60.0f);
    float d = std::clamp(params_.mainDepth > 0.5f ? params_.mainDepth : 18.0f, 10.0f, 45.0f);
    float flH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.6f, 2.8f, 5.5f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt = params_.useExtendedParams ? params_.windowType : WindowType::DoubleHungSash;
    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::ClearFloat;
    DoorType dt = params_.useExtendedParams ? params_.doorType : DoorType::DoubleGlassCommercial;
    bool canopy = params_.useExtendedParams ? params_.hasEntranceCanopy : true;

    // Foundation plinth
    float wingW = std::clamp(params_.wingLength > 0.5f ? params_.wingLength : 10.0f, 5.0f, 24.0f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.45f - wingW - 0.5f, -0.6f, -hd - 0.5f},
        Vec3{hw * 0.45f + wingW + 0.5f, 0.0f, hd + 0.5f}, foundMat);

    // Central educational hall
    float centralW = w * 0.90f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.45f, 0.0f, -hd},
        Vec3{hw * 0.45f, h, hd}, wallMat);

    // East classroom wing & West auditorium/gym wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw * 0.45f, 0.0f, -hd * 0.6f},
        Vec3{hw * 0.45f + wingW, h * 0.85f, hd * 0.6f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.45f - wingW, 0.0f, -hd * 0.7f},
        Vec3{-hw * 0.45f, h * 1.10f, hd * 0.7f}, wallMat);

    // Scaled main entrance on central block
    float doorW = std::clamp(params_.useExtendedParams ? params_.doorWidth : 2.8f, 1.2f, centralW * 0.45f);
    float doorH = std::clamp(params_.useExtendedParams ? params_.doorHeight : 2.7f, 2.1f, flH - 0.25f);
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{0.0f, 0.0f, hd + 0.02f}, doorW, doorH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, dt, canopy);
    }

    // Scaled classroom windows across central hall and classroom wing
    float winH = std::clamp(params_.useExtendedParams ? params_.windowHeight : 1.65f, 0.6f, flH - 0.8f);
    float sill = std::clamp(params_.useExtendedParams ? params_.windowSillHeight : 0.80f, 0.30f, flH - winH - 0.25f);
    int cols = std::max(2, params_.useExtendedParams ? params_.windowsPerFacade : 4);
    float cSpan = std::max(2.0f, centralW - 1.4f);
    float winW = std::clamp(params_.useExtendedParams ? params_.windowWidth : 1.6f, 0.6f, (cSpan / cols) - 0.35f);

    for (int story = 0; story < stories; ++story) {
        float winY = story * flH + sill + winH * 0.5f;
        for (int i = 0; i < cols; ++i) {
            float wx = -cSpan * 0.5f + (i + 0.5f) * (cSpan / cols);
            if (story == 0 && std::abs(wx) < doorW * 0.65f + winW * 0.5f) continue;
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{wx, winY, hd + 0.02f}, winW, winH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{wx, winY, -hd - 0.02f}, winW, winH,
                Vec3{0, 0, -1}, Vec3{-1, 0, 0}, wt, gt);
        }

        if (story * flH + sill + winH < h * 0.85f - 0.2f) {
            float eWinW = std::min(winW, wingW * 0.32f);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{hw * 0.45f + wingW * 0.32f, winY, hd * 0.6f + 0.02f}, eWinW, winH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{hw * 0.45f + wingW * 0.68f, winY, hd * 0.6f + 0.02f}, eWinW, winH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
        }
    }

    if (params_.useExtendedParams && params_.hasCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, centralW, d, h, 0.35f);
    }

    // Adaptive roof over central hall
    RoofType rt = params_.useExtendedParams ? params_.roofStyle : RoofType::Gable;
    if (rt == RoofType::NoneFlat) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.45f - 0.1f, h - 0.05f, -hd - 0.1f},
            Vec3{hw * 0.45f + 0.1f, h + 0.24f, hd + 0.1f},
            static_cast<uint32_t>(MaterialSlot::RoofSurface));
    } else {
        RoofGenParams rParams;
        rParams.type = rt;
        rParams.baseWidth = centralW;
        rParams.baseDepth = d;
        rParams.baseTopY = h;
        rParams.roofHeight = params_.useExtendedParams ? params_.roofHeight : 3.0f;
        rParams.overhang = params_.useExtendedParams ? params_.roofOverhang : 0.5f;
        rParams.showRooftopClutter = params_.useExtendedParams ? params_.showRooftopClutter : false;
        RoofGen::generate(outVerts, outIndices, rParams);
    }
}

} // namespace procedural::buildings
