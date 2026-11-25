#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendPalmAccent(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x, float y0, float z,
    float height,
    uint32_t trunkMat,
    uint32_t leafMat
) {
    int segs = 5;
    float segH = height / static_cast<float>(segs);
    float cx = x, cy = y0, cz = z;
    for (int i = 0; i < segs; ++i) {
        float r0 = 0.14f - i * 0.015f;
        float r1 = 0.12f - i * 0.015f;
        float nx = cx + ((i % 2 == 0) ? 0.04f : -0.02f);
        float ny = cy + segH;
        float nz = cz + 0.03f;
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{cx, cy, cz}, Vec3{nx, ny, nz},
            r0, r1, 8, trunkMat, true, true
        );
        cx = nx; cy = ny; cz = nz;
    }
    const float twoPi = 6.2831853f;
    for (int f = 0; f < 7; ++f) {
        float ang = twoPi * static_cast<float>(f) / 7.0f;
        float dx = std::cos(ang), dz = std::sin(ang);
        Vec3 pMid{cx + dx * 0.78f, cy + 0.34f, cz + dz * 0.78f};
        Vec3 pTip{cx + dx * 1.48f, cy - 0.14f, cz + dz * 1.48f};
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices, Vec3{cx, cy + 0.2f, cz}, pMid,
            0.08f, 0.05f, 6, leafMat, true, true
        );
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices, pMid, pTip,
            0.05f, 0.015f, 6, leafMat, true, true
        );
    }
}

void appendWoodRibbonPanel(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    int slotCount,
    uint32_t woodMat,
    uint32_t glassMat,
    bool frontFace
) {
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{x0, y0, z0}, Vec3{x1, y1, z1}, woodMat);
    float totalH = y1 - y0;
    float step = totalH / static_cast<float>(slotCount);
    for (int i = 0; i < slotCount; ++i) {
        float sy = y0 + (i + 0.45f) * step;
        if (frontFace) {
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{x0 + 0.25f, sy, z1 - 0.02f},
                Vec3{x1 - 0.25f, sy + 0.22f, z1 + 0.04f},
                glassMat
            );
        } else {
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{x0 - 0.04f, sy, z0 + 0.25f},
                Vec3{x0 + 0.02f, sy + 0.22f, z1 - 0.25f},
                glassMat
            );
        }
    }
}

} // namespace

void HouseGen::generateStaggeredTerraceApartments(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 16.0f, 24.0f);
    float d = res_scale::depth(params_, 11.0f, 16.0f);
    float flH = res_scale::floorHeight(params_, 3.15f);
    int stories = res_scale::stories(params_, 3, 7);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t slateGrey   = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t darkAlum    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t woodSlat    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t greenPlant  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t balcGlass   = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t darkGlass   = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t warmGlass   = static_cast<uint32_t>(MaterialSlot::GlassGold);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::PictureGlass);

    // 1. Foreground Asphalt Street & Lush Perimeter Hedge + Crisp White Boundary Privacy Wall
    float plinthY = 0.24f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.6f, -0.18f, -hd - 1.5f},
        Vec3{ hw + 2.6f,  0.02f,  hd + 5.8f},
        tarmacMat);
    // Green tropical shrub border outside perimeter wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, 0.02f, hd + 1.8f},
        Vec3{ hw + 2.2f, 0.55f, hd + 3.4f},
        greenPlant);
    // Crisp White Perimeter Compound Wall around left & front courtyard
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.4f, 0.02f, hd + 1.35f},
        Vec3{ hw * 0.42f, 2.25f, hd + 1.65f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.4f, 0.02f, -hd - 0.8f},
        Vec3{-hw - 1.1f, 2.25f, hd + 1.65f},
        crispWhite);

    // Foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.3f, 0.0f, -hd - 0.3f},
        Vec3{ hw + 0.3f, plinthY, hd + 0.3f},
        slateGrey);

    // 2. Main Multi-Storey Apartment Core Massing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, -hd},
        Vec3{ hw, plinthY + totalH, hd - 0.8f},
        crispWhite);

    // Dark Slate Vertical Accent Tower Rib on Left Corner (-X, +Z) and Center-Right Core
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, plinthY, hd - 3.6f},
        Vec3{-hw + 0.65f, plinthY + totalH + 0.25f, hd - 1.6f},
        slateGrey);
    // Cantilevered dark canopy on left ground corner
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.3f, plinthY + flH * 0.92f, hd - 4.2f},
        Vec3{-hw + 0.3f, plinthY + flH * 0.92f + 0.32f, hd - 0.6f},
        slateGrey);

    // Left Side Facade Warm Timber Cladding Panel with Horizontal Ribbon Slots
    appendWoodRibbonPanel(
        outVerts, outIndices,
        -hw - 0.14f, plinthY + 0.4f, -hd + 1.6f,
        -hw + 0.08f, plinthY + totalH - 0.4f, hd - 4.2f,
        std::max(5, stories * 2), woodSlat, darkGlass, false
    );

    // 3. Front Facade Alternating Vertical Modules:
    // Module A (x: -hw .. -hw * 0.42): Projecting White Cantilevered Glass Balconies
    float modA_x0 = -hw + 0.2f;
    float modA_x1 = -hw * 0.38f;
    // Module B (x: -hw * 0.38 .. -hw * 0.02): Full-Height Timber Cladding Core with Horizontal Slots
    float modB_x0 = modA_x1;
    float modB_x1 = 0.05f;
    // Module C (x: 0.05 .. hw * 0.48): Crisp White C-Frame enclosing Recessed Balconies
    float modC_x0 = modB_x1 + 0.35f;
    float modC_x1 = hw * 0.48f;
    // Module D (x: hw * 0.48 .. hw): Interlocking Dark Slate & White U-Band Balconies
    float modD_x0 = modC_x1 + 0.25f;
    float modD_x1 = hw;

    // Vertical dark spine separating Module C & D
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{modC_x1 - 0.2f, plinthY, hd - 1.1f},
        Vec3{modD_x0 + 0.2f, plinthY + totalH, hd + 0.15f},
        slateGrey);

    // Module B: Full-Height Timber Horizontal-Slot Panel
    appendWoodRibbonPanel(
        outVerts, outIndices,
        modB_x0, plinthY + 0.2f, hd - 1.0f,
        modB_x1, plinthY + totalH - 0.2f, hd + 0.10f,
        std::max(6, stories * 2), woodSlat, darkGlass, true
    );

    // Module A: Projecting Stacked Corner Glass Balconies per Story
    for (int s = 1; s < stories; ++s) {
        float fy = plinthY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modA_x0 - 0.45f, fy, hd - 1.2f},
            Vec3{modA_x1, fy + 0.24f, hd + 1.05f},
            crispWhite);
        if (res_scale::showBalconies(params_)) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{modA_x0 - 0.42f, fy + 0.24f, hd + 0.96f},
                Vec3{modA_x1 - 0.05f, fy + 1.18f, hd + 1.04f},
                balcGlass);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{modA_x0 - 0.42f, fy + 0.24f, hd - 0.9f},
                Vec3{modA_x0 - 0.34f, fy + 1.18f, hd + 1.04f},
                balcGlass);
        }
        // Glazed balcony door behind
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modA_x0 + 0.25f, fy + 0.24f, hd - 0.85f},
            Vec3{modA_x1 - 0.25f, fy + flH - 0.25f, hd - 0.72f},
            warmGlass);
    }

    // Module C: Tall Interlocking White C-Frame surrounding Recessed Terraces
    float fThick = 0.28f;
    float mCY1 = plinthY + totalH - 0.35f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{modC_x0, plinthY + 0.8f, hd - 1.2f},
        Vec3{modC_x0 + fThick, mCY1, hd + 0.55f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{modC_x1 - fThick, plinthY + 0.8f, hd - 1.2f},
        Vec3{modC_x1, mCY1, hd + 0.55f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{modC_x0, mCY1 - fThick, hd - 1.2f},
        Vec3{modC_x1, mCY1, hd + 0.55f},
        crispWhite);
    for (int s = 1; s < stories; ++s) {
        float fy = plinthY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modC_x0 + fThick, fy, hd - 1.0f},
            Vec3{modC_x1 - fThick, fy + 0.22f, hd + 0.45f},
            (s == stories - 1) ? darkAlum : crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modC_x0 + fThick + 0.1f, fy + 0.22f, hd + 0.36f},
            Vec3{modC_x1 - fThick - 0.1f, fy + 1.12f, hd + 0.42f},
            balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modC_x0 + fThick + 0.3f, fy + 0.25f, hd - 0.85f},
            Vec3{modC_x1 - fThick - 0.3f, fy + flH - 0.25f, hd - 0.72f},
            darkGlass);
    }

    // Module D: Staggered White Outer Frame + Inner Charcoal U-Frames with Continuous Balconies
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{modD_x0, plinthY, hd - 1.1f},
        Vec3{modD_x0 + fThick, plinthY + totalH - 0.3f, hd + 0.48f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{modD_x1 - fThick, plinthY, hd - 1.1f},
        Vec3{modD_x1, plinthY + totalH - 0.3f, hd + 0.48f},
        crispWhite);
    for (int s = 1; s < stories; ++s) {
        float fy = plinthY + s * flH;
        uint32_t bandMat = (s % 2 == 1) ? crispWhite : darkAlum;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modD_x0 + fThick, fy, hd - 1.0f},
            Vec3{modD_x1 - fThick, fy + 0.24f, hd + 0.44f},
            bandMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modD_x0 + fThick + 0.1f, fy + 0.24f, hd + 0.36f},
            Vec3{modD_x1 - fThick - 0.1f, fy + 1.15f, hd + 0.42f},
            balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{modD_x0 + fThick + 0.35f, fy + 0.25f, hd - 0.85f},
            Vec3{modD_x1 - fThick - 0.35f, fy + flH - 0.25f, hd - 0.72f},
            darkGlass);
    }

    // 4. Right Side Facade & Rear Windows (360-degree architectural finish)
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.03f}, w * 0.88f, fy, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            5, wt, gt, 1.80f, flH * 0.62f, 0.50f,
            0.0f, 0.0f, false, false);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.03f, 0.0f, -0.4f}, d * 0.78f, fy, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            3, wt, gt, 1.55f, flH * 0.60f, 0.50f,
            0.0f, 0.0f, false, false);
    }

    // 5. Signature Deep Overhanging White Flat Roof Fascia over Dark Shadow Trim
    float roofY = plinthY + totalH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.45f, roofY - 0.15f, -hd - 0.45f},
        Vec3{ hw + 0.45f, roofY + 0.25f,  hd + 0.45f},
        slateGrey);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.10f, roofY + 0.25f, -hd - 1.10f},
        Vec3{ hw + 1.10f, roofY + 0.65f,  hd + 1.25f},
        crispWhite);

    // 6. Signature Palm Trees along the Street Frontage
    appendPalmAccent(outVerts, outIndices, -hw * 0.62f, 0.02f, hd + 2.35f, 6.8f, woodSlat, greenPlant);
    appendPalmAccent(outVerts, outIndices,  hw * 0.22f, 0.02f, hd + 2.45f, 7.6f, woodSlat, greenPlant);
    appendPalmAccent(outVerts, outIndices,  hw * 0.78f, 0.02f, hd + 2.15f, 5.4f, woodSlat, greenPlant);
}

} // namespace procedural::buildings
