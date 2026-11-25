#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateAfricanContemporaryGableVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.0f, 17.5f);
    float d = res_scale::depth(params_, 8.5f, 12.5f);
    float flH = res_scale::floorHeight(params_, 3.25f);
    int stories = res_scale::stories(params_, 2, 3);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t creamWall    = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t ashlarStone  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t ochreTrim    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t plinthStone  = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofCharcoal = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t solarGlass   = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasLoggias       = res_scale::showBalconies(params_);

    // 1. Stone Perimeter Plinth & Entry Walk
    float plinthY = 0.30f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.4f, -0.35f, -hd - 0.4f},
        Vec3{hw + 0.4f, plinthY, hd + 0.8f},
        plinthStone);

    // 2. Main 2-Storey Villa Volume + Dressed Ashlar Stone Quoin Corner Piers
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        creamWall);

    // Dressed stone masonry strips at outer corners & stairwell flanks
    float pierW = 1.15f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.04f, plinthY, hd - 0.5f},
        Vec3{-hw + pierW, plinthY + h, hd + 0.06f},
        ashlarStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - pierW, plinthY, hd - 0.5f},
        Vec3{hw + 0.04f, plinthY + h, hd + 0.06f},
        ashlarStone);

    // Warm Ochre Trim Horizontal Floor Bands & Column Trim
    res_scale::appendBeltCourses(outVerts, outIndices, hw, hd, stories, flH, ochreTrim);

    // 3. Signature Double-Height Projected Stairwell Window Tower (Left-Center Bay)
    float stairW = w * 0.22f;
    float stairCx = -hw * 0.42f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairCx - stairW * 0.5f - 0.35f, plinthY, hd},
        Vec3{stairCx + stairW * 0.5f + 0.35f, plinthY + h, hd + 0.55f},
        ashlarStone);
    BuildingComponents::generateWindow(
        outVerts, outIndices,
        Vec3{stairCx, plinthY + h * 0.48f, hd + 0.57f},
        stairW, h * 0.78f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::MultiPaneFactory, gt);

    // 4. Center & Right-Wing Upper Loggia Balconies Supported by Ochre Porch Columns
    if (hasLoggias && stories >= 2) {
        float y1 = plinthY + flH;
        // Center porch pillar & upper loggia balcony
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-0.22f, plinthY, hd + 0.55f},
            Vec3{0.22f, y1, hd + 1.05f},
            ochreTrim);
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{w * 0.06f, y1, hd},
            w * 0.34f, res_scale::balconyDepth(params_, 1.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::WroughtIron);
        // Right wing balcony
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{hw * 0.72f, y1, hd},
            w * 0.24f, res_scale::balconyDepth(params_, 1.15f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::WroughtIron);
    }

    // Main Front Entry Door
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{w * 0.06f, plinthY, hd + 0.02f}, flH, w * 0.36f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.35f, 2.35f, true);

    // 5. Multi-Pane Black Framed Windows on Front, Sides & Rear
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw * 0.42f, 0.0f, hd + 0.02f}, w * 0.44f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            2, wt, gt, 1.55f, flH * 0.62f, 0.65f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.60f, flH * 0.62f, 0.70f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            3, wt, gt, 1.50f, flH * 0.60f, 0.70f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.50f, flH * 0.60f, 0.70f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 6. Optional Attached Carport / Garage (+X side)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.30f, 3.6f, 5.4f);
        float gd = std::clamp(d * 0.56f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.4f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.4f + gd},
            creamWall);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.4f + gd + 0.02f},
            std::min(3.2f, gw - 0.6f), std::min(2.3f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.4f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::Hip, 1.45f, 0.45f);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, plinthY + h, 0.35f);
    }

    // 7. Nested Multi-Gable Dark Tile Roof + Timber Eave Accent + Solar & Dish
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, plinthY + h, RoofType::Gable, 3.4f, 0.75f, false);

    // Secondary Front Nested Gable Over Stairwell Bay
    res_scale::appendWingRoof(
        outVerts, outIndices, params_,
        stairCx, hd - 1.55f,
        stairW + 1.8f, 4.2f, plinthY + h, RoofType::Gable, 2.0f, 0.55f);

    if (res_scale::showClutter(params_, true)) {
        // Rooftop Solar PV Collector Array on Gable Slope
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{0.4f, plinthY + h + 1.25f, 1.0f},
            Vec3{3.6f, plinthY + h + 1.42f, 3.2f},
            solarGlass);
        // Roof Satellite Dish
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, plinthY + h + 3.2f, 0.0f},
            Vec3{0.0f, plinthY + h + 3.9f, 0.0f},
            0.06f, 0.06f, 8, roofCharcoal);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, plinthY + h + 3.9f, 0.0f},
            Vec3{0.0f, plinthY + h + 4.15f, 0.25f},
            0.52f, 0.10f, 12, static_cast<uint32_t>(MaterialSlot::Glass));
    }
}

} // namespace procedural::buildings
