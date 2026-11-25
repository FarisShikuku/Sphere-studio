#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generatePenthouseTerraceHouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 9.0f, 16.0f);
    float d = res_scale::depth(params_, 8.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.35f);
    int stories = res_scale::stories(params_, 2, 6);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t podiumMat   = res_scale::wallMat(params_, MaterialSlot::WallBasalt);
    uint32_t glassWall   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t marbleSlab  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t timberDeck  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t plinthStone = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::CurtainWall);
    GlassType gt         = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasSkyTerrace   = res_scale::showBalconies(params_);

    // 1. Urban Travertine Base Podium
    float plinthY = 0.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, -0.35f, -hd - 0.5f},
        Vec3{hw + 0.5f, plinthY, hd + 1.0f},
        plinthStone);

    // 2. Lower Stories Podium Mass + Offset Upper Penthouse Suite
    int lowerFloors = std::max(1, stories - 1);
    float lowerH = lowerFloors * flH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + lowerH, hd},
        podiumMat);

    // Wraparound Sky Terrace Slab & Timber Deck on top of lower podium
    float terraceY = plinthY + lowerH;
    if (hasSkyTerrace) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.85f, terraceY - 0.14f, -hd - 0.65f},
            Vec3{hw + 0.85f, terraceY + 0.12f, hd + 1.15f},
            marbleSlab);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.65f, terraceY + 0.12f, -hd - 0.45f},
            Vec3{hw + 0.65f, terraceY + 0.18f, hd + 0.95f},
            timberDeck);
        // Perimeter glass terrace balustrade around front & side sky terrace
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{0.0f, terraceY + 0.08f, hd},
            w * 0.92f, res_scale::balconyDepth(params_, 1.15f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::CantileveredGlass);
    }

    // Offset Recessed Penthouse Glass Crown Box
    float pentW = w * 0.66f;
    float pentD = d * 0.64f;
    float pentMinX = -hw + 0.55f;
    float pentMaxX = pentMinX + pentW;
    float pentMinZ = -hd + 0.45f;
    float pentMaxZ = pentMinZ + pentD;
    float pentTopY = terraceY + flH;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pentMinX, terraceY, pentMinZ},
        Vec3{pentMaxX, pentTopY, pentMaxZ},
        glassWall);

    // Ground Floor Entry Door + Canopy
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, plinthY, hd + 0.02f}, flH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, 1.85f, 2.50f, true);

    // 3. Curtain Wall & Panorama Windows across lower floors & penthouse level
    for (int s = 0; s < lowerFloors; ++s) {
        float y0 = plinthY + s * flH;
        float exMin = (s == 0) ? -1.45f : 0.0f;
        float exMax = (s == 0) ? 1.45f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 1.80f, flH * 0.72f, 0.35f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.80f, flH * 0.72f, 0.35f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.65f, flH * 0.70f, 0.40f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // Penthouse Floor Glazing
    float pentCx = (pentMinX + pentMaxX) * 0.5f;
    float pentCz = (pentMinZ + pentMaxZ) * 0.5f;
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{pentCx, 0.0f, pentMaxZ + 0.02f}, pentW, terraceY + 0.18f, flH,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        3, WindowType::CurtainWall, gt, 1.95f, flH * 0.75f, 0.25f,
        0.0f, 0.0f, false, wantSunshades);
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{pentMaxX + 0.02f, 0.0f, pentCz}, pentD, terraceY + 0.18f, flH,
        Vec3{1, 0, 0}, Vec3{0, 0, -1},
        2, WindowType::CurtainWall, gt, 1.85f, flH * 0.75f, 0.25f,
        0.0f, 0.0f, false, wantSunshades);

    // 4. Optional Underground-Style Flush Podium Garage (+X wing)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.34f, 3.8f, 6.0f);
        float gd = std::clamp(d * 0.58f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.45f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.45f + gd},
            podiumMat);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.45f + gd + 0.02f},
            std::min(3.5f, gw - 0.65f), std::min(2.35f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.45f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.55f, 0.0f);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, pentW, pentD, pentTopY, 0.30f);
    }

    // 5. Sleek Flat Parapet Crown Roof over Penthouse
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        pentW, pentD, pentTopY, RoofType::FlatParapet, 0.65f, 0.60f, true);
}

} // namespace procedural::buildings
