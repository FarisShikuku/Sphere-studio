#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateMediterraneanVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 12.0f, 20.0f);
    float d = res_scale::depth(params_, 10.0f, 14.0f);
    float flH = res_scale::floorHeight(params_, 3.3f);
    int stories = res_scale::stories(params_, 1, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t stuccoWall  = res_scale::wallMat(params_, MaterialSlot::WallLimestone);
    uint32_t terracotta  = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t roofTile    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    WindowType wt        = res_scale::windowType(params_, WindowType::ArchedHistoric);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, true);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasLoggia       = res_scale::showBalconies(params_);

    // 1. Raised Travertine Terrace Foundation Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, -0.35f, -hd - 0.6f},
        Vec3{hw + 0.6f, 0.22f, hd + 1.8f},
        foundMat);

    // 2. Main U-Shaped Courtyard Stucco Estate Massing
    float wingW = std::clamp(w * 0.28f, 3.6f, 6.2f);
    float wingDepth = std::clamp(d * 0.55f, 5.0f, d);
    // Main central villa wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + wingW * 0.6f, 0.22f, -hd},
        Vec3{hw - wingW * 0.6f, 0.22f + h, hd * 0.72f},
        stuccoWall);
    // Left & Right Pavilion Wings projecting forward
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.22f, -hd},
        Vec3{-hw + wingW, 0.22f + h, hd},
        stuccoWall);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - wingW, 0.22f, -hd},
        Vec3{hw, 0.22f + h, hd},
        stuccoWall);

    // Iconic Mediterranean Bell-Cote Tower on Left Wing
    if (stories >= 2) {
        float tw = std::min(wingW, 4.2f);
        float towerH = 0.22f + h + 2.2f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.1f, 0.22f + h, hd - tw},
            Vec3{-hw + tw - 0.1f, towerH, hd},
            stuccoWall);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.2f, towerH, hd - tw - 0.2f},
            Vec3{-hw + tw + 0.2f, towerH + 0.85f, hd + 0.2f},
            terracotta);
    }

    // 3. Central Colonnade Arcade Loggia connecting Left & Right Wings
    float loggiaSpan = std::max(3.0f, w - 2.0f * wingW);
    float loggiaZ0 = hd * 0.72f;
    float loggiaZ1 = hd;
    if (hasLoggia && loggiaSpan >= 2.8f) {
        int arches = std::max(3, static_cast<int>(loggiaSpan / 2.2f));
        for (int a = 0; a <= arches; ++a) {
            float ax = -loggiaSpan * 0.5f + a * (loggiaSpan / arches);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ax, 0.22f, loggiaZ1 - 0.25f},
                Vec3{ax, flH, loggiaZ1 - 0.25f},
                0.18f, 0.16f, 12, stuccoWall, true, true);
        }
        // Arcade cornice lintel & upper loggia terrace floor
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-loggiaSpan * 0.5f - 0.1f, flH - 0.28f, loggiaZ0},
            Vec3{loggiaSpan * 0.5f + 0.1f, flH + 0.15f, loggiaZ1},
            stuccoWall);
        if (stories >= 2) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{0.0f, flH, loggiaZ1},
                loggiaSpan - 0.4f, res_scale::balconyDepth(params_, 1.2f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::WroughtIron);
        }
    }

    // 4. Grand Arched Entry & Scaled Windows
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, 0.22f, loggiaZ0 + 0.02f}, flH - 0.22f, loggiaSpan,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::ArchedWooden, 1.55f, 2.45f, false);

    for (int s = 0; s < stories; ++s) {
        float y0 = 0.22f + s * flH;
        // Center wing upper windows
        if (s > 0) {
            res_scale::appendScaledWindowRow(
                outVerts, outIndices, params_,
                Vec3{0.0f, 0.0f, loggiaZ0 + 0.02f}, loggiaSpan, y0, flH,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                3, wt, gt, 1.25f, 1.65f, 0.65f,
                0.0f, 0.0f, wantShutters, wantSunshades);
        }
        // Left & Right front pavilion windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw + wingW * 0.5f, 0.0f, hd + 0.02f}, wingW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 1.3f, 1.7f, 0.70f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw - wingW * 0.5f, 0.0f, hd + 0.02f}, wingW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 1.3f, 1.7f, 0.70f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        // Side & Rear windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.35f, 1.6f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, wingDepth, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.25f, 1.6f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, wingDepth, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.25f, 1.6f, 0.75f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, 0.22f + h, 0.40f);
    }

    // 5. Low-Pitched Terracotta Hip Roof across estate
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, 0.22f + h, RoofType::Hip, 2.4f, 0.75f, false);

    // Optional right-side terracotta Pergola Carport
    if (params_.hasGarage) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, 0.22f, -hd * 0.4f},
            Vec3{hw + 3.6f, flH - 0.2f, hd * 0.6f},
            timberMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw - 0.1f, flH - 0.2f, -hd * 0.45f},
            Vec3{hw + 3.8f, flH + 0.35f, hd * 0.65f},
            roofTile);
    }
}

} // namespace procedural::buildings
