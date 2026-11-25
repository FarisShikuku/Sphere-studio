#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendParkedSedan(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float y0, float cz,
    float len, float wid, float hgt,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat
) {
    float hw = wid * 0.5f;
    float hl = len * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw, y0 + 0.18f, cz - hl},
        Vec3{cx + hw, y0 + hgt * 0.52f, cz + hl}, bodyMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.84f, y0 + hgt * 0.52f, cz - hl * 0.45f},
        Vec3{cx + hw * 0.84f, y0 + hgt, cz + hl * 0.25f}, glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.78f, y0 + hgt - 0.04f, cz - hl * 0.40f},
        Vec3{cx + hw * 0.78f, y0 + hgt + 0.02f, cz + hl * 0.20f}, bodyMat);
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.60f, hl * 0.60f}) {
            float wx = cx + sx * (hw - 0.04f);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{wx - 0.10f, y0 + 0.29f, cz + sz},
                Vec3{wx + 0.10f, y0 + 0.29f, cz + sz},
                0.29f, 0.29f, 12, tireMat, true, true);
        }
    }
}

} // namespace

void HouseGen::generateTaupeInterlockingFrameVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 12.0f, 16.0f);
    float d = res_scale::depth(params_, 10.0f, 13.5f);
    float flH = res_scale::floorHeight(params_, 3.35f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t warmTaupe  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t darkCharc  = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t fluteTaupe = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
    uint32_t greenLawn  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t paverStone = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t amberGlass = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t whiteCar   = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t tireMat    = static_cast<uint32_t>(MaterialSlot::TireRubber);

    GlassType gt = res_scale::glassType(params_, GlassType::GoldMirroredLuxury);
    WindowType wt = res_scale::windowType(params_, WindowType::PictureGlass);

    float plinthY = 0.24f;
    // 1. Foreground Courtyard Lawn & Segmented Concrete Driveway Slabs
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.8f, -0.18f, -hd - 1.2f},
        Vec3{ hw + 1.8f,  0.02f,  hd + 4.8f}, greenLawn);
    for (int track = 0; track < 2; ++track) {
        float tx0 = -0.35f + track * ((hw + 0.5f) * 0.52f);
        float tx1 = tx0 + ((hw + 0.5f) * 0.46f);
        for (int seg = 0; seg < 3; ++seg) {
            float sz0 = hd + 0.2f + seg * 1.45f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{tx0, 0.02f, sz0}, Vec3{tx1, 0.12f, sz0 + 1.32f}, paverStone);
        }
    }
    // 3 Wide Floating Entry Porch Steps on Left
    for (int st = 0; st < 3; ++st) {
        float yTop = 0.08f + st * 0.08f;
        float zFront = hd + 2.4f - st * 0.58f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.55f, yTop - 0.08f, hd - 0.8f},
            Vec3{-2.4f, yTop, zFront}, paverStone);
    }

    // Foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, warmTaupe);

    // 2. Main Rear Living Envelope
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.45f, plinthY, -hd},
        Vec3{ hw - 0.45f, plinthY + totalH, hd - 2.6f}, crispWhite);

    // 3. Outer Signature Tall Warm-Taupe L-Frame (Left Pillar + Crown Overhang Beam)
    float outerThick = 0.62f;
    float outerTopY = plinthY + totalH + 0.55f;
    float outerBeamEndX = 2.15f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, hd - 2.6f},
        Vec3{-hw + outerThick, outerTopY, hd + 0.35f}, warmTaupe);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, outerTopY - 0.55f, hd - 2.6f},
        Vec3{outerBeamEndX, outerTopY, hd + 0.35f}, warmTaupe);
    // White fascia liner tucked beneath outer L-frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + outerThick, outerTopY - 0.78f, hd - 2.5f},
        Vec3{outerBeamEndX - 0.3f, outerTopY - 0.55f, hd + 0.05f}, crispWhite);

    // Recessed Entry Alcove (-hw+outerThick .. -2.55) with Tall Charcoal Slat & Glazed Door
    float alcoveX0 = -hw + outerThick;
    float alcoveX1 = -2.55f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{alcoveX0 + 0.45f, plinthY, hd - 2.6f},
        Vec3{alcoveX0 + 1.25f, plinthY + totalH - 0.45f, hd - 2.15f}, darkCharc);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{alcoveX0 + 1.55f, plinthY, hd - 2.55f},
        Vec3{alcoveX1 - 0.15f, plinthY + 2.38f, hd - 2.35f}, darkCharc);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{alcoveX0 + 1.75f, plinthY + 0.25f, hd - 2.36f},
        Vec3{alcoveX0 + 2.02f, plinthY + 2.18f, hd - 2.30f}, amberGlass);

    // 4. Center Double-Height Frameless Glass Stair Hall + Visible Floating Stair Treads
    float stairX0 = alcoveX1;
    float stairX1 = -0.35f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX0, plinthY, hd - 1.15f},
        Vec3{stairX1, plinthY + totalH - 0.75f, hd - 1.00f}, amberGlass);
    for (int st = 0; st < 8; ++st) {
        float ty = plinthY + 0.45f + st * 0.42f;
        float tx = stairX0 + 0.25f + (st % 4) * 0.38f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx, ty, hd - 2.2f},
            Vec3{tx + 0.65f, ty + 0.10f, hd - 1.25f}, crispWhite);
    }

    // 5. Inner Interlocking Upper Taupe C-Frame wrapping Fluted Slat Wall & Balcony
    float cX0 = -2.95f;
    float cX1 = hw;
    float cBotY = plinthY + flH;
    float cTopY = plinthY + totalH + 0.05f;
    float fluteX0 = 0.05f;
    float fluteX1 = 2.45f;

    // Top horizontal beam spanning stair glass + fluted wall + balcony
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cX0, cTopY - 0.52f, hd - 2.6f},
        Vec3{cX1, cTopY, hd + 0.55f}, warmTaupe);
    // Right leg dropping to bottom of upper C-frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cX1 - 0.58f, cBotY, hd - 2.6f},
        Vec3{cX1, cTopY, hd + 0.55f}, warmTaupe);
    // Bottom cantilevered shelf of upper C-frame over carport
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX1 - 0.1f, cBotY - 0.46f, hd - 2.6f},
        Vec3{cX1, cBotY, hd + 0.55f}, warmTaupe);

    // Fluted Vertical Taupe Batten Panel (x: fluteX0 .. fluteX1)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{fluteX0, cBotY, hd - 1.4f},
        Vec3{fluteX1, cTopY - 0.52f, hd - 1.0f}, darkCharc);
    for (int fl = 0; fl < 12; ++fl) {
        float fx = fluteX0 + 0.08f + fl * ((fluteX1 - fluteX0 - 0.16f) / 12.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx, cBotY + 0.02f, hd - 1.0f},
            Vec3{fx + 0.09f, cTopY - 0.54f, hd - 0.88f}, fluteTaupe);
    }

    // Upper-Right Recessed Glass Balcony Loggia (x: fluteX1 .. cX1-0.58)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{fluteX1, cBotY, hd - 2.2f},
        Vec3{cX1 - 0.58f, cTopY - 0.52f, hd - 2.04f}, darkGlass);
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fluteX1, cBotY, hd + 0.35f},
            Vec3{cX1 - 0.58f, cBotY + 0.92f, hd + 0.43f}, balcGlass);
    }

    // 6. Ground Right Open 2-Car Carport + Two Sleek Sedans
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX1, plinthY, hd - 4.2f},
        Vec3{hw - 0.2f, cBotY - 0.46f, hd - 3.9f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.45f, plinthY, -hd},
        Vec3{hw, plinthY + totalH, hd - 2.6f}, crispWhite);

    if (params_.hasGarage) {
        float carZoneW = hw - stairX1 - 0.8f;
        appendParkedSedan(outVerts, outIndices,
            stairX1 + 0.4f + carZoneW * 0.28f, 0.12f, hd - 1.2f,
            4.25f, 1.80f, 1.44f, darkCharc, darkGlass, tireMat);
        appendParkedSedan(outVerts, outIndices,
            stairX1 + 0.4f + carZoneW * 0.74f, 0.12f, hd - 1.2f,
            4.25f, 1.80f, 1.44f, whiteCar, darkGlass, tireMat);
    }

    // 7. Side, Rear Windows & Flat Concealed Roof
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.6f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 3, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{-hw + 0.45f, 0.0f, -1.0f}, d * 0.45f, fy, flH,
            Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 2, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.9f, d - 2.6f, plinthY + totalH, RoofType::FlatParapet, 0.45f, 0.0f);
}

} // namespace procedural::buildings
