#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendCurtainGridBay(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x0, float y0, float x1, float y1,
    float zFace,
    int cols, int rows,
    uint32_t glassMat,
    uint32_t frameMat
) {
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{x0, y0, zFace - 0.10f},
        Vec3{x1, y1, zFace + 0.04f},
        glassMat
    );
    // Perimeter & grid silver/aluminum mullions
    float spanX = x1 - x0;
    float spanY = y1 - y0;
    for (int c = 0; c <= cols; ++c) {
        float mx = x0 + c * (spanX / static_cast<float>(cols));
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{mx - 0.045f, y0, zFace + 0.01f},
            Vec3{mx + 0.045f, y1, zFace + 0.10f},
            frameMat
        );
    }
    for (int r = 0; r <= rows; ++r) {
        float my = y0 + r * (spanY / static_cast<float>(rows));
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{x0, my - 0.045f, zFace + 0.01f},
            Vec3{x1, my + 0.045f, zFace + 0.10f},
            frameMat
        );
    }
}

} // namespace

void TowerGen::generateCorporateGlassPark(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 34.0f, 18.0f, 64.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 18.0f, 10.0f, 40.0f);
    int32_t stories = std::clamp(params_.stories, 1, 4);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 5.2f, 3.4f, 6.8f);
    float h = stories * storyH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite  = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t glassBlue   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t frameSilver = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t frameDark   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t paverWalk   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t planterBox  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t greenPlant  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    // 1. Tarmac Parking Lot, Curb Sidewalk & Brick Central Entrance Walkway
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, -0.18f, -hd - 3.0f},
        Vec3{ hw + 4.0f,  0.02f,  hd + 8.5f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, 0.02f, hd - 1.5f},
        Vec3{ hw + 2.0f, 0.16f, hd + 1.8f},
        paverWalk);
    // Brick-paver central walkway projecting to curb
    float centerBayHalfW = std::clamp(w * 0.145f, 3.2f, 6.2f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-centerBayHalfW * 0.72f, 0.02f, hd + 1.5f},
        Vec3{ centerBayHalfW * 0.72f, 0.16f, hd + 5.8f},
        paverWalk);

    // Twin Symmetric Entrance Landscape Planters with Ornamental Grass Bushes
    for (float side : {-1.0f, 1.0f}) {
        float px0 = (side < 0) ? (-centerBayHalfW * 2.15f) : (centerBayHalfW * 0.78f);
        float px1 = (side < 0) ? (-centerBayHalfW * 0.78f) : (centerBayHalfW * 2.15f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0, 0.02f, hd + 1.8f},
            Vec3{px1, 0.24f, hd + 4.9f},
            planterBox);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0 + 0.15f, 0.24f, hd + 1.95f},
            Vec3{px1 - 0.15f, 0.56f, hd + 4.75f},
            greenPlant);
        // Taller ornamental grass cluster inside planter
        float bx = 0.5f * (px0 + px1);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{bx, 0.50f, hd + 3.1f},
            Vec3{bx, 1.48f, hd + 3.1f},
            0.95f, 0.52f, 10, greenPlant, true, true
        );
    }

    // 2. Projecting Left & Right Crisp White Commercial Wings
    float plinthY = 0.16f;
    float recessZ = hd - 1.55f;
    float wingOuterMargin = 1.35f;

    // Rear full-width building core connecting left, center & right volumes
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{ hw, plinthY + h, recessZ},
        crispWhite);

    // Left Projecting Wing Mass
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, recessZ},
        Vec3{-centerBayHalfW, plinthY + h + 0.35f, hd},
        crispWhite);
    // Right Projecting Wing Mass
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerBayHalfW, plinthY, recessZ},
        Vec3{hw, plinthY + h + 0.35f, hd},
        crispWhite);

    // Recessed Center Entrance Vestibule Facade & Quad Storefront Doors
    float centerBayH = h - 0.45f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-centerBayHalfW, plinthY + centerBayH, recessZ - 0.2f},
        Vec3{ centerBayHalfW, plinthY + centerBayH + 0.24f, recessZ + 0.25f},
        frameSilver);
    // Double pairs of commercial glass entry doors at ground of center bay
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{-centerBayHalfW * 0.42f, plinthY, recessZ + 0.05f},
        2.35f, 2.55f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, false
    );
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{ centerBayHalfW * 0.42f, plinthY, recessZ + 0.05f},
        2.35f, 2.55f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, false
    );
    // Upper curtain glazing over recessed center doorway
    appendCurtainGridBay(
        outVerts, outIndices,
        -centerBayHalfW + 0.12f, plinthY + 2.60f, centerBayHalfW - 0.12f, plinthY + centerBayH,
        recessZ + 0.04f,
        3, std::max(3, stories * 3), glassBlue, frameSilver
    );

    // 3. Large Full-Height Reflective Curtain-Wall Grid Bays on Left & Right Wings
    float leftWinX0 = -hw + wingOuterMargin;
    float leftWinX1 = -centerBayHalfW - wingOuterMargin;
    if (leftWinX1 - leftWinX0 > 2.0f) {
        appendCurtainGridBay(
            outVerts, outIndices,
            leftWinX0, plinthY + 0.15f, leftWinX1, plinthY + h - 1.15f,
            hd + 0.04f,
            4, std::max(4, stories * 4), glassBlue, frameSilver
        );
    }
    float rightWinX0 = centerBayHalfW + wingOuterMargin;
    float rightWinX1 = hw - wingOuterMargin;
    if (rightWinX1 - rightWinX0 > 2.0f) {
        appendCurtainGridBay(
            outVerts, outIndices,
            rightWinX0, plinthY + 0.15f, rightWinX1, plinthY + h - 1.15f,
            hd + 0.04f,
            4, std::max(4, stories * 4), glassBlue, frameSilver
        );
    }

    // 4. Parking Signage Post on Right Curb (subtle real commercial detail)
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{centerBayHalfW * 2.5f, 0.16f, hd + 2.6f},
        Vec3{centerBayHalfW * 2.5f, 1.85f, hd + 2.6f},
        0.04f, 0.04f, 8, frameDark, true, true
    );
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerBayHalfW * 2.5f - 0.22f, 1.55f, hd + 2.58f},
        Vec3{centerBayHalfW * 2.5f + 0.22f, 2.05f, hd + 2.64f},
        glassBlue);

    // 5. Rear & Side Windows + Flat Roof Coping
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * storyH;
        for (int i = 0; i < 6; ++i) {
            float wx = -hw + 2.2f + i * ((w - 4.4f) / 5.0f);
            BuildingComponents::generateWindow(
                outVerts, outIndices,
                Vec3{wx, fy + storyH * 0.52f, -hd - 0.03f},
                2.1f, storyH * 0.68f,
                Vec3{0, 0, -1}, Vec3{-1, 0, 0},
                WindowType::CurtainWall, GlassType::ClearFloat
            );
        }
    }

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.5f, plinthY + h - 0.08f, -hd + 0.5f},
        Vec3{ hw - 0.5f, plinthY + h + 0.15f, recessZ - 0.3f},
        roofMat);

    if (params_.showRooftopClutter) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-3.0f, plinthY + h + 0.15f, -3.5f},
            Vec3{ 3.0f, plinthY + h + 1.25f,  0.5f},
            static_cast<uint32_t>(MaterialSlot::HvacMetal));
    }
}

} // namespace procedural::buildings
