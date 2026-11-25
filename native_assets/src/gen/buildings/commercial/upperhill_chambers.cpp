#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateUpperHillChambers(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Upper Hill Chambers — 2nd Ngong Ave, Nairobi (104m terraced modern office tower)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 32.0f, 20.0f, 48.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 26.0f, 18.0f, 42.0f);
    int32_t stories = std::clamp(params_.stories, 12, 42);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.7f, 3.0f, 5.0f);
    float mainTopY = std::max(stories * storyH, 96.0f);
    float totalH = std::max(mainTopY + 8.0f, 104.0f);

    uint32_t darkBasalt   = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t silverAlum   = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t gunmetal     = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t mirrorGlass  = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t tarmacMat    = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t turfGreen    = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Landscaped Tiered Corporate Podium & Drop-Off Court
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 9.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 9.0f,  0.02f,  hd + 9.0f},
        tarmacMat);
    float podiumH = storyH * 3.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, 0.02f, -hd - 2.2f},
        Vec3{ hw + 3.5f, podiumH, hd + 2.2f},
        darkBasalt);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.2f, podiumH, -hd - 2.0f},
        Vec3{-hw * 0.3f, podiumH + 0.35f, hd + 2.0f},
        turfGreen);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.25f},
        3.6f, 3.1f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true
    );

    // 2. Offset Lower-Tier Glazed Wing (up to 62% height) & Tall Main Gunmetal Curtain Tower
    float wingTopY = mainTopY * 0.62f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.94f, podiumH, -hd * 0.65f},
        Vec3{-hw * 0.15f, wingTopY, hd * 0.92f},
        mirrorGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.18f, podiumH, -hd * 0.85f},
        Vec3{ hw * 0.92f, mainTopY, hd * 0.85f},
        mirrorGlass);

    // 3. Vertical Brise-Soleil Aluminum Blade Fins & Floor Spandrels
    for (int f = 3; f <= stories; ++f) {
        float fy = podiumH + (f - 2) * storyH;
        if (fy > mainTopY) break;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.20f, fy - 0.14f, -hd * 0.87f},
            Vec3{ hw * 0.94f, fy + 0.14f,  hd * 0.87f},
            gunmetal);
        if (fy <= wingTopY) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw * 0.96f, fy - 0.14f, -hd * 0.67f},
                Vec3{-hw * 0.14f, fy + 0.14f,  hd * 0.94f},
                silverAlum);
        }
    }

    for (int c = 0; c <= 7; ++c) {
        float fx = -hw * 0.14f + c * ((hw * 1.02f) / 7.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.14f, podiumH, hd * 0.84f},
            Vec3{fx + 0.14f, mainTopY + 1.5f, hd * 0.98f},
            silverAlum);
    }

    // 4. Stepped Top Cantilever Penthouse & Slatted Sun Louver Screen at 104.0m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.45f, mainTopY, -hd * 0.72f},
        Vec3{ hw * 0.75f, totalH - 1.5f, hd * 0.72f},
        gunmetal);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.48f, totalH - 1.5f, -hd * 0.75f},
        Vec3{ hw * 0.78f, totalH,        hd * 0.75f},
        silverAlum);
}

} // namespace procedural::buildings
