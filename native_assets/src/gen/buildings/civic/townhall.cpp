#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void CivicGen::generateTownHall(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int stories = std::clamp(params_.stories, 1, 8);
    float w = std::clamp(params_.mainWidth > 0.5f ? params_.mainWidth : 28.0f, 14.0f, 60.0f);
    float d = std::clamp(params_.mainDepth > 0.5f ? params_.mainDepth : 18.0f, 10.0f, 45.0f);
    float flH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.6f, 2.8f, 5.5f);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    WindowType wt = params_.useExtendedParams ? params_.windowType : WindowType::ArchedHistoric;
    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::ClearFloat;
    DoorType dt = params_.useExtendedParams ? params_.doorType : DoorType::ArchedWooden;
    bool canopy = params_.useExtendedParams ? params_.hasEntranceCanopy : false;

    // Elevated classical stone plinth with grand entrance steps
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.8f, -1.0f, -hd - 0.8f},
        Vec3{hw + 0.8f, 0.0f, hd + 0.8f}, foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-std::min(6.0f, hw * 0.45f), -0.65f, hd + 0.8f},
        Vec3{std::min(6.0f, hw * 0.45f), 0.0f, hd + 3.2f}, foundMat);

    // Main municipal masonry building body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, h, hd}, wallMat);

    // Grand Entrance Neoclassical Portico Colonnade
    float porticoW = std::min(14.0f, w * 0.62f);
    if (params_.hasPortico) {
        float porticoD = 3.2f;
        float porticoH = h * 0.94f;
        int cols = std::clamp(params_.columnsCount, 2, 10);
        float colSpacing = porticoW / (cols - 1);

        for (int c = 0; c < cols; ++c) {
            float cx = -porticoW * 0.5f + c * colSpacing;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - 0.30f, 0.0f, hd + porticoD - 0.30f},
                Vec3{cx + 0.30f, porticoH, hd + porticoD + 0.30f}, foundMat);
        }

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-porticoW * 0.5f - 0.45f, porticoH, hd},
            Vec3{ porticoW * 0.5f + 0.45f, porticoH + 0.75f, hd + porticoD + 0.45f}, foundMat);

        Vec3 pedL{-porticoW * 0.5f - 0.45f, porticoH + 0.75f, hd + porticoD + 0.45f};
        Vec3 pedR{ porticoW * 0.5f + 0.45f, porticoH + 0.75f, hd + porticoD + 0.45f};
        Vec3 pedApex{0.0f, porticoH + 2.6f, hd + porticoD + 0.45f};
        Vec3 norm{0.0f, 0.0f, 1.0f};
        uint32_t bIdx = static_cast<uint32_t>(outVerts.size());
        outVerts.push_back({pedL, norm, Vec2{0, 0}, foundMat});
        outVerts.push_back({pedR, norm, Vec2{1, 0}, foundMat});
        outVerts.push_back({pedApex, norm, Vec2{0.5f, 1}, foundMat});
        outIndices.push_back(bIdx);
        outIndices.push_back(bIdx + 1);
        outIndices.push_back(bIdx + 2);
    }

    // Heavy entrance door clamped cleanly
    float doorW = std::clamp(params_.useExtendedParams ? params_.doorWidth : 2.4f, 1.2f, w * 0.35f);
    float doorH = std::clamp(params_.useExtendedParams ? params_.doorHeight : 3.0f, 2.1f, flH - 0.25f);
    if (dt != DoorType::None) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{0.0f, 0.0f, hd + 0.02f}, doorW, doorH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, dt, canopy);
    }

    // Scaled arched / historic windows across all stories
    int cols = std::max(2, params_.useExtendedParams ? params_.windowsPerFacade : 4);
    float span = std::max(3.0f, w - 2.0f);
    float winW = std::clamp(params_.useExtendedParams ? params_.windowWidth : 1.55f, 0.6f, (span / cols) - 0.4f);
    float winH = std::clamp(params_.useExtendedParams ? params_.windowHeight : 1.85f, 0.7f, flH - 0.8f);
    float sill = std::clamp(params_.useExtendedParams ? params_.windowSillHeight : 0.85f, 0.30f, flH - winH - 0.25f);

    for (int story = 0; story < stories; ++story) {
        float winY = story * flH + sill + winH * 0.5f;
        for (int i = 0; i < cols; ++i) {
            float wx = -span * 0.5f + (i + 0.5f) * (span / cols);
            if (story == 0 && std::abs(wx) < doorW * 0.65f + winW * 0.5f) continue;
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{wx, winY, hd + 0.02f}, winW, winH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{wx, winY, -hd - 0.02f}, winW, winH,
                Vec3{0, 0, -1}, Vec3{-1, 0, 0}, wt, gt);
        }
    }

    if (params_.useExtendedParams ? params_.hasCornice : true) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, h, 0.42f);
    }

    // Central Clock Tower & Spire
    if (params_.hasClockTower) {
        float towerW = std::min(6.0f, std::min(w, d) * 0.38f);
        float towerH = 9.0f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-towerW * 0.5f, h, -towerW * 0.5f},
            Vec3{ towerW * 0.5f, h + towerH, towerW * 0.5f}, wallMat);

        float clockY = h + towerH * 0.65f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-1.1f, clockY - 1.1f, towerW * 0.5f},
            Vec3{ 1.1f, clockY + 1.1f, towerW * 0.5f + 0.15f}, frameMat);

        Vec3 spApex{0.0f, h + towerH + 5.5f, 0.0f};
        Vec3 spFL{-towerW * 0.5f, h + towerH,  towerW * 0.5f};
        Vec3 spFR{ towerW * 0.5f, h + towerH,  towerW * 0.5f};
        Vec3 spBL{-towerW * 0.5f, h + towerH, -towerW * 0.5f};
        Vec3 spBR{ towerW * 0.5f, h + towerH, -towerW * 0.5f};

        auto addTri = [&](const Vec3& a, const Vec3& b, const Vec3& c) {
            uint32_t base = static_cast<uint32_t>(outVerts.size());
            Vec3 n = (b - a).cross(c - a).normalized();
            outVerts.push_back({a, n, Vec2{0, 0}, roofMat});
            outVerts.push_back({b, n, Vec2{1, 0}, roofMat});
            outVerts.push_back({c, n, Vec2{0.5f, 1}, roofMat});
            outIndices.push_back(base);
            outIndices.push_back(base + 1);
            outIndices.push_back(base + 2);
        };
        addTri(spFL, spFR, spApex);
        addTri(spFR, spBR, spApex);
        addTri(spBR, spBL, spApex);
        addTri(spBL, spFL, spApex);
    }

    // Adaptive Roof
    RoofType rt = params_.useExtendedParams ? params_.roofStyle : RoofType::Mansard;
    if (rt == RoofType::NoneFlat) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.1f, h - 0.05f, -hd - 0.1f},
            Vec3{hw + 0.1f, h + 0.24f, hd + 0.1f}, roofMat);
    } else {
        RoofGenParams rParams;
        rParams.type = rt;
        rParams.baseWidth = w;
        rParams.baseDepth = d;
        rParams.baseTopY = h;
        rParams.roofHeight = params_.useExtendedParams ? params_.roofHeight : 3.2f;
        rParams.overhang = params_.useExtendedParams ? params_.roofOverhang : 0.6f;
        rParams.showRooftopClutter = params_.useExtendedParams ? params_.showRooftopClutter : false;
        RoofGen::generate(outVerts, outIndices, rParams);
    }
}

} // namespace procedural::buildings
