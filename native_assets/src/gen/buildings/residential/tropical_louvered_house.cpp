#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void HouseGen::generateTropicalLouveredHouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.5f, 16.0f);
    float d = res_scale::depth(params_, 7.0f, 11.5f);
    float flH = res_scale::floorHeight(params_, 3.3f);
    int stories = res_scale::stories(params_, 1, 4);
    float h = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t stuccoWall  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t teakWood    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t darkSteel   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t stoneDeck   = static_cast<uint32_t>(MaterialSlot::Foundation);
    WindowType wt        = res_scale::windowType(params_, WindowType::LouveredJalousie);
    GlassType gt         = res_scale::glassType(params_, GlassType::BronzeArchitectural);
    bool wantShutters    = res_scale::showShutters(params_, true);
    bool wantSunshades   = res_scale::showSunshades(params_, true);
    bool wantCornice     = res_scale::showCornice(params_, true);
    bool hasVeranda      = res_scale::showBalconies(params_);

    // 1. Elevated Stilts & Wraparound Teak Veranda Deck
    float deckY = 0.55f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.2f, deckY - 0.18f, -hd - 1.0f},
        Vec3{hw + 1.2f, deckY, hd + 1.5f},
        teakWood);
    for (int px = -1; px <= 1; ++px) {
        for (int pz = -1; pz <= 1; pz += 2) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px * (hw + 0.8f) - 0.16f, -0.35f, pz * (hd + 0.7f) - 0.16f},
                Vec3{px * (hw + 0.8f) + 0.16f, deckY - 0.18f, pz * (hd + 0.7f) + 0.16f},
                stoneDeck);
        }
    }
    // Wide entry steps
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.6f, -0.10f, hd + 1.5f},
        Vec3{1.6f, deckY * 0.55f, hd + 2.0f},
        stoneDeck);

    // 2. Main Tropical Living Core
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, deckY, -hd},
        Vec3{hw, deckY + h, hd},
        stuccoWall);

    // 3. Architectural Vertical Teak Privacy Brise-Soleil Fins on Side Wings
    float finSpanStart = -hw + 0.6f;
    float finSpanEnd   = -hw + w * 0.28f;
    for (float fx = finSpanStart; fx <= finSpanEnd; fx += 0.45f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.04f, deckY + 0.1f, hd + 0.02f},
            Vec3{fx + 0.04f, deckY + h - 0.1f, hd + 0.42f},
            teakWood);
    }
    // Deep Horizontal Cantilevered Flat Slab Roof Fascia Between Floors
    for (int s = 1; s < stories; ++s) {
        float sy = deckY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.85f, sy - 0.12f, -hd - 0.65f},
            Vec3{hw + 0.85f, sy + 0.10f, hd + 0.95f},
            darkSteel);
    }

    // Entry Door & Canopy
    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.6f, deckY, hd + 0.02f}, flH, w,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, 2.0f, 2.40f, true);

    // 4. Breezy Floor-to-Ceiling Louvered Windows & Wraparound Balconies
    for (int s = 0; s < stories; ++s) {
        float y0 = deckY + s * flH;
        float exMin = (s == 0) ? -1.1f : 0.0f;
        float exMax = (s == 0) ? 2.2f : 0.0f;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, hd + 0.02f}, w, y0, flH,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            4, wt, gt, 1.55f, flH * 0.72f, 0.32f,
            exMin, exMax, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.02f}, w, y0, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.55f, flH * 0.72f, 0.32f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.45f, flH * 0.68f, 0.40f,
            0.0f, 0.0f, wantShutters, wantSunshades);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.02f, 0.0f, 0.0f}, d, y0, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            3, wt, gt, 1.45f, flH * 0.68f, 0.40f,
            0.0f, 0.0f, wantShutters, wantSunshades);

        if (hasVeranda && s > 0) {
            BuildingComponents::generateBalcony(
                outVerts, outIndices,
                Vec3{0.0f, y0, hd},
                w * 0.70f, res_scale::balconyDepth(params_, 1.35f),
                Vec3{0, 0, 1}, Vec3{1, 0, 0},
                BalconyType::CantileveredGlass);
        }
    }

    // 5. Optional Shaded Open/Closed Garage Bay
    if (params_.hasGarage) {
        float gw = std::clamp(w * 0.34f, 3.8f, 6.0f);
        float gd = std::clamp(d * 0.56f, 5.2f, 6.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, deckY, -hd * 0.4f},
            Vec3{hw + gw, deckY + flH, -hd * 0.4f + gd},
            teakWood);
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{hw + gw * 0.5f, deckY, -hd * 0.4f + gd + 0.02f},
            std::min(3.3f, gw - 0.65f), std::min(2.3f, flH - 0.25f),
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, false);
        res_scale::appendWingRoof(
            outVerts, outIndices, params_,
            hw + gw * 0.5f, -hd * 0.4f + gd * 0.5f,
            gw, gd, deckY + flH, RoofType::NoneFlat, 0.4f, 0.5f);
    }

    if (wantCornice) {
        BuildingComponents::generateCornice(outVerts, outIndices, w, d, deckY + h, 0.65f);
    }

    // 6. Deep Overhanging Flat Tropical Canopy Roof
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w, d, deckY + h, RoofType::NoneFlat, 0.5f, 1.35f, false);
}

} // namespace procedural::buildings
