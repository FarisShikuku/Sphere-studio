#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateParapetConcealedBungalow(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 9.0f, 15.5f);
    float d = res_scale::depth(params_, 7.5f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.2f);
    int stories = res_scale::stories(params_, 1, 3);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t peachCream   = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t peachTrim    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t stoneAccent  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t whiteCoping  = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t darkCharcoal = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t metalSheet   = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t solarBlue    = static_cast<uint32_t>(MaterialSlot::Glass);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasPorch         = res_scale::showBalconies(params_);

    // 1. Dark Charcoal Plinth & Paved Front Patio Steps
    float plinthY = 0.42f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, -0.35f, -hd - 0.25f},
        Vec3{hw + 0.25f, plinthY, hd + 0.85f},
        darkCharcoal);
    // Entry steps leading to recessed front veranda
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.4f, -0.05f, hd + 0.85f},
        Vec3{2.4f, plinthY * 0.72f, hd + 1.45f},
        stoneAccent);

    // 2. Main L-Stepped Concealed-Roof Bungalow Massing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        peachCream);

    // Stone-Clad Framed Corner & Window Pilaster Bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.04f, plinthY, -hd - 0.04f},
        Vec3{-hw + 0.55f, plinthY + h, hd + 0.04f},
        stoneAccent);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.2f, plinthY, hd - 0.02f},
        Vec3{-0.8f, plinthY + h - 0.15f, hd + 0.10f},
        stoneAccent);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - w * 0.24f, plinthY, hd - 0.02f},
        Vec3{hw + 0.04f, plinthY + h - 0.15f, hd + 0.10f},
        stoneAccent);

    // Iconic Twin-Column Front Veranda Architectural Frame (rising into parapet crown)
    if (hasPorch) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{0.55f, plinthY, hd + 0.45f},
            Vec3{0.82f, plinthY + h + 0.85f, hd + 0.72f},
            peachTrim);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{2.95f, plinthY, hd + 0.45f},
            Vec3{3.25f, plinthY + h + 0.85f, hd + 0.72f},
            peachTrim);
        // Veranda balustrade railing on right half of porch
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{1.9f, plinthY, hd},
            2.3f, res_scale::balconyDepth(params_, 0.65f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::WroughtIron);
    }

    // Entry Door tucked inside Front Veranda
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{1.05f, plinthY, hd + 0.04f}, flH, w * 0.45f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.20f, 2.25f, false);

    // 3. Large Black Multi-Pane Windows Across Facades
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        // Left front window & right front window (skip center veranda door)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw * 0.52f, 0.0f, hd + 0.12f}, hw * 0.85f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.20f, flH * 0.68f, 0.48f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw - w * 0.14f, 0.0f, hd + 0.12f}, w * 0.24f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 1.85f, flH * 0.68f, 0.48f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Rear & Side windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.60f, flH * 0.65f, 0.55f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.85f, flH * 0.65f, 0.55f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 4. Stepped White Parapet Crown Concealing Interior Dark Metal Roof
    float topY = plinthY + h;
    if (wantCornice) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.35f, topY - 0.12f, -hd - 0.35f},
            Vec3{hw + 0.35f, topY + 0.14f, hd + 0.72f},
            peachTrim);
    }

    // Raised White Parapet Perimeter Walls
    float pWallH = 0.72f;
    float pThick = 0.32f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.20f, topY + 0.14f, hd + 0.35f},
        Vec3{hw + 0.20f, topY + 0.14f + pWallH, hd + 0.35f + pThick},
        whiteCoping);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.20f, topY + 0.14f, -hd - 0.20f},
        Vec3{hw + 0.20f, topY + 0.14f + pWallH, -hd + 0.12f},
        whiteCoping);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.20f, topY + 0.14f, -hd - 0.20f},
        Vec3{-hw + 0.14f, topY + 0.14f + pWallH, hd + 0.65f},
        whiteCoping);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.14f, topY + 0.14f, -hd - 0.20f},
        Vec3{hw + 0.20f, topY + 0.14f + pWallH, hd + 0.65f},
        whiteCoping);

    // Concealed Sunken Ribbed Charcoal Metal Roof Deck Inside Parapet
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.12f, topY + 0.08f, -hd + 0.10f},
        Vec3{hw - 0.12f, topY + 0.32f, hd + 0.35f},
        metalSheet);

    // Optional Rooftop Solar PV Panel Array & Black Cylinder Water Tank in Alcove
    if (res_scale::showClutter(params_, true)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-2.2f, topY + 0.32f, -1.2f},
            Vec3{2.2f, topY + 0.48f, 1.4f},
            solarBlue);
        // Rear-Left Water Tank Alcove + Cylindrical Tank
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.20f, topY + 0.14f, -hd - 0.20f},
            Vec3{-hw + 2.4f, topY + 1.05f, -hd + 2.2f},
            whiteCoping);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{-hw + 1.15f, topY + 0.45f, -hd + 1.05f},
            Vec3{-hw + 1.15f, topY + 1.55f, -hd + 1.05f},
            0.72f, 0.72f, 14, darkCharcoal, true, true);
    }

    // 5. Optional Attached Flat-Parapet Carport / Garage (+X wing)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.32f, 3.6f, 5.4f);
        float gd = std::clamp(d * 0.56f, 5.2f, 6.6f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.35f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.35f + gd},
            peachCream);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.35f + gd + 0.02f},
            std::min(3.1f, gw - 0.6f), std::min(2.25f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.35f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.55f, 0.0f);
    }

    // Respect user roofStyle override if user switches away from FlatParapet/NoneFlat
    RoofType rt = res_scale::roofType(params_, RoofType::FlatParapet);
    if (rt != RoofType::FlatParapet && rt != RoofType::NoneFlat) {
        res_scale::appendAdaptiveRoof(
            outVerts, outIndices, params_,
            w, d, topY + pWallH + 0.14f, rt, 2.2f, 0.5f, false);
    }
}

} // namespace procedural::buildings
