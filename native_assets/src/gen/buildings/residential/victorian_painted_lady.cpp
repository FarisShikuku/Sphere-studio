#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateVictorianPaintedLady(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.5f, 14.0f);
    float d = res_scale::depth(params_, 8.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.2f);
    int stories = res_scale::stories(params_, 2, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t mainWall    = res_scale::wallMat(params_, MaterialSlot::WallBrick);
    uint32_t trimMat     = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t slateRoof   = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    WindowType wt        = res_scale::windowType(params_, WindowType::ArchedHistoric);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, true);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasPorch        = res_scale::showBalconies(params_);

    // 1. Raised Victorian Brick/Stone Basement Plinth (0.65m raised entry)
    float baseY = 0.65f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, -0.35f, -hd - 0.25f},
        Vec3{hw + 0.25f, baseY, hd + 0.25f},
        foundMat);

    // 2. Main Rectangular Body & Projecting Front Gable Wing (Right half)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, baseY, -hd},
        Vec3{hw, baseY + h, hd},
        mainWall);

    // Projecting right-side gable bay
    float bayW = std::clamp(w * 0.42f, 3.6f, 6.2f);
    float bayX0 = hw - bayW - 0.3f;
    float bayX1 = hw - 0.1f;
    float bayMidX = (bayX0 + bayX1) * 0.5f;
    float bayProj = 1.4f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{bayX0, baseY, hd},
        Vec3{bayX1, baseY + h, hd + bayProj},
        mainWall);

    // Decorative Gingerbread Belt Courses between floors
    for (int s = 1; s < stories; ++s) {
        float fy = baseY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.10f, fy - 0.09f, -hd - 0.10f},
            Vec3{hw + 0.10f, fy + 0.09f, hd + 0.10f},
            trimMat);
    }

    // 3. Corner Octagonal Queen Anne Turret + Witch's Hat Conical Spire (Front-Left corner)
    float turretR = std::clamp(std::min(w, d) * 0.21f, 1.8f, 3.0f);
    float tx = -hw + turretR * 0.35f;
    float tz = hd - turretR * 0.35f;
    float turretTopY = baseY + h + 1.2f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{tx, baseY, tz},
        Vec3{tx, turretTopY, tz},
        turretR, turretR, 8, trimMat, true, true);

    // Turret windows per floor on front & outer angled faces
    for (int s = 0; s < stories; ++s) {
        float wy = baseY + s * flH + flH * 0.52f;
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{tx, wy, tz + turretR + 0.02f},
            std::min(1.05f, turretR * 0.65f), std::min(1.65f, flH * 0.56f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{tx - turretR - 0.02f, wy, tz},
            std::min(1.05f, turretR * 0.65f), std::min(1.65f, flH * 0.56f),
            Vec3{-1, 0, 0}, Vec3{0, 0, 1}, wt, gt);
    }

    // Conical spire & brass finial
    float spireH = std::clamp(turretR * 1.9f, 3.0f, 5.2f);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{tx, turretTopY, tz},
        Vec3{tx, turretTopY + spireH, tz},
        turretR + 0.32f, 0.06f, 8, slateRoof, true, true);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{tx, turretTopY + spireH, tz},
        Vec3{tx, turretTopY + spireH + 0.9f, tz},
        0.04f, 0.01f, 8, trimMat, true, true);

    // 4. Covered Front Veranda Porch + Turned Spindle Posts + Front Stairs (Left-center zone)
    float porchX0 = tx + turretR * 0.55f;
    float porchX1 = bayX0;
    float porchSpan = std::max(2.6f, porchX1 - porchX0);
    float porchMidX = (porchX0 + porchX1) * 0.5f;
    float porchD = hasPorch ? std::clamp(res_scale::balconyDepth(params_, 1.8f), 1.4f, 2.6f) : 0.0f;

    if (hasPorch && porchSpan >= 2.2f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{porchX0, 0.0f, hd},
            Vec3{porchX1, baseY, hd + porchD},
            timberMat);
        // Porch steps down to ground
        for (int st = 0; st < 3; ++st) {
            float sy0 = st * (baseY / 3.0f);
            float sy1 = (st + 1) * (baseY / 3.0f);
            float sz0 = hd + porchD + (2 - st) * 0.30f;
            float sz1 = hd + porchD + (3 - st) * 0.30f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{porchMidX - 1.0f, sy0, sz0},
                Vec3{porchMidX + 1.0f, sy1, sz1},
                timberMat);
        }
        // Veranda posts & canopy
        float porchRoofY = baseY + std::min(flH - 0.20f, 2.65f);
        for (float px : {porchX0 + 0.15f, porchX1 - 0.15f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.10f, baseY, hd + porchD - 0.25f},
                Vec3{px + 0.10f, porchRoofY, hd + porchD - 0.05f},
                trimMat);
        }
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{porchX0 - 0.15f, porchRoofY, hd},
            Vec3{porchX1 + 0.15f, porchRoofY + 0.38f, hd + porchD + 0.20f},
            slateRoof);
    }

    // 5. Entrance Door & Scaled Facade Windows
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{porchMidX, baseY, hd + 0.02f}, flH, porchSpan,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::ArchedWooden, 1.25f, 2.35f, !hasPorch);

    for (int s = 0; s < stories; ++s) {
        float y0 = baseY + s * flH;
        // Projecting right gable bay windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{bayMidX, 0.0f, hd + bayProj + 0.02f}, bayW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            2, wt, gt, 1.15f, 1.65f, 0.65f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        // Upper floor window above porch
        if (s > 0 && porchSpan >= 2.2f) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{porchMidX, 0.0f, hd + 0.02f}, porchSpan, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                1, wt, gt, 1.25f, 1.60f, 0.70f,
                0.0f, 0.0f, wantShutters, wantSunshades);
        }
        // Rear & Right Side windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 1.25f, 1.55f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.2f, 1.55f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, baseY + h, 0.35f);
    }

    // 6. Adaptive Steep Roof (defaults to Gable)
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, baseY + h, RoofType::Gable, 3.6f, 0.6f, false);

    // Optional detached carriage garage on +X
    if (params_.hasGarage) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw + 0.4f, 0.0f, -hd + 0.5f},
            Vec3{hw + 4.4f, flH, hd - 1.5f},
            mainWall);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + 2.4f, 0.0f, hd - 1.48f},
            2.6f, 2.25f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        RoofGenParams gRoof;
        gRoof.type = RoofType::Gable;
        gRoof.baseWidth = 4.0f;
        gRoof.baseDepth = std::max(4.0f, d - 2.0f);
        gRoof.baseTopY = flH;
        gRoof.roofHeight = 1.6f;
        gRoof.overhang = 0.35f;
        // Shift via offset box gable cap
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw + 0.25f, flH, -hd + 0.35f},
            Vec3{hw + 4.55f, flH + 1.1f, hd - 1.35f},
            slateRoof);
    }
}

} // namespace procedural::buildings
