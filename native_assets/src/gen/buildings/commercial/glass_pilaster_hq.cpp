#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

constexpr float kHqPi = 3.14159265358979323846f;

void appendArchedGlazedCanopy(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float ySpring,
    float zBack, float zFront,
    float spanW, float archRise,
    int arcSegs,
    uint32_t glassMat,
    uint32_t frameMat,
    uint32_t pillarMat
) {
    float halfW = spanW * 0.5f;
    float thick = 0.14f;
    for (int i = 0; i < arcSegs; ++i) {
        float t0 = static_cast<float>(i) / static_cast<float>(arcSegs);
        float t1 = static_cast<float>(i + 1) / static_cast<float>(arcSegs);
        float a0 = kHqPi * (1.0f - t0);
        float a1 = kHqPi * (1.0f - t1);

        float x0 = cx + halfW * std::cos(a0);
        float y0 = ySpring + archRise * std::sin(a0);
        float x1 = cx + halfW * std::cos(a1);
        float y1 = ySpring + archRise * std::sin(a1);

        Vec3 norm{0.5f * (std::cos(a0) + std::cos(a1)), 0.5f * (std::sin(a0) + std::sin(a1)), 0.0f};
        float len = norm.length();
        if (len > 1e-4f) norm = norm * (1.0f / len);

        // Top arched glass shell
        BuildingComponents::appendQuad(
            outVerts, outIndices,
            Vec3{x0, y0, zFront},
            Vec3{x1, y1, zFront},
            Vec3{x1, y1, zBack},
            Vec3{x0, y0, zBack},
            norm, glassMat
        );
        // Underside soffit quad
        BuildingComponents::appendQuad(
            outVerts, outIndices,
            Vec3{x0, y0 - thick, zBack},
            Vec3{x1, y1 - thick, zBack},
            Vec3{x1, y1 - thick, zFront},
            Vec3{x0, y0 - thick, zFront},
            -norm, frameMat
        );
    }
    // White side fascia beams & 4 supporting bronze/timber round columns
    for (float side : {-1.0f, 1.0f}) {
        float px = cx + side * halfW;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{px - 0.18f, ySpring - 0.25f, zBack},
            Vec3{px + 0.18f, ySpring + 0.18f, zFront + 0.15f},
            frameMat
        );
        for (float pz : {zBack + 1.0f, zFront - 0.35f}) {
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{px, 0.02f, pz}, Vec3{px, ySpring, pz},
                0.22f, 0.22f, 12, pillarMat, true, true
            );
        }
    }
}

} // namespace

void TowerGen::generateGlassPilasterHQ(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 46.0f, 22.0f, 76.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 18.0f, 12.0f, 38.0f);
    int32_t stories = std::clamp(params_.stories, 3, 12);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.35f, 2.8f, 4.8f);
    float totalH = stories * storyH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite  = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t blueGlass   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t tealCanopy  = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t frameDark   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t frameSilver = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t timberGold  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t greenLawn   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t curbWhite   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t hvacMat     = static_cast<uint32_t>(MaterialSlot::HvacMetal);

    // 1. Forecourt Arrival Driveway Loop, Black/White Curbing & Lawn Courtyard
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 5.0f, -0.18f, -hd - 3.5f},
        Vec3{ hw + 5.0f,  0.02f,  hd + 11.0f},
        tarmacMat);
    // Landscaped front lawn courtyard peninsula
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.45f, 0.02f, hd + 3.2f},
        Vec3{ hw * 0.78f, 0.22f, hd + 10.2f},
        greenLawn);
    // Curbing border along driveway edge
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.48f, 0.02f, hd + 3.0f},
        Vec3{ hw * 0.80f, 0.26f, hd + 3.25f},
        curbWhite);

    // 2. Main Multi-Story Curtain-Wall Core Box
    float plinthY = 0.22f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.15f, plinthY, -hd + 0.15f},
        Vec3{ hw - 0.15f, plinthY + totalH, hd - 0.15f},
        blueGlass);

    // Horizontal black aluminum spandrel bands & floor mullions wrapping all 4 facades
    for (int f = 0; f <= stories; ++f) {
        float fy = plinthY + f * storyH;
        float bandH = (f == 0 || f == stories) ? 0.35f : 0.28f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.05f, fy - bandH * 0.5f, -hd + 0.05f},
            Vec3{ hw - 0.05f, fy + bandH * 0.5f,  hd - 0.05f},
            frameDark);
        // Fine intermediate mullion rail mid-story
        if (f < stories) {
            float my = fy + storyH * 0.5f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw + 0.08f, my - 0.03f, -hd + 0.08f},
                Vec3{ hw - 0.08f, my + 0.03f,  hd - 0.08f},
                frameDark);
        }
    }

    // 3. Signature Rhythm of Proud White Vertical Structural Pilasters (13 Bays across Front & Back)
    int bayCount = std::clamp(static_cast<int>(w / 3.4f), 8, 16);
    float bayW = w / static_cast<float>(bayCount);
    float pilasterW = 0.72f;
    float parapetH = 1.15f;

    for (int i = 0; i <= bayCount; ++i) {
        float px = -hw + i * bayW;
        // Front vertical pilaster pier
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - pilasterW * 0.5f, plinthY, hd - 0.35f},
            Vec3{px + pilasterW * 0.5f, plinthY + totalH + parapetH, hd + 0.32f},
            crispWhite);
        // Rear vertical pilaster pier
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - pilasterW * 0.5f, plinthY, -hd - 0.32f},
            Vec3{px + pilasterW * 0.5f, plinthY + totalH + parapetH, -hd + 0.35f},
            crispWhite);

        // Center vertical slim glass mullion inside each bay
        if (i < bayCount) {
            float mx = px + bayW * 0.5f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{mx - 0.04f, plinthY, hd - 0.22f},
                Vec3{mx + 0.04f, plinthY + totalH, hd - 0.06f},
                frameDark);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{mx - 0.04f, plinthY, -hd + 0.06f},
                Vec3{mx + 0.04f, plinthY + totalH, -hd + 0.22f},
                frameDark);
        }
    }

    // End corner white structural wing walls (-X and +X ends)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.32f, plinthY, -hd - 0.32f},
        Vec3{-hw + 0.85f, plinthY + totalH + parapetH, hd + 0.32f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.85f, plinthY, -hd - 0.32f},
        Vec3{hw + 0.32f, plinthY + totalH + parapetH, hd + 0.32f},
        crispWhite);

    // 4. Crisp White Overhanging Top Parapet Crown Fascia & Crest Medallion Centerpiece
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.38f, plinthY + totalH, -hd - 0.38f},
        Vec3{ hw + 0.38f, plinthY + totalH + parapetH, hd + 0.42f},
        crispWhite);
    // Ornamental rooftop fascia crest tablet above entrance axis
    float crestX = hw * 0.16f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{crestX - 4.2f, plinthY + totalH + parapetH, hd - 0.1f},
        Vec3{crestX + 4.2f, plinthY + totalH + parapetH + 1.05f, hd + 0.36f},
        crispWhite);
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{crestX - 2.8f, plinthY + totalH + parapetH + 0.52f, hd + 0.34f},
        Vec3{crestX - 2.8f, plinthY + totalH + parapetH + 0.52f, hd + 0.48f},
        0.42f, 0.42f, 12, timberGold, true, true
    );

    // 5. Signature Arched Teal Glazed Porte-Cochère Entrance Canopy & Doors
    float entX = hw * 0.18f;
    appendArchedGlazedCanopy(
        outVerts, outIndices,
        entX, plinthY + 3.25f,
        hd + 0.25f, hd + 5.2f,
        7.6f, 1.45f, 12,
        tealCanopy, crispWhite, timberGold
    );
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{entX, plinthY, hd + 0.12f},
        3.4f, 2.85f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, false
    );

    // 6. Three Tall Ceremonial Silver Flagpoles beside Entrance Driveway
    for (int fp = 0; fp < 3; ++fp) {
        float fx = entX + 4.8f + fp * 1.45f;
        float fz = hd + 4.8f + fp * 0.4f;
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{fx, 0.22f, fz}, Vec3{fx, 8.8f, fz},
            0.065f, 0.04f, 8, frameSilver, true, true
        );
    }

    // 7. Flat Roof Deck & Mechanical Penthouse
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.8f, plinthY + totalH + parapetH - 0.25f, -hd + 0.8f},
        Vec3{ hw - 0.8f, plinthY + totalH + parapetH + 0.05f,  hd - 0.8f},
        static_cast<uint32_t>(MaterialSlot::RoofSurface));
    if (params_.showRooftopClutter) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-4.2f, plinthY + totalH + parapetH, -2.5f},
            Vec3{ 4.2f, plinthY + totalH + parapetH + 1.4f, 2.5f},
            hvacMat);
    }
}

} // namespace procedural::buildings
