#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateTropicalGableManor(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.0f, 16.5f);
    float d = res_scale::depth(params_, 8.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.25f);
    int stories = res_scale::stories(params_, 2, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t creamStucco  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t stoneBlock   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t ochreTrim    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t darkCharcoal = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t cedarSoffit  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t plinthStone  = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasBalconies     = res_scale::showBalconies(params_);

    // 1. Landscaped Driveway Apron & Raised Manor Stone Plinth
    float plinthY = 0.28f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, -0.35f, -hd - 0.5f},
        Vec3{hw + 0.6f, plinthY, hd + 1.3f},
        plinthStone);

    // 2. Main Manor Body & Asymmetrical Stepped Front Wings
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd - 0.5f},
        creamStucco);

    // Right Forward Projecting Bay Wing (+X)
    float rightWingW = w * 0.34f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - rightWingW, plinthY, hd - 0.5f},
        Vec3{hw, plinthY + h, hd + 0.6f},
        creamStucco);

    // Left Side Veranda / Guest Wing (-X)
    float leftWingW = w * 0.24f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.2f, plinthY, -hd * 0.5f},
        Vec3{-hw, plinthY + h - 0.35f, hd - 0.7f},
        creamStucco);

    // 3. Textured Stone Block Ashlar Piers & Ground-Floor Bands
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.05f, plinthY, -hd - 0.05f},
        Vec3{-hw + 0.95f, plinthY + flH, hd - 0.45f},
        stoneBlock);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.95f, plinthY, -hd - 0.05f},
        Vec3{hw + 0.05f, plinthY + h, hd + 0.65f},
        stoneBlock);
    // Center entry porch ochre support pier column (signature feature from image 1)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.28f, plinthY, hd + 0.35f},
        Vec3{0.28f, plinthY + h - 0.15f, hd + 0.92f},
        ochreTrim);
    // Warm ochre horizontal floor band trim
    res_scale::appendBeltCourses(outVerts, outIndices, hw, hd + 0.6f, stories, flH, ochreTrim);

    // 4. Double-Height Curved Stairwell Bay Tower (Left-Center Signature)
    float towerX = -hw + w * 0.32f;
    float towerZ = hd - 0.45f;
    float towerR = std::clamp(w * 0.10f, 1.25f, 1.85f);
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{towerX, plinthY, towerZ},
        Vec3{towerX, plinthY + h, towerZ},
        towerR, towerR, 14, creamStucco, true, true);
    // Tall double-height black-mullioned stairwell bay glass
    for (int s = 0; s < stories; ++s) {
        BuildingComponents::generateWindow(
            outVerts, outIndices,
            Vec3{towerX, plinthY + s * flH + flH * 0.52f, towerZ + towerR + 0.03f},
            towerR * 1.15f, flH * 0.78f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::MultiPaneFactory, gt);
    }

    // 5. Recessed Front Porch Entry + Decorative Upper Wrought-Iron Balconies
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{1.15f, plinthY, hd - 0.48f}, flH, rightWingW + 2.0f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.20f, 2.30f, true);

    if (hasBalconies && stories >= 2) {
        // Center upper covered balcony (beside ochre column pier)
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{0.85f, plinthY + flH, hd - 0.5f},
            w * 0.32f, res_scale::balconyDepth(params_, 1.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::WroughtIron);
        // Right side upper balcony
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{hw - rightWingW * 0.5f, plinthY + flH, hd + 0.6f},
            rightWingW * 0.75f, res_scale::balconyDepth(params_, 1.10f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::WroughtIron);
    }

    // 6. Scaled Multi-Pane Grid Windows Across Wings
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        // Main center & right wing front windows
        float exMin = (s == 0) ? -2.2f : 0.0f;
        float exMax = (s == 0) ? 2.2f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw - rightWingW * 0.5f, 0.0f, hd + 0.62f}, rightWingW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            2, wt, gt, 1.25f, flH * 0.62f, 0.55f,
            exMin, exMax, wantShutters, wantSunshades);
        // Rear & right facades
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.35f, flH * 0.62f, 0.60f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.20f, flH * 0.60f, 0.60f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 7. Optional Attached Stone-Clad Garage Bay (+X rear wing)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.30f, 3.6f, 5.4f);
        float gd = std::clamp(d * 0.55f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.35f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.35f + gd},
            stoneBlock);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.35f + gd + 0.02f},
            std::min(3.1f, gw - 0.6f), std::min(2.25f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.35f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::Hip, 1.6f, 0.5f);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, plinthY + h, 0.30f);
    }

    // 8. Multi-Tiered Dark Charcoal Gable Roof + Timber Soffits + Wing Gable Caps
    float topY = plinthY + h;
    // Cedar plank soffit eaves fascia under main roof
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.45f, topY - 0.10f, -hd - 0.45f},
        Vec3{hw + 0.45f, topY + 0.08f, hd + 0.85f},
        cedarSoffit);

    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, topY, RoofType::Gable, 2.85f, 0.70f, true);

    // Secondary Right Front Gable Wing Crown
    res_scale::appendWingRoof(
        outVerts, outIndices, params_,
        hw - rightWingW * 0.5f, hd - 1.65f,
        rightWingW + 0.6f, 4.5f, topY, RoofType::Gable, 2.0f, 0.55f);

    // Rooftop Solar PV Sloped Panel & Satellite Dish when clutter enabled
    if (res_scale::showClutter(params_, true)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-1.6f, topY + 1.0f, 0.6f},
            Vec3{1.4f, topY + 1.35f, 2.2f},
            darkCharcoal);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, topY + 1.4f, 0.0f},
            Vec3{0.0f, topY + 2.5f, 0.0f},
            0.08f, 0.08f, 8, stoneBlock, true, true);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, topY + 2.55f, 0.05f},
            Vec3{0.0f, topY + 2.75f, 0.40f},
            0.55f, 0.12f, 12, stoneBlock, true, true);
    }
}

} // namespace procedural::buildings
