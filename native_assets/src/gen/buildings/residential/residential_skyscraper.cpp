#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateResidentialSkyscraper(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 12.0f, 22.0f);
    float d = res_scale::depth(params_, 12.0f, 18.0f);
    float flH = res_scale::floorHeight(params_, 3.2f);
    int stories = res_scale::stories(params_, 4, 30);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat      = res_scale::wallMat(params_, MaterialSlot::WallConcrete);
    uint32_t concMat      = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    GlassType gt          = res_scale::glassType(params_, GlassType::ClearFloat);
    uint32_t glassMat     = BuildingComponents::getGlassMaterial(gt);
    uint32_t frameMat     = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat     = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t balcGlassMat = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t balcMetalMat = static_cast<uint32_t>(MaterialSlot::BalconyMetal);
    uint32_t poolMat      = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    WindowType wt         = res_scale::windowType(params_, WindowType::PictureGlass);
    float balcProj        = res_scale::balconyDepth(params_, 1.5f);
    bool wantBalconies    = res_scale::showBalconies(params_);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);

    // 1. Foundation plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.0f, -0.6f, -hd - 1.0f},
        Vec3{hw + 1.0f, 0.0f, hd + 1.0f},
        foundMat);

    // 2. Double-height Grand Concierge Lobby (or single height if low story count)
    int lobbyStories = (stories >= 6) ? 2 : 1;
    float lobbyH = flH * static_cast<float>(lobbyStories);

    // Core structural shear wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.35f, 0.0f, -hd * 0.35f},
        Vec3{hw * 0.35f, h, hd * 0.35f},
        concMat);

    // Grand lobby glass curtain wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{hw, lobbyH, hd},
        glassMat);

    // Lobby perimeter pilotis columns
    for (float cx : {-hw + 0.6f, -hw * 0.3f, hw * 0.3f, hw - 0.6f}) {
        for (float cz : {-hd + 0.6f, hd - 0.6f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{cx, 0.0f, cz},
                Vec3{cx, lobbyH, cz},
                0.35f, 0.35f, 12, frameMat, true, true);
        }
    }

    // Scaled Lobby Entry Door & optional canopy
    bool showCanopy = params_.useExtendedParams ? params_.hasEntranceCanopy : true;
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.0f, hd + 0.02f}, lobbyH, w * 0.6f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::RevolvingDoor, 2.6f, std::min(3.2f, lobbyH - 0.25f), showCanopy);

    if (showCanopy) {
        float canW = std::min(7.5f, w * 0.55f);
        float canD = 3.2f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-canW * 0.5f, lobbyH * 0.68f, hd},
            Vec3{canW * 0.5f, lobbyH * 0.75f, hd + canD},
            frameMat);
        for (float side : {-1.0f, 1.0f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{side * canW * 0.42f, lobbyH * 0.72f, hd + canD - 0.3f},
                Vec3{side * canW * 0.42f, lobbyH * 0.96f, hd + 0.1f},
                0.035f, 0.035f, 8, frameMat, true, true);
        }
    }

    // 3. Main Tower Shaft & Setback Penthouse Tiers
    int setbackStartStory = std::max(lobbyStories + 1, stories - std::max(1, stories / 5));

    for (int story = lobbyStories; story < stories; ++story) {
        bool isPenthouse = (story >= setbackStartStory && stories >= 6);
        float curHw = isPenthouse ? hw * 0.78f : hw;
        float curHd = isPenthouse ? hd * 0.78f : hd;
        float y0 = story * flH;
        float y1 = (story + 1) * flH;

        // Floor concrete spandrel slab
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-curHw - 0.15f, y0 - 0.20f, -curHd - 0.15f},
            Vec3{curHw + 0.15f, y0 + 0.05f, curHd + 0.15f},
            concMat);

        // Tower floor volume
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-curHw, y0, -curHd},
            Vec3{curHw, y1, curHd},
            wallMat);

        // Scaled windows on front, back, left, right facades
        int defCols = std::max(2, static_cast<int>((curHw * 2.0f) / 3.5f));
        int sideCols = std::max(2, static_cast<int>((curHd * 2.0f) / 3.5f));
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, curHd + 0.02f}, curHw * 2.0f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            defCols, wt, gt, 1.8f, flH * 0.66f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -curHd - 0.02f}, curHw * 2.0f, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            defCols, wt, gt, 1.8f, flH * 0.66f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-curHw - 0.02f, 0.0f, 0.0f}, curHd * 2.0f, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            sideCols, wt, gt, 1.8f, flH * 0.66f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{curHw + 0.02f, 0.0f, 0.0f}, curHd * 2.0f, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            sideCols, wt, gt, 1.8f, flH * 0.66f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Cantilevered wrap-around glass balconies on non-penthouse levels
        if (wantBalconies && !isPenthouse) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-curHw * 0.90f, y0, curHd},
                Vec3{curHw * 0.90f, y0 + 0.14f, curHd + balcProj},
                concMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-curHw * 0.90f, y0 + 0.14f, curHd + balcProj - 0.04f},
                Vec3{curHw * 0.90f, y0 + 1.12f, curHd + balcProj},
                balcGlassMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-curHw * 0.92f, y0 + 1.10f, curHd + balcProj - 0.06f},
                Vec3{curHw * 0.92f, y0 + 1.18f, curHd + balcProj + 0.04f},
                balcMetalMat);
            for (float side : {-1.0f, 1.0f}) {
                float bx = side * (curHw * 0.90f);
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{bx - 0.02f, y0 + 0.14f, curHd},
                    Vec3{bx + 0.02f, y0 + 1.12f, curHd + balcProj},
                    balcGlassMat);
            }
        }

        // Setback terrace wind barrier
        if (story == setbackStartStory && isPenthouse) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw * 0.95f, y0, -hd * 0.95f},
                Vec3{hw * 0.95f, y0 + 0.18f, hd * 0.95f},
                concMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw * 0.95f, y0 + 0.18f, hd * 0.95f - 0.05f},
                Vec3{hw * 0.95f, y0 + 1.4f, hd * 0.95f},
                balcGlassMat);
        }
    }

    float topHw = (stories >= 6) ? hw * 0.78f : hw;
    float topHd = (stories >= 6) ? hd * 0.78f : hd;
    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, topHw * 2.0f, topHd * 2.0f, h, 0.35f);
    }

    // 4. Adaptive roof (FlatParapet / Pyramid / Mansard / etc.) + optional rooftop pool & crown
    RoofType rt = res_scale::roofType(params_, RoofType::FlatParapet);
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        topHw * 2.0f, topHd * 2.0f, h,
        RoofType::FlatParapet, 2.4f, 0.35f, false);

    if (rt == RoofType::FlatParapet || rt == RoofType::NoneFlat) {
        // Sky lounge infinity pool basin (scaled safely inside top deck)
        if (topHw * 2.0f >= 10.0f && topHd * 2.0f >= 10.0f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-topHw * 0.65f, h + 0.10f, 0.5f},
                Vec3{topHw * 0.65f, h + 0.85f, topHd - 0.6f},
                poolMat);
        }
        float crownH = std::clamp(flH * 1.1f, 2.8f, 4.4f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-topHw * 0.42f, h + 0.22f, -topHd * 0.42f},
            Vec3{topHw * 0.42f, h + 0.22f + crownH, topHd * 0.42f},
            glassMat);
        float finStep = std::max(1.2f, topHw * 0.28f);
        for (float lx = -topHw * 0.50f; lx <= topHw * 0.50f; lx += finStep) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - 0.08f, h + crownH, -topHd * 0.50f},
                Vec3{lx + 0.08f, h + crownH + 1.1f, topHd * 0.50f},
                frameMat);
        }
    }
}

} // namespace procedural::buildings
