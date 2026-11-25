#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateCapeCodCottage(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.5f, 13.0f);
    float d = res_scale::depth(params_, 7.0f, 9.5f);
    float flH = res_scale::floorHeight(params_, 2.95f);
    int stories = res_scale::stories(params_, 1, 3);
    float mainWallH = flH * static_cast<float>(std::max(1, stories - 1));
    if (stories == 1) mainWallH = flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t shingleWall = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t brickMat    = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofShingle = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    WindowType wt        = res_scale::windowType(params_, WindowType::DoubleHungSash);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, true);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);

    // 1. Coastal Granite Foundation Plinth + Stoop
    float plinthY = 0.40f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.22f, -0.35f, -hd - 0.22f},
        Vec3{hw + 0.22f, plinthY, hd + 0.22f},
        foundMat);

    // Stoop steps at center front
    for (int st = 0; st < 2; ++st) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-1.15f, st * 0.20f, hd + 0.22f + (1 - st) * 0.35f},
            Vec3{1.15f, (st + 1) * 0.20f, hd + 0.22f + (2 - st) * 0.35f},
            foundMat);
    }

    // 2. Main Shingled Cottage Body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + mainWallH, hd},
        shingleWall);

    // 3. Symmetrical Center Front Door + Flanking Shuttered Windows
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, plinthY, hd + 0.02f}, flH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.25f, 2.20f, true);

    for (int s = 0; s < std::max(1, stories - 1); ++s) {
        float y0 = plinthY + s * flH;
        float exMin = (s == 0) ? -1.1f : 0.0f;
        float exMax = (s == 0) ? 1.1f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 1.20f, 1.50f, 0.75f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 1.20f, 1.50f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.15f, 1.45f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.15f, 1.45f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, plinthY + mainWallH, 0.28f);
    }

    // 4. Steep Cape Cod Roof + Iconic Twin Attic Doghouse Dormers (when stories >= 2)
    float roofTopY = plinthY + mainWallH;
    float defRoofH = std::clamp(flH * 1.15f, 2.8f, 4.2f);
    float rh = res_scale::roofHeight(params_, defRoofH);
    RoofType rt = res_scale::roofType(params_, RoofType::Gable);
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, roofTopY, RoofType::Gable, defRoofH, 0.45f, false);

    if (stories >= 2 && (rt == RoofType::Gable || rt == RoofType::Hip) && rh >= 2.2f) {
        for (float dx : {-hw * 0.48f, hw * 0.48f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{dx - 0.90f, roofTopY + 0.25f, hd - 1.2f},
                Vec3{dx + 0.90f, roofTopY + 1.80f, hd + 0.05f},
                shingleWall);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{dx - 1.05f, roofTopY + 1.80f, hd - 1.35f},
                Vec3{dx + 1.05f, roofTopY + 2.35f, hd + 0.22f},
                roofShingle);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{dx, roofTopY + 1.05f, hd + 0.07f},
                0.95f, 1.10f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
                wt, gt);
        }
    }

    // 5. Central Brick Chimney Straddling the Ridge
    float chimTop = roofTopY + rh + 0.85f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.65f, plinthY, -0.55f},
        Vec3{0.65f, chimTop, 0.55f},
        brickMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.75f, chimTop, -0.65f},
        Vec3{0.75f, chimTop + 0.16f, 0.65f},
        foundMat);

    // Optional Connected Breezeway + Single-Car Garage (+X)
    if (params_.hasGarage) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -1.2f},
            Vec3{hw + 1.8f, plinthY + flH * 0.88f, 1.2f},
            shingleWall);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw + 1.8f, 0.0f, -hd * 0.7f},
            Vec3{hw + 5.8f, flH, hd * 0.7f},
            shingleWall);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + 3.8f, 0.0f, hd * 0.7f + 0.02f},
            2.7f, 2.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw + 1.6f, flH, -hd * 0.75f},
            Vec3{hw + 6.0f, flH + 1.2f, hd * 0.75f},
            roofShingle);
    }
}

} // namespace procedural::buildings
