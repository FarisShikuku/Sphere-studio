#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateHipArchedBungalow(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.0f, 16.5f);
    float d = res_scale::depth(params_, 8.0f, 11.5f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 1, 3);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t creamStucco  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t stoneClad    = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t whiteFascia  = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t darkCharcoal = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t metalHip     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasPorch         = res_scale::showBalconies(params_);

    // 1. Dark Plinth Base & Paved Front Courtyard Apron
    float plinthY = 0.42f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.35f, -0.35f, -hd - 0.35f},
        Vec3{hw + 0.35f, plinthY, hd + 1.65f},
        darkCharcoal);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.55f, -0.35f, hd + 0.4f},
        Vec3{hw * 0.55f, plinthY - 0.05f, hd + 2.2f},
        stoneClad);

    // 2. Symmetrical Main Cream Stucco Volume & Side Stone-Clad Bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        creamStucco);

    // Rustic Stone-Block Clad Left & Right Projecting Wing Piers (matching image 4)
    float sideBayW = w * 0.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.06f, plinthY, -hd - 0.04f},
        Vec3{-hw + sideBayW, plinthY + h, hd + 0.15f},
        stoneClad);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - sideBayW, plinthY, -hd - 0.04f},
        Vec3{hw + 0.06f, plinthY + h, hd + 0.15f},
        stoneClad);

    // Crisp Horizontal Dark Accent Soffit Band under Eaves
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.10f, plinthY + h - 0.42f, -hd - 0.08f},
        Vec3{hw + 0.10f, plinthY + h - 0.30f, hd + 0.75f},
        darkCharcoal);

    // 3. Central Twin-Arch Veranda Porch with Paired White Columns & Balusters
    float porchW = w * 0.46f;
    float porchZ = hd + 0.72f;
    if (hasPorch) {
        // Porch slab floor
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-porchW * 0.5f, plinthY - 0.12f, hd},
            Vec3{porchW * 0.5f, plinthY, porchZ},
            whiteFascia);
        // Three pier pedestals + paired white porch columns
        float colXs[3] = {-porchW * 0.45f, -0.25f, porchW * 0.45f};
        for (float cx : colXs) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - 0.26f, plinthY, porchZ - 0.26f},
                Vec3{cx + 0.26f, plinthY + 0.95f, porchZ + 0.12f},
                creamStucco);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{cx - 0.10f, plinthY + 0.95f, porchZ - 0.05f},
                Vec3{cx - 0.10f, plinthY + flH - 0.35f, porchZ - 0.05f},
                0.08f, 0.07f, 10, whiteFascia, true, true);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{cx + 0.10f, plinthY + 0.95f, porchZ - 0.05f},
                Vec3{cx + 0.10f, plinthY + flH - 0.35f, porchZ - 0.05f},
                0.08f, 0.07f, 10, whiteFascia, true, true);
        }
        // Veranda Baluster Railings in Left & Right Portico Bays
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{0.0f, plinthY, hd},
            porchW * 0.88f, res_scale::balconyDepth(params_, 0.70f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::WroughtIron);
    }

    // Modern Sleek Dark Paneled Front Door inside Left Portico Bay
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-porchW * 0.24f, plinthY, hd + 0.03f}, flH, porchW * 0.5f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.30f, 2.30f, false);

    // 4. Black-Mullioned Multi-Pane Grid Windows Across Wings & Center Veranda
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        // Left & Right Stone Wing Large Front Windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw + sideBayW * 0.5f, 0.0f, hd + 0.17f}, sideBayW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.25f, flH * 0.62f, 0.58f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw - sideBayW * 0.5f, 0.0f, hd + 0.17f}, sideBayW, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.25f, flH * 0.62f, 0.58f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        // Center Right Veranda Window
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{porchW * 0.22f, 0.0f, hd + 0.03f}, porchW * 0.45f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            1, wt, gt, 2.15f, flH * 0.64f, 0.55f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Rear & Side facades
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.75f, flH * 0.62f, 0.60f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.08f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.65f, flH * 0.62f, 0.60f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 5. Optional Attached Matching Side Garage (+X wing)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.30f, 3.6f, 5.4f);
        float gd = std::clamp(d * 0.56f, 5.2f, 6.6f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.35f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.35f + gd},
            creamStucco);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.35f + gd + 0.02f},
            std::min(3.1f, gw - 0.6f), std::min(2.25f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.35f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::Hip, 1.8f, 0.55f);
    }

    float topY = plinthY + h;
    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w + 0.2f, d + 0.4f, topY, 0.38f);
    }

    // 6. Signature High Charcoal Hip Roof + Stepped Front Porch Hip & Twin Gablet Vents
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w + 0.2f, d + 0.4f, topY, RoofType::Hip, 3.35f, 0.75f, false);

    // Front Center Portico Lower Hip Roof Cap + Double Gablet Dormers
    RoofGenParams porchCap;
    porchCap.type = RoofType::Gable;
    porchCap.baseWidth = porchW + 0.8f;
    porchCap.baseDepth = 3.2f;
    porchCap.baseTopY = topY;
    porchCap.roofHeight = 1.45f;
    porchCap.overhang = 0.45f;
    porchCap.showRooftopClutter = false;
    RoofGen::generate(outVerts, outIndices, porchCap);

    // Optional Satellite Dish on Upper Ridge (matching image 4)
    if (res_scale::showClutter(params_, true)) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.0f, topY + 2.8f, 0.0f},
            Vec3{0.0f, topY + 3.4f, 0.35f},
            0.50f, 0.10f, 12, metalHip, true, true);
    }
}

} // namespace procedural::buildings
