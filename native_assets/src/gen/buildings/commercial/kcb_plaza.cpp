#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateKcbPlaza(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // KCB Plaza (Kencom House Upper Hill Tower) — Nairobi (109m landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 34.0f, 22.0f, 52.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 26.0f, 18.0f, 42.0f);
    int32_t stories = std::clamp(params_.stories, 12, 42);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 4.0f, 3.0f, 5.2f);
    float mainTopY = std::max(stories * storyH, 96.0f);
    float totalH = std::max(mainTopY + 13.0f, 109.0f);

    uint32_t silverAlum   = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t crispWhite   = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t emeraldGlass = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t darkFrame    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat    = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t kcbGreen     = static_cast<uint32_t>(MaterialSlot::HydraulicGreen);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Cantilevered Banking Podium & Sweeping Entrance Porte-Cochere
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 9.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 9.0f,  0.02f,  hd + 10.0f},
        tarmacMat);
    float podiumH = storyH * 2.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, 0.02f, -hd - 2.0f},
        Vec3{ hw + 3.5f, podiumH, hd + 2.5f},
        silverAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.58f, podiumH - 0.4f, hd + 2.2f},
        Vec3{ hw * 0.58f, podiumH + 0.45f, hd + 7.2f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.45f, podiumH + 0.45f, hd + 6.7f},
        Vec3{ hw * 0.45f, podiumH + 0.95f, hd + 7.2f},
        kcbGreen);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.55f},
        4.2f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::RevolvingDoor, true
    );

    // 2. Main Emerald Curtain-Wall Tower Volume & White Alucobond Pier Ribs
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.88f, podiumH, -hd * 0.82f},
        Vec3{ hw * 0.88f, mainTopY, hd * 0.82f},
        emeraldGlass);
    // Horizontal white aluminum spandrel ribbons across every floor
    for (int f = 3; f <= stories; ++f) {
        float fy = podiumH + (f - 2) * storyH;
        if (fy > mainTopY) break;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.90f, fy - 0.16f, -hd * 0.84f},
            Vec3{ hw * 0.90f, fy + 0.16f,  hd * 0.84f},
            (f % 4 == 0) ? crispWhite : darkFrame);
    }

    // Left Anchor White Stone/Clad Spine
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.95f, podiumH, -hd * 0.65f},
        Vec3{-hw * 0.60f, mainTopY + 3.0f, hd * 0.88f},
        crispWhite);

    // 3. Signature Projecting Upper Cantilevered Sky-Box Wing (Floors 17..23)
    float cantBotY = mainTopY * 0.66f;
    float cantTopY = mainTopY - 2.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw * 0.05f, cantBotY, hd * 0.40f},
        Vec3{hw * 0.98f, cantTopY, hd * 1.10f},
        emeraldGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw * 0.02f, cantBotY - 0.5f, hd * 0.38f},
        Vec3{hw * 1.01f, cantBotY + 0.3f, hd * 1.13f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw * 0.02f, cantTopY - 0.3f, hd * 0.38f},
        Vec3{hw * 1.01f, cantTopY + 0.7f, hd * 1.13f},
        crispWhite);

    // 4. Corporate Rooftop Pergola Crown & Helipad Spine reaching 109.0m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.72f, mainTopY, -hd * 0.68f},
        Vec3{ hw * 0.72f, totalH - 3.5f, hd * 0.68f},
        silverAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.75f, totalH - 3.5f, -hd * 0.72f},
        Vec3{ hw * 0.75f, totalH - 2.2f,  hd * 0.72f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.35f, totalH - 2.2f, -hd * 0.25f},
        Vec3{ hw * 0.35f, totalH,         hd * 0.25f},
        kcbGreen);
}

} // namespace procedural::buildings
