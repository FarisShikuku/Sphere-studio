#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateDesertModernistPavilion(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.0f, 18.0f);
    float d = res_scale::depth(params_, 7.0f, 11.0f);
    float flH = res_scale::floorHeight(params_, 3.25f);
    int stories = res_scale::stories(params_, 1, 3);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t limestone   = res_scale::wallMat(params_, MaterialSlot::WallLimestone);
    uint32_t stucco      = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t bronzeBeam  = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
    uint32_t plinthMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::ClerestoryTransom);
    GlassType gt         = res_scale::glassType(params_, GlassType::BronzeArchitectural);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, true);
    bool hasPatio        = res_scale::showBalconies(params_);

    // 1. Desert Flagstone Terrace Pad
    float plinthY = 0.22f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.2f, -0.35f, -hd - 0.8f},
        Vec3{hw + 1.2f, plinthY, hd + 2.2f},
        plinthMat);

    // 2. Horizontal Limestone & Stucco Living Volume
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        limestone);

    // Signature Palm Springs Stacked Breeze-Block Entry Privacy Wall (+Z front-left)
    float screenW = std::clamp(w * 0.24f, 2.5f, 4.2f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, plinthY, hd + 1.35f},
        Vec3{-hw - 0.6f + screenW, plinthY + flH * 0.78f, hd + 1.65f},
        stucco);

    // Architectural Steel Hearth/Fireplace Core Tower breaking flat roofline
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.2f, plinthY, -hd - 0.45f},
        Vec3{1.2f, plinthY + h + 1.35f, -hd + 0.55f},
        limestone);

    // 3. Deep Overhanging Flat Roof Fascia Slab (Iconic Desert Shade Canopy)
    float ov = res_scale::roofOverhang(params_, 1.25f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - ov, plinthY + h - 0.08f, -hd - ov},
        Vec3{hw + ov, plinthY + h + 0.28f, hd + ov + 0.55f},
        bronzeBeam);

    // 4. Sliding Patio Doors & High Clerestory Ribbon Glazing
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{hw * 0.25f, plinthY, hd + 0.02f}, flH, w * 0.55f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, 2.1f, 2.40f, false);

    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float exMin = (s == 0) ? -1.4f : 0.0f;
        float exMax = (s == 0) ? 1.4f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, WindowType::PictureGlass, gt, 1.75f, flH * 0.72f, 0.35f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.85f, flH * 0.35f, flH * 0.52f,
            0.0f, 0.0f, false, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            3, wt, gt, 1.65f, flH * 0.35f, flH * 0.52f,
            0.0f, 0.0f, false, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.65f, flH * 0.35f, flH * 0.52f,
            0.0f, 0.0f, false, wantSunshades);
    }

    // 5. Shaded Flat Carport Canopy (+X)
    if (params_.hasGarage) {
        float cpW = 5.2f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY + flH - 0.20f, -hd + 0.5f},
            Vec3{hw + cpW, plinthY + flH + 0.08f, hd - 0.5f},
            bronzeBeam);
        for (float pz : {-hd + 0.8f, hd - 0.8f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{hw + cpW - 0.25f, plinthY, pz - 0.10f},
                Vec3{hw + cpW - 0.05f, plinthY + flH - 0.20f, pz + 0.10f},
                bronzeBeam);
        }
    }

    if (hasPatio) {
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{0.0f, plinthY, hd},
            w * 0.65f, 1.6f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::ConcreteLedge);
    }

    // 6. Adaptive Flat Roof (defaults to NoneFlat since fascia slab is integrated)
    RoofType rt = res_scale::roofType(params_, RoofType::NoneFlat);
    if (rt != RoofType::NoneFlat) {
        res_scale::appendAdaptiveRoof(
            outVerts, outIndices, params_,
            w, d, plinthY + h + 0.28f, rt, 1.6f, ov, false);
    }
}

} // namespace procedural::buildings
