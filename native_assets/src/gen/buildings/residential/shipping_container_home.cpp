#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateShippingContainerHome(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 7.5f, 13.0f);
    float d = res_scale::depth(params_, 5.5f, 9.5f);
    float flH = res_scale::floorHeight(params_, 2.9f);
    int stories = res_scale::stories(params_, 1, 4);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t cortenMat   = res_scale::wallMat(params_, MaterialSlot::WallCorten);
    uint32_t gunmetalMat = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t steelMat    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t timberDeck  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t pierMat     = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt         = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasDeck         = res_scale::showBalconies(params_);

    // 1. Helical Steel Piers & Timber Patio Foundation
    float plinthY = 0.35f;
    for (int ix = 0; ix <= 2; ++ix) {
        float px = -hw + ix * hw;
        for (int iz = 0; iz <= 1; ++iz) {
            float pz = -hd + iz * d;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.18f, -0.30f, pz - 0.18f},
                Vec3{px + 0.18f, plinthY, pz + 0.18f},
                pierMat);
        }
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, plinthY - 0.14f, -hd - 0.4f},
        Vec3{hw + 0.6f, plinthY, hd + 1.4f},
        timberDeck);

    // 2. Stacked Offset ISO Shipping Container Modules (40ft + 20ft offsets)
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float y1 = y0 + flH;
        float xShift = (s % 2 == 1) ? 0.9f : 0.0f;
        float modW = (s % 2 == 1) ? (w * 0.86f) : w;
        float mMinX = -modW * 0.5f + xShift;
        float mMaxX = modW * 0.5f + xShift;
        float mMinZ = -hd + ((s % 2 == 1) ? 0.55f : 0.0f);
        float mMaxZ = hd - ((s % 2 == 1) ? 0.35f : 0.0f);
        float mD = mMaxZ - mMinZ;
        uint32_t boxMat = (s % 2 == 0) ? cortenMat : gunmetalMat;

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mMinX, y0, mMinZ},
            Vec3{mMaxX, y1, mMaxZ},
            boxMat);

        // Exposed steel corner castings & ISO frame rails
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mMinX - 0.04f, y0, mMinZ - 0.04f},
            Vec3{mMaxX + 0.04f, y0 + 0.12f, mMaxZ + 0.04f},
            steelMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mMinX - 0.04f, y1 - 0.12f, mMinZ - 0.04f},
            Vec3{mMaxX + 0.04f, y1, mMaxZ + 0.04f},
            steelMat);

        // Corrugated steel vertical ribs along front/back container sides
        int ribCount = std::max(6, static_cast<int>(modW / 0.85f));
        for (int r = 1; r < ribCount; ++r) {
            float rx = mMinX + (modW * r) / static_cast<float>(ribCount);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rx - 0.04f, y0 + 0.14f, mMaxZ},
                Vec3{rx + 0.04f, y1 - 0.14f, mMaxZ + 0.04f},
                boxMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rx - 0.04f, y0 + 0.14f, mMinZ - 0.04f},
                Vec3{rx + 0.04f, y1 - 0.14f, mMinZ},
                boxMat);
        }

        // Glazed cutout windows on front & back walls
        float exMin = (s == 0) ? -1.4f : 0.0f;
        float exMax = (s == 0) ? 1.4f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{(mMinX + mMaxX) * 0.5f, 0.0f, mMaxZ + 0.04f}, modW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            3, wt, gt, 1.85f, flH * 0.68f, 0.35f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{(mMinX + mMaxX) * 0.5f, 0.0f, mMinZ - 0.04f}, modW, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 1.75f, flH * 0.65f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Full-height glazed container end walls (+X and -X)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{mMaxX + 0.02f, 0.0f, (mMinZ + mMaxZ) * 0.5f}, mD, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, WindowType::CurtainWall, gt, 1.90f, flH * 0.78f, 0.25f,
            0.0f, 0.0f, false, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{mMinX - 0.02f, 0.0f, (mMinZ + mMaxZ) * 0.5f}, mD, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, WindowType::CurtainWall, gt, 1.90f, flH * 0.78f, 0.25f,
            0.0f, 0.0f, false, wantSunshades);

        if (hasDeck && s == 1) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{-hw * 0.35f, y0, mMaxZ},
                modW * 0.48f, res_scale::balconyDepth(params_, 1.25f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::WroughtIron);
        }
    }

    // 3. Entry Door + Industrial Steel Canopy
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, plinthY, hd + 0.04f}, flH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, 1.65f, 2.25f, true);

    // 4. Optional Attached Container Garage Bay (-X wing)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.34f, 3.6f, 5.5f);
        float gd = std::clamp(d * 0.65f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - gw, plinthY, -hd * 0.4f},
            Vec3{-hw, plinthY + flH, -hd * 0.4f + gd},
            steelMat);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{-hw - gw * 0.5f, plinthY, -hd * 0.4f + gd + 0.02f},
            std::min(3.1f, gw - 0.6f), std::min(2.2f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            -hw - gw * 0.5f, -hd * 0.4f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.45f, 0.0f);
    }

    float topY = plinthY + stories * flH;
    float topShift = ((stories - 1) % 2 == 1) ? 0.9f : 0.0f;
    float topW = ((stories - 1) % 2 == 1) ? (w * 0.86f) : w;
    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, topW, d, topY, 0.18f);
    }

    // 5. Flat Parapet Container Roof (plus rooftop deck option)
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        topW + std::abs(topShift) * 0.2f, d, topY,
        RoofType::FlatParapet, 0.55f, 0.20f, false);
}

} // namespace procedural::buildings
