#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendCorinthianColumn(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x, float y0, float z,
    float topY, float rad,
    uint32_t shaftMat, uint32_t capMat
) {
    // Stepped classical plinth & torus base
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{x - rad * 1.65f, y0, z - rad * 1.65f},
        Vec3{x + rad * 1.65f, y0 + 0.24f, z + rad * 1.65f}, shaftMat);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{x, y0 + 0.24f, z}, Vec3{x, y0 + 0.42f, z},
        rad * 1.38f, rad * 1.12f, 16, shaftMat, true, true);
    // Monumental fluted tapered cylinder shaft
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{x, y0 + 0.42f, z}, Vec3{x, topY - 0.75f, z},
        rad, rad * 0.88f, 16, shaftMat, true, true);
    // Flared ornate Corinthian bell + abacus capital
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{x, topY - 0.75f, z}, Vec3{x, topY - 0.22f, z},
        rad * 0.92f, rad * 1.52f, 16, capMat, true, true);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{x - rad * 1.65f, topY - 0.22f, z - rad * 1.65f},
        Vec3{x + rad * 1.65f, topY, z + rad * 1.65f}, shaftMat);
}

void appendGarageSedan(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float y0, float cz,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat
) {
    float hw = 0.90f, hl = 2.15f, hgt = 1.48f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw, y0 + 0.20f, cz - hl},
        Vec3{cx + hw, y0 + hgt * 0.54f, cz + hl}, bodyMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.84f, y0 + hgt * 0.54f, cz - hl * 0.45f},
        Vec3{cx + hw * 0.84f, y0 + hgt, cz + hl * 0.25f}, glassMat);
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.58f, hl * 0.58f}) {
            float wx = cx + sx * (hw - 0.04f);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{wx - 0.10f, y0 + 0.29f, cz + sz},
                Vec3{wx + 0.10f, y0 + 0.29f, cz + sz},
                0.29f, 0.29f, 12, tireMat, true, true);
        }
    }
}

} // namespace

void HouseGen::generateCorinthianPorticoModernVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 15.0f, 21.5f);
    float d = res_scale::depth(params_, 10.5f, 13.5f);
    float flH = res_scale::floorHeight(params_, 3.35f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = 2.0f * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t limestone  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t darkBasalt = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t bronzeCap  = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
    uint32_t greenLawn  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t blackGlass = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t frameMetal = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t whiteCar   = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t tireMat    = static_cast<uint32_t>(MaterialSlot::TireRubber);
    uint32_t redCar     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::CurtainWall);

    float plinthY = 0.32f;
    // 1. Landscaped Driveway, Lawn & Steps
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, -0.18f, -hd - 1.2f},
        Vec3{ hw + 2.2f,  0.02f,  hd + 4.8f}, limestone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, 0.02f, hd + 1.8f},
        Vec3{ hw * 0.1f, 0.20f, hd + 4.8f}, greenLawn);

    // 4 Ceremonial White Marble Steps leading to Grand Right Portico
    for (int st = 0; st < 4; ++st) {
        float yTop = 0.08f * (st + 1);
        float zFront = hd + 2.6f - st * 0.36f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-0.4f, yTop - 0.08f, hd - 0.5f},
            Vec3{ hw * 0.72f, yTop, zFront}, crispWhite);
    }

    // Main foundation base
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, crispWhite);

    // 2. Main Dark-Basalt Accent Core & Rear Crisp White Residence Volume
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.5f, plinthY, -hd},
        Vec3{ hw - 0.5f, plinthY + totalH, hd - 2.8f}, darkBasalt);

    // 3. Left Wing: Wide Open 2-Car Garage + Terrace Deck Above + Diagonal Signature Strut
    float garX0 = -hw;
    float garX1 = -0.75f;
    float garTopY = plinthY + flH - 0.15f;
    // Garage left wall + back wall + deep white overhanging garage slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garX0, plinthY, -hd + 0.8f},
        Vec3{garX0 + 0.45f, garTopY, hd + 1.0f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garX0, plinthY, hd - 3.6f},
        Vec3{garX1, garTopY, hd - 3.25f}, darkBasalt);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garX0 - 0.25f, garTopY - 0.42f, hd - 3.5f},
        Vec3{garX1 + 0.25f, garTopY, hd + 1.45f}, crispWhite);
    // Glass balustrade wrapping rooftop terrace above garage
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{garX0 - 0.15f, garTopY, hd + 1.32f},
            Vec3{garX1 + 0.15f, garTopY + 0.98f, hd + 1.40f}, balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{garX0 - 0.15f, garTopY, hd - 2.6f},
            Vec3{garX0 - 0.07f, garTopY + 0.98f, hd + 1.40f}, balcGlass);
    }

    // Upper-left recessed panoramic glass floor above garage terrace
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garX0 + 0.65f, garTopY, hd - 1.85f},
        Vec3{garX1 - 0.35f, plinthY + totalH - 0.35f, hd - 1.68f}, blackGlass);
    for (int m = 0; m < 5; ++m) {
        float mx = garX0 + 0.75f + m * ((garX1 - garX0 - 1.25f) / 4.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.04f, garTopY, hd - 1.70f},
            Vec3{mx + 0.04f, plinthY + totalH - 0.35f, hd - 1.62f}, frameMetal);
    }

    // Slanted diagonal white architectural buttress pier connecting terrace slab to upper fascia
    Vec3 strutCenter{-1.05f, (garTopY + plinthY + totalH) * 0.5f, hd - 0.35f};
    Vec3 strutUp = Vec3{-0.42f, flH, 0.25f}.normalized();
    Vec3 strutTan{1.0f, 0.0f, 0.0f};
    Vec3 strutNorm = strutTan.cross(strutUp).normalized();
    BuildingComponents::appendOrientedBox(outVerts, outIndices,
        strutCenter, strutTan, strutUp, strutNorm,
        0.28f, flH * 0.56f, 0.45f, crispWhite);

    if (params_.hasGarage) {
        float midG = (garX0 + garX1) * 0.5f;
        appendGarageSedan(outVerts, outIndices, midG - 1.65f, 0.05f, hd - 0.55f, whiteCar, blackGlass, tireMat);
        appendGarageSedan(outVerts, outIndices, midG + 1.65f, 0.05f, hd - 0.85f, darkBasalt, blackGlass, tireMat);
    }

    // 4. Right Double-Height Monumental Corinthian Portico & Center Glass Doorway
    float portX0 = -0.35f;
    float portX1 = hw * 0.68f;
    float portRoofTop = plinthY + totalH + 0.35f;
    // Recessed double-height black gridded glass wall behind columns
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{portX0 + 0.2f, plinthY, hd - 2.35f},
        Vec3{portX1 - 0.2f, portRoofTop - 0.55f, hd - 2.20f}, blackGlass);
    // Double paneled modern timber-black entry door centered between columns
    float midPortX = (portX0 + portX1) * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midPortX - 0.95f, plinthY, hd - 2.24f},
        Vec3{midPortX + 0.95f, plinthY + 2.65f, hd - 2.12f}, darkBasalt);
    // Projecting curved-edge white middle balcony above doorway
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midPortX - 1.85f, garTopY - 0.12f, hd - 2.25f},
        Vec3{midPortX + 1.85f, garTopY + 0.22f, hd - 0.45f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midPortX - 1.75f, garTopY + 0.22f, hd - 0.54f},
        Vec3{midPortX + 1.75f, garTopY + 1.15f, hd - 0.46f}, balcGlass);

    // TWO Monumental Fluted Corinthian Columns framing the Portico
    appendCorinthianColumn(outVerts, outIndices,
        portX0 + 0.65f, plinthY, hd + 0.25f, portRoofTop - 0.55f, 0.38f, limestone, bronzeCap);
    appendCorinthianColumn(outVerts, outIndices,
        portX1 - 0.65f, plinthY, hd + 0.25f, portRoofTop - 0.55f, 0.38f, limestone, bronzeCap);

    // Continuous Sweeping Upper White Cantilever Fascia Roof across Garage & Corinthian Portico
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.55f, portRoofTop - 0.55f, hd - 2.8f},
        Vec3{ hw + 0.45f, portRoofTop, hd + 1.35f}, crispWhite);

    // 5. Far-Right 2-Storey Corner Glass Bay + Vertical Louver Accent
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{portX1 + 0.3f, plinthY, hd - 2.6f},
        Vec3{hw - 0.2f, portRoofTop - 0.55f, hd - 1.6f}, blackGlass);
    for (int lv = 0; lv < 6; ++lv) {
        float lx = portX1 - 0.15f + lv * 0.16f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{lx, plinthY, hd - 1.8f}, Vec3{lx + 0.05f, portRoofTop - 0.55f, hd - 1.65f}, crispWhite);
    }

    // 6. Towering Center-Back Dark Basalt Penthouse Core & 3rd-Tier Roof Suite
    float pentY0 = portRoofTop;
    float pentY1 = pentY0 + flH * 0.88f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-3.8f, pentY0 - 0.2f, -hd + 0.8f},
        Vec3{ 1.2f, pentY1 + 0.55f, hd - 3.2f}, darkBasalt);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-7.2f, pentY0, -hd + 1.2f},
        Vec3{ 4.6f, pentY1, hd - 3.8f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-6.6f, pentY0 + 0.2f, hd - 3.82f},
        Vec3{ 3.8f, pentY1 - 0.25f, hd - 3.70f}, blackGlass);
    // Optional parked red coupe accent on right driveway
    if (params_.hasGarage) {
        appendGarageSedan(outVerts, outIndices, hw * 0.72f, 0.04f, hd + 2.6f, redCar, blackGlass, tireMat);
    }

    // 7. Side & Rear Windows + Crown Flat Roof
    for (int s = 0; s < 2; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 2.0f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 4, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 1.2f, d - 3.0f, plinthY + totalH, RoofType::FlatParapet, 0.45f, 0.0f);
    (void)stories;
}

} // namespace procedural::buildings
