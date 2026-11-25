#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateHiddenRoofParapetBungalow(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.0f, 16.5f);
    float d = res_scale::depth(params_, 8.5f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.30f);
    int stories = res_scale::stories(params_, 1, 2);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t creamStucco  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t dressedStone = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t peachOchre   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t plinthDark   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t whiteCrown   = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t hiddenRoof   = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t solarBlue    = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t tankBlack    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasVerandah      = res_scale::showBalconies(params_);

    // 1. Dark Plinth & Stepped Porch Patio Approach
    float plinthY = 0.42f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.3f, -0.35f, -hd - 0.3f},
        Vec3{hw + 0.3f, plinthY, hd + 0.5f},
        plinthDark);
    if (hasVerandah) {
        // Grand front verandah entry steps
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-w * 0.04f, -0.10f, hd + 0.50f},
            Vec3{w * 0.34f, plinthY * 0.45f, hd + 1.45f},
            whiteCrown);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-w * 0.02f, plinthY * 0.45f, hd + 0.50f},
            Vec3{w * 0.32f, plinthY * 0.90f, hd + 1.15f},
            whiteCrown);
    }

    // 2. Main Single/Dual-Storey Body with Recessed Right Verandah Cutout
    float leftWingW = w * 0.42f;
    float rightWingW = w * 0.24f;
    // Left Stone & Stucco Bay
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{-hw + leftWingW, plinthY + h, hd + 0.25f},
        dressedStone);
    // Main Rear & Center Hall Core
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + leftWingW - 0.2f, plinthY, -hd},
        Vec3{hw, plinthY + h, hd - 1.15f},
        creamStucco);
    // Right Wing End Pier
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - rightWingW, plinthY, -hd * 0.2f},
        Vec3{hw, plinthY + h, hd + 0.15f},
        dressedStone);

    // Warm Peach / Ochre Corner Pilasters & Vertical Porch Framing Columns
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.06f, plinthY, hd - 0.10f},
        Vec3{-hw + 0.36f, plinthY + h + 0.35f, hd + 0.32f},
        peachOchre);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.36f, plinthY, hd - 0.20f},
        Vec3{hw + 0.06f, plinthY + h + 0.35f, hd + 0.22f},
        peachOchre);

    if (hasVerandah) {
        // Tall Signature Ochre Porch Framing Pillars on Right-Center Verandah
        float colX1 = -hw + leftWingW + 0.35f;
        float colX2 = hw - rightWingW - 0.35f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{colX1 - 0.22f, plinthY, hd - 0.05f},
            Vec3{colX1 + 0.22f, plinthY + h + 0.55f, hd + 0.38f},
            peachOchre);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{colX2 - 0.22f, plinthY, hd - 0.05f},
            Vec3{colX2 + 0.22f, plinthY + h + 0.55f, hd + 0.38f},
            peachOchre);
        // Verandah balustrade rail
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{colX1 + 0.8f, plinthY, hd + 0.12f},
            Vec3{colX2, plinthY + 0.85f, hd + 0.26f},
            whiteCrown);
    }

    // Main Black-Framed Glass Entry Door inside Recessed Verandah
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-hw + leftWingW + 1.05f, plinthY, hd - 1.13f}, flH, w * 0.35f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.30f, 2.30f, false);

    // 3. Large Multi-Pane Black-Framed Windows
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        // Left stone wing wide picture/multi-pane window
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw + leftWingW * 0.5f, 0.0f, hd + 0.27f}, leftWingW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.55f, flH * 0.58f, 0.72f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        // Verandah back wall window
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw * 0.18f, 0.0f, hd - 1.13f}, w * 0.34f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.05f, flH * 0.58f, 0.72f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        // Right wing front window
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw - rightWingW * 0.5f, 0.0f, hd + 0.17f}, rightWingW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 1.75f, flH * 0.58f, 0.72f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        // Left & Rear facades
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 2.15f, flH * 0.58f, 0.72f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.80f, flH * 0.56f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 4. Optional Attached Flat-Parapet Garage (+X side)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.32f, 3.8f, 5.8f);
        float gd = std::clamp(d * 0.55f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.4f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.4f + gd},
            creamStucco);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.4f + gd + 0.02f},
            std::min(3.3f, gw - 0.65f), std::min(2.3f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.4f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.65f, 0.0f);
    }

    // 5. Stepped Crisp White Hidden-Roof Parapet Basin + Recessed Metal Roof + Solar + Tank
    float topY = plinthY + h;
    if (wantCornice) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.25f, topY - 0.05f, -hd - 0.25f},
            Vec3{hw + 0.25f, topY + 0.20f, hd + 0.45f},
            peachOchre);
    }

    // Thick Crisp White Stepped Parapet Perimeter Walls hiding the interior pitched iron roof
    float paraH = 1.05f;
    float t = 0.42f;
    // Front, Rear, Left, Right parapet wall rims
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.20f, topY + 0.20f, hd + 0.40f - t},
        Vec3{hw + 0.20f, topY + 0.20f + paraH, hd + 0.40f},
        whiteCrown);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.20f, topY + 0.20f, -hd - 0.20f},
        Vec3{hw + 0.20f, topY + 0.20f + paraH, -hd - 0.20f + t},
        whiteCrown);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.20f, topY + 0.20f, -hd - 0.20f},
        Vec3{-hw - 0.20f + t, topY + 0.20f + paraH, hd + 0.40f},
        whiteCrown);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw + 0.20f - t, topY + 0.20f, -hd - 0.20f},
        Vec3{hw + 0.20f, topY + 0.20f + paraH, hd + 0.40f},
        whiteCrown);

    // Interior Hidden Sloped Corrugated Charcoal Metal Deck Inside the Parapet Basin
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.22f, topY + 0.20f, -hd + 0.22f},
        Vec3{hw - 0.22f, topY + 0.58f, hd - 0.02f},
        hiddenRoof);

    // Elevated Corner Tank Parapet + Black Cylindrical Water Tank + Rooftop Solar PV Panel Array
    if (res_scale::showClutter(params_, true)) {
        // Rear-Left Elevated Corner Water Tank Turret
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.15f, topY + 0.20f, -hd - 0.15f},
            Vec3{-hw + 2.8f, topY + 1.55f, -hd + 2.6f},
            whiteCrown);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{-hw + 1.35f, topY + 1.55f, -hd + 1.25f},
            Vec3{-hw + 1.35f, topY + 2.65f, -hd + 1.25f},
            0.92f, 0.90f, 14, tankBlack);
        // Center Solar Photovoltaic Panel Bank
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-1.8f, topY + 0.58f, -1.0f},
            Vec3{2.4f, topY + 0.78f, 1.3f},
            solarBlue);
    }

    // Also allow user-selected RoofType overrides if explicitly set to Gable/Hip/GreenRoof
    if (params_.useExtendedParams &&
        params_.roofStyle != RoofType::FlatParapet &&
        params_.roofStyle != RoofType::NoneFlat) {
        res_scale::appendAdaptiveRoof(
            outVerts, outIndices, params_,
            w, d, topY + 0.20f, params_.roofStyle, 2.2f, 0.45f, false);
    }
}

} // namespace procedural::buildings
