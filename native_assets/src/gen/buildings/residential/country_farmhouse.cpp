#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateCountryFarmhouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.0f, 16.0f);
    float d = res_scale::depth(params_, 8.0f, 11.0f);
    float flH = res_scale::floorHeight(params_, 3.1f);
    int stories = res_scale::stories(params_, 1, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t mainWall   = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t stoneMat   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t brickMat   = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t metalRoof  = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t timberMat  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    WindowType wt       = res_scale::windowType(params_, WindowType::DoubleHungSash);
    GlassType gt        = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters   = res_scale::showShutters(params_, true);
    bool wantSunshades  = res_scale::showSunshades(params_, false);
    bool wantCornice    = res_scale::showCornice(params_, true);
    bool hasPorch       = res_scale::showBalconies(params_);
    float porchD        = hasPorch ? std::clamp(res_scale::balconyDepth(params_, 2.4f), 1.6f, 3.4f) : 0.0f;

    // 1. Foundation fieldstone plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.35f, -0.4f, -hd - 0.35f},
        Vec3{hw + 0.35f, 0.0f, hd + 0.35f},
        foundMat);

    // 2. Main house volume
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{hw, h, hd},
        mainWall);

    // Rear kitchen ell wing (extending back along -Z)
    float ellW = std::clamp(w * 0.45f, 4.5f, 8.0f);
    float ellD = std::clamp(d * 0.40f, 3.5f, 5.2f);
    float ellH = std::min(h, flH * 1.2f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.8f, 0.0f, -hd - ellD},
        Vec3{-hw + 0.8f + ellW, ellH, -hd},
        mainWall);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.6f, ellH, -hd - ellD - 0.3f},
        Vec3{-hw + 1.0f + ellW, ellH + 1.3f, -hd + 0.2f},
        metalRoof);

    // 3. Wraparound Covered Front Porch
    float deckTopY = hasPorch ? 0.32f : 0.0f;
    float porchRoofH = std::min(flH - 0.25f, 2.65f);
    if (hasPorch) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.2f, 0.0f, hd},
            Vec3{hw + porchD * 0.85f, deckTopY, hd + porchD},
            timberMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, 0.0f, -hd * 0.35f},
            Vec3{hw + porchD * 0.85f, deckTopY, hd},
            timberMat);

        int frontPosts = std::max(4, static_cast<int>(w / 3.0f) + 1);
        float totalSpan = hw * 2.0f + porchD * 0.85f;
        for (int p = 0; p < frontPosts; ++p) {
            float px = -hw + p * (totalSpan / (frontPosts - 1));
            float pz = hd + porchD - 0.22f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.11f, deckTopY, pz - 0.11f},
                Vec3{px + 0.11f, porchRoofH, pz + 0.11f},
                mainWall);
            if (p < frontPosts - 1 && p != (frontPosts / 2 - 1)) {
                float nextPx = -hw + (p + 1) * (totalSpan / (frontPosts - 1));
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{px + 0.11f, 0.95f, pz - 0.04f},
                    Vec3{nextPx - 0.11f, 1.04f, pz + 0.04f},
                    timberMat);
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{px + 0.11f, 0.44f, pz - 0.04f},
                    Vec3{nextPx - 0.11f, 0.52f, pz + 0.04f},
                    timberMat);
            }
        }

        // Porch sloped roof apron
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.3f, porchRoofH, hd},
            Vec3{hw + porchD * 0.85f + 0.3f, porchRoofH + 0.55f, hd + porchD + 0.3f},
            metalRoof);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, porchRoofH, -hd * 0.35f - 0.3f},
            Vec3{hw + porchD * 0.85f + 0.3f, porchRoofH + 0.55f, hd},
            metalRoof);

        // Front steps
        float stepW = 2.1f;
        for (int st = 0; st < 3; ++st) {
            float sy0 = st * (deckTopY / 3.0f);
            float sy1 = (st + 1) * (deckTopY / 3.0f);
            float sz0 = hd + porchD + (2 - st) * 0.32f;
            float sz1 = hd + porchD + (3 - st) * 0.32f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-stepW * 0.5f, sy0, sz0},
                Vec3{stepW * 0.5f, sy1, sz1},
                timberMat);
        }
    }

    // Main Farmhouse Door
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, deckTopY, hd + 0.02f}, flH - deckTopY, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.3f, 2.2f, !hasPorch);

    // 4. Scaled windows across all stories
    for (int s = 0; s < stories; ++s) {
        float y0 = s * flH;
        float exMin = (s == 0) ? -1.1f : 0.0f;
        float exMax = (s == 0) ? 1.1f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 1.3f, 1.65f, (s == 0 && hasPorch) ? 0.90f : 0.75f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w * 0.5f, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            2, wt, gt, 1.25f, 1.55f, 0.80f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.25f, 1.55f, 0.80f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, h, 0.35f);
    }

    // 5. Roof (Adaptive) + Gabled Attic Dormers if pitched
    RoofType rt = res_scale::roofType(params_, RoofType::Gable);
    float rh = res_scale::roofHeight(params_, 3.2f);
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, h, RoofType::Gable, 3.2f, 0.6f, false);

    if ((rt == RoofType::Gable || rt == RoofType::Hip || rt == RoofType::Mansard) && rh >= 2.2f) {
        for (float dx : {-hw * 0.48f, hw * 0.48f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{dx - 0.95f, h + 0.15f, hd - 1.3f},
                Vec3{dx + 0.95f, h + 1.65f, hd + 0.1f},
                mainWall);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{dx - 1.1f, h + 1.65f, hd - 1.45f},
                Vec3{dx + 1.1f, h + 2.15f, hd + 0.25f},
                metalRoof);
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{dx, h + 0.92f, hd + 0.12f},
                0.95f, 1.1f,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                wt, gt);
        }
    }

    // 6. Exterior Brick Hearth Chimney (-X)
    float chimW = 1.6f;
    float chimD = 1.1f;
    float chimH = h + rh + 0.9f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - chimD, 0.0f, -chimW * 0.5f},
        Vec3{-hw, chimH, chimW * 0.5f},
        brickMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - chimD - 0.10f, chimH, -chimW * 0.5f - 0.10f},
        Vec3{-hw + 0.05f, chimH + 0.18f, chimW * 0.5f + 0.10f},
        stoneMat);
}

} // namespace procedural::buildings
