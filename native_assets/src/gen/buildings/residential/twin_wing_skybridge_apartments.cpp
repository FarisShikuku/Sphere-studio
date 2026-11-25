#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendPalmAccentTree(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x, float y0, float z,
    float hgt, uint32_t trunkMat, uint32_t leafMat
) {
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{x, y0, z}, Vec3{x + 0.06f, y0 + hgt, z + 0.04f},
        0.14f, 0.09f, 8, trunkMat, true, true);
    const float twoPi = 6.2831853f;
    for (int f = 0; f < 6; ++f) {
        float ang = twoPi * static_cast<float>(f) / 6.0f;
        Vec3 tip{x + 0.06f + std::cos(ang) * 1.45f, y0 + hgt - 0.15f, z + 0.04f + std::sin(ang) * 1.45f};
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{x + 0.06f, y0 + hgt + 0.12f, z + 0.04f}, tip,
            0.07f, 0.015f, 6, leafMat, true, true);
    }
}

void appendDiagonalBalconyWing(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x0, float x1, float fy, float zBack, float zFront,
    float skewSign,
    uint32_t slabMat, uint32_t glassMat, uint32_t glowMat
) {
    float skew = skewSign * 0.85f;
    Vec3 p0{x0, fy, zBack};
    Vec3 p1{x1, fy, zBack};
    Vec3 p2{x1 + skew * 0.4f, fy, zFront + std::abs(skew) * 0.35f};
    Vec3 p3{x0 - skew * 0.2f, fy, zFront};
    float thick = 0.24f;
    // Top, bottom, and front slab band
    BuildingComponents::appendQuad(outVerts, outIndices,
        p0 + Vec3{0, thick, 0}, p1 + Vec3{0, thick, 0},
        p2 + Vec3{0, thick, 0}, p3 + Vec3{0, thick, 0}, {0, 1, 0}, slabMat);
    BuildingComponents::appendQuad(outVerts, outIndices,
        p3, p2, p1, p0, {0, -1, 0}, slabMat);
    Vec3 fTan = (p2 - p3).normalized();
    Vec3 fNorm = fTan.cross(Vec3{0, 1, 0}).normalized();
    BuildingComponents::appendQuad(outVerts, outIndices,
        p3, p2, p2 + Vec3{0, thick, 0}, p3 + Vec3{0, thick, 0}, fNorm, slabMat);
    // Warm cove soffit glow bar & glass rail
    BuildingComponents::appendOrientedBox(outVerts, outIndices,
        (p3 + p2) * 0.5f + Vec3{0, thick * 0.5f, 0} + fNorm * 0.04f,
        fTan, {0, 1, 0}, fNorm,
        (p2 - p3).length() * 0.42f, 0.04f, 0.025f, glowMat);
    BuildingComponents::appendOrientedBox(outVerts, outIndices,
        (p3 + p2) * 0.5f + Vec3{0, thick + 0.48f, 0},
        fTan, {0, 1, 0}, fNorm,
        (p2 - p3).length() * 0.48f, 0.46f, 0.03f, glassMat);
}

} // namespace

void HouseGen::generateTwinWingSkybridgeApartments(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 22.0f, 30.0f);
    float d = res_scale::depth(params_, 12.0f, 16.0f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 4, 8);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t darkSpine  = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t warmAmber  = static_cast<uint32_t>(MaterialSlot::WallCorten);
    uint32_t greenPlant = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t glowCove   = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t tarmacMat  = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t timberMat  = static_cast<uint32_t>(MaterialSlot::WallTimber);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::PictureGlass);

    float plinthY = 0.22f;
    // 1. Forecourt Plaza & Road
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.4f, -0.18f, -hd - 1.5f},
        Vec3{ hw + 2.4f,  0.02f,  hd + 5.2f}, tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, 0.0f, -hd - 0.3f},
        Vec3{ hw + 0.5f, plinthY, hd + 0.4f}, darkSpine);

    // 2. Central Monumental Dark Elevator/Feature Spine + Warm Recessed Reveals + Rounded Top Cap
    float spineHW = w * 0.11f;
    float spineTopY = plinthY + totalH + 1.6f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-spineHW, plinthY, -hd * 0.72f},
        Vec3{ spineHW, spineTopY, hd - 1.8f}, darkSpine);
    // Warm amber vertical framing flanking center spine
    for (float sx : {-1.0f, 1.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx * (spineHW + 0.02f), plinthY, -hd * 0.65f},
            Vec3{sx * (spineHW + 0.95f), spineTopY - 0.8f, hd - 2.4f}, warmAmber);
    }
    // Rounded cantilevered white hood capping central spine
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-spineHW - 1.15f, spineTopY, -hd * 0.75f},
        Vec3{ spineHW + 1.15f, spineTopY + 0.55f, hd - 1.15f}, crispWhite);

    // 3. Central 2-Storey Grand Portico Skybridge Linking Left & Right Apartment Towers
    float bridgeY = plinthY + flH * 1.45f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-spineHW - 2.8f, bridgeY, hd - 3.6f},
        Vec3{ spineHW + 2.8f, bridgeY + 0.48f, hd + 1.45f}, crispWhite);
    // Double-height white columns & glazed lobby entrance beneath skybridge
    for (float cx : {-spineHW - 1.6f, -spineHW + 0.2f, spineHW - 0.2f, spineHW + 1.6f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.22f, plinthY, hd + 0.65f},
            Vec3{cx + 0.22f, bridgeY, hd + 1.05f}, crispWhite);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-spineHW + 0.4f, plinthY, hd - 1.85f},
        Vec3{ spineHW - 0.4f, bridgeY - 0.25f, hd - 1.70f}, darkGlass);
    // Planter garden atop skybridge roof
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-spineHW - 2.6f, bridgeY + 0.48f, hd - 2.6f},
        Vec3{ spineHW + 2.6f, bridgeY + 1.05f, hd + 1.25f}, greenPlant);

    // 4. Symmetrical Left & Right Tiered Wings (Inner Stepped Tower + Outer Wide Wing)
    for (float side : {-1.0f, 1.0f}) {
        float innerX0 = side * (spineHW + 1.4f);
        float innerX1 = side * (spineHW + w * 0.24f);
        float iMinX = std::min(innerX0, innerX1);
        float iMaxX = std::max(innerX0, innerX1);

        float outerX0 = side * (spineHW + w * 0.255f);
        float outerX1 = side * hw;
        float oMinX = std::min(outerX0, outerX1);
        float oMaxX = std::max(outerX0, outerX1);

        // Main inner wing & recessed outer wing volumes
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{iMinX, plinthY, -hd}, Vec3{iMaxX, plinthY + totalH, hd - 1.4f}, crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{oMinX, plinthY, -hd}, Vec3{oMaxX, plinthY + totalH, hd - 2.4f}, crispWhite);

        // Lower Levels 1-2: Wraparound White Planter Bands filled with lush green hedges
        for (int lv = 1; lv <= 2 && lv < stories; ++lv) {
            float ly = plinthY + lv * flH;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{iMinX - 0.45f, ly, hd - 2.2f},
                Vec3{oMaxX + 0.65f, ly + 0.42f, hd + 1.25f}, crispWhite);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{iMinX - 0.35f, ly + 0.42f, hd - 0.6f},
                Vec3{oMaxX + 0.55f, ly + 0.98f, hd + 1.15f}, greenPlant);
        }

        // Upper Levels 3..stories-1: Angled Signature Diagonal Balcony Cantilevers
        if (res_scale::showBalconies(params_)) {
            for (int lv = 3; lv < stories; ++lv) {
                float ly = plinthY + lv * flH;
                appendDiagonalBalconyWing(outVerts, outIndices,
                    iMinX + 0.2f, iMaxX - 0.2f, ly, hd - 1.6f, hd + 0.55f,
                    -side, crispWhite, balcGlass, glowCove);
                appendDiagonalBalconyWing(outVerts, outIndices,
                    oMinX + 0.2f, oMaxX - 0.2f, ly, hd - 2.4f, hd - 0.35f,
                    side, crispWhite, balcGlass, glowCove);
            }
        }

        // Glazed apartment windows along wings (front & rear)
        for (int s = 0; s < stories; ++s) {
            float fy = plinthY + s * flH;
            res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
                Vec3{(iMinX + iMaxX) * 0.5f, 0.0f, hd - 1.4f}, (iMaxX - iMinX) * 0.82f, fy, flH,
                Vec3{0.0f, 0.0f, 1.0f}, Vec3{1.0f, 0.0f, 0.0f}, 2, wt, gt);
            res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
                Vec3{(iMinX + iMaxX) * 0.5f, 0.0f, -hd}, (iMaxX - iMinX) * 0.85f, fy, flH,
                Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 2, wt, gt);
            res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
                Vec3{(oMinX + oMaxX) * 0.5f, 0.0f, -hd}, (oMaxX - oMinX) * 0.85f, fy, flH,
                Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 2, wt, gt);
        }

        // Palm accents flanking courtyard boulevard
        appendPalmAccentTree(outVerts, outIndices, side * (hw * 0.72f), 0.02f, hd + 2.6f, 4.6f, timberMat, greenPlant);
    }

    // 5. Rooftop Mechanical Penthouses & Flat Roofs
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w * 0.92f, d - 2.4f, plinthY + totalH, RoofType::FlatParapet, 0.55f, 0.0f);
}

} // namespace procedural::buildings
