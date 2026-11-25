#include "HouseGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateCondominium88Nairobi(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // 88 Nairobi Condominium — Upper Hill, Nairobi (176m 44-storey residential landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 30.0f, 20.0f, 48.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 26.0f, 18.0f, 44.0f);
    int32_t stories = std::clamp(params_.stories, 18, 60);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.8f, 3.0f, 5.0f);
    float towerTopY = std::max(stories * storyH, 167.2f);
    float totalH = std::max(towerTopY + 8.8f, 176.0f);

    uint32_t crispWhite     = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t terracottaRed  = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t darkBasalt     = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t silverAlum     = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t condoGlass     = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t goldSkyLounge  = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t tarmacMat      = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Wide Charcoal Basalt & Crisp White 4-Level Residential Podium Base (matching uploaded photo)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 10.0f, -0.22f, -hd - 8.0f},
        Vec3{ hw + 10.0f,  0.02f,  hd + 10.0f},
        tarmacMat);
    float podiumH = storyH * 4.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 5.5f, 0.02f, -hd - 3.0f},
        Vec3{ hw + 5.5f, podiumH, hd + 3.0f},
        darkBasalt);
    // White overhanging podium terrace roof rim & recessed feature lobby billboard wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 5.8f, podiumH - 0.85f, -hd - 3.2f},
        Vec3{ hw + 5.8f, podiumH + 0.35f,  hd + 3.2f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.65f, 2.8f, hd + 2.95f},
        Vec3{ hw * 0.45f, podiumH - 2.8f, hd + 3.25f},
        crispWhite);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{-hw * 0.85f, 0.02f, hd + 3.1f},
        3.2f, 2.8f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true
    );

    // 2. Slender 44-Storey Residential Tower Glazed Envelope & Recessed Balconies
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.86f, podiumH, -hd * 0.84f},
        Vec3{ hw * 0.86f, towerTopY, hd * 0.84f},
        condoGlass);

    // Side & Corner Stacked White Projecting Residential Balcony Slabs across All Floors
    int floorSteps = std::clamp(stories, 20, 48);
    float spanH = towerTopY - podiumH;
    for (int f = 0; f <= floorSteps; ++f) {
        float fy = podiumH + f * (spanH / static_cast<float>(floorSteps));
        // Wraparound continuous slim white slab edge
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.94f, fy - 0.14f, -hd * 0.92f},
            Vec3{ hw * 0.94f, fy + 0.14f,  hd * 0.92f},
            crispWhite);
    }

    // 3. Signature Front Terracotta-Red & Charcoal Vertical Geometric Lattice Frames (above & below Sky Lounge)
    float loungeBotY = podiumH + spanH * 0.56f;
    float loungeTopY = loungeBotY + storyH * 2.5f;

    // Outer Bold White Flanking Vertical Pier Frames (0..176m)
    for (float sx : {-1.0f, 1.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx * hw * 0.84f - 0.85f, podiumH, -hd * 0.88f},
            Vec3{sx * hw * 0.84f + 0.85f, towerTopY + 4.5f, hd * 0.96f},
            crispWhite);
    }

    // Iconic Vertical Terracotta-Red Fins crowning the Upper Half & Geometric Lower Grid
    for (int fin = -3; fin <= 3; ++fin) {
        float fx = fin * (hw * 0.19f);
        // Upper section vertical terracotta fins extending past roof into penthouse pergola (loungeTopY .. totalH)
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.22f, loungeTopY, hd * 0.84f},
            Vec3{fx + 0.22f, totalH,     hd * 0.98f},
            terracottaRed);
        // Lower section terracotta/charcoal vertical frame grid
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.18f, podiumH,    hd * 0.84f},
            Vec3{fx + 0.18f, loungeBotY, hd * 0.95f},
            (fin % 2 == 0) ? darkBasalt : terracottaRed);
    }

    // 4. SIGNATURE CANTILEVERED CHAMPAGNE-GOLD MID-TOWER SKY-LOUNGE BOX (Floors ~26..28)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.52f, loungeBotY, hd * 0.45f},
        Vec3{ hw * 0.56f, loungeTopY, hd * 1.24f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.46f, loungeBotY + 0.75f, hd * 0.50f},
        Vec3{ hw * 0.50f, loungeTopY - 0.75f, hd * 1.27f},
        goldSkyLounge);

    // 5. Crown Penthouse Enclosure & Sky Pergola Beams reaching 176.0m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.72f, towerTopY, -hd * 0.70f},
        Vec3{ hw * 0.72f, totalH - 1.5f, hd * 0.70f},
        darkBasalt);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.84f, totalH - 1.5f, -hd * 0.84f},
        Vec3{ hw * 0.84f, totalH,         hd * 0.84f},
        silverAlum);
}

} // namespace procedural::buildings
