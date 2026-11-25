#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateColonialFourSquare(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 9.0f, 13.0f);
    float d = res_scale::depth(params_, 8.5f, 11.0f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 2, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t brickWall   = res_scale::wallMat(params_, MaterialSlot::WallBrick);
    uint32_t trimStone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofSlate   = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    WindowType wt        = res_scale::windowType(params_, WindowType::DoubleHungSash);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, true);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasPorch        = res_scale::showBalconies(params_);

    // 1. Limestone Base Plinth + Quoining Corner Pilasters
    float plinthY = 0.45f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, -0.35f, -hd - 0.25f},
        Vec3{hw + 0.25f, plinthY, hd + 0.25f},
        foundMat);

    // Symmetrical Georgian/American Four-Square Cube Body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        brickWall);

    // Classical Corner Quoins / Pilasters & Horizontal Belt Course
    for (float cx : {-hw - 0.04f, hw - 0.28f}) {
        for (float cz : {-hd - 0.04f, hd - 0.28f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx, plinthY, cz},
                Vec3{cx + 0.32f, plinthY + h, cz + 0.32f},
                trimStone);
        }
    }
    res_scale::appendBeltCourses(outVerts, outIndices, hw, hd, stories, flH, trimStone);

    // 2. Full-Width Columned Front Porch (+Z)
    float porchD = hasPorch ? std::clamp(res_scale::balconyDepth(params_, 2.2f), 1.5f, 2.8f) : 0.0f;
    if (hasPorch) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw, 0.0f, hd},
            Vec3{hw, plinthY, hd + porchD},
            foundMat);
        // Brick piers & Doric Columns supporting porch entablature
        float entabY = plinthY + flH - 0.32f;
        int cols = 4;
        for (int c = 0; c < cols; ++c) {
            float cx = -hw + 0.45f + c * ((w - 0.9f) / (cols - 1));
            float cz = hd + porchD - 0.30f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - 0.22f, plinthY, cz - 0.22f},
                Vec3{cx + 0.22f, plinthY + 0.85f, cz + 0.22f},
                brickWall);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{cx, plinthY + 0.85f, cz},
                Vec3{cx, entabY, cz},
                0.15f, 0.13f, 12, trimStone, true, true);
        }
        // Porch entablature roof beam
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.18f, entabY, hd},
            Vec3{hw + 0.18f, entabY + 0.45f, hd + porchD + 0.15f},
            roofSlate);

        // Grand center front entry stairs
        for (int st = 0; st < 3; ++st) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-1.35f, st * (plinthY / 3.0f), hd + porchD + (2 - st) * 0.32f},
                Vec3{1.35f, (st + 1) * (plinthY / 3.0f), hd + porchD + (3 - st) * 0.32f},
                foundMat);
        }
    }

    // 3. Centered Georgian Entry Door + Balanced 4-Square Symmetrical Window Bays
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, plinthY, hd + 0.02f}, flH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.45f, 2.30f, !hasPorch);

    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float exMin = (s == 0) ? -1.2f : 0.0f;
        float exMax = (s == 0) ? 1.2f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            3, wt, gt, 1.30f, 1.65f, 0.70f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 1.30f, 1.65f, 0.70f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.25f, 1.60f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.25f, 1.60f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, plinthY + h, 0.42f);
    }

    // 4. Classic Four-Square Pyramidal/Hip Roof + Central Front Dormer + Twin Side Chimneys
    float roofY = plinthY + h;
    float rh = res_scale::roofHeight(params_, 3.0f);
    RoofType rt = res_scale::roofType(params_, RoofType::Hip);
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, roofY, RoofType::Hip, 3.0f, 0.65f, false);

    if ((rt == RoofType::Hip || rt == RoofType::PyramidalApex || rt == RoofType::Gable) && rh >= 2.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-1.15f, roofY + 0.15f, hd - 1.3f},
            Vec3{1.15f, roofY + 1.65f, hd + 0.10f},
            timberMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-1.30f, roofY + 1.65f, hd - 1.45f},
            Vec3{1.30f, roofY + 2.10f, hd + 0.25f},
            roofSlate);
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{0.0f, roofY + 0.90f, hd + 0.12f},
            1.15f, 1.00f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            wt, gt);
    }

    // Twin brick end chimneys
    for (float side : {-1.0f, 1.0f}) {
        float x0 = (side < 0.0f) ? (-hw - 0.65f) : hw;
        float x1 = (side < 0.0f) ? -hw : (hw + 0.65f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{x0, 0.0f, -0.55f},
            Vec3{x1, roofY + rh + 0.6f, 0.55f},
            brickWall);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{x0 - 0.08f, roofY + rh + 0.6f, -0.63f},
            Vec3{x1 + 0.08f, roofY + rh + 0.78f, 0.63f},
            trimStone);
    }
}

} // namespace procedural::buildings
