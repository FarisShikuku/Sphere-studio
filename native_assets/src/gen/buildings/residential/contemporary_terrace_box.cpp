#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateContemporaryTerraceBox(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 9.0f, 15.5f);
    float d = res_scale::depth(params_, 8.0f, 11.5f);
    float flH = res_scale::floorHeight(params_, 3.25f);
    int stories = res_scale::stories(params_, 2, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite   = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t slateGrey    = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t darkSteel    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t planterStone = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t greenPlant   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t plinthMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, false);
    bool wantSunshades    = res_scale::showSunshades(params_, true);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasTerrace       = res_scale::showBalconies(params_);

    // 1. Plinth & Front Built-In Landscape Planter Trough
    float plinthY = 0.28f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.45f, -0.35f, -hd - 0.45f},
        Vec3{hw + 0.45f, plinthY, hd + 1.0f},
        plinthMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.16f, plinthY, hd + 0.10f},
        Vec3{w * 0.18f, plinthY + 0.42f, hd + 0.85f},
        planterStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.14f, plinthY + 0.42f, hd + 0.18f},
        Vec3{w * 0.16f, plinthY + 0.56f, hd + 0.77f},
        greenPlant);

    // 2. Primary White Stucco Main Residence Volume + Slate-Grey Accent Pier Spine
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + h, hd},
        crispWhite);

    // Central slate-grey architectural vertical tower spine separating front bays
    float spineMinX = -w * 0.24f;
    float spineMaxX = -w * 0.10f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{spineMinX, plinthY, hd - 0.2f},
        Vec3{spineMaxX, plinthY + h + 0.45f, hd + 0.35f},
        slateGrey);

    // Upper Story Projected White Cantilever Picture-Frame Balcony Box (Center & Left Wing)
    if (stories >= 2) {
        float uY0 = plinthY + flH;
        float uY1 = plinthY + 2 * flH;
        // Center boxed balcony frame
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{spineMaxX, uY0 - 0.10f, hd},
            Vec3{w * 0.16f, uY0 + 0.14f, hd + 0.85f},
            crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{spineMaxX, uY1 - 0.22f, hd},
            Vec3{w * 0.16f, uY1, hd + 0.85f},
            crispWhite);
        // Vertical Privacy Brise-Soleil Fins inside projected frame
        for (float fx = spineMaxX + 0.25f; fx < spineMaxX + 1.25f; fx += 0.34f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx - 0.05f, uY0 + 0.14f, hd + 0.72f},
                Vec3{fx + 0.05f, uY1 - 0.22f, hd + 0.83f},
                crispWhite);
        }

        // Left cantilevered balcony wing + dark vertical louver slats
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.95f, uY0 - 0.10f, -hd * 0.35f},
            Vec3{spineMinX, uY0 + 0.14f, hd + 0.35f},
            crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.95f, uY1 - 0.20f, -hd * 0.35f},
            Vec3{spineMinX, uY1, hd + 0.35f},
            crispWhite);
        for (float lx = -hw - 0.65f; lx < -hw + 1.15f; lx += 0.38f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - 0.04f, uY0 + 0.14f, hd + 0.24f},
                Vec3{lx + 0.04f, uY1 - 0.20f, hd + 0.34f},
                slateGrey);
        }
        if (hasTerrace) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{-hw * 0.62f, uY0 + 0.14f, hd + 0.35f},
                w * 0.28f, res_scale::balconyDepth(params_, 0.95f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::CantileveredGlass);
        }
    }

    // 3. Signature Full-Height 2-Storey Stairwell Curtain Glass Bay (Right Corner)
    float stairX = hw - w * 0.20f;
    float stairH = std::min(h - 0.4f, flH * 2.0f - 0.35f);
    BuildingComponents::generateWindow(
        outVerts, outIndices,
        Vec3{stairX, plinthY + stairH * 0.5f + 0.20f, hd + 0.03f},
        w * 0.26f, stairH,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::CurtainWall, gt);

    // Entry Door in Recessed Left Vestibule
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-hw * 0.55f, plinthY, hd + 0.02f}, flH, w * 0.35f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.30f, 2.30f, true);

    // 4. Facade Windows across floors
    for (int s = 0; s < stories; ++s) {
        float y0 = plinthY + s * flH;
        // Center-front bay windows (between spine and stairwell bay)
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.03f}, w * 0.38f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            2, wt, gt, 1.65f, flH * 0.66f, 0.38f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        // Rear & Right Side windows
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            3, wt, gt, 1.60f, flH * 0.65f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.45f, flH * 0.64f, 0.45f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 5. Optional Side Carport / Garage
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.32f, 3.6f, 5.5f);
        float gd = std::clamp(d * 0.55f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.4f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.4f + gd},
            slateGrey);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.4f + gd + 0.02f},
            std::min(3.2f, gw - 0.6f), std::min(2.3f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.4f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.5f, 0.0f);
    }

    // 6. Signature Rooftop Entertainment Deck, Concrete Pergola Frame, Lounge Box & Tanks
    float roofY = plinthY + h;
    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, roofY, 0.26f);
    }
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, roofY, RoofType::FlatParapet, 0.55f, 0.25f, false);

    // Rooftop Glass Guardrail Parapet + Concrete Pergola Portal
    if (hasTerrace) {
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{0.0f, roofY + 0.12f, hd},
            w * 0.88f, 0.85f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::CantileveredGlass);
    }

    // Recessed Rooftop Penthouse Lounge & Stairhead Core
    float phY0 = roofY + 0.20f;
    float phY1 = phY0 + 2.85f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.18f, phY0, -hd * 0.55f},
        Vec3{w * 0.24f, phY1, hd * 0.15f},
        slateGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.24f, phY1, -hd * 0.62f},
        Vec3{w * 0.30f, phY1 + 0.22f, hd * 0.26f},
        crispWhite);
    BuildingComponents::generateWindow(
        outVerts, outIndices,
        Vec3{w * 0.03f, phY0 + 1.40f, hd * 0.16f},
        w * 0.26f, 2.05f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::CurtainWall, gt);

    // Left White Concrete Pergola Portal Colonnade on Roof Terrace
    for (int p = 0; p < 5; ++p) {
        float px = -hw * 0.72f + p * (w * 0.12f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.10f, phY0, hd * 0.18f},
            Vec3{px + 0.10f, phY1 + 0.15f, hd * 0.32f},
            crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.10f, phY1, -hd * 0.45f},
            Vec3{px + 0.10f, phY1 + 0.22f, hd * 0.32f},
            crispWhite);
    }

    // Elevated Penthouse Utility Tank Housing + Twin Black Cylindrical Tanks
    if (res_scale::showClutter(params_, true)) {
        float tankBoxY = phY1 + 0.22f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-w * 0.05f, tankBoxY, -hd * 0.45f},
            Vec3{w * 0.24f, tankBoxY + 0.95f, hd * 0.05f},
            slateGrey);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{w * 0.06f, tankBoxY + 0.95f, -hd * 0.20f},
            Vec3{w * 0.06f, tankBoxY + 1.65f, -hd * 0.20f},
            0.52f, 0.50f, 12, darkSteel);
    }
}

} // namespace procedural::buildings
