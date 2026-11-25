#include "HouseGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateGtcResidential1(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // GTC Residential 1 — Westlands, Nairobi (116m luxury apartment tower)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 30.0f, 20.0f, 48.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 24.0f, 16.0f, 40.0f);
    int32_t stories = std::clamp(params_.stories, 14, 45);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.5f, 2.8f, 4.8f);
    float shaftTopY = std::max(stories * storyH, 108.5f);
    float totalH = std::max(shaftTopY + 7.5f, 116.0f);

    uint32_t champagneStone = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t crispWhite     = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t bronzeGlass    = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassBronze);
    uint32_t darkFrame      = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t turfGreen      = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t tarmacMat      = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Club Garden Podium Deck & Entrance Porte-Cochere
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 8.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 8.0f,  0.02f,  hd + 9.0f},
        tarmacMat);
    float podiumH = storyH * 3.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, 0.02f, -hd - 2.2f},
        Vec3{ hw + 3.5f, podiumH, hd + 2.2f},
        champagneStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.0f, podiumH, -hd - 2.0f},
        Vec3{ hw + 3.0f, podiumH + 0.35f, hd + 2.0f},
        turfGreen);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.25f},
        3.4f, 3.0f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true
    );

    // 2. Twin Linked Apartment Wing Massing with Recessed Loggia Balconies
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.88f, podiumH, -hd * 0.84f},
        Vec3{ hw * 0.88f, shaftTopY, hd * 0.84f},
        bronzeGlass);

    // Vertical Stone Pilaster Piers dividing Residential Balcony Bays
    int bays = 6;
    for (int b = 0; b <= bays; ++b) {
        float bx = -hw * 0.90f + b * ((2.0f * hw * 0.90f) / static_cast<float>(bays));
        bool isCornerOrCenter = (b == 0 || b == bays || b == bays / 2);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx - (isCornerOrCenter ? 0.62f : 0.32f), podiumH, -hd * 0.90f},
            Vec3{bx + (isCornerOrCenter ? 0.62f : 0.32f), shaftTopY + 3.5f, hd * 0.90f},
            champagneStone);
    }

    // Stacked Balcony Floor Slabs & Glass Balustrades across Every Level
    int floorCount = std::clamp(stories, 16, 36);
    float spanH = shaftTopY - podiumH;
    for (int f = 0; f <= floorCount; ++f) {
        float fy = podiumH + f * (spanH / static_cast<float>(floorCount));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.92f, fy - 0.14f, -hd * 0.92f},
            Vec3{ hw * 0.92f, fy + 0.14f,  hd * 0.92f},
            crispWhite);
        if (f < floorCount) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw * 0.86f, fy + 0.14f, hd * 0.89f},
                Vec3{ hw * 0.86f, fy + 1.05f, hd * 0.92f},
                static_cast<uint32_t>(MaterialSlot::BalconyGlass));
        }
    }

    // 3. Signature GTC Crown Portal Frame & Penthouse Pergola at 116.0m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.72f, shaftTopY, -hd * 0.70f},
        Vec3{ hw * 0.72f, totalH - 2.2f, hd * 0.70f},
        darkFrame);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.92f, totalH - 2.2f, -hd * 0.90f},
        Vec3{ hw * 0.92f, totalH,        hd * 0.90f},
        crispWhite);
}

} // namespace procedural::buildings
