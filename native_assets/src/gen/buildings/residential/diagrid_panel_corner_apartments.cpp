#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateDiagridPanelCornerApartments(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 12.0f, 16.0f);
    float d = res_scale::depth(params_, 10.5f, 13.5f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 4, 8);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t taupePanel = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t cedarWood  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t stonePier  = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t stairGlass = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t silverGrid = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t tarmacMat  = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t greenShrub = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Compound Base, Perimeter Stone-Pier Wall & Front Gate
    float plinthY = 0.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.4f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.4f,  0.02f,  hd + 4.2f}, tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, stonePier);

    // Front boundary fence with alternating stone pillars, white wall bays & metal sliding gate
    float fenceZ = hd + 2.8f;
    for (int i = 0; i <= 3; ++i) {
        float px = -hw - 1.4f + i * 3.4f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.24f, 0.02f, fenceZ - 0.24f},
            Vec3{px + 0.24f, 2.05f, fenceZ + 0.24f}, stonePier);
        if (i < 3) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px + 0.24f, 0.02f, fenceZ - 0.10f},
                Vec3{px + 3.16f, 1.75f, fenceZ + 0.10f}, crispWhite);
        }
    }
    // Double metal-and-white gate panel on front right
    float gateX0 = -hw - 1.4f + 3 * 3.4f + 0.24f;
    float gateX1 = hw + 1.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{gateX0, 0.02f, fenceZ - 0.06f},
        Vec3{gateX1, 1.85f, fenceZ + 0.06f}, silverGrid);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{gateX1, 0.02f, fenceZ - 0.24f},
        Vec3{gateX1 + 0.48f, 2.05f, fenceZ + 0.24f}, stonePier);

    // 2. Partitioned Apartment Core & Wing Geometry
    float leftWingX0 = -hw;
    float leftWingX1 = -0.65f;
    float diagPanelX0 = -0.65f;
    float diagPanelX1 = 1.45f;
    float stairX0 = 1.45f;
    float stairX1 = 3.55f;
    float rightWingX0 = 3.55f;
    float rightWingX1 = hw;

    // Main building body (recessed behind wraparound balconies and stair shaft)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.65f, plinthY, -hd},
        Vec3{ hw - 0.20f, plinthY + totalH, hd - 1.35f}, crispWhite);

    // 3. Left Cedar Cladding Accent Tower & Side Projecting Balconies
    float cedarZ0 = -hd * 0.45f;
    float cedarZ1 = hd * 0.15f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.15f, plinthY, cedarZ0},
        Vec3{-hw + 0.72f, plinthY + totalH + 0.65f, cedarZ1}, cedarWood);
    // Horizontal side balconies along left facade
    for (int s = 1; s < stories; ++s) {
        float sy = plinthY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 0.55f, sy - 0.12f, -hd + 0.6f},
            Vec3{-hw + 0.65f, sy + 0.18f, cedarZ0 - 0.15f}, crispWhite);
        if (res_scale::showBalconies(params_)) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw - 0.52f, sy + 0.18f, -hd + 0.6f},
                Vec3{-hw - 0.44f, sy + 1.05f, cedarZ0 - 0.15f}, balcGlass);
        }
    }

    // 4. Left-Front Corner Wraparound L-Shaped Glass Balconies & Floor Slabs
    for (int s = 1; s <= stories; ++s) {
        float sy = plinthY + s * flH;
        bool isRoofDeck = (s == stories);
        float slabH = isRoofDeck ? 0.30f : 0.72f;
        float slabTop = sy + slabH * 0.35f;
        float slabBot = slabTop - slabH;
        // Wraparound cantilever L-slab wrapping left and front corner
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{leftWingX0 - 0.15f, slabBot, cedarZ1 + 0.1f},
            Vec3{diagPanelX0, slabTop, hd + 0.45f}, crispWhite);
        if (res_scale::showBalconies(params_)) {
            float railTop = slabTop + 0.92f;
            // Front glass rail
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{leftWingX0 - 0.10f, slabTop, hd + 0.34f},
                Vec3{diagPanelX0 - 0.05f, railTop, hd + 0.42f}, balcGlass);
            // Left side glass rail
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{leftWingX0 - 0.12f, slabTop, cedarZ1 + 0.1f},
                Vec3{leftWingX0 - 0.04f, railTop, hd + 0.42f}, balcGlass);
        }
        if (!isRoofDeck) {
            // Sliding patio glass inside corner balcony
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{leftWingX0 + 0.75f, sy + 0.10f, hd - 1.38f},
                Vec3{diagPanelX0 - 0.30f, sy + flH - 0.32f, hd - 1.24f}, darkGlass);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{leftWingX0 + 0.62f, sy + 0.10f, cedarZ1 + 0.4f},
                Vec3{leftWingX0 + 0.72f, sy + flH - 0.32f, hd - 1.55f}, darkGlass);
        }
    }

    // 5. Central Taupe Structural Panel with Crisp White Diagonal X-Diagrid Relief
    float panelTopY = plinthY + totalH + 1.35f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{diagPanelX0, plinthY, hd - 1.35f},
        Vec3{diagPanelX1, panelTopY, hd + 0.18f}, taupePanel);

    float pMidX = (diagPanelX0 + diagPanelX1) * 0.5f;
    float cellW = (diagPanelX1 - diagPanelX0) * 0.86f;
    for (int s = 0; s < stories; ++s) {
        float cy0 = plinthY + 0.35f + s * flH;
        float cy1 = cy0 + flH * 0.88f;
        Vec3 bl{pMidX - cellW * 0.5f, cy0, hd + 0.21f};
        Vec3 tr{pMidX + cellW * 0.5f, cy1, hd + 0.21f};
        Vec3 br{pMidX + cellW * 0.5f, cy0, hd + 0.21f};
        Vec3 tl{pMidX - cellW * 0.5f, cy1, hd + 0.21f};
        Vec3 d1 = (tr - bl).normalized();
        Vec3 d2 = (tl - br).normalized();
        Vec3 n{0.0f, 0.0f, 1.0f};
        Vec3 u1 = n.cross(d1).normalized();
        Vec3 u2 = n.cross(d2).normalized();
        float halfLen = (tr - bl).length() * 0.5f;
        BuildingComponents::appendOrientedBox(outVerts, outIndices,
            (bl + tr) * 0.5f, d1, u1, n, halfLen, 0.045f, 0.045f, crispWhite);
        BuildingComponents::appendOrientedBox(outVerts, outIndices,
            (br + tl) * 0.5f, d2, u2, n, halfLen, 0.045f, 0.045f, crispWhite);
        // Horizontal separator bar
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{diagPanelX0 + 0.08f, cy1 - 0.03f, hd + 0.17f},
            Vec3{diagPanelX1 - 0.08f, cy1 + 0.03f, hd + 0.23f}, crispWhite);
    }

    // 6. Full-Height 4-Storey Gridded Curtain-Glass Stairwell Core
    float stairTopY = plinthY + totalH + 0.95f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{stairX0, plinthY + 0.35f, hd - 0.32f},
        Vec3{stairX1, stairTopY, hd - 0.16f}, stairGlass);
    for (int gy = 0; gy <= stories * 2; ++gy) {
        float yL = plinthY + 0.35f + gy * ((stairTopY - (plinthY + 0.35f)) / (stories * 2));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{stairX0, yL - 0.035f, hd - 0.16f},
            Vec3{stairX1, yL + 0.035f, hd - 0.08f}, crispWhite);
    }
    for (int gx = 0; gx <= 2; ++gx) {
        float xL = stairX0 + gx * ((stairX1 - stairX0) * 0.5f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xL - 0.035f, plinthY + 0.35f, hd - 0.16f},
            Vec3{xL + 0.035f, stairTopY, hd - 0.08f}, crispWhite);
    }

    // 7. Right Wing: Crisp White Framing Pier, Staggered Balconies & Rooftop Cantilever Pergola
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0, plinthY, hd - 1.35f},
        Vec3{rightWingX0 + 0.58f, stairTopY, hd + 0.22f}, crispWhite);
    // Overhead white roof beam bridging diagrid tower across stair well to right wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{diagPanelX1 - 0.1f, stairTopY, hd - 1.35f},
        Vec3{rightWingX1 - 0.4f, stairTopY + 0.55f, hd + 0.45f}, crispWhite);
    for (int r = 0; r < 5; ++r) {
        float rx = stairX0 + 0.20f + r * 0.45f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rx, stairTopY - 0.22f, hd - 1.2f},
            Vec3{rx + 0.12f, stairTopY + 0.02f, hd + 0.35f}, crispWhite);
    }

    for (int s = 1; s <= stories; ++s) {
        float sy = plinthY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rightWingX0 + 0.55f, sy - 0.14f, hd - 1.35f},
            Vec3{rightWingX1 + 0.25f, sy + 0.18f, hd + 0.35f}, crispWhite);
        if (res_scale::showBalconies(params_)) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rightWingX0 + 0.60f, sy + 0.18f, hd + 0.24f},
                Vec3{rightWingX1 + 0.20f, sy + 1.05f, hd + 0.32f}, balcGlass);
        }
        if (s < stories) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rightWingX0 + 1.15f, sy + 0.15f, hd - 1.38f},
                Vec3{rightWingX1 - 0.35f, sy + flH - 0.28f, hd - 1.24f}, darkGlass);
        }
    }

    // Palm shrubs along compound wall
    for (float px : {-hw + 1.4f, 0.2f, 3.0f}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{px, 0.1f, hd + 1.4f}, Vec3{px, 3.2f, hd + 1.4f},
            0.12f, 0.08f, 6, cedarWood, true, true);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{px, 2.7f, hd + 1.4f}, Vec3{px, 3.65f, hd + 1.4f},
            0.95f, 0.25f, 8, greenShrub, true, true);
    }

    // 8. Rear & Right Side Windows + Roof Cap
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.4f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 4, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{hw - 0.20f, 0.0f, -0.8f}, d * 0.65f, fy, flH,
            Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f}, 3, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.8f, d - 1.4f, plinthY + totalH, RoofType::FlatParapet, 0.5f, 0.0f);
}

} // namespace procedural::buildings
