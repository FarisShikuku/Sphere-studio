#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendTriangularMaskWedge(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& a, const Vec3& b, const Vec3& c,
    float zBack, float zFront,
    uint32_t mat
) {
    Vec3 aF{a.x, a.y, zFront}, bF{b.x, b.y, zFront}, cF{c.x, c.y, zFront};
    Vec3 aB{a.x, a.y, zBack},  bB{b.x, b.y, zBack},  cB{c.x, c.y, zBack};

    // Front triangle (split quad with collapsed edge) & back triangle
    BuildingComponents::appendQuad(outVerts, outIndices, aF, bF, cF, cF, Vec3{0.0f, 0.0f, 1.0f}, mat);
    BuildingComponents::appendQuad(outVerts, outIndices, bB, aB, cB, cB, Vec3{0.0f, 0.0f, -1.0f}, mat);

    // Side extruded bevel faces around triangle edges
    auto extrudeEdge = [&](const Vec3& p0F, const Vec3& p1F, const Vec3& p0B, const Vec3& p1B) {
        Vec3 e = p1F - p0F;
        Vec3 n = Vec3{e.y, -e.x, 0.0f}.normalized();
        BuildingComponents::appendQuad(outVerts, outIndices, p0B, p1B, p1F, p0F, n, mat);
    };
    extrudeEdge(aF, bF, aB, bB);
    extrudeEdge(bF, cF, bB, cB);
    extrudeEdge(cF, aF, cB, aB);
}

void appendParkedSuv(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float y0, float cz,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat
) {
    float hw = 0.96f, hl = 2.25f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw, y0 + 0.24f, cz - hl},
        Vec3{cx + hw, y0 + 0.98f, cz + hl}, bodyMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.85f, y0 + 0.98f, cz - hl * 0.52f},
        Vec3{cx + hw * 0.85f, y0 + 1.66f, cz + hl * 0.30f}, glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cx - hw * 0.78f, y0 + 1.58f, cz - hl * 0.44f},
        Vec3{cx + hw * 0.78f, y0 + 1.68f, cz + hl * 0.24f}, bodyMat);
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.58f, hl * 0.58f}) {
            float wx = cx + sx * (hw - 0.04f);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.11f, y0 + 0.32f, cz + sz},
                Vec3{wx + 0.11f, y0 + 0.32f, cz + sz},
                0.32f, 0.32f, 10, tireMat, true, true);
        }
    }
}

} // namespace

void HouseGen::generateTriangularVoidGeometricVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 13.0f, 16.5f);
    float d = res_scale::depth(params_, 11.5f, 15.0f);
    float flH = res_scale::floorHeight(params_, 3.45f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t stonePier  = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t timberAcc  = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t blackFrame = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t amberPot   = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t forecourt  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t asphaltMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t greenPlant = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t whiteCar   = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t tireMat    = static_cast<uint32_t>(MaterialSlot::TireRubber);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Cobble Forecourt & Planter Islands
    float plinthY = 0.22f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.6f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.6f,  0.02f,  hd + 5.8f}, asphaltMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, 0.02f, -hd},
        Vec3{ hw + 2.0f, 0.12f, hd + 3.6f}, forecourt);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, crispWhite);

    // Glowing planter pots & green palms flanking the triangular centerpiece
    for (float px : {0.85f, 1.55f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.24f, 0.12f, hd + 0.35f},
            Vec3{px + 0.24f, 0.72f, hd + 0.85f}, amberPot);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{px, 0.72f, hd + 0.60f}, Vec3{px, 1.48f, hd + 0.60f},
            0.35f, 0.12f, 7, greenPlant, true, true);
    }

    // 2. Main Villa Body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.35f, plinthY, -hd},
        Vec3{ hw - 0.35f, plinthY + totalH, hd - 2.0f}, crispWhite);

    // 3. Left Wing: Monumental White Hooded Frame, Textured Stone Pier, Vertical Slats & Entry Portal
    float midSplitX = -1.65f;
    float wingTopY = plinthY + totalH + 0.75f;
    // Left flanking vertical blade wall + hood slab + tall white tower divider
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, plinthY, hd - 2.0f},
        Vec3{-hw + 0.42f, wingTopY, hd + 0.65f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, wingTopY - 0.42f, hd - 2.0f},
        Vec3{midSplitX, wingTopY, hd + 0.65f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midSplitX - 0.55f, plinthY, hd - 2.0f},
        Vec3{midSplitX + 0.10f, wingTopY + 0.28f, hd + 0.85f}, crispWhite);

    // Ground textured dark stone cladding pier + tall recessed entry doorway with timber frame
    float pierX0 = -hw + 0.42f;
    float pierX1 = midSplitX - 2.05f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pierX0, plinthY, hd - 1.55f},
        Vec3{pierX1, plinthY + flH * 0.92f, hd + 0.35f}, stonePier);
    // Side window beside stone pier & double-height timber-trimmed entry door
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pierX1 + 0.05f, plinthY, hd - 1.35f},
        Vec3{midSplitX - 0.55f, plinthY + totalH, hd - 1.20f}, timberAcc);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pierX1 + 0.30f, plinthY + 0.05f, hd - 1.22f},
        Vec3{midSplitX - 0.75f, plinthY + totalH - 0.25f, hd - 1.12f}, darkGlass);

    // Upper left louvered privacy balcony over stone pier
    float uY0 = plinthY + flH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pierX0, plinthY + flH * 0.92f, hd - 1.55f},
        Vec3{pierX1 + 0.15f, uY0 + 0.12f, hd + 0.40f}, crispWhite);
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{pierX0, uY0 + 0.12f, hd + 0.30f},
            Vec3{pierX1 + 0.12f, uY0 + 1.05f, hd + 0.38f}, balcGlass);
    }
    // Vertical charcoal slat louver screen behind left balcony
    for (int sl = 0; sl < 9; ++sl) {
        float sx = pierX0 + 0.18f + sl * ((pierX1 - pierX0 - 0.25f) / 9.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx, uY0 + 0.15f, hd - 0.95f},
            Vec3{sx + 0.08f, wingTopY - 0.42f, hd - 0.78f}, blackFrame);
    }

    // 4. Center Signature Sculpted Triangular Void Exposing 2-Storey Black-Gridded Curtain Wall
    float triX0 = midSplitX + 0.10f;
    float triX1 = hw - 0.65f;
    float triY0 = plinthY;
    float triY1 = plinthY + totalH + 0.35f;
    float glassZ = hd - 0.95f;

    // 2-storey full gridded glass behind triangular cutout
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{triX0 + 0.25f, triY0 + 0.18f, glassZ - 0.08f},
        Vec3{triX1 - 0.25f, triY1 - 0.25f, glassZ + 0.06f}, darkGlass);
    for (int gy = 1; gy <= 6; ++gy) {
        float yL = triY0 + 0.18f + gy * ((triY1 - triY0 - 0.43f) / 7.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{triX0 + 0.25f, yL - 0.04f, glassZ + 0.05f},
            Vec3{triX1 - 0.25f, yL + 0.04f, glassZ + 0.12f}, blackFrame);
    }
    for (int gx = 1; gx <= 4; ++gx) {
        float xL = triX0 + 0.25f + gx * ((triX1 - triX0 - 0.50f) / 5.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xL - 0.04f, triY0 + 0.18f, glassZ + 0.05f},
            Vec3{xL + 0.04f, triY1 - 0.25f, glassZ + 0.12f}, blackFrame);
    }

    // Left vertical crisp white reveal cheek & bottom right sloping sill wedge & top left diagonal mask wedge
    float maskZBack = glassZ + 0.14f;
    float maskZFront = hd + 0.55f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{triX0, triY0, maskZBack},
        Vec3{triX0 + 0.58f, triY1, maskZFront}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{triX1 - 0.52f, triY0, maskZBack},
        Vec3{triX1, triY1, maskZFront}, crispWhite);

    // Upper diagonal white geometric triangular panel sloping from top-left down toward center-right
    float apexX = triX0 + 0.72f;
    float apexTopY = triY1 - 0.65f;
    float midRightY = triY0 + totalH * 0.48f;
    appendTriangularMaskWedge(outVerts, outIndices,
        Vec3{apexX, triY1, 0.0f},
        Vec3{triX1 - 0.45f, triY1, 0.0f},
        Vec3{triX1 - 0.45f, midRightY, 0.0f},
        maskZBack, maskZFront, crispWhite);
    appendTriangularMaskWedge(outVerts, outIndices,
        Vec3{apexX, triY1, 0.0f},
        Vec3{triX1 - 0.45f, midRightY, 0.0f},
        Vec3{apexX, apexTopY, 0.0f},
        maskZBack, maskZFront, crispWhite);

    // Lower diagonal white geometric triangular knee sloping up from bottom-left to bottom-right
    appendTriangularMaskWedge(outVerts, outIndices,
        Vec3{triX0 + 0.52f, triY0 + 0.85f, 0.0f},
        Vec3{triX1 - 0.45f, triY0 + totalH * 0.34f, 0.0f},
        Vec3{triX1 - 0.45f, triY0, 0.0f},
        maskZBack, maskZFront, crispWhite);
    appendTriangularMaskWedge(outVerts, outIndices,
        Vec3{triX0 + 0.52f, triY0 + 0.85f, 0.0f},
        Vec3{triX1 - 0.45f, triY0, 0.0f},
        Vec3{triX0 + 0.52f, triY0, 0.0f},
        maskZBack, maskZFront, crispWhite);

    // 5. Right Corner Cantilevered Balcony with Pergola Louver Roof
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.35f, uY0 - 0.14f, -hd * 0.35f},
        Vec3{hw + 1.05f, uY0 + 0.22f, hd - 1.45f}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.35f, plinthY + totalH - 0.32f, -hd * 0.35f},
        Vec3{hw + 1.05f, plinthY + totalH + 0.08f, hd - 1.45f}, crispWhite);
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw + 0.94f, uY0 + 0.22f, -hd * 0.35f},
            Vec3{hw + 1.02f, uY0 + 1.15f, hd - 1.45f}, balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw - 0.35f, uY0 + 0.22f, hd - 1.53f},
            Vec3{hw + 1.02f, uY0 + 1.15f, hd - 1.45f}, balcGlass);
    }

    // Parked Luxury SUVs Flanking Forecourt
    if (params_.hasGarage) {
        appendParkedSuv(outVerts, outIndices, -hw - 0.75f, 0.12f, hd + 1.25f,
            whiteCar, darkGlass, tireMat);
        appendParkedSuv(outVerts, outIndices, hw + 1.25f, 0.12f, hd + 0.65f,
            whiteCar, darkGlass, tireMat);
    }

    // 6. Rear & Side Facade Windows + Roof Cap
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.2f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 4, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{-hw + 0.35f, 0.0f, -1.0f}, d * 0.55f, fy, flH,
            Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 2, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{hw - 0.35f, 0.0f, -2.2f}, d * 0.42f, fy, flH,
            Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f}, 2, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.7f, d - 2.0f, plinthY + totalH, RoofType::FlatParapet, 0.48f, 0.0f);
}

} // namespace procedural::buildings
