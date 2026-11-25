#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendGreenTerraceDrum(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float cz,
    float y0, float height,
    float radius,
    float slabThick,
    uint32_t slabMat,
    uint32_t glassMat,
    uint32_t greenMat,
    uint32_t frameMat,
    bool hasTopGarden = true
) {
    float glassH = std::max(0.5f, height - slabThick);
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{cx, y0, cz},
        Vec3{cx, y0 + glassH, cz},
        radius - 0.25f, radius - 0.25f, 24, glassMat, true, true
    );
    // Vertical perimeter fins
    for (int i = 0; i < 12; ++i) {
        float ang = 6.2831853f * static_cast<float>(i) / 12.0f;
        float mx = cx + std::cos(ang) * (radius - 0.18f);
        float mz = cz + std::sin(ang) * (radius - 0.18f);
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{mx - 0.05f, y0, mz - 0.05f},
            Vec3{mx + 0.05f, y0 + glassH, mz + 0.05f},
            frameMat
        );
    }
    // Overhanging white cantilevered slab disk
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{cx, y0 + glassH, cz},
        Vec3{cx, y0 + height, cz},
        radius + 0.45f, radius + 0.45f, 24, slabMat, true, true
    );
    if (hasTopGarden) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{cx, y0 + height, cz},
            Vec3{cx, y0 + height + 0.24f, cz},
            radius + 0.15f, radius + 0.15f, 24, greenMat, true, true
        );
    }
}

void appendMiniEcoTree(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x, float y0, float z,
    float scale,
    uint32_t trunkMat, uint32_t leafMat
) {
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{x, y0, z}, Vec3{x, y0 + 1.0f * scale, z},
        0.10f * scale, 0.07f * scale, 6, trunkMat, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{x, y0 + 0.8f * scale, z}, Vec3{x, y0 + 2.2f * scale, z},
        0.78f * scale, 0.35f * scale, 8, leafMat, true, true
    );
}

} // namespace

void TowerGen::generateEcoTerraceSpire(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 34.0f, 20.0f, 68.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 26.0f, 16.0f, 52.0f);
    int32_t stories = std::clamp(params_.stories, 5, 16);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.25f, 2.6f, 4.8f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t ecoGlass   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t frameDark  = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t greenTurf  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t waterGlass = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t timberMat  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t fountain   = static_cast<uint32_t>(MaterialSlot::GlassFrosted);

    // 1. Reflective Lagoon Water Basin + Tiered Green Landscaped Island Peninsulas
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 5.0f, -0.25f, -hd - 4.0f},
        Vec3{ hw + 5.0f,  0.02f,  hd + 6.0f},
        waterGlass);
    // Tiered contoured green landscaped island slabs
    for (int t = 0; t < 4; ++t) {
        float shrink = t * 1.4f;
        float ty0 = 0.02f + t * 0.24f;
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, ty0, 0.0f},
            Vec3{0.0f, ty0 + 0.24f, 0.0f},
            std::min(hw, hd) + 2.0f - shrink, std::min(hw, hd) + 1.6f - shrink,
            28, greenTurf, true, true
        );
    }
    // Side fountain grass island & glowing vertical water spray plume
    float fx = -hw * 0.56f;
    float fz = hd * 0.58f;
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{fx, 0.02f, fz}, Vec3{fx, 0.28f, fz},
        2.6f, 2.4f, 16, greenTurf, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{fx, 0.28f, fz}, Vec3{fx, 3.8f, fz},
        0.55f, 0.14f, 12, fountain, true, true
    );

    float groundY = 0.98f;

    // 2. Twin Slender Vertical White Elevator Core Spires (Rising full height)
    float spireH = (stories + 2.5f) * storyH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.2f, groundY, -2.6f},
        Vec3{ 2.6f, groundY + spireH - 1.8f, 0.4f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ 1.2f, groundY, -3.4f},
        Vec3{ 3.6f, groundY + spireH + 1.2f, -0.6f},
        crispWhite);
    // Vertical dark glazed shaft reveals on core towers
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ 0.35f, groundY + 1.0f, 0.36f},
        Vec3{ 1.05f, groundY + spireH - 2.4f, 0.48f},
        frameDark);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ 2.05f, groundY + 1.0f, -0.64f},
        Vec3{ 2.65f, groundY + spireH + 0.6f, -0.52f},
        ecoGlass);

    // 3. Left Stacked Cantilevered White Terrace Rings (5 spiraling cantilevered green terraces)
    int leftTiers = std::max(4, stories - 1);
    float leftR = std::clamp(w * 0.165f, 3.6f, 6.4f);
    for (int i = 0; i < leftTiers; ++i) {
        float y0 = groundY + (i + 1) * storyH * 0.95f;
        float offsetX = -3.4f - 0.35f * std::sin(static_cast<float>(i) * 0.7f);
        float offsetZ = 1.0f + 0.35f * std::cos(static_cast<float>(i) * 0.7f);
        appendGreenTerraceDrum(
            outVerts, outIndices,
            offsetX, offsetZ,
            y0, storyH * 0.92f, leftR - i * 0.15f, 0.48f,
            crispWhite, ecoGlass, greenTurf, frameDark, true
        );
        if (i == leftTiers - 1) {
            appendMiniEcoTree(outVerts, outIndices, offsetX - 1.2f, y0 + storyH * 0.92f + 0.24f, offsetZ, 1.1f, timberMat, greenTurf);
        }
    }

    // 4. Right Stepped Cylindrical Pavilion Tiers + Broad Ground Glass Rotunda
    float rotundaR = std::clamp(w * 0.20f, 4.4f, 7.6f);
    appendGreenTerraceDrum(
        outVerts, outIndices,
        3.2f, 2.6f,
        groundY, storyH * 1.15f, rotundaR, 0.55f,
        crispWhite, ecoGlass, greenTurf, frameDark, true
    );
    // Mid-height right glass drum stack
    int rightUpperTiers = std::max(3, stories - 2);
    for (int i = 0; i < rightUpperTiers; ++i) {
        float y0 = groundY + storyH * 1.15f + i * storyH * 0.92f;
        appendGreenTerraceDrum(
            outVerts, outIndices,
            3.6f, 0.4f,
            y0, storyH * 0.90f, rotundaR * 0.72f - i * 0.18f, 0.44f,
            crispWhite, ecoGlass, greenTurf, frameDark, (i == rightUpperTiers - 1)
        );
    }

    // 5. Left Low-Slung Sloped Glass Conservatory Wing + Sculptural White Ribbon Walkway
    float wingX0 = -hw * 0.68f;
    float wingX1 = -1.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingX0, groundY, -2.4f},
        Vec3{wingX1, groundY + storyH * 0.88f, 2.8f},
        ecoGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingX0 - 0.35f, groundY + storyH * 0.88f, -2.75f},
        Vec3{wingX1 + 0.2f, groundY + storyH * 0.88f + 0.42f, 3.15f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingX0 - 0.1f, groundY + storyH * 0.88f + 0.42f, -2.5f},
        Vec3{wingX1, groundY + storyH * 0.88f + 0.62f, 2.9f},
        greenTurf);

    // Conservatory glass mullions
    for (int m = 0; m < 6; ++m) {
        float mx = wingX0 + 0.4f + m * ((wingX1 - wingX0 - 0.8f) / 5.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.05f, groundY, 2.75f},
            Vec3{mx + 0.05f, groundY + storyH * 0.88f, 2.88f},
            crispWhite);
    }

    // Sculptural White Ribbed Pedestrian Bridge approaching front-right
    for (int b = 0; b < 8; ++b) {
        float t = static_cast<float>(b) / 7.0f;
        float bx = 5.2f + t * 4.5f;
        float bz = 4.6f + t * 4.2f;
        float by = groundY - t * 0.65f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx - 0.9f, by, bz - 0.22f},
            Vec3{bx + 0.9f, by + 0.16f, bz + 0.22f},
            crispWhite);
    }
    appendMiniEcoTree(outVerts, outIndices, wingX0 + 1.5f, groundY + storyH * 0.88f + 0.62f, 0.2f, 1.35f, timberMat, greenTurf);
    appendMiniEcoTree(outVerts, outIndices, 3.2f, groundY + storyH * 1.15f + 0.24f, 4.4f, 1.0f, timberMat, greenTurf);
}

} // namespace procedural::buildings
