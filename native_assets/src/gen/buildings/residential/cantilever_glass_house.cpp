#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateCantileverGlassHouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 11.0f, 18.0f);
    float d = res_scale::depth(params_, 8.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.4f);
    int stories = res_scale::stories(params_, 2, 4);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t concWall    = res_scale::wallMat(params_, MaterialSlot::WallConcrete);
    uint32_t basaltCore  = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t cedarSoffit = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t steelBeam   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t poolMat     = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantSunshades   = res_scale::showSunshades(params_, true);
    bool wantBalconies   = res_scale::showBalconies(params_);
    float balcProj       = res_scale::balconyDepth(params_, 1.5f);

    // 1. Anchored Basalt Foundation Podium & Infinity Terrace Pool
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.85f, -0.45f, -hd - 0.5f},
        Vec3{hw * 0.85f, 0.0f, hd + 1.8f},
        foundMat);

    if (wantBalconies) {
        // Infinity pool slab under the cantilever on front-right plaza
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{0.0f, 0.0f, hd * 0.35f},
            Vec3{hw * 0.82f, 0.25f, hd + 1.7f},
            poolMat);
    }

    // 2. Ground Anchor Core Podium (narrower shifted left so right half cantilevers dramatically!)
    float coreX0 = -hw * 0.78f;
    float coreX1 = hw * 0.28f;
    float coreMidX = (coreX0 + coreX1) * 0.5f;
    float coreW = coreX1 - coreX0;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{coreX0, 0.0f, -hd * 0.85f},
        Vec3{coreX1, flH, hd * 0.75f},
        basaltCore);

    // Ground Floor Glass Entry & Optional Covered Garage Bay under Cantilever
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{coreMidX, 0.0f, hd * 0.75f + 0.02f}, flH, coreW,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.6f, 2.5f, true);

    if (params_.hasGarage) {
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{coreX0 + coreW * 0.24f, 0.0f, hd * 0.75f + 0.02f},
            std::min(3.4f, coreW * 0.40f), std::min(2.45f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
    }

    // 3. Dramatic Cantilevered Upper Glass Living Volumes (Stories 1..stories-1)
    float cantShiftX = hw * 0.28f;
    float topY = flH;
    for (int s = 1; s < stories; ++s) {
        float y0 = s * flH;
        float y1 = (s + 1) * flH;
        float shift = (s % 2 == 1) ? cantShiftX : 0.0f;
        float uX0 = -hw + shift * 0.45f;
        float uX1 = hw + shift;
        float uW = uX1 - uX0;
        float uMidX = (uX0 + uX1) * 0.5f;
        float uZ0 = -hd;
        float uZ1 = hd + ((s == 1) ? 0.85f : 0.0f);

        // Exposed steel cantilever I-beam soffit & cedar ceiling under overhang
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{uX0 - 0.1f, y0 - 0.22f, uZ0 - 0.1f},
            Vec3{uX1 + 0.1f, y0 + 0.06f, uZ1 + 0.1f},
            steelBeam);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{uX0, y0 - 0.24f, uZ0},
            Vec3{uX1, y0 - 0.20f, uZ1},
            cedarSoffit);

        // Upper panoramic glass/concrete slab shell
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{uX0, y0, uZ0},
            Vec3{uX1, y1, uZ1},
            concWall);

        // Floor-to-ceiling panoramic ribbon windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{uMidX, 0.0f, uZ1 + 0.02f}, uW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 2.2f, flH * 0.78f, 0.25f,
            0.0f, 0.0f, false, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{uMidX, 0.0f, uZ0 - 0.02f}, uW, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 2.0f, flH * 0.72f, 0.35f,
            0.0f, 0.0f, false, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{uX1 + 0.02f, 0.0f, (uZ0 + uZ1) * 0.5f}, uZ1 - uZ0, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 2.0f, flH * 0.78f, 0.25f,
            0.0f, 0.0f, false, wantSunshades);

        if (wantBalconies) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{uMidX, y0, uZ1},
                uW * 0.78f, balcProj,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::CantileveredGlass);
        }
        topY = y1;
    }

    // Diagonal structural steel tension strut under right cantilever
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{coreX1 - 0.3f, 0.2f, 0.0f},
        Vec3{hw + cantShiftX - 1.2f, flH - 0.2f, 0.0f},
        0.16f, 0.16f, 10, steelBeam, true, true);

    // 4. Adaptive Roof over upper cantilevered volume
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w + cantShiftX * 0.55f, d + 0.85f, topY, RoofType::NoneFlat, 1.4f, 0.70f, false);
}

} // namespace procedural::buildings
