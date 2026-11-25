#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateModernDuplex(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 12.0f, 18.0f);
    float d = res_scale::depth(params_, 9.0f, 13.0f);
    float flH = res_scale::floorHeight(params_, 3.3f);
    int stories = res_scale::stories(params_, 1, 6);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t primaryWall  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t charcoalZinc = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t cedarWood    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t foundMat     = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t balcGlass    = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t roofMat      = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    WindowType wt         = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt          = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool wantBalconies    = res_scale::showBalconies(params_);
    float balcProj        = res_scale::balconyDepth(params_, 1.2f);
    RoofType rt           = res_scale::roofType(params_, RoofType::NoneFlat);

    // 1. Foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.3f, -0.4f, -hd - 0.3f},
        Vec3{hw + 0.3f, 0.0f, hd + 0.3f},
        foundMat);

    // Center vertical party wall dividing Unit A (left) and Unit B (right)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.15f, 0.0f, -hd},
        Vec3{0.15f, h + 0.45f, hd + 0.3f},
        charcoalZinc);

    for (int u = 0; u < 2; ++u) {
        float uX0 = (u == 0) ? -hw : 0.15f;
        float uX1 = (u == 0) ? -0.15f : hw;
        float uMidX = (uX0 + uX1) * 0.5f;
        float uW = uX1 - uX0;

        // Ground Floor: optional integrated garage + sheltered entry portal
        if (params_.hasGarage) {
            float garW = uW * 0.56f;
            float garX = (u == 0) ? (uX0 + garW * 0.5f) : (uX1 - garW * 0.5f);
            float entW = uW - garW;
            float entX = (u == 0) ? (uX1 - entW * 0.5f) : (uX0 + entW * 0.5f);

            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{garX - garW * 0.5f, 0.0f, -hd},
                Vec3{garX + garW * 0.5f, flH, hd},
                charcoalZinc);

            float gDoorW = std::clamp(garW * 0.82f, 2.2f, 4.5f);
            float gDoorH = std::min(flH - 0.25f, 2.45f);
            BuildingComponents::generateDoor(outVerts, outIndices,
                Vec3{garX, 0.0f, hd + 0.02f},
                gDoorW, gDoorH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                DoorType::IndustrialRollUp, false);

            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{entX - entW * 0.5f, 0.0f, -hd},
                Vec3{entX + entW * 0.5f, flH, hd - 0.75f},
                cedarWood);

            res_scale::appendScaledDoor(
                outVerts, outIndices, params_,
                Vec3{entX, 0.0f, hd - 0.73f}, flH, entW,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                DoorType::SingleModern, 1.15f, 2.25f, true);
        } else {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{uX0, 0.0f, -hd},
                Vec3{uX1, flH, hd},
                primaryWall);
            res_scale::appendScaledDoor(
                outVerts, outIndices, params_,
                Vec3{uMidX, 0.0f, hd + 0.02f}, flH, uW,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                DoorType::SingleModern, 1.25f, 2.25f, true);
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{uMidX, 0.0f, hd + 0.02f}, uW, 0.0f, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                2, wt, gt, 1.3f, 1.6f, 0.75f,
                -1.0f, 1.0f, wantShutters, wantSunshades);
        }

        // Upper Floors (Story 1..stories-1)
        for (int s = 1; s < stories; ++s) {
            float y0 = s * flH;
            float y1 = (s + 1) * flH;
            bool isTopSetback = (s == stories - 1 && stories >= 3);
            float terraceD = isTopSetback ? std::min(3.4f, d * 0.28f) : 0.0f;
            float boxProj = (!isTopSetback && s == 1) ? ((u == 0) ? 0.9f : 0.35f) : 0.0f;

            float z0 = (isTopSetback && u == 0) ? (-hd + terraceD) : -hd;
            float z1 = (isTopSetback && u == 1) ? (hd - terraceD) : (hd + boxProj);
            uint32_t floorMat = (s % 2 == 1)
                ? ((u == 0) ? primaryWall : cedarWood)
                : ((u == 0) ? charcoalZinc : primaryWall);

            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{uX0 + 0.05f, y0, z0},
                Vec3{uX1 - 0.05f, y1, z1},
                floorMat);

            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{uMidX, 0.0f, z1 + 0.02f}, uW - 0.2f, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                2, wt, gt, std::min(2.2f, uW * 0.35f), flH * 0.65f, 0.5f,
                0.0f, 0.0f, wantShutters, wantSunshades);

            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{uMidX, 0.0f, z0 - 0.02f}, uW - 0.2f, y0, flH,
                Vec3{0, 0, -1}, Vec3{-1, 0, 0},
                2, wt, gt, 1.5f, flH * 0.58f, 0.7f,
                0.0f, 0.0f, wantShutters, wantSunshades);

            if (wantBalconies && !isTopSetback) {
                BuildingComponents::generateBalcony(
                    outVerts, outIndices,
                    Vec3{uMidX, y0, z1},
                    uW * 0.68f, balcProj,
                    Vec3{0, 0, 1}, Vec3{1, 0, 0},
                    BalconyType::CantileveredGlass);
            }
        }

        // Rooftop terrace deck & pergola when flat/terrace roof is used
        if (rt == RoofType::NoneFlat || rt == RoofType::FlatParapet) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{uX0 + 0.05f, h, -hd},
                Vec3{uX1 - 0.05f, h + 0.18f, hd},
                roofMat);
            if (wantBalconies) {
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{uX0 + 0.15f, h + 0.18f, hd - 0.04f},
                    Vec3{uX1 - 0.15f, h + 1.12f, hd},
                    balcGlass);
                float pergW = uW * 0.72f;
                for (float pz = -hd * 0.55f; pz <= hd * 0.55f; pz += 1.6f) {
                    BuildingComponents::appendBox(outVerts, outIndices,
                        Vec3{uMidX - pergW * 0.5f, h + 2.35f, pz - 0.07f},
                        Vec3{uMidX + pergW * 0.5f, h + 2.48f, pz + 0.07f},
                        cedarWood);
                }
            }
        }
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, h, 0.30f);
    }

    if (rt != RoofType::NoneFlat) {
        res_scale::appendAdaptiveRoof(
            outVerts, outIndices, params_,
            w, d, h, RoofType::FlatParapet, 2.6f, 0.5f, false);
    }
}

} // namespace procedural::buildings
