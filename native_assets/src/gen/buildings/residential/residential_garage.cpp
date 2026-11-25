#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateResidentialGarage(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 6.0f, 10.5f);
    float d = res_scale::depth(params_, 5.5f, 8.0f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float flH = res_scale::floorHeight(params_, 3.1f);
    int stories = res_scale::stories(params_, 1, 2); // Supports 1-story garage or 2-story carriage loft
    float wallH = stories * flH;

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t mainWall    = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t brickMat    = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t woodDoor    = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    uint32_t glassMat    = BuildingComponents::getGlassMaterial(gt);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t evEmerald   = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    WindowType wt        = res_scale::windowType(params_, WindowType::DoubleHungSash);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);

    bool showCarport = params_.hasPorchOrBalcony;
    float carportW = showCarport ? std::clamp(w * 0.35f, 2.8f, 4.0f) : 0.0f;

    // 1. Concrete Driveway Apron & Garage Foundation Slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.0f, -0.35f, -hd - 0.6f},
        Vec3{ hw + carportW + 1.0f, 0.0f, hd + 4.2f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, 0.0f, -hd - 0.25f},
        Vec3{ hw + 0.25f, 0.22f, hd + 0.25f},
        foundMat);

    // 2. Main Garage Enclosure Walls
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.22f, -hd},
        Vec3{ hw, wallH,  hd},
        mainWall);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.04f, 0.22f, -hd - 0.04f},
        Vec3{ hw + 0.04f, 1.00f,  hd + 0.04f},
        brickMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.05f, wallH - 0.25f, -hd - 0.05f},
        Vec3{ hw + 0.05f, wallH + 0.02f,  hd + 0.05f},
        timberMat);

    // 3. Scaled Sectional Carriage Garage Doors on Front (+Z)
    int bayCount = (w >= 12.5f) ? 3 : ((w >= 7.8f) ? 2 : 1);
    float totalFrontMargin = 1.2f;
    float bayPier = 0.60f;
    float usableW = w - totalFrontMargin - (bayCount - 1) * bayPier;
    float doorW = std::max(2.2f, usableW / static_cast<float>(bayCount));
    float doorH = std::min(flH - 0.45f, 2.40f);
    float startX = -hw + totalFrontMargin * 0.5f;

    for (int i = 0; i < bayCount; ++i) {
        float x0 = startX + i * (doorW + bayPier);
        float x1 = x0 + doorW;

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{x0 - 0.09f, 0.22f, hd - 0.02f},
            Vec3{x1 + 0.09f, 0.22f + doorH + 0.11f, hd + 0.10f},
            timberMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{x0, 0.22f, hd},
            Vec3{x1, 0.22f + doorH, hd + 0.07f},
            woodDoor);

        for (int s = 1; s <= 3; ++s) {
            float ry = 0.22f + s * (doorH * 0.25f);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{x0 + 0.05f, ry - 0.03f, hd + 0.07f},
                Vec3{x1 - 0.05f, ry + 0.03f, hd + 0.09f},
                frameMat);
        }

        int panes = 4;
        float paneSpan = (doorW - 0.28f) / static_cast<float>(panes);
        float glassY0 = 0.22f + doorH - 0.50f;
        float glassY1 = 0.22f + doorH - 0.16f;
        for (int p = 0; p < panes; ++p) {
            float px0 = x0 + 0.14f + p * paneSpan + 0.05f;
            float px1 = x0 + 0.14f + (p + 1) * paneSpan - 0.05f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px0, glassY0, hd + 0.07f},
                Vec3{px1, glassY1, hd + 0.11f},
                glassMat);
        }

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{x0 - 0.26f, 1.50f, hd},
            Vec3{x0 - 0.13f, 1.90f, hd + 0.20f},
            blackAlum);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{x0 - 0.23f, 1.58f, hd + 0.20f},
            Vec3{x0 - 0.16f, 1.84f, hd + 0.24f},
            evEmerald);
    }

    // Optional upper carriage loft windows if 2 stories
    if (stories >= 2) {
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, flH, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            bayCount + 1, wt, gt, 1.25f, 1.4f, 0.8f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 4. Side Workshop Access Door (-X) & Side/Rear Windows
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-hw, 0.22f, 0.0f}, flH - 0.22f, d,
        Vec3{-1, 0, 0}, Vec3{0, 0, 1},
        DoorType::SingleModern, 1.05f, 2.15f, true);
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{-hw - 0.02f, 0.0f, -hd * 0.55f}, d * 0.42f, 0.22f, flH - 0.22f,
        Vec3{-1, 0, 0}, Vec3{0, 0, 1},
        1, wt, gt, 1.1f, 1.15f, 0.90f,
        0.0f, 0.0f, wantShutters, wantSunshades);
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.0f, -hd - 0.02f}, w, 0.22f, flH - 0.22f,
        Vec3{0, 0, -1}, Vec3{-1, 0, 0},
        2, wt, gt, 1.4f, 1.2f, 0.90f,
        0.0f, 0.0f, wantShutters, wantSunshades);

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, wallH, 0.30f);
    }

    // 5. Adaptive Roof
    RoofType rt = res_scale::roofType(params_, RoofType::Gable);
    float rh = res_scale::roofHeight(params_, std::clamp(w * 0.22f, 2.0f, 3.2f));
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, wallH, RoofType::Gable, rh, 0.55f, false);

    if (rt == RoofType::Gable && rh >= 1.8f) {
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{0.0f, wallH + rh * 0.42f, hd + 0.02f},
            1.1f, 0.95f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::RoundOculus, gt);
    }

    // 6. Optional Side Carport / Timber Pergola Lean-To (+X side)
    if (showCarport) {
        float cpX0 = hw;
        float cpX1 = hw + carportW;
        float cpZ0 = -hd + 0.4f;
        float cpZ1 =  hd - 0.4f;
        float cpH  = flH - 0.25f;
        for (float pz : {cpZ0 + 0.3f, 0.0f, cpZ1 - 0.3f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cpX1 - 0.26f, 0.0f, pz - 0.13f},
                Vec3{cpX1,         cpH,  pz + 0.13f},
                pergolaMat);
        }
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cpX0, cpH - 0.16f, cpZ0},
            Vec3{cpX1 + 0.22f, cpH + 0.12f, cpZ1},
            pergolaMat);
    }
}

} // namespace procedural::buildings
