#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendSlantedCheekWall(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x0, float x1,
    float y0, float y1,
    float zBack, float zFrontBot, float zFrontTop,
    uint32_t mat
) {
    // Bottom, top, back faces + outward left/right trapezoidal cheeks + slanted front face
    Vec3 lbb{x0, y0, zBack}, lbt{x0, y1, zBack}, lfb{x0, y0, zFrontBot}, lft{x0, y1, zFrontTop};
    Vec3 rbb{x1, y0, zBack}, rbt{x1, y1, zBack}, rfb{x1, y0, zFrontBot}, rft{x1, y1, zFrontTop};

    BuildingComponents::appendQuad(outVerts, outIndices, lbb, lfb, lft, lbt, Vec3{-1.0f, 0.0f, 0.0f}, mat);
    BuildingComponents::appendQuad(outVerts, outIndices, rfb, rbb, rbt, rft, Vec3{ 1.0f, 0.0f, 0.0f}, mat);
    Vec3 fNorm = Vec3{0.0f, -(zFrontTop - zFrontBot), (y1 - y0)}.normalized();
    BuildingComponents::appendQuad(outVerts, outIndices, lfb, rfb, rft, lft, fNorm, mat);
    BuildingComponents::appendQuad(outVerts, outIndices, rbb, lbb, lbt, rbt, Vec3{0.0f, 0.0f, -1.0f}, mat);
    BuildingComponents::appendQuad(outVerts, outIndices, lft, rft, rbt, lbt, Vec3{0.0f,  1.0f, 0.0f}, mat);
    BuildingComponents::appendQuad(outVerts, outIndices, lbb, rbb, rfb, lfb, Vec3{0.0f, -1.0f, 0.0f}, mat);
}

} // namespace

void HouseGen::generateSlantedProwModernVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 10.5f, 13.0f);
    float d = res_scale::depth(params_, 11.0f, 15.0f);
    float flH = res_scale::floorHeight(params_, 3.3f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t darkGlass  = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balcGlass  = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t blackFrame = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t timberDoor = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t amberGlow  = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t sidewalk   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t greenLawn  = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);

    GlassType gt = res_scale::glassType(params_, GlassType::SolarTintedSmoke);
    WindowType wt = res_scale::windowType(params_, WindowType::ModernCasement);

    // 1. Yard Curb, Paver Apron & Stepped Plinth
    float plinthY = 0.24f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.15f, -hd - 1.5f},
        Vec3{ hw + 2.0f,  0.02f,  hd + 4.4f}, greenLawn);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.2f, 0.02f, -hd * 0.2f},
        Vec3{ hw + 1.2f, 0.12f, hd + 3.6f}, sidewalk);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd}, Vec3{hw, plinthY, hd}, sidewalk);

    // 2. Main Rectangular Body Behind Slanted Front Zone
    float splitZ = hd - 3.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.15f, plinthY, -hd},
        Vec3{ hw - 0.15f, plinthY + totalH, splitZ}, crispWhite);

    // 3. Signature Forward-Slanted White Trapezoidal Prow Walls & Overhanging Canopy
    float topProjZ = hd + 1.25f;
    float botProjZ = hd - 0.35f;
    float prowTopY = plinthY + totalH + 0.55f;
    // Left forward-raked cheek wall
    appendSlantedCheekWall(outVerts, outIndices,
        -hw, -hw + 0.55f,
        plinthY, prowTopY,
        splitZ, botProjZ, topProjZ, crispWhite);
    // Right forward-raked fin on stair edge
    appendSlantedCheekWall(outVerts, outIndices,
        hw - 0.55f, hw,
        plinthY, prowTopY,
        splitZ, botProjZ, topProjZ, crispWhite);
    // High overhanging prow roof canopy + recessed soffit LED reveal
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, prowTopY - 0.42f, splitZ},
        Vec3{ hw, prowTopY, topProjZ}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.65f, prowTopY - 0.46f, splitZ + 0.4f},
        Vec3{ hw - 0.65f, prowTopY - 0.42f, topProjZ - 0.3f}, amberGlow);

    // 4. Recessed Double-Height Black Curtain-Glass Stairwell Bay (Left Side)
    float midX = -0.35f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.55f, plinthY + 0.12f, splitZ + 0.18f},
        Vec3{midX, plinthY + totalH - 0.25f, splitZ + 0.34f}, darkGlass);
    for (int gy = 1; gy <= 3; ++gy) {
        float yL = plinthY + 0.12f + gy * ((totalH - 0.37f) / 4.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.55f, yL - 0.04f, splitZ + 0.34f},
            Vec3{midX, yL + 0.04f, splitZ + 0.42f}, blackFrame);
    }
    for (int gx = 1; gx <= 2; ++gx) {
        float xL = -hw + 0.55f + gx * ((midX - (-hw + 0.55f)) / 3.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xL - 0.04f, plinthY + 0.12f, splitZ + 0.34f},
            Vec3{xL + 0.04f, plinthY + totalH - 0.25f, splitZ + 0.42f}, blackFrame);
    }

    // 5. Right Bay: Recessed Ground Timber French Porch + Cantilevered Coffered Upper Balcony Box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX, plinthY, splitZ},
        Vec3{hw - 0.55f, plinthY + flH, splitZ + 0.20f}, crispWhite);
    // Warm timber French double entry doors + side window
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX + 0.38f, plinthY, splitZ + 0.18f},
        Vec3{midX + 1.98f, plinthY + 2.45f, splitZ + 0.30f}, timberDoor);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX + 0.55f, plinthY + 0.25f, splitZ + 0.28f},
        Vec3{midX + 1.81f, plinthY + 2.28f, splitZ + 0.34f}, darkGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX + 2.55f, plinthY + 0.55f, splitZ + 0.18f},
        Vec3{hw - 1.05f, plinthY + 2.35f, splitZ + 0.30f}, darkGlass);

    // Front ground terrace safety rail (horizontal steel bars + posts)
    for (float px : {-hw + 0.85f, -hw + 2.6f, midX, midX + 2.4f, hw - 0.85f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.03f, plinthY, hd - 0.15f},
            Vec3{px + 0.03f, plinthY + 0.92f, hd - 0.09f}, blackFrame);
    }
    for (int bar = 1; bar <= 3; ++bar) {
        float by = plinthY + bar * 0.26f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.55f, by - 0.015f, hd - 0.13f},
            Vec3{midX + 0.1f, by + 0.015f, hd - 0.10f}, blackFrame);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{midX + 2.3f, by - 0.015f, hd - 0.13f},
            Vec3{hw - 0.55f, by + 0.015f, hd - 0.10f}, blackFrame);
    }

    // Cantilevered Upper-Right Balcony Box Frame
    float uY0 = plinthY + flH;
    float uY1 = plinthY + totalH;
    float bFrontZ = hd + 0.65f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX - 0.15f, uY0 - 0.14f, splitZ},
        Vec3{hw + 0.20f, uY0 + 0.24f, bFrontZ}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX - 0.15f, uY1 - 0.36f, splitZ},
        Vec3{hw + 0.20f, uY1 + 0.08f, bFrontZ}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.22f, uY0 + 0.24f, splitZ},
        Vec3{hw + 0.20f, uY1 - 0.36f, bFrontZ}, crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX - 0.15f, uY0 + 0.24f, splitZ},
        Vec3{midX + 0.25f, uY1 - 0.36f, bFrontZ}, crispWhite);
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{midX + 0.25f, uY0 + 0.24f, bFrontZ - 0.14f},
            Vec3{hw - 0.22f, uY0 + 1.18f, bFrontZ - 0.06f}, balcGlass);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX + 0.45f, uY0 + 0.24f, splitZ + 0.15f},
        Vec3{hw - 0.55f, uY1 - 0.42f, splitZ + 0.28f}, darkGlass);
    // Amber vertical accent light strip on corner reveal
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.62f, plinthY + 0.15f, splitZ + 0.35f},
        Vec3{hw - 0.56f, uY0 - 0.14f, splitZ + 0.42f}, amberGlow);

    // 6. Side & Rear Windows + Adaptive Roof
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd}, w - 1.2f, fy, flH,
            Vec3{0.0f, 0.0f, -1.0f}, Vec3{-1.0f, 0.0f, 0.0f}, 3, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{-hw + 0.15f, 0.0f, -1.5f}, d * 0.60f, fy, flH,
            Vec3{-1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, -1.0f}, 4, wt, gt);
        res_scale::appendScaledWindowRow(outVerts, outIndices, params_,
            Vec3{hw - 0.15f, 0.0f, -1.5f}, d * 0.60f, fy, flH,
            Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f}, 4, wt, gt);
    }
    res_scale::appendAdaptiveRoof(outVerts, outIndices, params_,
        w - 0.3f, d - 3.2f, plinthY + totalH, RoofType::FlatParapet, 0.48f, 0.0f);
}

} // namespace procedural::buildings
