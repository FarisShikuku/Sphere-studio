#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateUapOldMutualTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // UAP Old Mutual Tower — Upper Hill, Nairobi (163m limestone & terracotta landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 34.0f, 22.0f, 54.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 28.0f, 18.0f, 46.0f);
    int32_t stories = std::clamp(params_.stories, 18, 55);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 4.0f, 3.0f, 5.5f);
    float coreTopY = std::max(stories * storyH, 132.0f);
    float lanternBaseY = coreTopY;
    float lanternTopY = std::max(lanternBaseY + 13.0f, 145.0f);
    float spireTipY = std::max(lanternTopY + 18.0f, 163.0f);

    uint32_t creamLimestone = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t roseTerracotta = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t crispWhite     = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t darkGlass      = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t spandrelDark   = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t silverAlum     = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t redBrandSign   = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t tarmacMat      = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Wide Ceremonial Colonnade Podium (Terracotta Stone + Dark Glazed Lobby)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 12.0f, -0.22f, -hd - 9.0f},
        Vec3{ hw + 12.0f,  0.02f,  hd + 10.0f},
        tarmacMat);

    float podiumColonH = 11.5f;
    // Recessed glass lobby wings flanking the tower base
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 9.5f, 0.02f, -hd * 0.55f},
        Vec3{ hw + 9.5f, podiumColonH - 1.4f, hd * 0.85f},
        spandrelDark);
    // Overhanging terracotta podium entablature beam + bold columns
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 10.0f, podiumColonH - 1.4f, -hd * 0.62f},
        Vec3{ hw + 10.0f, podiumColonH,        hd * 0.94f},
        roseTerracotta);
    for (int col = 0; col <= 8; ++col) {
        float cx = -hw - 9.4f + col * ((2.0f * (hw + 9.4f)) / 8.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.55f, 0.02f, hd * 0.82f},
            Vec3{cx + 0.55f, podiumColonH - 1.4f, hd * 0.94f},
            roseTerracotta);
    }
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd * 0.95f},
        4.2f, 3.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true
    );

    // 2. Rose-Terracotta 6-Storey Lower Shaft Waistband (11.5m .. 36m)
    float terraWaistTopY = 36.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.86f, podiumColonH, -hd * 0.84f},
        Vec3{ hw * 0.86f, terraWaistTopY, hd * 0.84f},
        roseTerracotta);

    // 3. Stepped Cream Limestone Upper Tower Volume (36m .. 132m)
    // Tier A: Main Central Shaft up to full coreTopY
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.68f, terraWaistTopY, -hd * 0.72f},
        Vec3{ hw * 0.68f, coreTopY,        hd * 0.68f},
        creamLimestone);

    // Tier B: Front Projecting Stepped Shoulder Block (up to 64% height)
    float midShoulderTopY = coreTopY * 0.64f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.48f, terraWaistTopY, hd * 0.35f},
        Vec3{ hw * 0.52f, midShoulderTopY, hd * 0.86f},
        creamLimestone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.49f, midShoulderTopY - 0.5f, hd * 0.34f},
        Vec3{ hw * 0.53f, midShoulderTopY + 0.45f, hd * 0.88f},
        silverAlum);

    // Tier C: Left Setback Shoulder Block (up to 78% height)
    float leftShoulderTopY = coreTopY * 0.78f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.84f, terraWaistTopY, -hd * 0.62f},
        Vec3{-hw * 0.40f, leftShoulderTopY, hd * 0.56f},
        creamLimestone);

    // Rear Curtain-Wall Glazed Spine Facade (-X/-Z corner glass wall as seen on left of photo)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.88f, podiumColonH, -hd * 0.55f},
        Vec3{-hw * 0.72f, leftShoulderTopY - 4.0f, hd * 0.32f},
        darkGlass);

    // 4. Deep Vertical Recessed Window Slots & Horizontal Stone Cornice Reveals
    for (int f = 3; f < stories; ++f) {
        float fy = podiumColonH + (f - 2) * storyH;
        if (fy > coreTopY - 5.0f) break;

        // Vertical punched/slotted windows across front faces
        for (float wx : {-0.46f, -0.22f, 0.0f, 0.22f, 0.44f}) {
            float xPos = wx * hw * 0.85f;
            float winZ = (fy < midShoulderTopY && std::abs(wx) < 0.48f) ? (hd * 0.865f) : (hd * 0.685f);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{xPos - 0.62f, fy + 0.4f, winZ - 0.12f},
                Vec3{xPos + 0.62f, fy + storyH * 0.72f, winZ + 0.05f},
                darkGlass);
        }
        // Subtle perimeter reveal band every 5 floors
        if (f % 5 == 0) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw * 0.70f, fy - 0.12f, -hd * 0.74f},
                Vec3{ hw * 0.70f, fy + 0.12f,  hd * 0.70f},
                silverAlum);
        }
    }

    // Iconic Red UAP Brand Square Plaque high on Front-Right Corner Pier
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw * 0.34f, coreTopY - 12.5f, hd * 0.68f},
        Vec3{hw * 0.64f, coreTopY - 6.8f,  hd * 0.74f},
        redBrandSign);

    // 5. Stepped Penthouse Attic, Pyramidal White Lantern Crown & Pinnacle Spire (reaching 163m)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.60f, coreTopY, -hd * 0.62f},
        Vec3{ hw * 0.60f, coreTopY + 5.2f, hd * 0.60f},
        creamLimestone);
    // Recessed penthouse sky-lounge glass band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.56f, coreTopY + 1.0f, -hd * 0.58f},
        Vec3{ hw * 0.56f, coreTopY + 4.0f,  hd * 0.56f},
        darkGlass);

    // Pyramidal 4-sided lantern roof rising above attic
    float pyrBaseY = coreTopY + 5.2f;
    float pyrR = hw * 0.36f;
    float pyrD = hd * 0.36f;
    float pyrApexY = lanternTopY;
    auto pushTri = [&](const Vec3& a, const Vec3& b, const Vec3& c, uint32_t m) {
        Vec3 n = (b - a).cross(c - a).normalized();
        uint32_t idx = static_cast<uint32_t>(outVerts.size());
        outVerts.push_back({a, n, {0.0f, 0.0f}, m});
        outVerts.push_back({b, n, {1.0f, 0.0f}, m});
        outVerts.push_back({c, n, {0.5f, 1.0f}, m});
        outIndices.push_back(idx);
        outIndices.push_back(idx + 1);
        outIndices.push_back(idx + 2);
    };
    Vec3 apex{0.0f, pyrApexY, 0.0f};
    Vec3 c0{-pyrR, pyrBaseY,  pyrD};
    Vec3 c1{ pyrR, pyrBaseY,  pyrD};
    Vec3 c2{ pyrR, pyrBaseY, -pyrD};
    Vec3 c3{-pyrR, pyrBaseY, -pyrD};
    pushTri(c0, c1, apex, crispWhite);
    pushTri(c1, c2, apex, crispWhite);
    pushTri(c2, c3, apex, crispWhite);
    pushTri(c3, c0, apex, crispWhite);

    // Lantern Finial Base Drum & Pinnacle Spire reaching Y = 163.0m
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{0.0f, pyrApexY - 0.5f, 0.0f},
        Vec3{0.0f, pyrApexY + 3.2f, 0.0f},
        1.55f, 1.15f, 12, silverAlum, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{0.0f, pyrApexY + 3.0f, 0.0f},
        Vec3{0.0f, spireTipY,       0.0f},
        0.55f, 0.08f, 10, darkGlass, true, true
    );
}

} // namespace procedural::buildings
