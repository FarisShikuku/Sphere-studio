#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateMinimalistCubicVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.0f, 16.0f);
    float d = res_scale::depth(params_, 7.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.3f);
    int stories = res_scale::stories(params_, 1, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t charcoal    = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t cedarAccent = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t plinthStone = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t waterMat    = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasTerrace      = res_scale::showBalconies(params_);

    // 1. Minimalist Travertine Plinth + Reflecting Lap Pool Edge (+Z front)
    float plinthY = 0.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, -0.35f, -hd - 0.6f},
        Vec3{hw + 0.6f, plinthY, hd + 1.8f},
        plinthStone);
    if (hasTerrace) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{0.4f, plinthY + 0.01f, hd + 0.25f},
            Vec3{hw + 0.35f, plinthY + 0.09f, hd + 1.55f},
            waterMat);
    }

    // 2. Intersecting Crisp White Stucco & Anthracite Cubic Volumes
    // Primary Left Cubic Block
    float leftW = w * 0.58f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{-hw + leftW, plinthY + h, hd},
        crispWhite);

    // Offset Recessed Dark Charcoal Portal & Right Living Cube
    float rightH = (stories >= 2) ? (h - 0.35f) : flH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + leftW - 0.4f, plinthY, -hd + 0.8f},
        Vec3{hw, plinthY + rightH, hd - 0.6f},
        charcoal);

    // Warm Cedar Recessed Entry Reveal Frame
    float entryX = -hw + leftW * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{entryX - 1.45f, plinthY, hd - 0.04f},
        Vec3{entryX + 1.45f, plinthY + flH - 0.15f, hd + 0.35f},
        cedarAccent);

    // Floating Flat Concrete Entrance Canopy
    if (params_.useExtendedParams ? params_.hasEntranceCanopy : true) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{entryX - 1.75f, plinthY + flH - 0.22f, hd},
            Vec3{entryX + 1.75f, plinthY + flH - 0.04f, hd + 1.45f},
            crispWhite);
    }

    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{entryX, plinthY, hd + 0.37f}, flH, leftW,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.35f, 2.35f, false);

    // Upper Floor Projected Glass Balcony Frame
    if (hasTerrace && stories >= 2) {
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{hw - w * 0.22f, plinthY + flH, hd - 0.6f},
            w * 0.38f, res_scale::balconyDepth(params_, 1.35f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::CantileveredGlass);
    }

    // 3. Human-Scale Glazing & Doors Across Facades
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        // Front left cube windows (skip door on ground floor)
        float exMin = (s == 0) ? -1.55f : 0.0f;
        float exMax = (s == 0) ? 1.55f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{entryX, 0.0f, hd + 0.02f}, leftW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            2, wt, gt, 1.55f, flH * 0.66f, 0.45f,
            exMin, exMax, wantShutters, wantSunshades);

        // Right recessed wing front windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw + leftW + (w - leftW) * 0.5f, 0.0f, hd - 0.58f}, w - leftW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            2, wt, gt, 1.75f, flH * 0.72f, 0.35f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Rear & Side panoramic windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.65f, flH * 0.68f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.55f, flH * 0.66f, 0.55f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 4. Optional Integrated Modern Flush Garage on +X wing
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.34f, 3.8f, 6.2f);
        float gd = std::clamp(d * 0.55f, 5.2f, 7.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.5f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.5f + gd},
            charcoal);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.5f + gd + 0.02f},
            std::min(3.4f, gw - 0.7f), std::min(2.35f, flH - 0.3f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.5f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.55f, 0.0f);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, leftW, d, plinthY + h, 0.24f);
    }

    // 5. Crisp Flat Parapet Roof (defaults to FlatParapet)
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, plinthY + h, RoofType::FlatParapet, 0.65f, 0.35f, false);
}

} // namespace procedural::buildings
