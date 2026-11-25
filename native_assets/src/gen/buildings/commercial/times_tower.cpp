#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateTimesTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Times Tower (KRA HQ) — Haile Selassie Ave, Nairobi (140m landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 34.0f, 22.0f, 54.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 28.0f, 18.0f, 46.0f);
    int32_t stories = std::clamp(params_.stories, 18, 55);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.85f, 3.0f, 5.2f);
    float shaftTopY = std::max(stories * storyH, 126.0f);
    float totalH = std::max(shaftTopY + 14.0f, 140.0f);

    uint32_t beigeStone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t crispWhite   = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t tealGlass    = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t darkMullion  = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverTrim   = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t tarmacMat    = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Banking Hall & Plaza Base Podium
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 8.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 8.0f,  0.02f,  hd + 8.0f},
        tarmacMat);
    float podiumH = storyH * 2.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, 0.02f, -hd - 2.5f},
        Vec3{ hw + 3.5f, podiumH, hd + 2.5f},
        beigeStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.55f, 0.02f, hd + 2.45f},
        Vec3{ hw * 0.55f, podiumH - 1.2f, hd + 2.75f},
        tealGlass);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.8f},
        4.0f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::RevolvingDoor, true
    );

    // 2. Monumental Precast Corner Piers (4 robust beige/cream vertical monoliths)
    float pierW = w * 0.25f;
    float pierD = d * 0.28f;
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-1.0f, 1.0f}) {
            float x0 = (sx < 0) ? -hw : (hw - pierW);
            float x1 = (sx < 0) ? (-hw + pierW) : hw;
            float z0 = (sz < 0) ? -hd : (hd - pierD);
            float z1 = (sz < 0) ? (-hd + pierD) : hd;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{x0, podiumH, z0},
                Vec3{x1, shaftTopY, z1},
                beigeStone);
        }
    }

    // 3. Deep Recessed Central Glazed Curtain-Wall Bays (matching Times Tower photo behind KICC)
    float recessHalfW = hw - pierW * 0.72f;
    float recessHalfD = hd - pierD * 0.72f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-recessHalfW, podiumH, -recessHalfD},
        Vec3{ recessHalfW, shaftTopY, recessHalfD},
        tealGlass);

    // Vertical & Horizontal Curtain-Wall Grid inside Recessed Central Bay
    for (int mc = -3; mc <= 3; ++mc) {
        float mx = mc * (recessHalfW * 0.24f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.14f, podiumH, -recessHalfD - 0.08f},
            Vec3{mx + 0.14f, shaftTopY, recessHalfD + 0.08f},
            darkMullion);
    }
    for (int f = 3; f < stories; ++f) {
        float fy = podiumH + (f - 2) * storyH;
        if (fy > shaftTopY - 2.0f) break;
        bool isMechanicalBelt = (f == stories / 2 || f == stories - 3);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-recessHalfW - 0.1f, fy - (isMechanicalBelt ? 0.55f : 0.16f), -recessHalfD - 0.12f},
            Vec3{ recessHalfW + 0.1f, fy + (isMechanicalBelt ? 0.55f : 0.16f),  recessHalfD + 0.12f},
            isMechanicalBelt ? beigeStone : darkMullion);
    }

    // 4. Signature Times Tower Wide Stepped Banking Crown & Rooftop Mechanical Caps (reaching 140m)
    float crownBotY = shaftTopY;
    float crownMidY = crownBotY + 6.5f;
    // Recessed crown glass collar directly beneath main cornice
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.96f, crownBotY, -hd * 0.96f},
        Vec3{ hw * 0.96f, crownBotY + 3.2f, hd * 0.96f},
        tealGlass);
    // Bold overhanging stepped banking crown slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.4f, crownBotY + 3.2f, -hd - 1.4f},
        Vec3{ hw + 1.4f, crownMidY,         hd + 1.4f},
        crispWhite);
    // Upper recessed crown parapet tier
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.72f, crownMidY, -hd * 0.72f},
        Vec3{ hw * 0.72f, totalH - 3.0f, hd * 0.72f},
        beigeStone);
    // Twin symmetric banking penthouse roof turrets reaching Y = 140.0m
    for (float sx : {-1.0f, 1.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx * hw * 0.25f - 2.8f, totalH - 3.2f, -3.5f},
            Vec3{sx * hw * 0.25f + 2.8f, totalH,         3.5f},
            silverTrim);
    }
}

} // namespace procedural::buildings
