#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateBauhausResidence(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 9.0f, 16.0f);
    float d = res_scale::depth(params_, 8.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 2, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t whiteStucco = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t steelPost   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t plinthMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t timberDeck  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    WindowType wt        = res_scale::windowType(params_, WindowType::RibbonStrip);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    bool wantShutters    = res_scale::showShutters(params_, false);
    bool wantSunshades   = res_scale::showSunshades(params_, true);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasTerrace      = res_scale::showBalconies(params_);

    // 1. Clean Bauhaus Terrazzo Plinth
    float plinthY = 0.20f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.4f, -0.35f, -hd - 0.4f},
        Vec3{hw + 0.4f, plinthY, hd + 1.0f},
        plinthMat);

    // 2. Asymmetric Main White Stucco Rectangular Volume + Curved Stair Drum (-X corner)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.6f, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        whiteStucco);

    // Iconic Bauhaus Cylindrical Stairwell Drum on -X/-Z-to-+Z transition
    float drumR = std::clamp(d * 0.24f, 1.9f, 2.9f);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{-hw + drumR * 0.65f, plinthY, -hd + drumR * 0.9f},
        Vec3{-hw + drumR * 0.65f, plinthY + h + 0.45f, -hd + drumR * 0.9f},
        drumR, drumR, 20, whiteStucco, true, true);

    // 3. Slender Steel Pilotis Supporting Cantilevered Front Upper Terrace Wing
    if (stories >= 2) {
        float overhangZ = hd + 1.45f;
        for (float px : {-hw + 2.2f, 0.0f, hw - 0.6f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{px, plinthY, overhangZ - 0.25f},
                Vec3{px, plinthY + flH, overhangZ - 0.25f},
                0.13f, 0.13f, 12, steelPost, true, true);
        }
        // Upper cantilevered Bauhaus slab extending forward over terrace
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 1.6f, plinthY + flH - 0.16f, hd},
            Vec3{hw, plinthY + flH + 0.14f, overhangZ},
            whiteStucco);
        if (hasTerrace) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{1.0f, plinthY + flH, hd},
                w * 0.72f, 1.45f,
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::WroughtIron);
        }
    }

    // 4. Bauhaus Recessed Steel Portal Door (+Z ground)
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-hw * 0.25f, plinthY, hd + 0.02f}, flH, w * 0.6f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, 1.65f, 2.35f, true);

    // 5. Horizontal Ribbon Strip Glazing on All Stories
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        float exMin = (s == 0) ? -1.4f : 0.0f;
        float exMax = (s == 0) ? 1.4f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.8f, 0.0f, hd + 0.02f}, w - 2.0f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 1.85f, flH * 0.44f, 0.92f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.8f, 0.0f, -hd - 0.02f}, w - 2.0f, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.85f, flH * 0.44f, 0.92f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.75f, flH * 0.44f, 0.92f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, plinthY + h, 0.20f);
    }

    // 6. Flat Parapet Roof + Bauhaus Solarium Pergola Bar
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, plinthY + h, RoofType::FlatParapet, 0.55f, 0.25f, false);

    if (hasTerrace) {
        float roofDeckY = plinthY + h + 0.45f;
        for (int b = 0; b < 5; ++b) {
            float bx = -hw * 0.35f + b * (w * 0.16f);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx - 0.06f, roofDeckY + 2.1f, -hd * 0.55f},
                Vec3{bx + 0.06f, roofDeckY + 2.25f, hd * 0.55f},
                timberDeck);
        }
    }
}

} // namespace procedural::buildings
