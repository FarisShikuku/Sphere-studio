#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateJapaneseMachiya(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Traditional narrow urban merchant townhouse (defaults 7.5m wide x 16m deep, 2 stories)
    float w = res_scale::width(params_, 5.0f, 7.5f);
    float d = res_scale::depth(params_, 9.0f, 16.0f);
    float flH = res_scale::floorHeight(params_, 2.85f);
    int stories = res_scale::stories(params_, 1, 3);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t plasterWall = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t darkCedar   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t roofTile    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::ModernCasement);
    GlassType gt         = res_scale::glassType(params_, GlassType::FrostedSatinPrivacy);
    bool wantSunshades   = res_scale::showSunshades(params_, false);
    bool wantCornice     = res_scale::showCornice(params_, true);

    // 1. Stone Threshold Foundation Plinth
    float plinthY = 0.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.20f, -0.30f, -hd - 0.20f},
        Vec3{hw + 0.20f, plinthY, hd + 0.20f},
        foundMat);

    // 2. Narrow Deep Timber-Post & Plaster Body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        plasterWall);

    // Exposed Machiya dark cedar corner & floor framing posts
    for (float cx : {-hw - 0.03f, hw - 0.09f}) {
        for (float cz : {-hd - 0.03f, 0.0f, hd - 0.09f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx, plinthY, cz},
                Vec3{cx + 0.12f, plinthY + h, cz + 0.12f},
                darkCedar);
        }
    }
    res_scale::appendBeltCourses(outVerts, outIndices, hw, hd, stories, flH, darkCedar);

    // 3. Traditional Projecting Tiled Hisashi Lower Eave Skirt (+Z street side)
    if (stories >= 2) {
        float hisashiY = plinthY + flH - 0.18f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.25f, hisashiY, hd},
            Vec3{hw + 0.25f, hisashiY + 0.35f, hd + 1.05f},
            roofTile);
    }

    // 4. Sliding Cedar Entrance & Koshi Vertical Wooden Lattice Privacy Screen (+Z)
    float doorX = -hw * 0.45f;
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{doorX, plinthY, hd + 0.02f}, flH, w * 0.5f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.4f, 2.15f, false);

    // Koshi lattice screen next to front sliding entrance
    float latticeX0 = 0.0f;
    float latticeX1 = hw - 0.35f;
    if (latticeX1 > latticeX0 + 0.8f) {
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{(latticeX0 + latticeX1) * 0.5f, plinthY + flH * 0.52f, hd + 0.02f},
            (latticeX1 - latticeX0) * 0.85f, flH * 0.62f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0}, wt, gt);
        for (float lx = latticeX0; lx <= latticeX1; lx += 0.16f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - 0.02f, plinthY + 0.35f, hd + 0.04f},
                Vec3{lx + 0.02f, plinthY + flH - 0.35f, hd + 0.10f},
                darkCedar);
        }
    }

    // Upper floor Mushi-ko-mado Lattice Windows & Rear Tsubo Garden Openings
    for (int s = 1; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            3, wt, gt, 1.35f, flH * 0.45f, 0.90f,
            0.0f, 0.0f, false, wantSunshades);

        if (params_.hasPorchOrBalcony) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{0.0f, y0, hd},
                w * 0.82f, res_scale::balconyDepth(params_, 0.95f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::WroughtIron);
        }
    }

    // Rear garden windows on all floors (-Z)
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            2, wt, gt, 1.5f, flH * 0.62f, 0.50f,
            0.0f, 0.0f, false, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.15f, flH * 0.45f, 1.0f,
            0.0f, 0.0f, false, wantSunshades);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, plinthY + h, 0.40f);
    }

    // 5. Traditional Deep Overhanging Kawara Tile Roof
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, plinthY + h, RoofType::Hip, 2.2f, 0.85f, false);
}

} // namespace procedural::buildings
