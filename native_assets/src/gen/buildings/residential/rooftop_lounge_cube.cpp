#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateRooftopLoungeCube(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.5f, 14.5f);
    float d = res_scale::depth(params_, 8.0f, 12.0f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 2, 4);
    float lowerH = std::max(1, stories - 1) * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite   = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t slateGrey    = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t blackFrame   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t plinthStone  = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t planterBox   = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t greenPlant   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    WindowType wt         = res_scale::windowType(params_, WindowType::MultiPaneFactory);
    GlassType gt          = res_scale::glassType(params_, GlassType::ReflectiveChromeMirror);
    bool wantShutters     = res_scale::showShutters(params_, true);
    bool wantSunshades    = res_scale::showSunshades(params_, false);
    bool wantCornice      = res_scale::showCornice(params_, true);
    bool hasBalconies     = res_scale::showBalconies(params_);

    // 1. Foundation Plinth, Entry Steps & Front Planter Box Strip
    float plinthY = 0.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, -0.35f, -hd - 0.5f},
        Vec3{hw + 0.5f, plinthY, hd + 1.25f},
        plinthStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.2f, plinthY, hd + 0.2f},
        Vec3{2.4f, plinthY + 0.35f, hd + 0.85f},
        planterBox);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.1f, plinthY + 0.35f, hd + 0.28f},
        Vec3{2.3f, plinthY + 0.52f, hd + 0.78f},
        greenPlant);

    // 2. Main White & Slate Interlocking Cubic Structure (Lower Stories)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{hw, plinthY + lowerH, hd},
        crispWhite);

    // Iconic Vertical Slate-Grey Architectural Tower Spine dividing Left/Center bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + w * 0.26f, plinthY, hd - 0.2f},
        Vec3{-hw + w * 0.40f, plinthY + lowerH + 0.55f, hd + 0.55f},
        slateGrey);

    // Left Projecting 2nd-Storey Cantilever Box + Privacy Louvre Slats
    if (stories >= 2) {
        float boxY0 = plinthY + flH;
        float boxY1 = boxY0 + flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.65f, boxY0, -hd * 0.4f},
            Vec3{-hw + w * 0.27f, boxY0 + 0.22f, hd + 0.85f},
            crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.65f, boxY1 - 0.22f, -hd * 0.4f},
            Vec3{-hw + w * 0.27f, boxY1, hd + 0.85f},
            crispWhite);
        for (int fin = 0; fin < 4; ++fin) {
            float fx = -hw + 0.2f + fin * 0.48f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx - 0.05f, boxY0 + 0.22f, hd + 0.45f},
                Vec3{fx + 0.05f, boxY1 - 0.22f, hd + 0.75f},
                blackFrame);
        }
        // Central Framed Upper Balcony Box with Vertical White Concrete Screen Fins
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + w * 0.39f, boxY0, hd},
            Vec3{hw - w * 0.26f, boxY1, hd + 0.55f},
            crispWhite);
        for (int wf = 0; wf < 3; ++wf) {
            float fx = -hw + w * 0.42f + wf * 0.42f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx - 0.06f, boxY0 + 0.2f, hd + 0.42f},
                Vec3{fx + 0.06f, boxY1 - 0.2f, hd + 0.55f},
                crispWhite);
        }
    }

    // 3. Signature Two-Storey Corner-Wrap Black-Grid Window on Right Front Corner
    float cornerWinW = std::clamp(w * 0.20f, 1.4f, 2.3f);
    for (int s = 0; s < std::min(2, stories); ++s) {
        float y0 = plinthY + s * flH;
        BuildingComponents::generateWindow(
            outVerts, outIndices,
            Vec3{hw - cornerWinW * 0.5f - 0.15f, y0 + flH * 0.52f, hd + 0.03f},
            cornerWinW, flH * 0.84f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::CornerWrap, gt);
    }

    // Entry Door on Recessed Left Patio
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{-hw + w * 0.16f, plinthY, hd + 0.03f}, flH, w * 0.3f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, 1.25f, 2.30f, true);

    // 4. Lower-Level Windows across Facades
    for (int s = 0; s < std::max(1, stories - 1); ++s) {
        float y0 = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w * 0.55f, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            2, wt, gt, 1.45f, flH * 0.65f, 0.45f,
            -1.2f, 0.4f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.50f, flH * 0.65f, 0.50f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.40f, flH * 0.66f, 0.48f,
            0.0f, 0.0f, wantShutters, wantSunshades);
    }

    // 5. Full Rooftop Entertainment Terrace + Glass Balustrades + Concrete Pergola Ribs
    float roofDeckY = plinthY + lowerH;
    if (hasBalconies) {
        BuildingComponents::generateBalcony(
            outVerts, outIndices,
            Vec3{0.0f, roofDeckY, hd},
            w * 0.94f, res_scale::balconyDepth(params_, 0.90f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            BalconyType::CantileveredGlass);
    }

    // Penthouse Lounge Suite Set Back on Rooftop Terrace
    float pentH = flH * 0.88f;
    float pentW = w * 0.58f;
    float pentD = d * 0.48f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-pentW * 0.5f + 0.6f, roofDeckY, -hd + 0.6f},
        Vec3{pentW * 0.5f + 0.6f, roofDeckY + pentH, -hd + 0.6f + pentD},
        slateGrey);
    res_scale::appendScaledWindowRow(
        outVerts, outIndices, params_,
        Vec3{0.6f, 0.0f, -hd + 0.6f + pentD + 0.02f}, pentW, roofDeckY, pentH + 0.5f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        3, WindowType::PictureGlass, gt, 1.55f, pentH * 0.78f, 0.15f,
        0.0f, 0.0f, false, false);

    // Iconic White Concrete Architectural Pergola Portal Frames on Rooftop Left
    for (int p = 0; p < 5; ++p) {
        float px = -hw + 0.8f + p * 1.15f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.12f, roofDeckY, -hd * 0.15f},
            Vec3{px + 0.12f, roofDeckY + pentH + 0.25f, -hd * 0.15f + 0.24f},
            crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.12f, roofDeckY + pentH + 0.05f, -hd * 0.15f},
            Vec3{px + 0.12f, roofDeckY + pentH + 0.28f, hd * 0.45f},
            crispWhite);
    }

    // Flat Canopy Slab over Penthouse Suite
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-pentW * 0.5f + 0.1f, roofDeckY + pentH, -hd + 0.2f},
        Vec3{pentW * 0.5f + 1.1f, roofDeckY + pentH + 0.22f, -hd + 1.2f + pentD},
        crispWhite);

    // Optional Rooftop Water Tank Enclosure Box (as seen atop image 2)
    if (res_scale::showClutter(params_, true)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-0.8f, roofDeckY + pentH + 0.22f, -hd + 1.0f},
            Vec3{2.4f, roofDeckY + pentH + 1.15f, -hd + 3.0f},
            slateGrey);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{0.8f, roofDeckY + pentH + 1.15f, -hd + 2.0f},
            Vec3{0.8f, roofDeckY + pentH + 1.75f, -hd + 2.0f},
            0.55f, 0.55f, 12, blackFrame, true, true);
    }

    // Optional Attached Ground Garage (+X side)
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.34f, 3.6f, 5.6f);
        float gd = std::clamp(d * 0.56f, 5.0f, 6.6f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, plinthY, -hd * 0.4f},
            Vec3{hw + gw, plinthY + flH, -hd * 0.4f + gd},
            slateGrey);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, plinthY, -hd * 0.4f + gd + 0.02f},
            std::min(3.2f, gw - 0.6f), std::min(2.25f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.4f + gd * 0.5f,
            gw, gd, plinthY + flH, RoofType::FlatParapet, 0.5f, 0.0f);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, roofDeckY, 0.24f);
    }
}

} // namespace procedural::buildings
