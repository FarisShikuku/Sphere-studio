#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendSportsCar(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float y0, float cz,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat
) {
    float hw = 0.88f, hl = 1.98f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw, y0 + 0.18f, cz - hl},
        Vec3{cx + hw, y0 + 0.74f, cz + hl}, bodyMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.82f, y0 + 0.74f, cz - hl * 0.44f},
        Vec3{cx + hw * 0.82f, y0 + 1.24f, cz + hl * 0.28f}, glassMat);
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.56f, hl * 0.56f}) {
            float wx = cx + sx * (hw - 0.04f);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.10f, y0 + 0.27f, cz + sz},
                Vec3{wx + 0.10f, y0 + 0.27f, cz + sz},
                0.27f, 0.27f, 8, tireMat, true, true);
        }
    }
}

} // namespace

void HouseGen::generateBrickPergolaPenthouseVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 14.0f, 18.0f);
    float d = res_scale::depth(params_, 11.5f, 14.5f);
    float flH = res_scale::floorHeight(params_, 3.35f);
    int stories = res_scale::stories(params_, 3, 4);
    float baseTwoH = 2.0f * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t brickWall  = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t darkGrey   = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t blackFrame = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t pentConcr  = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t amberGlass = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t greenTurf  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t timberMat  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t paverBrick = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t whiteCar   = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t yellowCar  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t tireMat    = static_cast<uint32_t>(MaterialSlot::TireRubber);

    GlassType gt = res_scale::glassType(params_, GlassType::GoldMirroredLuxury);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Compound Yard, White Fence Wall & Brick-Pattern Driveway
    float plinthY = 0.22f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.4f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.4f,  0.02f,  hd + 4.8f}, greenTurf);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.5f, 0.02f, -hd * 0.2f},
        Vec3{ hw - 0.5f, 0.11f, hd + 4.4f}, paverBrick);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, pentConcr);

    // Right perimeter slotted white garden fence
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 1.45f, 0.02f, -hd},
        Vec3{hw + 1.75f, 1.85f, hd + 4.2f}, crispWhite);

    // 2. Main Rear Structural Volume (Levels 1-2)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.35f, plinthY, -hd},
        Vec3{ hw - 0.35f, plinthY + baseTwoH, hd - 2.6f}, crispWhite);

    // 3. Left Brick Volume with Double-Height Dark Grey Framed Amber Curtain Bay
    float leftX0 = -hw;
    float leftX1 = -hw + w * 0.31f;
    float centerX1 = -hw + w * 0.53f;
    float rightX1 = hw;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX0, plinthY, hd - 2.6f},
        Vec3{leftX1, plinthY + baseTwoH + 0.25f, hd + 0.35f}, brickWall);
    // Bold overhanging dark grey box window collar + warm amber double-height grid window
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX0 + 0.45f, plinthY + 0.35f, hd + 0.15f},
        Vec3{leftX1 - 0.45f, plinthY + baseTwoH - 0.45f, hd + 0.65f}, darkGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX0 + 0.68f, plinthY + 0.58f, hd + 0.48f},
        Vec3{leftX1 - 0.68f, plinthY + baseTwoH - 0.68f, hd + 0.68f}, amberGlass);
    for (int gx = 1; gx <= 2; ++gx) {
        float xL = leftX0 + 0.68f + gx * ((leftX1 - leftX0 - 1.36f) / 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xL - 0.04f, plinthY + 0.58f, hd + 0.64f},
            Vec3{xL + 0.04f, plinthY + baseTwoH - 0.68f, hd + 0.72f}, blackFrame);
    }
    for (int gy = 1; gy <= 3; ++gy) {
        float yL = plinthY + 0.58f + gy * ((baseTwoH - 1.26f) / 4.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{leftX0 + 0.68f, yL - 0.04f, hd + 0.64f},
            Vec3{leftX1 - 0.68f, yL + 0.04f, hd + 0.72f}, blackFrame);
    }
    // Flanking warm carriage lights on brick pier
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX0 + 0.22f, plinthY + 2.45f, hd + 0.35f},
        Vec3{leftX0 + 0.36f, plinthY + 2.75f, hd + 0.45f}, amberGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX1 - 0.36f, plinthY + 2.45f, hd + 0.35f},
        Vec3{leftX1 - 0.22f, plinthY + 2.75f, hd + 0.45f}, amberGlass);

    // 4. Center Recessed Atrium: Vertical Timber/Brick Trim + Tall Amber Stair Window + Thick Dark Canopy
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX1, plinthY, hd - 2.6f},
        Vec3{centerX1, plinthY + baseTwoH, hd - 0.85f}, crispWhite);
    // Double-height glowing amber stair glass & side door
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX1 + 0.85f, plinthY + 0.12f, hd - 0.88f},
        Vec3{centerX1 - 0.45f, plinthY + baseTwoH - 0.35f, hd - 0.74f}, amberGlass);
    for (int gy = 1; gy <= 5; ++gy) {
        float yL = plinthY + 0.12f + gy * ((baseTwoH - 0.47f) / 6.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{leftX1 + 0.85f, yL - 0.03f, hd - 0.76f},
            Vec3{centerX1 - 0.45f, yL + 0.03f, hd - 0.68f}, blackFrame);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX1 + 0.18f, plinthY, hd - 0.88f},
        Vec3{leftX1 + 0.78f, plinthY + 2.42f, hd - 0.72f}, blackFrame);

    // Cantilevered Horizontal Dark Grey Floating Canopy Box over Center/Right Porch
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{leftX1 - 0.2f, plinthY + baseTwoH - 0.55f, hd - 1.8f},
        Vec3{centerX1 + 1.2f, plinthY + baseTwoH + 0.18f, hd + 1.15f}, darkGrey);

    // 5. Right Wing: Warm Brick Twin Carport Piers + Cantilevered White Upper Balcony + Sports Cars
    float uY0 = plinthY + flH;
    float uY1 = plinthY + baseTwoH;
    // Left and right terracotta brick columns supporting carport
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerX1, plinthY, hd - 0.8f},
        Vec3{centerX1 + 0.55f, uY0, hd + 0.45f}, brickWall);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX1 - 0.58f, plinthY, hd - 0.8f},
        Vec3{rightX1, uY0, hd + 0.45f}, brickWall);
    // Glazed lounge wall inside back of carport
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerX1 + 0.85f, plinthY + 0.15f, hd - 2.64f},
        Vec3{rightX1 - 0.85f, uY0 - 0.35f, hd - 2.52f}, darkGlass);

    if (params_.hasGarage) {
        float midCarX = (centerX1 + rightX1) * 0.5f;
        appendSportsCar(outVerts, outIndices, midCarX - 1.15f, plinthY, hd - 0.65f,
            whiteCar, darkGlass, tireMat);
        appendSportsCar(outVerts, outIndices, midCarX + 1.15f, plinthY, hd - 0.65f,
            yellowCar, darkGlass, tireMat);
    }

    // Cantilevered crisp white upper balcony over carport with dark black metal rail
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerX1 - 0.15f, uY0 - 0.10f, hd - 2.6f},
        Vec3{rightX1 + 0.15f, uY0 + 0.30f, hd + 0.85f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerX1 - 0.15f, uY1 - 0.25f, hd - 2.6f},
        Vec3{rightX1 + 0.15f, uY1 + 0.10f, hd + 0.85f}, crispWhite);
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{centerX1, uY0 + 1.15f, hd + 0.72f},
            Vec3{rightX1, uY0 + 1.24f, hd + 0.82f}, blackFrame);
        for (int b = 0; b < 4; ++b) {
            float ry = uY0 + 0.45f + b * 0.20f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{centerX1, ry, hd + 0.74f},
                Vec3{rightX1, ry + 0.04f, hd + 0.80f}, blackFrame);
        }
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerX1 + 0.45f, uY0 + 0.30f, hd - 2.64f},
        Vec3{rightX1 - 0.65f, uY1 - 0.28f, hd - 2.50f}, darkGlass);

    // 6. Level 3 Rooftop Green Terrace Garden, Glass Balustrade, Charcoal Penthouse Loft & 5 Pergola Rafters
    float roofDeckY = plinthY + baseTwoH + 0.10f;
    // Green turf & glass balustrade along right terrace
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{centerX1 - 0.2f, roofDeckY - 0.02f, hd - 2.5f},
        Vec3{rightX1 + 0.10f, roofDeckY + 0.10f, hd + 0.80f}, greenTurf);
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{centerX1 - 0.2f, roofDeckY + 0.10f, hd + 0.68f},
            Vec3{rightX1 + 0.08f, roofDeckY + 1.05f, hd + 0.76f}, balcGlass);
    }
    // Level 3 Charcoal/Grey Concrete Penthouse Volume
    if (stories >= 3) {
        float pentH = flH * 0.92f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.35f, roofDeckY, -hd + 0.6f},
            Vec3{rightX1 - 0.35f, roofDeckY + pentH, hd - 2.4f}, pentConcr);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{leftX1 + 1.2f, roofDeckY + 0.35f, hd - 2.42f},
            Vec3{centerX1 + 0.6f, roofDeckY + pentH - 0.42f, hd - 2.30f}, darkGlass);
        // Signature 5 Bold Projecting Dark Grey Cantilevered Pergola Rafters Over Right Roof Terrace
        for (int p = 0; p < 5; ++p) {
            float px = centerX1 + 0.65f + p * ((rightX1 - centerX1 - 1.25f) / 4.0f);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.16f, roofDeckY + pentH - 0.38f, hd - 2.4f},
                Vec3{px + 0.16f, roofDeckY + pentH - 0.04f, hd + 0.55f}, darkGrey);
        }
        // Left stepped parapet frame ring
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.6f, roofDeckY, -hd + 1.2f},
            Vec3{-hw * 0.35f, roofDeckY + pentH * 0.72f, hd - 2.4f}, darkGrey);
    }

    // 7. Rear & Side Windows + Penthouse Roof Cap
    for (int s = 0; s < 2; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.4f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 4, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{-hw + 0.35f, 0.0f, -1.0f}, d * 0.52f, fy, flH,
            Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 2, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{hw - 0.35f, 0.0f, -1.0f}, d * 0.52f, fy, flH,
            Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f}, 2, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.8f, d - 2.6f, plinthY + baseTwoH, RoofType::FlatParapet, 0.45f, 0.0f);
}

} // namespace procedural::buildings
