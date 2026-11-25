#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateTwinStairCornerApartments(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 11.5f, 14.8f);
    float d = res_scale::depth(params_, 11.5f, 14.5f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 4, 8);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t midGrey    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t espresso   = static_cast<uint32_t>(MaterialSlot::WallCorten);
    uint32_t darkTrim   = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t amberWood  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t mirrorGl   = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t blackFrame = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat  = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t greenPlant = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Podium Plaza & Raised Corner Entry Terrace with Twin Symmetrical Grand Staircases
    float plinthY = 1.05f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.2f,  0.02f,  hd + 4.4f}, tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.45f, 0.02f, -hd},
        Vec3{ hw + 0.25f, plinthY, hd + 1.55f}, crispWhite);

    // Glazed double ground lobby doors tucked beneath cantilevered floor 1
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.5f, plinthY, -hd},
        Vec3{ hw - 0.5f, plinthY + flH, hd - 0.55f}, midGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.45f, plinthY + 0.05f, hd - 0.58f},
        Vec3{ 1.85f, plinthY + 2.55f, hd - 0.46f}, mirrorGl);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ 2.15f, plinthY + 0.05f, hd - 0.58f},
        Vec3{ hw - 0.9f, plinthY + 2.55f, hd - 0.46f}, darkGlass);

    // Front podium glass balustrade between stairs
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.40f, plinthY, hd + 1.44f},
            Vec3{-0.85f, plinthY + 0.95f, hd + 1.52f}, balcGlass);
    }

    // Twin Symmetrical Front Exterior Staircases ascending to podium
    for (int st = 0; st < 2; ++st) {
        float sx0 = (st == 0) ? -0.65f : 2.35f;
        float sx1 = (st == 0) ? 1.85f  : 4.85f;
        int steps = 6;
        for (int i = 0; i < steps; ++i) {
            float stepTop = (i + 1) * (plinthY / steps);
            float szBack = hd + 1.50f + (steps - 1 - i) * 0.26f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx0, 0.02f, szBack - 0.26f},
                Vec3{sx1, stepTop, szBack + 0.04f}, crispWhite);
        }
        // Sloped side glass stair guard rails
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx0 - 0.06f, plinthY * 0.35f, hd + 1.50f},
            Vec3{sx0 + 0.02f, plinthY + 0.85f, hd + 3.05f}, balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx1 - 0.02f, plinthY * 0.35f, hd + 1.50f},
            Vec3{sx1 + 0.06f, plinthY + 0.85f, hd + 3.05f}, balcGlass);
    }

    // 2. Main Apartment Core Massing
    float splitX = 1.65f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.65f, plinthY + flH, -hd},
        Vec3{ hw - 0.20f, plinthY + totalH, hd - 1.35f}, midGrey);

    // 3. Left Facade: 4-Storey Dark-Gridded Stairwell Curtain Glass Shaft & Side Amber Wood Tower
    float stairZ0 = hd - 3.65f;
    float stairZ1 = hd - 1.75f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.12f, plinthY, -hd + 0.8f},
        Vec3{-hw + 0.68f, plinthY + totalH + 0.85f, stairZ0}, amberWood);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.62f, plinthY, stairZ0 - 0.2f},
        Vec3{-hw + 0.92f, plinthY + totalH + 0.65f, stairZ1 + 0.2f}, midGrey);

    // Full-height dark gridded stairwell glass along left corner
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.42f, plinthY + 0.2f, stairZ0 + 0.05f},
        Vec3{-hw + 0.56f, plinthY + totalH - 0.25f, stairZ1 - 0.05f}, darkGlass);
    for (int gy = 1; gy <= stories * 2; ++gy) {
        float yL = plinthY + 0.2f + gy * ((totalH - 0.45f) / (stories * 2));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.36f, yL - 0.035f, stairZ0 + 0.05f},
            Vec3{-hw + 0.46f, yL + 0.035f, stairZ1 - 0.05f}, blackFrame);
    }
    for (int gz = 1; gz <= 2; ++gz) {
        float zL = stairZ0 + 0.05f + gz * ((stairZ1 - stairZ0 - 0.10f) / 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.36f, plinthY + 0.2f, zL - 0.035f},
            Vec3{-hw + 0.46f, plinthY + totalH - 0.25f, zL + 0.035f}, blackFrame);
    }

    // 4. Open Corner Stairway & 3 Stacked Cantilevered Crisp White L-Balconies with Espresso Corner Columns
    // Corner structural dark espresso column stack
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.28f, plinthY, hd - 0.25f},
        Vec3{-hw + 0.72f, plinthY + totalH - 0.10f, hd + 0.18f}, espresso);

    // Open structural stair flights visible inside the corner bay
    for (int s = 0; s < stories - 1; ++s) {
        float fy = plinthY + s * flH;
        for (int step = 0; step < 6; ++step) {
            float sy = fy + step * (flH / 6.0f);
            float sz = hd - 1.55f + step * 0.20f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw + 0.85f, sy, sz},
                Vec3{-hw + 1.95f, sy + 0.12f, sz + 0.22f}, midGrey);
        }
    }

    // 3 Stacked Cantilevered White Corner L-Balconies + Top Overhanging Hood Frame
    for (int s = 1; s <= stories; ++s) {
        float fy = plinthY + s * flH;
        bool isRoofHood = (s == stories);
        float fasciaH = isRoofHood ? 0.65f : 0.48f;
        // L-balcony deck wrapping left and front corner
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.15f, fy - 0.12f, stairZ1 - 0.1f},
            Vec3{splitX + 0.45f, fy + fasciaH, hd + 0.95f}, crispWhite);
        if (!isRoofHood && res_scale::showBalconies(params_)) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw + 0.20f, fy + fasciaH, hd + 0.85f},
                Vec3{splitX + 0.40f, fy + fasciaH + 0.92f, hd + 0.93f}, balcGlass);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw + 0.16f, fy + fasciaH, stairZ1 - 0.05f},
                Vec3{-hw + 0.24f, fy + fasciaH + 0.92f, hd + 0.92f}, balcGlass);
        }
        if (s < stories) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw + 2.25f, fy + 0.45f, hd - 1.38f},
                Vec3{splitX - 0.25f, fy + flH - 0.22f, hd - 1.24f}, darkGlass);
        }
    }
    // Left vertical cheek of upper white hood wrapping floors 3-4
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.10f, plinthY + (stories - 1) * flH, stairZ0},
        Vec3{-hw + 0.35f, plinthY + totalH + 0.65f, hd + 0.95f}, crispWhite);
    // Cascading balcony planter greenery on 1st balcony corner
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX - 1.15f, plinthY + flH + 0.38f, hd + 0.72f},
        Vec3{splitX + 0.35f, plinthY + flH + 0.75f, hd + 1.05f}, greenPlant);

    // 5. Right Wing: Espresso-Brown Overhanging C-Frame, Square White-Framed Windows & Wood Loft Crown
    float rwTopY = plinthY + totalH - 0.35f;
    float rwBotY = plinthY + flH * 0.92f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX, rwBotY, hd - 1.35f},
        Vec3{hw + 0.25f, rwTopY, hd + 0.55f}, espresso);
    // Mid-grey recessed inset inside espresso C-frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX + 0.42f, rwBotY + 0.35f, hd - 0.95f},
        Vec3{hw - 0.10f, rwTopY - 0.38f, hd + 0.42f}, midGrey);
    // Signature crisp white square-framed punched windows + dark louver slot
    for (int row = 1; row < stories - 1; ++row) {
        float wy = plinthY + row * flH + 1.25f;
        float wx = splitX + 1.25f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx - 0.48f, wy - 0.48f, hd + 0.38f},
            Vec3{wx + 0.48f, wy + 0.48f, hd + 0.54f}, crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx - 0.34f, wy - 0.34f, hd + 0.42f},
            Vec3{wx + 0.34f, wy + 0.34f, hd + 0.56f}, darkGlass);
    }
    // Vertical black louver panel on right wing front
    for (int l = 0; l < 6; ++l) {
        float lx = hw - 1.15f + l * 0.14f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{lx, rwBotY + 0.65f, hd + 0.38f},
            Vec3{lx + 0.05f, rwTopY - 0.65f, hd + 0.48f}, blackFrame);
    }
    // Upper amber timber penthouse band crowning right wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX + 0.45f, rwTopY, -hd + 0.8f},
        Vec3{hw + 0.10f, plinthY + totalH + 0.72f, hd - 0.65f}, amberWood);

    // 6. Rear & Right Side Facade Windows + Roof Cap
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.4f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 3, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{hw - 0.20f, 0.0f, -0.8f}, d * 0.65f, fy, flH,
            Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f}, 3, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.9f, d - 1.4f, plinthY + totalH, RoofType::FlatParapet, 0.48f, 0.0f);
}

} // namespace procedural::buildings
