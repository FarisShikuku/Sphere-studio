#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendPalmTree(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x, float y0, float z,
    float trunkH,
    uint32_t trunkMat,
    uint32_t leafMat
) {
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{x, y0, z}, Vec3{x + 0.15f, y0 + trunkH, z + 0.10f},
        0.16f, 0.11f, 8, trunkMat, true, true);
    Vec3 crown{x + 0.15f, y0 + trunkH, z + 0.10f};
    for (int i = 0; i < 6; ++i) {
        float ang = static_cast<float>(i) * (6.283185f / 6.0f);
        Vec3 tip{crown.x + std::cos(ang) * 1.45f, crown.y + 0.25f, crown.z + std::sin(ang) * 1.45f};
        Vec3 side{-std::sin(ang), 0.0f, std::cos(ang)};
        BuildingComponents::appendOrientedBox(
            outVerts, outIndices,
            (crown + tip) * 0.5f,
            (tip - crown).normalized(), Vec3{0.0f, 1.0f, 0.0f}, side,
            0.78f, 0.05f, 0.26f, leafMat);
    }
}

void appendParkedSuvAndBike(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float y0, float cz,
    uint32_t whiteCar, uint32_t darkGlass, uint32_t tireMat, uint32_t bikeRed
) {
    // White SUV
    float hw = 0.94f, hl = 2.15f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw, y0 + 0.22f, cz - hl},
        Vec3{cx + hw, y0 + 0.92f, cz + hl}, whiteCar);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.84f, y0 + 0.92f, cz - hl * 0.50f},
        Vec3{cx + hw * 0.84f, y0 + 1.56f, cz + hl * 0.30f}, darkGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.78f, y0 + 1.50f, cz - hl * 0.44f},
        Vec3{cx + hw * 0.78f, y0 + 1.58f, cz + hl * 0.25f}, whiteCar);
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.60f, hl * 0.60f}) {
            float wx = cx + sx * (hw - 0.04f);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.11f, y0 + 0.31f, cz + sz},
                Vec3{wx + 0.11f, y0 + 0.31f, cz + sz},
                0.31f, 0.31f, 10, tireMat, true, true);
        }
    }
    // Red sport motorcycle beside SUV
    float bx = cx - 1.55f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{bx - 0.18f, y0 + 0.42f, cz - 0.72f},
        Vec3{bx + 0.18f, y0 + 0.95f, cz + 0.72f}, bikeRed);
    for (float sz : {-0.58f, 0.58f}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{bx - 0.08f, y0 + 0.28f, cz + sz},
            Vec3{bx + 0.08f, y0 + 0.28f, cz + sz},
            0.28f, 0.28f, 10, tireMat, true, true);
    }
}

} // namespace

void HouseGen::generateStoneAtriumPergolaVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 13.0f, 17.5f);
    float d = res_scale::depth(params_, 10.5f, 13.0f);
    float flH = res_scale::floorHeight(params_, 3.4f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t stoneClad  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t darkStone  = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t timberMat  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t pergolaMat = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t amberGlass = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t blackFrame = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t greenLawn  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t drivePaver = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t tireMat    = static_cast<uint32_t>(MaterialSlot::TireRubber);
    uint32_t whiteCar   = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t redAccent  = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);

    GlassType gt = res_scale::glassType(params_, GlassType::GoldMirroredLuxury);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Yard, Stone Curb & Paved Driveway Apron
    float plinthY = 0.22f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.0f,  0.02f,  hd + 4.8f}, greenLawn);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.8f, 0.02f, hd + 0.1f},
        Vec3{ hw + 1.2f, 0.10f, hd + 3.6f}, drivePaver);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, stoneClad);

    // Entry steps in front of center timber portal
    float atriumX0 = -hw + w * 0.25f;
    float atriumX1 = -hw + w * 0.51f;
    for (int st = 0; st < 3; ++st) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{atriumX0 - 0.2f, 0.08f + st * 0.05f, hd + 0.15f + (2 - st) * 0.28f},
            Vec3{atriumX1 + 0.15f, 0.13f + st * 0.05f, hd + 0.45f + (2 - st) * 0.28f},
            crispWhite);
    }

    // 2. Main Rear Structural Massing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.35f, plinthY, -hd},
        Vec3{ hw - 0.35f, plinthY + totalH, hd - 2.6f}, crispWhite);

    // 3. Left Wing: Stone Pier & Double-Height Amber-Lit Black-Grid Window + Rooftop Deck
    float leftX1 = atriumX0;
    // Leftmost crisp white stepped fin + stacked stone band at top & corner
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, hd - 2.6f},
        Vec3{-hw + 0.52f, plinthY + totalH * 0.86f, hd - 0.1f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.52f, plinthY, hd - 2.6f},
        Vec3{leftX1, plinthY + totalH * 0.86f, hd - 0.55f}, crispWhite);
    // Stone top lintel cladding above left bay window
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.55f, plinthY + flH * 1.48f, hd - 0.58f},
        Vec3{leftX1 - 0.1f, plinthY + totalH * 0.86f, hd - 0.42f}, stoneClad);
    // Double-height glowing warm amber window with dark aluminum grid
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.72f, plinthY + 0.32f, hd - 0.50f},
        Vec3{leftX1 - 0.28f, plinthY + flH * 1.46f, hd - 0.38f}, amberGlass);
    for (int mx = 1; mx <= 2; ++mx) {
        float xPos = -hw + 0.72f + mx * ((leftX1 - 0.28f - (-hw + 0.72f)) / 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xPos - 0.04f, plinthY + 0.32f, hd - 0.38f},
            Vec3{xPos + 0.04f, plinthY + flH * 1.46f, hd - 0.31f}, blackFrame);
    }
    for (int my = 1; my <= 3; ++my) {
        float yPos = plinthY + 0.32f + my * ((flH * 1.46f - 0.32f) / 4.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.72f, yPos - 0.04f, hd - 0.38f},
            Vec3{leftX1 - 0.28f, yPos + 0.04f, hd - 0.31f}, blackFrame);
    }
    // Left roof balcony glass railing + rooftop palm
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.52f, plinthY + totalH * 0.86f, hd - 0.45f},
            Vec3{leftX1, plinthY + totalH * 0.86f + 0.92f, hd - 0.37f}, balcGlass);
    }
    appendPalmTree(outVerts, outIndices, -hw + 1.4f, plinthY + totalH * 0.86f, hd - 1.5f, 2.3f, timberMat, greenLawn);

    // 4. Center Monumental Double-Height White Portal, Timber Entry & Stone Glass Tower
    float towerTopY = plinthY + totalH + 1.15f;
    // Outer sweeping white inverted-L portal frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{atriumX0, plinthY, hd - 2.6f},
        Vec3{atriumX0 + 0.48f, towerTopY, hd + 0.35f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{atriumX0, towerTopY - 0.45f, hd - 2.6f},
        Vec3{atriumX1 + 0.85f, towerTopY, hd + 0.35f}, crispWhite);

    // Tall timber vertical strip & wooden front pivot door inside portal left
    float doorBayX0 = atriumX0 + 0.56f;
    float doorBayX1 = doorBayX0 + 1.38f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorBayX0, plinthY, hd - 0.75f},
        Vec3{doorBayX1, towerTopY - 0.45f, hd - 0.62f}, timberMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorBayX0 + 0.14f, plinthY, hd - 0.62f},
        Vec3{doorBayX1 - 0.14f, plinthY + 2.55f, hd - 0.52f}, darkStone);
    // Thin vertical amber sidelight strips alongside timber panel
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorBayX0 + 0.04f, plinthY + 2.75f, hd - 0.61f},
        Vec3{doorBayX0 + 0.34f, towerTopY - 0.52f, hd - 0.54f}, amberGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorBayX1 - 0.34f, plinthY + 2.75f, hd - 0.61f},
        Vec3{doorBayX1 - 0.04f, towerTopY - 0.52f, hd - 0.54f}, amberGlass);

    // Stacked-stone center stair shaft housing monumental gridded window
    float stoneShaftX0 = doorBayX1 + 0.05f;
    float stoneShaftX1 = atriumX1;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stoneShaftX0, plinthY, hd - 2.6f},
        Vec3{stoneShaftX1, towerTopY - 0.45f, hd - 0.35f}, stoneClad);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stoneShaftX0 + 0.35f, plinthY + 1.15f, hd - 0.36f},
        Vec3{stoneShaftX1 - 0.35f, towerTopY - 0.78f, hd - 0.25f}, darkGlass);
    for (int gy = 1; gy <= 4; ++gy) {
        float yL = plinthY + 1.15f + gy * ((towerTopY - 1.93f) / 5.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{stoneShaftX0 + 0.35f, yL - 0.03f, hd - 0.26f},
            Vec3{stoneShaftX1 - 0.35f, yL + 0.03f, hd - 0.20f}, blackFrame);
    }
    for (int gx = 1; gx <= 2; ++gx) {
        float xL = stoneShaftX0 + 0.35f + gx * ((stoneShaftX1 - stoneShaftX0 - 0.70f) / 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xL - 0.03f, plinthY + 1.15f, hd - 0.26f},
            Vec3{xL + 0.03f, towerTopY - 0.78f, hd - 0.20f}, blackFrame);
    }

    // 5. Right Wing: Sweeping White C-Frame Upper Balcony Over Open Carport
    float rightX0 = atriumX1;
    float rightX1 = hw;
    float uY0 = plinthY + flH;
    float uY1 = plinthY + totalH;
    // Ground floor recessed sitting bay & 2-car open carport on far right
    float livingRightX = rightX0 + (rightX1 - rightX0) * 0.48f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX0, plinthY, hd - 2.6f},
        Vec3{livingRightX, uY0, hd - 0.65f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX0 + 0.9f, plinthY + 0.35f, hd - 0.66f},
        Vec3{livingRightX - 0.4f, uY0 - 0.35f, hd - 0.56f}, darkGlass);

    // Right outer structural pier supporting the sweeping C-frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX1 - 0.42f, plinthY, hd - 2.6f},
        Vec3{rightX1, uY1, hd + 0.15f}, crispWhite);

    if (params_.hasGarage) {
        appendParkedSuvAndBike(outVerts, outIndices,
            (livingRightX + rightX1 - 0.42f) * 0.5f + 0.35f, plinthY, hd - 0.9f,
            whiteCar, darkGlass, tireMat, redAccent);
    }

    // Sweeping white C-frame (left pier, floor slab, upper overhang roof beam)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX0, uY0 - 0.12f, hd - 2.6f},
        Vec3{rightX1 + 0.22f, uY0 + 0.30f, hd + 0.38f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX0, uY1 - 0.36f, hd - 2.6f},
        Vec3{rightX1 + 0.22f, uY1 + 0.08f, hd + 0.38f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX0, uY0 + 0.30f, hd - 2.6f},
        Vec3{rightX0 + 0.38f, uY1 - 0.36f, hd + 0.38f}, crispWhite);

    // Signature 3 Vertical White Accent Mullions on Upper Right Balcony
    float trioX = livingRightX + 0.45f;
    for (int m = 0; m < 3; ++m) {
        float mx = trioX + m * 0.34f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx, uY0 + 0.28f, hd + 0.12f},
            Vec3{mx + 0.14f, uY0 + 1.48f, hd + 0.36f}, crispWhite);
    }
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rightX0 + 0.38f, uY0 + 0.30f, hd + 0.22f},
            Vec3{rightX1 - 0.42f, uY0 + 1.24f, hd + 0.30f}, balcGlass);
    }
    // Upper balcony panoramic sliding windows & dark soffit louvers
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightX0 + 0.85f, uY0 + 0.30f, hd - 1.35f},
        Vec3{livingRightX + 0.35f, uY1 - 0.40f, hd - 1.22f}, darkGlass);
    for (int p = 0; p < 7; ++p) {
        float px = livingRightX + 0.35f + p * ((rightX1 - 0.5f - (livingRightX + 0.35f)) / 7.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px, uY1 - 0.45f, hd - 1.4f},
            Vec3{px + 0.08f, uY1 - 0.36f, hd + 0.20f}, pergolaMat);
    }

    // Upper-Right Rooftop Pergola Terrace & Glass Balustrade
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{atriumX1, uY1 + 0.08f, hd - 0.45f},
            Vec3{livingRightX + 1.1f, uY1 + 1.02f, hd - 0.37f}, balcGlass);
    }
    // Left-center cantilevered pergola beams protruding from stair tower
    for (int r = 0; r < 6; ++r) {
        float rz = hd - 2.3f + r * 0.42f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{atriumX0 - 1.35f, towerTopY - 0.42f, rz},
            Vec3{atriumX0, towerTopY - 0.24f, rz + 0.14f}, darkStone);
    }

    // Front landscaping hedge shrubs
    for (float sx : {-hw + 1.1f, -hw + 2.2f, -hw + 3.3f, rightX0 + 1.2f, rightX0 + 2.3f}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{sx, 0.05f, hd + 0.35f}, Vec3{sx, 0.72f, hd + 0.35f},
            0.36f, 0.22f, 8, greenLawn, true, true);
    }

    // 6. Rear & Side Facade Windows + Roof Cap
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.2f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 4, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{-hw + 0.35f, 0.0f, -1.2f}, d * 0.55f, fy, flH,
            Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 2, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{hw - 0.35f, 0.0f, -1.2f}, d * 0.55f, fy, flH,
            Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f}, 2, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.7f, d - 2.6f, plinthY + totalH, RoofType::FlatParapet, 0.5f, 0.0f);
}

} // namespace procedural::buildings
