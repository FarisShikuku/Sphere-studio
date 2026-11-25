#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendAngledBeveledStrut(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& p0,
    const Vec3& p1,
    float radius,
    uint32_t mat
) {
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        p0, p1, radius, radius, 6, mat, true, true
    );
}

} // namespace

void TowerGen::generateDiagonalStrutPavilion(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 38.0f, 20.0f, 68.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 18.0f, 12.0f, 36.0f);
    int32_t stories = std::clamp(params_.stories, 2, 6);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.6f, 2.8f, 5.2f);
    float totalH = stories * storyH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite   = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t limestone    = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t emeraldGlass = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t frameDark    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat    = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t greenPlant   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t timberMat    = static_cast<uint32_t>(MaterialSlot::WallTimber);

    float plinthY = 0.35f;

    // 1. Central Paved Forecourt Promenade & Symmetrical Landscaped Hedges
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 5.0f, -0.18f, -hd - 3.0f},
        Vec3{ hw + 5.0f,  0.02f,  hd + 9.0f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-4.8f, 0.02f, hd - 0.5f},
        Vec3{ 4.8f, 0.20f, hd + 8.5f},
        limestone);
    // Broad building granite step plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.0f, 0.02f, -hd - 0.6f},
        Vec3{ hw + 1.0f, plinthY, hd + 0.9f},
        frameDark);

    // Symmetrical Foreground Manicured Hedges & Conical Cypress Trees
    for (float side : {-1.0f, 1.0f}) {
        float hx0 = (side < 0) ? (-hw - 2.0f) : 5.2f;
        float hx1 = (side < 0) ? -5.2f : (hw + 2.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hx0, 0.02f, hd + 2.6f},
            Vec3{hx1, 1.15f, hd + 7.2f},
            greenPlant);
        float tx = side * hw * 0.62f;
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{tx, 1.15f, hd + 4.4f}, Vec3{tx, 5.2f, hd + 4.4f},
            1.45f, 0.25f, 10, greenPlant, true, true
        );
    }

    // 2. Central Monumental Beige Limestone Spine Tower & Dark Recessed Atrium Wings
    float spineHalfW = std::clamp(w * 0.068f, 1.8f, 3.2f);
    float atriumHalfW = spineHalfW + 1.95f;
    float spineTopY = plinthY + totalH + 1.15f;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-spineHalfW, plinthY, -hd * 0.4f},
        Vec3{ spineHalfW, spineTopY, hd + 0.55f},
        limestone);

    // Signature Architectural V-Crest Emblem High on Central Stone Spine
    float vY0 = spineTopY - 3.0f;
    float vY1 = spineTopY - 1.1f;
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{0.0f, vY0, hd + 0.58f},
        Vec3{-0.75f, vY1, hd + 0.58f},
        0.13f, 0.15f, 8, frameDark, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{0.0f, vY0, hd + 0.58f},
        Vec3{ 0.75f, vY1, hd + 0.58f},
        0.13f, 0.15f, 8, frameDark, true, true
    );

    // Flanking Dark Glazed Core Entrance Atrium Slots & Dual Canopied Doors
    for (float side : {-1.0f, 1.0f}) {
        float ax0 = (side < 0) ? -atriumHalfW : spineHalfW;
        float ax1 = (side < 0) ? -spineHalfW  : atriumHalfW;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{ax0, plinthY, hd - 0.15f},
            Vec3{ax1, spineTopY - 0.15f, hd + 0.35f},
            frameDark);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{ax0 + 0.18f, plinthY + 2.8f, hd + 0.26f},
            Vec3{ax1 - 0.18f, spineTopY - 0.35f, hd + 0.40f},
            emeraldGlass);
        BuildingComponents::generateDoor(
            outVerts, outIndices,
            Vec3{0.5f * (ax0 + ax1), plinthY, hd + 0.38f},
            1.65f, 2.55f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::DoubleGlassCommercial, true
        );
    }

    // 3. Main Symmetrical Curtain-Wall Facade Mass Behind Angled Exoskeleton
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{ hw, plinthY + totalH, hd - 0.35f},
        emeraldGlass);
    // Dark structural aluminum mullions across full width & height
    int cols = std::clamp(static_cast<int>(w / 2.2f), 10, 22);
    for (int c = 0; c <= cols; ++c) {
        float mx = -hw + c * (w / static_cast<float>(cols));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.05f, plinthY, hd - 0.38f},
            Vec3{mx + 0.05f, plinthY + totalH, hd - 0.25f},
            frameDark);
    }
    for (int f = 0; f <= stories; ++f) {
        float fy = plinthY + f * storyH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.05f, fy - 0.14f, -hd - 0.05f},
            Vec3{ hw + 0.05f, fy + 0.14f,  hd - 0.22f},
            frameDark);
    }

    // 4. SIGNATURE SYMMETRICAL WHITE DIAGONAL A-STRUT WING FRAMES & CANTILEVER OVERHANGS
    float topFasciaY0 = plinthY + totalH - 0.35f;
    float topFasciaY1 = plinthY + totalH + 0.85f;
    float midDeckY    = plinthY + storyH;

    for (float side : {-1.0f, 1.0f}) {
        float wingOuterX = side * (hw + 0.85f);
        float wingTopInnerX = side * (hw * 0.56f);
        float wingFootX = side * (atriumHalfW + 1.35f);

        // Top overhanging white fascia beam on outer wing
        float tx0 = std::min(wingOuterX, wingTopInnerX);
        float tx1 = std::max(wingOuterX, wingTopInnerX);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx0, topFasciaY0, hd - 1.2f},
            Vec3{tx1, topFasciaY1, hd + 0.85f},
            crispWhite);

        // Ground-floor wide white horizontal terrace overhang slab above ground podium
        float gx0 = std::min(wingOuterX + side * 0.55f, wingFootX);
        float gx1 = std::max(wingOuterX + side * 0.55f, wingFootX);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{gx0, midDeckY - 0.35f, hd - 0.6f},
            Vec3{gx1, midDeckY + 0.28f, hd + 1.10f},
            crispWhite);

        // Signature Diagonal Sweeping White Exoskeleton Strut from Top Corner down to Plinth Foot
        appendAngledBeveledStrut(
            outVerts, outIndices,
            Vec3{wingFootX, plinthY, hd + 0.72f},
            Vec3{wingTopInnerX, topFasciaY0 + 0.4f, hd + 0.68f},
            0.38f, crispWhite
        );

        // 3 Stepped Horizontal White Brise-Soleil Cantilever Fins across Outer Upper Wing
        for (int fin = 0; fin < 3; ++fin) {
            float fy = midDeckY + 1.25f + fin * 0.72f;
            float innerSpanX = side * (hw * (0.48f + fin * 0.12f));
            float fx0 = std::min(wingOuterX, innerSpanX);
            float fx1 = std::max(wingOuterX, innerSpanX);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx0, fy, hd - 0.2f},
                Vec3{fx1, fy + 0.18f, hd + 0.58f},
                crispWhite);
        }
    }

    // 5. Rear Facade Pilots & Flat Roof Slab
    for (int f = 1; f < stories; ++f) {
        float fy = plinthY + f * storyH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.12f, fy - 0.10f, -hd - 0.25f},
            Vec3{ hw + 0.12f, fy + 0.10f, -hd + 0.05f},
            crispWhite);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.1f, plinthY + totalH, -hd - 0.1f},
        Vec3{ hw + 0.1f, plinthY + totalH + 0.45f, hd - 0.35f},
        static_cast<uint32_t>(MaterialSlot::RoofSurface));
}

} // namespace procedural::buildings
