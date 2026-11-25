#include "HouseGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateAlturaTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Altura Tower — Kilimani / Upper Hill, Nairobi (110m residential high-rise landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 28.0f, 18.0f, 46.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 24.0f, 16.0f, 40.0f);
    int32_t stories = std::clamp(params_.stories, 14, 44);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.45f, 2.8f, 4.8f);
    float shaftTopY = std::max(stories * storyH, 103.5f);
    float totalH = std::max(shaftTopY + 6.5f, 110.0f);

    uint32_t crispWhite    = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t darkBasalt    = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t warmTimber    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t glassPane     = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t balconyGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t turfGreen     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Podium Lobby Base & Arrival Court
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 8.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 8.0f,  0.02f,  hd + 9.0f},
        tarmacMat);
    float podiumH = storyH * 2.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.0f, 0.02f, -hd - 2.0f},
        Vec3{ hw + 3.0f, podiumH, hd + 2.0f},
        darkBasalt);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.05f},
        3.2f, 2.8f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true
    );

    // 2. Interlocking High-Rise Residential Shaft with Warm Cedar & White C-Frame Bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.86f, podiumH, -hd * 0.82f},
        Vec3{ hw * 0.86f, shaftTopY, hd * 0.82f},
        glassPane);

    // Sculpted Left & Right Interlocking White C-Frame Spine Bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.94f, podiumH, -hd * 0.88f},
        Vec3{-hw * 0.25f, shaftTopY * 0.96f, hd * 0.88f},
        darkBasalt);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.90f, podiumH, hd * 0.72f},
        Vec3{-hw * 0.30f, shaftTopY * 0.96f, hd * 0.90f},
        warmTimber);

    // Wraparound Balcony Floor Slabs & Two Double-Height Sky-Garden Cutout Terraces
    int floorCount = std::clamp(stories, 16, 36);
    float spanH = shaftTopY - podiumH;
    int gardenFloorA = floorCount / 3;
    int gardenFloorB = (floorCount * 2) / 3;

    for (int f = 0; f <= floorCount; ++f) {
        float fy = podiumH + f * (spanH / static_cast<float>(floorCount));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.94f, fy - 0.14f, -hd * 0.90f},
            Vec3{ hw * 0.94f, fy + 0.14f,  hd * 0.90f},
            crispWhite);
        if (f < floorCount) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw * 0.18f, fy + 0.14f, hd * 0.87f},
                Vec3{ hw * 0.88f, fy + 1.05f, hd * 0.90f},
                balconyGlass);
        }
        // Sky-Garden Terrace planters
        if (f == gardenFloorA || f == gardenFloorB) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw * 0.18f, fy + 0.14f, hd * 0.60f},
                Vec3{ hw * 0.86f, fy + 0.85f, hd * 0.92f},
                turfGreen);
        }
    }

    // 3. Rooftop Sky-Lounge Pergola & Crown Parapet at 110.0m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.72f, shaftTopY, -hd * 0.68f},
        Vec3{ hw * 0.72f, totalH - 1.5f, hd * 0.68f},
        darkBasalt);
    for (int r = -3; r <= 3; ++r) {
        float rx = r * (hw * 0.22f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rx - 0.18f, totalH - 1.5f, -hd * 0.76f},
            Vec3{rx + 0.18f, totalH,         hd * 0.76f},
            crispWhite);
    }
}

} // namespace procedural::buildings
