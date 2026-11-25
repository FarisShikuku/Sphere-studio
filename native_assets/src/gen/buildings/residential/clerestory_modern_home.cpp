#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateClerestoryModernHome(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.5f, 16.5f);
    float d = res_scale::depth(params_, 7.0f, 11.5f);
    float flH = res_scale::floorHeight(params_, 3.2f);
    int stories = res_scale::stories(params_, 1, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t limestone   = res_scale::wallMat(params_, MaterialSlot::WallLimestone);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t cedarTrim   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t foundation  = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasPorch        = res_scale::showBalconies(params_);

    // 1. Concrete Foundation Apron & Front Entry Walk
    float plinthY = 0.28f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, -0.35f, -hd - 0.5f},
        Vec3{hw + 0.5f, plinthY, hd + 1.2f},
        foundation);

    // 2. Main Architectural Flat Volume
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        limestone);

    // Contrast black aluminum framing pilasters at corners
    float colW = 0.38f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.04f, plinthY, hd - colW}, Vec3{-hw + colW, plinthY + h, hd + 0.04f}, blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - colW, plinthY, hd - colW}, Vec3{hw + 0.04f, plinthY + h, hd + 0.04f}, blackAlum);

    // Entry vestibule recess & canopy
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, plinthY, hd + 0.02f}, flH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::CanopiedGrandPortal, 1.55f, 2.40f, true);

    // 3. Facade Fenestration across all storeys
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float exMin = (s == 0) ? -1.45f : 0.0f;
        float exMax = (s == 0) ? 1.45f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 1.55f, flH * 0.66f, 0.45f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.55f, flH * 0.66f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.50f, flH * 0.65f, 0.50f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        if (hasPorch && s > 0) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{0.0f, y0, hd},
                w * 0.45f, res_scale::balconyDepth(params_, 1.2f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::CantileveredGlass);
        }
    }

    // 4. Optional Attached Flat-Roof Two-Car Garage (+X wing)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.36f, 4.0f, 6.2f);
        float gd = std::clamp(d * 0.60f, 5.4f, 7.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.45f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.45f + gd},
            cedarTrim);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.45f + gd + 0.02f},
            std::min(3.6f, gw - 0.7f), std::min(2.35f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.45f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.5f, 0.0f);
    }

    // 5. Lower Main Flat Roof Slab
    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, plinthY + h, 0.30f);
    }
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, plinthY + h, RoofType::FlatParapet, 0.45f, 0.50f, false);

    // 6. Signature Elevated Central Clerestory Daylight Monitor Box (Flat Cap)
    float monW = w * 0.58f;
    float monD = d * 0.44f;
    float monH = 1.35f;
    float monBaseY = plinthY + h + 0.20f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-monW * 0.5f, monBaseY, -monD * 0.5f},
        Vec3{monW * 0.5f, monBaseY + monH, monD * 0.5f},
        blackAlum);

    // Continuous Clerestory Transom ribbon glazing on front & rear of pop-up monitor
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.0f, monD * 0.5f + 0.02f}, monW, monBaseY, monH + 0.8f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        4, WindowType::ClerestoryTransom, gt, 1.40f, 0.78f, 0.25f,
        0.0f, 0.0f, false, false);
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.0f, -monD * 0.5f - 0.02f}, monW, monBaseY, monH + 0.8f,
        Vec3{0, 0, -1}, Vec3{-1, 0, 0},
        4, WindowType::ClerestoryTransom, gt, 1.40f, 0.78f, 0.25f,
        0.0f, 0.0f, false, false);

    // Overhanging Flat Roof Cap over Clerestory Monitor
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-monW * 0.5f - 0.45f, monBaseY + monH, -monD * 0.5f - 0.45f},
        Vec3{monW * 0.5f + 0.45f, monBaseY + monH + 0.22f, monD * 0.5f + 0.45f},
        cedarTrim);
}

} // namespace procedural::buildings
