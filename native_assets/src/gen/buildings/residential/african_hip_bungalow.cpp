#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateAfricanHipBungalow(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.0f, 17.5f);
    float d = res_scale::depth(params_, 8.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.35f);
    int stories = res_scale::stories(params_, 1, 2);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t creamStucco  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t dressedStone = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t darkBand     = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t plinthStone  = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t corniceWhite = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t dishMetal    = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasVerandah      = res_scale::showBalconies(params_);

    // 1. Dark Raised Foundation Plinth & Front Brick-Paved Patio Terrace
    float plinthY = 0.38f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.35f, -0.35f, -hd - 0.35f},
        Vec3{hw + 0.35f, plinthY, hd + 0.45f},
        plinthStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.28f, -0.25f, hd + 0.45f},
        Vec3{w * 0.28f, plinthY - 0.04f, hd + 2.60f},
        dressedStone);

    // 2. Main Symmetrical Bungalow Core & Slightly Projected Front Wing Bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd - 0.4f},
        creamStucco);

    // Flanking stone-clad symmetrical corner piers & left/right bay projection
    float wingW = w * 0.28f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, hd - 0.4f},
        Vec3{-hw + wingW, plinthY + h, hd + 0.35f},
        dressedStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - wingW, plinthY, hd - 0.4f},
        Vec3{hw, plinthY + h, hd + 0.35f},
        dressedStone);

    // Signature Black Architectural Trim Belt & Crisp White Eave Fascia Cornice
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.05f, plinthY + flH - 0.48f, -hd - 0.05f},
        Vec3{hw + 0.05f, plinthY + flH - 0.36f, hd + 0.40f},
        darkBand);

    // 3. Recessed Twin-Bay Front Verandah with Tuscan Columns & White Turned Balusters
    float porchW = w * 0.42f;
    float porchDepth = 1.65f;
    float porchZ0 = hd - 0.4f;
    float porchZ1 = porchZ0 + porchDepth;
    if (hasVerandah) {
        // Porched floor deck
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-porchW * 0.5f, plinthY - 0.04f, porchZ0},
            Vec3{porchW * 0.5f, plinthY + 0.04f, porchZ1},
            corniceWhite);
        // Porch fascia entablature header
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-porchW * 0.5f - 0.1f, plinthY + flH - 0.35f, porchZ0},
            Vec3{porchW * 0.5f + 0.1f, plinthY + flH, porchZ1 + 0.08f},
            creamStucco);
        // 4 Paired White Columns supporting verandah lintel
        float colXs[4] = {
            -porchW * 0.46f, -porchW * 0.14f,
            porchW * 0.04f, porchW * 0.46f
        };
        for (float cx : colXs) {
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{cx, plinthY + 0.85f, porchZ1 - 0.14f},
                Vec3{cx, plinthY + flH - 0.35f, porchZ1 - 0.14f},
                0.13f, 0.11f, 10, corniceWhite);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - 0.18f, plinthY, porchZ1 - 0.30f},
                Vec3{cx + 0.18f, plinthY + 0.85f, porchZ1},
                creamStucco);
        }
        // White Baluster Spindles along Right-Center Verandah Rail
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-porchW * 0.46f, plinthY + 0.78f, porchZ1 - 0.20f},
            Vec3{porchW * 0.46f, plinthY + 0.88f, porchZ1 - 0.06f},
            corniceWhite);
        for (float bx = porchW * 0.08f; bx <= porchW * 0.42f; bx += 0.24f) {
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{bx, plinthY + 0.04f, porchZ1 - 0.14f},
                Vec3{bx, plinthY + 0.78f, porchZ1 - 0.14f},
                0.055f, 0.045f, 8, corniceWhite);
        }
    }

    // Main Recessed Black Panel Front Entry Door (Left side of verandah)
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-porchW * 0.28f, plinthY, porchZ0 + 0.03f}, flH, porchW * 0.5f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.35f, 2.30f, false);

    // 4. Multi-Pane Black-Framed Windows on Wings, Verandah Back, Rear & Sides
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        // Left & right stone wing front windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw + wingW * 0.5f, 0.0f, hd + 0.37f}, wingW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.25f, flH * 0.56f, 0.78f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw - wingW * 0.5f, 0.0f, hd + 0.37f}, wingW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.25f, flH * 0.56f, 0.78f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        // Center verandah lounge window
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{porchW * 0.22f, 0.0f, porchZ0 + 0.03f}, porchW * 0.52f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.15f, flH * 0.56f, 0.78f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Side & Rear elevations
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.80f, flH * 0.55f, 0.80f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.75f, flH * 0.55f, 0.80f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.75f, flH * 0.55f, 0.80f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 5. Optional Attached Carport / Garage Wing
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.28f, 3.6f, 5.4f);
        float gd = std::clamp(d * 0.55f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.4f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.4f + gd},
            creamStucco);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.4f + gd + 0.02f},
            std::min(3.1f, gw - 0.6f), std::min(2.25f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.4f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::Hip, 1.55f, 0.45f);
    }

    // White Moulded Crown Eave Cornice
    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w + 0.3f, d + 0.6f, plinthY + h, 0.45f);
    }

    // 6. Signature High-Pitch Charcoal Standing-Seam Hip Roof + Verandah Accent Gablet
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w + 0.3f, d + 0.6f, plinthY + h, RoofType::Hip, 3.35f, 0.85f, false);

    // Front Center Double Nested Accent Gablet Hood over Verandah + Roof Satellite Dish
    RoofGenParams gablet;
    gablet.type = RoofType::Gable;
    gablet.baseWidth = porchW * 0.72f;
    gablet.baseDepth = 3.2f;
    gablet.baseTopY = plinthY + h + 0.15f;
    gablet.roofHeight = 1.45f;
    gablet.overhang = 0.45f;
    RoofGen::generate(outVerts, outIndices, gablet);

    if (res_scale::showClutter(params_, true)) {
        // Classic Blue/Grey Roof Satellite Dish at Upper Ridge
        float dishY = plinthY + h + 3.15f;
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, dishY - 0.45f, 0.2f},
            Vec3{0.0f, dishY + 0.15f, 0.2f},
            0.05f, 0.05f, 8, dishMetal);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, dishY + 0.15f, 0.2f},
            Vec3{0.0f, dishY + 0.40f, 0.42f},
            0.48f, 0.08f, 14, static_cast<uint32_t>(MaterialSlot::Glass));
    }
}

} // namespace procedural::buildings
