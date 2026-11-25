#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void CivicGen::generateParliamentTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Parliament Tower (Bunge Tower) — Parliament Road, Nairobi (125m civic landmark)
    float w = std::clamp(params_.mainWidth > 1.0f ? params_.mainWidth : 34.0f, 22.0f, 52.0f);
    float d = std::clamp(params_.mainDepth > 1.0f ? params_.mainDepth : 28.0f, 18.0f, 44.0f);
    int32_t stories = std::clamp(params_.stories, 14, 48);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 4.0f, 3.0f, 5.2f);
    float shaftTopY = std::max(stories * storyH, 112.0f);
    float totalH = std::max(shaftTopY + 13.0f, 125.0f);

    uint32_t limestone     = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t champagneAlum = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t crispWhite    = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t cobaltGlass   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t goldGlass     = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t darkFrame     = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Ceremonial Parliamentary Plinth, Colonnade Arcade & Grand Portico Steps
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 10.0f, -0.22f, -hd - 8.0f},
        Vec3{ hw + 10.0f,  0.02f,  hd + 10.0f},
        tarmacMat);
    float podiumH = storyH * 3.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, 0.02f, -hd - 2.5f},
        Vec3{ hw + 4.0f, podiumH, hd + 2.5f},
        limestone);
    // Ceremonial steps at front entrance
    for (int s = 0; s < 4; ++s) {
        float sy = 0.02f + s * 0.35f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.48f, sy, hd + 2.5f},
            Vec3{ hw * 0.48f, sy + 0.35f, hd + 6.0f - s * 0.8f},
            limestone);
    }
    // Tall Double-Height Parliamentary Colonnade Pillars across Front & Rear Podium
    for (int c = -4; c <= 4; ++c) {
        float cx = c * (hw * 0.25f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.50f, 1.4f, hd + 1.8f},
            Vec3{cx + 0.50f, podiumH, hd + 2.8f},
            crispWhite);
    }
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 1.4f, hd + 2.55f},
        4.2f, 3.6f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::CanopiedGrandPortal, true
    );

    // 2. Faceted Diamond-Chamfered Blue/Gold Legislative Curtain-Wall Tower Shaft
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.85f, podiumH, -hd * 0.85f},
        Vec3{ hw * 0.85f, shaftTopY, hd * 0.85f},
        cobaltGlass);
    // Prominent projecting center chamfered legislative chamber spine
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.42f, podiumH, -hd * 0.96f},
        Vec3{ hw * 0.42f, shaftTopY + 3.0f, hd * 0.96f},
        goldGlass);

    // Horizontal Spandrels & Rhythmic Champagne-Gold Vertical Fin Piers
    for (int f = 3; f <= stories; ++f) {
        float fy = podiumH + (f - 2) * storyH;
        if (fy > shaftTopY) break;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.87f, fy - 0.16f, -hd * 0.87f},
            Vec3{ hw * 0.87f, fy + 0.16f,  hd * 0.87f},
            darkFrame);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.43f, fy - 0.16f, -hd * 0.98f},
            Vec3{ hw * 0.43f, fy + 0.16f,  hd * 0.98f},
            champagneAlum);
    }

    for (int c = -5; c <= 5; ++c) {
        float fx = c * (hw * 0.16f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.25f, podiumH, -hd * 0.99f},
            Vec3{fx + 0.25f, shaftTopY + 3.5f, hd * 0.99f},
            (std::abs(c) >= 4) ? limestone : champagneAlum);
    }

    // 3. Signature Cantilevered Flaring Legislative Sky-Gallery & Stepped Crown Cornice (125.0m)
    float crownNeckY = shaftTopY + 3.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.98f, crownNeckY, -hd * 0.98f},
        Vec3{ hw * 0.98f, crownNeckY + 4.2f, hd * 0.98f},
        limestone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.92f, crownNeckY + 1.0f, -hd * 0.92f},
        Vec3{ hw * 0.92f, crownNeckY + 3.2f,  hd * 0.92f},
        goldGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.62f, crownNeckY + 4.2f, -hd * 0.62f},
        Vec3{ hw * 0.62f, totalH - 3.5f,      hd * 0.62f},
        champagneAlum);
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{0.0f, totalH - 3.6f, 0.0f},
        Vec3{0.0f, totalH,        0.0f},
        0.42f, 0.12f, 10, limestone, true, true
    );
}

} // namespace procedural::buildings
