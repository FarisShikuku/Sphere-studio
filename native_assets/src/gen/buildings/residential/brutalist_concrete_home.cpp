#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateBrutalistConcreteHome(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.5f, 15.0f);
    float d = res_scale::depth(params_, 7.5f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.35f);
    int stories = res_scale::stories(params_, 2, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t castConcrete = res_scale::wallMat(params_, MaterialSlot::WallConcrete);
    uint32_t darkSlate    = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t foundMat     = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt         = res_scale::windowType(params_, WindowType::BriseSoleilDeepFinned);
    GlassType gt          = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, true);
    bool hasCantilever    = res_scale::showBalconies(params_);

    // 1. Heavy Brutalist Retaining Plinth & Stepped Approach Bridge
    float plinthY = 0.45f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.4f, -0.35f, -hd - 0.4f},
        Vec3{hw + 0.4f, plinthY, hd + 0.4f},
        foundMat);

    // Floating Concrete Entry Bridge over dry gravel moat
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.45f, 0.22f, hd + 0.4f},
        Vec3{1.45f, plinthY, hd + 2.4f},
        castConcrete);

    // 2. Monolithic Board-Formed Concrete Core + Sculptural Shear Fins
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        castConcrete);

    // Deep vertical concrete structural fins around perimeter
    int finCount = std::max(3, static_cast<int>(w / 3.5f));
    for (int f = 0; f <= finCount; ++f) {
        float fx = -hw + f * (w / static_cast<float>(finCount));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.14f, plinthY, hd},
            Vec3{fx + 0.14f, plinthY + h + 0.55f, hd + 0.55f},
            darkSlate);
    }

    // Offset Cantilevered Concrete Upper Box Massing (+X upper floor projection)
    if (stories >= 2 && hasCantilever) {
        float projW = w * 0.52f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw - projW, plinthY + flH, hd},
            Vec3{hw + 0.8f, plinthY + flH * 2.0f, hd + 1.8f},
            castConcrete);
    }

    // 3. Recessed Brutalist Portal Entry
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, plinthY, hd + 0.04f}, flH, w * 0.5f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, 1.55f, 2.45f, true);

    // 4. Deep Recessed Coffered Windows
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float frontZ = (s == 1 && hasCantilever) ? (hd + 1.82f) : (hd + 0.04f);
        float exMin = (s == 0) ? -1.4f : 0.0f;
        float exMax = (s == 0) ? 1.4f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, frontZ}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            3, wt, gt, 1.50f, flH * 0.62f, 0.55f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 1.50f, flH * 0.62f, 0.55f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.45f, flH * 0.60f, 0.60f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.45f, flH * 0.60f, 0.60f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // Optional Integrated Garage Void under Concrete Wing
    if (params_.hasGarage) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{-hw * 0.55f, plinthY, hd + 0.04f},
            std::min(3.2f, w * 0.28f), std::min(2.35f, flH - 0.30f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
    }

    // 5. Heavy Brutalist Flat Parapet Roof
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, plinthY + h, RoofType::FlatParapet, 0.85f, 0.35f, false);
}

} // namespace procedural::buildings
