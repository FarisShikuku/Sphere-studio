#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateGreenRoofEcoVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.5f, 16.5f);
    float d = res_scale::depth(params_, 7.0f, 11.5f);
    float flH = res_scale::floorHeight(params_, 3.25f);
    int stories = res_scale::stories(params_, 1, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallCore    = res_scale::wallMat(params_, MaterialSlot::WallLimestone);
    uint32_t timberSiding= static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t pergolaWood = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t greenTurf   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t plinthMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt         = res_scale::glassType(params_, GlassType::LowEEmerald);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, true);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasBalcony      = res_scale::showBalconies(params_);

    // 1. Landscaped Earth Plinth + Built-In Planter Box Strip
    float plinthY = 0.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.8f, -0.35f, -hd - 0.6f},
        Vec3{hw + 0.8f, plinthY, hd + 1.6f},
        plinthMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, plinthY, hd + 0.85f},
        Vec3{-1.8f, plinthY + 0.45f, hd + 1.45f},
        planterMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.42f, plinthY + 0.45f, hd + 0.92f},
        Vec3{-1.88f, plinthY + 0.60f, hd + 1.38f},
        greenTurf);

    // 2. Stepped Lower & Upper Eco-Villa Volumes
    float lowerW = w;
    float upperW = (stories >= 2) ? w * 0.72f : w;
    if (stories == 1) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw, plinthY, -hd},
            Vec3{hw, plinthY + flH, hd},
            wallCore);
    } else {
        // Ground storey full footprint
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw, plinthY, -hd},
            Vec3{hw, plinthY + flH, hd},
            wallCore);
        // Setback upper storeys with warm cedar cladding -> creates lower green-roof terrace!
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw, plinthY + flH, -hd},
            Vec3{-hw + upperW, plinthY + h, hd - 0.6f},
            timberSiding);
        // First-floor planted green roof terrace on the right setback roof
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + upperW + 0.05f, plinthY + flH, -hd + 0.2f},
            Vec3{hw - 0.15f, plinthY + flH + 0.22f, hd - 0.2f},
            planterMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + upperW + 0.2f, plinthY + flH + 0.22f, -hd + 0.35f},
            Vec3{hw - 0.3f, plinthY + flH + 0.34f, hd - 0.35f},
            greenTurf);
    }

    // Timber Pergola Shade Canopy over entry terrace
    if (params_.useExtendedParams ? params_.hasEntranceCanopy : true) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-2.2f, plinthY + flH - 0.15f, hd},
            Vec3{2.2f, plinthY + flH - 0.02f, hd + 1.55f},
            pergolaWood);
        for (int b = -3; b <= 3; ++b) {
            float bx = b * 0.62f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx - 0.05f, plinthY + flH - 0.02f, hd + 0.05f},
                Vec3{bx + 0.05f, plinthY + flH + 0.12f, hd + 1.52f},
                pergolaWood);
        }
    }

    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, plinthY, hd + 0.02f}, flH, lowerW,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.75f, 2.35f, false);

    // 3. Solar Low-E Windows with Sunshades
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float curW = (s == 0) ? lowerW : upperW;
        float cx = (s == 0) ? 0.0f : (-hw + upperW * 0.5f);
        float frontZ = (s == 0) ? hd : (hd - 0.6f);
        float exMin = (s == 0) ? -1.45f : 0.0f;
        float exMax = (s == 0) ? 1.45f : 0.0f;

        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{cx, 0.0f, frontZ + 0.02f}, curW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 1.65f, flH * 0.70f, 0.40f,
            exMin, exMax, wantShutters, wantSunshades);

        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{cx, 0.0f, -hd - 0.02f}, curW, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.65f, flH * 0.68f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d - (s > 0 ? 0.6f : 0.0f), y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.50f, flH * 0.66f, 0.50f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        if (hasBalcony && s == 1) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{cx, y0, frontZ},
                curW * 0.55f, res_scale::balconyDepth(params_, 1.3f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::CantileveredGlass);
        }
    }

    // 4. Optional Integrated Eco Carport / Garage (+X side)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.34f, 3.8f, 6.0f);
        float gd = std::clamp(d * 0.58f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.45f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.45f + gd},
            timberSiding);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.45f + gd + 0.02f},
            std::min(3.3f, gw - 0.65f), std::min(2.3f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.45f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::GreenRoofTerrace, 0.65f, 0.0f);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, upperW, d, plinthY + h, 0.25f);
    }

    // 5. Lush Planted Green Terrace Flat Roof
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        upperW, (stories >= 2) ? (d - 0.6f) : d, plinthY + h,
        RoofType::GreenRoofTerrace, 0.75f, 0.45f, false);
}

} // namespace procedural::buildings
