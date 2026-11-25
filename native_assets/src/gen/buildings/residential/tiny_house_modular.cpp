#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateTinyHouseModular(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Authentic human-scale micro-dwelling (defaults 5.2m x 8.5m, 1 or lofted 2 stories)
    float w = res_scale::width(params_, 3.5f, 5.2f);
    float d = res_scale::depth(params_, 5.5f, 8.5f);
    float flH = res_scale::floorHeight(params_, 2.85f);
    int stories = res_scale::stories(params_, 1, 2);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t cedarWall   = res_scale::wallMat(params_, MaterialSlot::WallTimber);
    uint32_t metalTrim   = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t steelPier   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassPv     = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    WindowType wt        = res_scale::windowType(params_, WindowType::ModernCasement);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, true);
    bool hasDeck         = res_scale::showBalconies(params_);

    // 1. Elevated Steel Helical Piers + Chassis Beams (0.55m ground clearance)
    float chassisY = 0.55f;
    for (float px : {-hw + 0.35f, hw - 0.35f}) {
        for (float pz : {-hd + 0.5f, 0.0f, hd - 0.5f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{px, 0.0f, pz}, Vec3{px, chassisY, pz},
                0.12f, 0.12f, 10, steelPier, true, true);
        }
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.05f, chassisY - 0.16f, -hd - 0.05f},
        Vec3{hw + 0.05f, chassisY, hd + 0.05f},
        metalTrim);

    // 2. Modular Prefab Container/Timber Pod Box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, chassisY, -hd},
        Vec3{hw, chassisY + h, hd},
        cedarWall);

    // Corner Matte Black Steel Frame Columns
    for (float cx : {-hw - 0.03f, hw - 0.09f}) {
        for (float cz : {-hd - 0.03f, hd - 0.09f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx, chassisY, cz},
                Vec3{cx + 0.12f, chassisY + h, cz + 0.12f},
                metalTrim);
        }
    }

    // 3. Optional Cedar Fold-Down Porch Deck & Entry Steps (+Z front)
    float deckExt = hasDeck ? std::clamp(res_scale::balconyDepth(params_, 1.8f), 1.2f, 2.4f) : 0.0f;
    if (hasDeck) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.1f, chassisY - 0.10f, hd},
            Vec3{hw + 0.1f, chassisY + 0.04f, hd + deckExt},
            cedarWall);
        for (int st = 0; st < 2; ++st) {
            float sy0 = st * (chassisY * 0.5f);
            float sy1 = (st + 1) * (chassisY * 0.5f);
            float sz0 = hd + deckExt + (1 - st) * 0.30f;
            float sz1 = hd + deckExt + (2 - st) * 0.30f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-0.8f, sy0, sz0},
                Vec3{0.8f, sy1, sz1},
                cedarWall);
        }
    }

    // 4. Human-Scale Entry Door & Panoramic Side/End Windows
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, chassisY, hd + 0.02f}, flH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.35f, 2.15f, true);

    for (int s = 0; s < stories; ++s) {
        float y0 = chassisY + s * flH;
        if (s > 0) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                1, wt, gt, 1.4f, 1.25f, 0.70f,
                0.0f, 0.0f, wantShutters, wantSunshades);
        }
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.55f, 1.45f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.55f, 1.45f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            1, wt, gt, 1.25f, 1.35f, 0.80f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 5. Adaptive Roof (defaults to ShedMonopitch roof) + Off-Grid Solar Array
    RoofType rt = res_scale::roofType(params_, RoofType::ShedMonopitch);
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, chassisY + h, RoofType::ShedMonopitch, 1.15f, 0.45f, false);

    // Rooftop solar PV array on flat/shed roofs
    if (rt == RoofType::ShedMonopitch || rt == RoofType::NoneFlat || rt == RoofType::FlatParapet) {
        float solY = chassisY + h + ((rt == RoofType::ShedMonopitch) ? 0.95f : 0.30f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.72f, solY, -hd * 0.55f},
            Vec3{hw * 0.72f, solY + 0.08f, hd * 0.55f},
            glassPv);
    }
}

} // namespace procedural::buildings
