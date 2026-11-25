#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendUndercroftCar(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float y0, float cz,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat
) {
    float hw = 0.86f, hl = 1.95f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw, y0 + 0.22f, cz - hl},
        Vec3{cx + hw, y0 + 0.84f, cz + hl}, bodyMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.84f, y0 + 0.84f, cz - hl * 0.45f},
        Vec3{cx + hw * 0.84f, y0 + 1.38f, cz + hl * 0.35f}, glassMat);
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.56f, hl * 0.56f}) {
            float wx = cx + sx * (hw - 0.04f);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.10f, y0 + 0.29f, cz + sz},
                Vec3{wx + 0.10f, y0 + 0.29f, cz + sz},
                0.29f, 0.29f, 8, tireMat, true, true);
        }
    }
}

} // namespace

void HouseGen::generateTimberBandPodiumApartments(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 18.0f, 26.0f);
    float d = res_scale::depth(params_, 11.5f, 14.5f);
    float flH = res_scale::floorHeight(params_, 3.05f);
    int stories = res_scale::stories(params_, 5, 10);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t darkCharc  = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t mahogany   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t silverGrid = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t tarmacMat  = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t whiteCar   = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t blueCar    = static_cast<uint32_t>(MaterialSlot::VehicleBlue);
    uint32_t orangeCar  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t redCar     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t tireMat    = static_cast<uint32_t>(MaterialSlot::TireRubber);
    uint32_t greenGrass = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t amberLight = static_cast<uint32_t>(MaterialSlot::GlassGold);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Open Parking Apron & Podium Columns (Undercroft Ground Floor)
    float undercroftH = flH * 1.08f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.2f,  0.02f,  hd + 4.5f}, greenGrass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.5f, 0.02f, -hd},
        Vec3{ hw + 1.5f, 0.14f, hd + 3.6f}, tarmacMat);

    // Perimeter low white boundary garden wall on left
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.5f, 0.14f, hd + 2.4f},
        Vec3{-hw + 3.2f, 1.45f, hd + 2.75f}, crispWhite);

    // Heavy structural concrete pilotis piers under podium
    for (int ix = 0; ix < 6; ++ix) {
        float px = -hw + 0.85f + ix * ((w - 1.7f) / 5.0f);
        for (float pz : {-hd + 0.9f, 0.0f, hd - 0.85f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.34f, 0.14f, pz - 0.34f},
                Vec3{px + 0.34f, undercroftH, pz + 0.34f}, darkCharc);
        }
    }
    // Recessed elevator/utility core at back of parking podium
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 2.2f, 0.14f, -hd + 0.8f},
        Vec3{ hw - 2.2f, undercroftH, -hd + 3.4f}, darkCharc);

    // Row of parked resident vehicles under the podium slab
    if (params_.hasGarage) {
        uint32_t palette[4] = {orangeCar, whiteCar, redCar, blueCar};
        for (int c = 0; c < 6; ++c) {
            float cx = -hw + 2.5f + c * ((w - 4.8f) / 5.0f);
            appendUndercroftCar(outVerts, outIndices, cx, 0.14f, hd - 0.25f,
                palette[c % 4], darkGlass, tireMat);
        }
    }

    // Podium transfer deck slab + glowing undercroft ceiling strip
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.35f, undercroftH, -hd - 0.2f},
        Vec3{ hw + 0.35f, undercroftH + 0.36f, hd + 0.35f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.4f, undercroftH - 0.05f, hd - 0.35f},
        Vec3{ hw - 0.4f, undercroftH, hd - 0.18f}, amberLight);

    // 2. Stepped Multi-Wing Apartment Towers (Phlox, Tulip, Daisy wings)
    float startY = undercroftH + 0.36f;
    struct WingSpec {
        float x0, x1;
        float zFront;
        int upperFloors;
        bool accentDarkLower;
    };
    int upperCount = std::max(4, stories - 1);
    WingSpec wings[3] = {
        {-hw, -hw + w * 0.44f, hd + 0.20f, upperCount, false},
        {-hw + w * 0.44f, -hw + w * 0.75f, hd - 0.85f, std::max(3, upperCount - 1), true},
        {-hw + w * 0.75f,  hw, hd - 1.45f, std::max(3, upperCount - 2), false}
    };

    for (int wi = 0; wi < 3; ++wi) {
        const auto& wg = wings[wi];
        float wingW = wg.x1 - wg.x0;
        float wingH = wg.upperFloors * flH;
        float wingTopY = startY + wingH;

        // Main crisp white residential block
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wg.x0, startY, -hd},
            Vec3{wg.x1, wingTopY, wg.zFront}, crispWhite);

        // Dark charcoal lower band on middle wing for alternating contrast
        if (wg.accentDarkLower) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{wg.x0 + 0.25f, startY, wg.zFront - 0.05f},
                Vec3{wg.x1 - 0.25f, startY + 2 * flH, wg.zFront + 0.18f}, darkCharc);
        }

        // Characteristic Mahogany Horizontal Timber Spandrel Bands + Balcony Stack
        float balcX0 = wg.x0 + wingW * 0.36f;
        float balcX1 = wg.x0 + wingW * 0.72f;
        for (int s = 0; s < wg.upperFloors; ++s) {
            float fy = startY + s * flH;

            // Left mahogany window bay rib
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{wg.x0 + 0.55f, fy + 0.18f, wg.zFront - 0.04f},
                Vec3{balcX0 - 0.15f, fy + flH - 0.28f, wg.zFront + 0.22f}, mahogany);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{wg.x0 + 0.85f, fy + 0.85f, wg.zFront + 0.18f},
                Vec3{balcX0 - 0.45f, fy + flH - 0.45f, wg.zFront + 0.28f}, darkGlass);

            // Central dark recessed balcony alcove + projecting slab + mahogany apron
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{balcX0, fy, wg.zFront - 0.12f},
                Vec3{balcX1, fy + 0.24f, wg.zFront + 0.95f}, darkCharc);
            if (res_scale::showBalconies(params_)) {
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{balcX0, fy + 0.24f, wg.zFront + 0.86f},
                    Vec3{balcX1, fy + 1.10f, wg.zFront + 0.94f}, balcGlass);
            }
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{balcX0 + 0.20f, fy + 0.24f, wg.zFront - 0.04f},
                Vec3{balcX1 - 0.20f, fy + flH - 0.25f, wg.zFront + 0.08f}, darkGlass);
            // Warm sconce dot inside balcony cheek
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{balcX0 + 0.06f, fy + 1.55f, wg.zFront + 0.45f},
                Vec3{balcX0 + 0.14f, fy + 1.72f, wg.zFront + 0.55f}, amberLight);

            // Right mahogany band beside balcony
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{balcX1 + 0.15f, fy + 0.18f, wg.zFront - 0.04f},
                Vec3{wg.x1 - 0.45f, fy + flH - 0.28f, wg.zFront + 0.22f}, mahogany);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{balcX1 + 0.42f, fy + 0.85f, wg.zFront + 0.18f},
                Vec3{wg.x1 - 0.72f, fy + flH - 0.45f, wg.zFront + 0.28f}, darkGlass);

            // Side elevation windows along left wing & rear elevation
            if (wi == 0) {
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{wg.x0 - 0.18f, fy + 0.25f, -hd + 1.6f},
                    Vec3{wg.x0 + 0.02f, fy + flH - 0.25f, hd - 2.6f}, mahogany);
                res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
                    Vec3{wg.x0, 0.0f, 0.0f}, d * 0.70f, fy, flH,
                    Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 3, wt, gt);
            }
            res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
                Vec3{(wg.x0 + wg.x1) * 0.5f, 0.0f, -hd}, wingW - 1.4f, fy, flH,
                Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 3, wt, gt);
        }

        // Signature Projecting Roof Pergola Cornice per Wing
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wg.x0 - 0.25f, wingTopY, -hd - 0.2f},
            Vec3{wg.x1 + 0.25f, wingTopY + 0.34f, wg.zFront + 1.15f}, crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wg.x0 - 0.18f, wingTopY + 0.34f, -hd},
            Vec3{wg.x1 + 0.18f, wingTopY + 0.62f, wg.zFront + 0.75f}, darkCharc);
        int finCount = std::max(4, static_cast<int>(wingW / 0.85f));
        for (int r = 0; r < finCount; ++r) {
            float rx = wg.x0 + 0.3f + r * ((wingW - 0.6f) / finCount);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rx, wingTopY + 0.05f, wg.zFront + 0.15f},
                Vec3{rx + 0.12f, wingTopY + 0.38f, wg.zFront + 1.05f}, silverGrid);
        }
    }
}

} // namespace procedural::buildings
