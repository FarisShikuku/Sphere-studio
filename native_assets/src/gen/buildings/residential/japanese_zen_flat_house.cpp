#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateJapaneseZenFlatHouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 7.5f, 13.5f);
    float d = res_scale::depth(params_, 7.0f, 11.5f);
    float flH = res_scale::floorHeight(params_, 3.05f);
    int stories = res_scale::stories(params_, 1, 3);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t whitePlaster = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t charredCedar = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t darkFascia   = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t stonePlinth  = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt         = res_scale::windowType(params_, WindowType::SlidingPocketSash);
    GlassType gt          = res_scale::glassType(params_, GlassType::FrostedSatinPrivacy);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, true);
    bool hasEngawaDeck    = res_scale::showBalconies(params_);

    // 1. Minimalist Basalt Stepping Plinth & Engawa Veranda Deck
    float plinthY = 0.28f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, -0.35f, -hd - 0.5f},
        Vec3{hw + 0.5f, plinthY, hd + (hasEngawaDeck ? 1.75f : 0.45f)},
        stonePlinth);

    if (hasEngawaDeck) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.3f, plinthY, hd},
            Vec3{hw - 0.3f, plinthY + 0.14f, hd + 1.50f},
            charredCedar);
    }

    // 2. Clean Cubic White Plaster Ground + Charred Cedar Upper Box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + flH, hd},
        whitePlaster);

    if (stories > 1) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.25f, plinthY + flH, -hd + 0.15f},
            Vec3{hw - 0.25f, plinthY + h, hd + 0.45f},
            charredCedar);
    }

    // Vertical Timber Privacy Fins flanking Zen Entry
    for (int f = 0; f < 5; ++f) {
        float fx = -hw + 0.6f + f * 0.32f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.04f, plinthY, hd + 0.02f},
            Vec3{fx + 0.04f, plinthY + flH - 0.15f, hd + 0.26f},
            charredCedar);
    }

    // 3. Sliding Shoji Entry Portal
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{hw * 0.18f, plinthY, hd + 0.03f}, flH, w * 0.55f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.50f, 2.25f, true);

    // 4. Shoji & Pocket Sash Windows
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float frontZ = (s == 0) ? (hd + 0.02f) : (hd + 0.47f);
        float exMin = (s == 0) ? -1.2f : 0.0f;
        float exMax = (s == 0) ? 1.2f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw * 0.18f, 0.0f, frontZ}, w * 0.68f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            3, wt, gt, 1.45f, flH * 0.60f, 0.55f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 1.45f, flH * 0.58f, 0.60f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.35f, flH * 0.56f, 0.65f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 5. Crisp Dark Aluminum Flat Fascia Roof (defaults to FlatParapet)
    float ov = res_scale::roofOverhang(params_, 0.65f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - ov, plinthY + h - 0.05f, -hd - ov},
        Vec3{hw + ov, plinthY + h + 0.22f, hd + ov + 0.45f},
        darkFascia);

    RoofType rt = res_scale::roofType(params_, RoofType::NoneFlat);
    if (rt != RoofType::NoneFlat && rt != RoofType::FlatParapet) {
        res_scale::appendAdaptiveRoof(
            outVerts, outIndices, params_,
            w, d, plinthY + h + 0.22f, rt, 1.8f, ov, false);
    }
}

} // namespace procedural::buildings
