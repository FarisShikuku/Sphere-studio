#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateCourtyardAtriumHouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 11.0f, 18.0f);
    float d = res_scale::depth(params_, 11.0f, 16.0f);
    float flH = res_scale::floorHeight(params_, 3.1f);
    int stories = res_scale::stories(params_, 1, 3);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat     = res_scale::wallMat(params_, MaterialSlot::WallLimestone);
    uint32_t cedarMat    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t poolMat     = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, true);
    bool wantCornice     = res_scale::showCornice(params_, true);

    // Wing thicknesses enclosing the central Zen Atrium
    float sideW = std::clamp(w * 0.25f, 3.0f, 4.8f);
    float backD = std::clamp(d * 0.28f, 3.2f, 5.2f);
    float frontD = std::clamp(d * 0.22f, 2.6f, 4.0f);

    // 1. Stone Perimeter Plinth + Interior Emerald Reflecting Pool & Timber Boardwalk
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.35f, -0.35f, -hd - 0.35f},
        Vec3{hw + 0.35f, 0.15f, hd + 0.35f},
        foundMat);

    float cX0 = -hw + sideW;
    float cX1 = hw - sideW;
    float cZ0 = -hd + backD;
    float cZ1 = hd - frontD;
    if (cX1 > cX0 + 1.5f && cZ1 > cZ0 + 1.5f) {
        // Interior reflecting pond
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cX0 + 0.5f, 0.15f, cZ0 + 0.5f},
            Vec3{cX1 - 0.5f, 0.30f, cZ1 - 0.5f},
            poolMat);
        // Timber stepping boardwalk crossing the atrium
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-0.75f, 0.15f, cZ0},
            Vec3{0.75f, 0.35f, cZ1},
            cedarMat);
    }

    // 2. Four Perimeter Courtyard Wings enclosing the open sky court
    // Rear main living wing (-Z, full width)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.15f, -hd},
        Vec3{hw, 0.15f + h, -hd + backD},
        wallMat);
    // Left bedroom wing (-X)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.15f, -hd + backD},
        Vec3{-hw + sideW, 0.15f + h, hd - frontD},
        wallMat);
    // Right studio wing (+X)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - sideW, 0.15f, -hd + backD},
        Vec3{hw, 0.15f + h, hd - frontD},
        wallMat);
    // Front entry gate pavilion (+Z, 1-story gallery bridge so inner courtyard catches sun)
    float gateH = std::min(h, flH);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.15f, hd - frontD},
        Vec3{hw, 0.15f + gateH, hd},
        cedarMat);

    // 3. Entry Door + Exterior & Courtyard-Facing Scaled Windows
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.15f, hd + 0.02f}, gateH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.5f, 2.3f, true);

    // Street-facing windows beside gate pavilion
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.0f, hd + 0.02f}, w, 0.15f, gateH,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        4, wt, gt, 1.4f, 1.55f, 0.65f,
        -1.25f, 1.25f, wantShutters, wantSunshades);

    for (int s = 0; s < stories; ++s) {
        float y0 = 0.15f + s * flH;
        // Outer Left & Right facades
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            3, wt, gt, 1.5f, 1.65f, 0.65f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.5f, 1.65f, 0.65f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.6f, 1.65f, 0.65f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Interior courtyard glass gallery windows facing the pond
        if (cX1 > cX0 + 1.5f && cZ1 > cZ0 + 1.5f) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{0.0f, 0.0f, cZ0 + 0.02f}, cX1 - cX0, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                3, WindowType::PictureGlass, gt, 1.8f, flH * 0.72f, 0.35f,
                0.0f, 0.0f, false, wantSunshades);
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{cX0 + 0.02f, 0.0f, (cZ0 + cZ1) * 0.5f}, cZ1 - cZ0, y0, flH,
                Vec3{1, 0, 0}, Vec3{0, 0, -1},
                2, WindowType::PictureGlass, gt, 1.6f, flH * 0.72f, 0.35f,
                0.0f, 0.0f, false, wantSunshades);
        }
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, 0.15f + h, 0.45f);
    }

    // 4. Deep Japanese/Modern Overhanging Pavilion Eaves Roofs over wings
    float ov = res_scale::roofOverhang(params_, 0.85f);
    RoofType rt = res_scale::roofType(params_, RoofType::NoneFlat);
    float wingRoofH = (rt == RoofType::NoneFlat || rt == RoofType::FlatParapet) ? 0.28f : std::min(1.4f, res_scale::roofHeight(params_, 1.2f));
    // Rear wing eave cap
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - ov, 0.15f + h, -hd - ov},
        Vec3{hw + ov, 0.15f + h + wingRoofH, -hd + backD + ov * 0.6f},
        roofMat);
    // Left & Right wing eave caps
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - ov, 0.15f + h, -hd + backD},
        Vec3{-hw + sideW + ov * 0.6f, 0.15f + h + wingRoofH, hd - frontD},
        roofMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - sideW - ov * 0.6f, 0.15f + h, -hd + backD},
        Vec3{hw + ov, 0.15f + h + wingRoofH, hd - frontD},
        roofMat);
    // Front gate wing eave cap
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - ov, 0.15f + gateH, hd - frontD - ov * 0.6f},
        Vec3{hw + ov, 0.15f + gateH + 0.28f, hd + ov},
        roofMat);
}

} // namespace procedural::buildings
