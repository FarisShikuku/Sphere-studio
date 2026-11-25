#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendCompactSuv(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float yGround, float cz,
    float length, float width, float height,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat
) {
    float hw = width * 0.5f;
    float hl = length * 0.5f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw, yGround + 0.22f, cz - hl},
        Vec3{cx + hw, yGround + height * 0.56f, cz + hl},
        bodyMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw * 0.85f, yGround + height * 0.56f, cz - hl * 0.50f},
        Vec3{cx + hw * 0.85f, yGround + height, cz + hl * 0.34f},
        glassMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw * 0.78f, yGround + height - 0.05f, cz - hl * 0.44f},
        Vec3{cx + hw * 0.78f, yGround + height + 0.02f, cz + hl * 0.28f},
        bodyMat
    );
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.60f, hl * 0.60f}) {
            float wx = cx + sx * (hw - 0.05f);
            float wy = yGround + 0.32f;
            float wz = cz + sz;
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.12f, wy, wz},
                Vec3{wx + 0.12f, wy, wz},
                0.32f, 0.32f, 12, tireMat, true, true
            );
        }
    }
}

} // namespace

void HouseGen::generateHaloCanopyVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 12.0f, 18.5f);
    float d = res_scale::depth(params_, 9.0f, 12.5f);
    float flH = res_scale::floorHeight(params_, 3.40f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite   = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t limestone    = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t blackAlum    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t basaltWall   = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t warmGold     = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t bronzeTrim   = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
    uint32_t clearGlass   = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t balcGlass    = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t frameMetal   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t greenPlant   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterBox   = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t tarmacMat    = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t vehicleWhite = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t tireMat      = static_cast<uint32_t>(MaterialSlot::TireRubber);

    GlassType gt = res_scale::glassType(params_, GlassType::GoldMirroredLuxury);
    WindowType wt = res_scale::windowType(params_, WindowType::PictureGlass);

    float splitX = 0.0f; // Left = Entry & Double-Height Atrium; Right = Open Carport + Upper Green Balcony
    float plinthY = 0.16f;

    // 1. Front Yard Lawn, Light Gravel Carport Driveway & Floating Entry Stepping Stones
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.4f, -0.18f, -hd - 1.2f},
        Vec3{ hw + 2.4f,  0.02f,  hd + 5.8f},
        tarmacMat);
    // Green front lawn on left entry side
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, 0.02f, hd - 1.0f},
        Vec3{splitX + 0.6f, 0.08f, hd + 4.4f},
        greenPlant);
    // Light aggregate paved driveway apron on right garage side
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX + 0.6f, 0.02f, -hd * 0.4f},
        Vec3{hw + 1.8f, 0.10f, hd + 4.4f},
        limestone);

    // 5 Floating Entry Stepping Stones leading up to the double-height fluted pivot door
    for (int st = 0; st < 5; ++st) {
        float sz = hd + 3.8f - st * 0.78f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{splitX - 3.6f, 0.08f + st * 0.02f, sz - 0.30f},
            Vec3{splitX + 0.15f, 0.16f + st * 0.02f, sz + 0.30f},
            crispWhite);
    }

    // Front-left raised tropical planter box + black marble accent feature wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.2f, 0.06f, hd - 0.6f},
        Vec3{splitX - 3.8f, 0.62f, hd + 2.2f},
        limestone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.0f, 0.62f, hd - 0.45f},
        Vec3{splitX - 3.9f, 1.32f, hd + 2.05f},
        greenPlant);
    // Bold black marble slab wall behind left planters with gold vein trim
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, plinthY, hd - 1.8f},
        Vec3{splitX - 4.0f, plinthY + flH * 1.15f, hd - 1.35f},
        basaltWall);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.55f, plinthY + 1.8f, hd - 1.36f},
        Vec3{splitX - 4.2f, plinthY + 1.88f, hd - 1.31f},
        bronzeTrim);

    // 2. Main Structural Envelope (Behind Atrium & Carport)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.5f, plinthY, -hd},
        Vec3{ hw, plinthY + totalH, hd - 3.2f},
        crispWhite);

    // 3. Left-Center Signature Double-Height Glazed Atrium + Fluted Monolith Pivot Door
    float doorW = 1.95f;
    float doorH = totalH - 1.15f;
    float doorX0 = splitX - 2.15f;
    float doorX1 = doorX0 + doorW;

    // Towering vertical fluted white/cream monolithic pivot entry door
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorX0, plinthY, hd - 0.25f},
        Vec3{doorX1, plinthY + doorH, hd + 0.05f},
        limestone);
    for (int fl = 0; fl < 10; ++fl) {
        float fx = doorX0 + 0.12f + fl * ((doorW - 0.24f) / 10.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx, plinthY + 0.08f, hd + 0.03f},
            Vec3{fx + 0.04f, plinthY + doorH - 0.08f, hd + 0.08f},
            crispWhite);
    }
    // Warm recessed soffit spot trio above pivot door
    for (int dDot = 0; dDot < 3; ++dDot) {
        float dx = doorX0 + 0.45f + dDot * 0.50f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{dx - 0.08f, plinthY + doorH - 0.18f, hd + 0.06f},
            Vec3{dx + 0.08f, plinthY + doorH - 0.04f, hd + 0.12f},
            warmGold);
    }

    // Black marble column with warm vertical LED light strip flanking left of entry door
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorX0 - 0.65f, plinthY, hd - 0.35f},
        Vec3{doorX0 - 0.12f, plinthY + doorH + 0.15f, hd + 0.15f},
        basaltWall);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorX0 - 0.44f, plinthY + 0.05f, hd + 0.14f},
        Vec3{doorX0 - 0.34f, plinthY + doorH + 0.10f, hd + 0.19f},
        warmGold);

    // Tall Double-Height Glass Corner Atrium left of marble column
    float atriumX0 = splitX - 5.45f;
    float atriumX1 = doorX0 - 0.65f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{atriumX0, plinthY, hd - 3.2f},
        Vec3{atriumX1, plinthY + doorH, hd - 0.10f},
        clearGlass);
    // Warm glowing circular chandelier visible inside atrium
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{0.5f * (atriumX0 + atriumX1), plinthY + doorH - 1.15f, hd - 1.4f},
        Vec3{0.5f * (atriumX0 + atriumX1), plinthY + doorH - 0.98f, hd - 1.4f},
        0.72f, 0.72f, 16, warmGold, true, true
    );

    // 4. Right Side Ground Open Carport (Slender white corner column + slit windows on back wall)
    float carportBackZ = hd - 4.2f;
    // Carport rear wall with 4 tall narrow illuminated vertical slot windows
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{splitX + 0.4f, plinthY, carportBackZ - 0.35f},
        Vec3{hw - 0.2f, plinthY + flH, carportBackZ},
        limestone);
    for (int sw = 0; sw < 4; ++sw) {
        float sx = splitX + 1.1f + sw * ((hw - splitX - 1.8f) / 3.5f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - 0.16f, plinthY + 0.85f, carportBackZ - 0.05f},
            Vec3{sx + 0.16f, plinthY + flH - 0.45f, carportBackZ + 0.08f},
            warmGold);
    }
    // Slender right front structural corner column supporting the upper carport overhang
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.35f, 0.10f, hd + 0.30f},
        Vec3{hw + 0.05f, plinthY + flH, hd + 0.70f},
        crispWhite);

    // Charcoal projecting upper carport slab fascia dividing ground garage & upper terrace
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorX1 - 0.15f, plinthY + flH - 0.38f, carportBackZ},
        Vec3{hw + 0.55f, plinthY + flH + 0.24f, hd + 0.95f},
        blackAlum);

    // Twin SUVs parked in carport (crisp White SUV on left + Dark Black SUV on right)
    if (params_.hasGarage) {
        float bayW = hw - splitX;
        appendCompactSuv(
            outVerts, outIndices,
            splitX + bayW * 0.34f, 0.10f, hd - 0.95f,
            4.35f, 1.92f, 1.66f,
            vehicleWhite, static_cast<uint32_t>(MaterialSlot::GlassTinted), tireMat
        );
        appendCompactSuv(
            outVerts, outIndices,
            splitX + bayW * 0.72f, 0.10f, hd - 1.35f,
            4.25f, 1.88f, 1.56f,
            blackAlum, static_cast<uint32_t>(MaterialSlot::GlassTinted), tireMat
        );
    }

    // 5. Upper Right Recessed Balcony Terrace + Living Vertical Green Wall
    float uY0 = plinthY + flH + 0.24f;
    float uY1 = plinthY + totalH;
    // White crisp C-frame enclosing upper-right balcony wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorX1 - 0.1f, uY1 - 0.35f, carportBackZ},
        Vec3{hw + 0.15f, uY1 + 0.45f, hd + 0.45f},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.45f, uY0, carportBackZ},
        Vec3{hw + 0.15f, uY1 + 0.45f, hd + 0.45f},
        crispWhite);

    // Warm backlit panoramic curtain glass on left half of upper balcony
    float balcMidX = 0.5f * (doorX1 + hw - 0.45f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{doorX1 + 0.15f, uY0, hd - 2.6f},
        Vec3{balcMidX + 0.4f, uY1 - 0.35f, hd - 2.4f},
        warmGold);
    // Signature Living Vertical Green Garden Wall filling right half of upper balcony alcove
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{balcMidX + 0.4f, uY0, hd - 2.55f},
        Vec3{hw - 0.50f, uY1 - 0.35f, hd - 2.05f},
        greenPlant);

    // Frameless glass balustrade across upper balcony opening
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{doorX1 + 0.1f, uY0, hd + 0.32f},
            Vec3{hw - 0.45f, uY0 + 1.05f, hd + 0.38f},
            balcGlass);
    }

    // 6. SIGNATURE CANTILEVERED CHARCOAL HALO CANOPY WITH CONTINUOUS WARM LED UNDER-GLOW
    float haloY0 = plinthY + doorH;
    float haloY1 = haloY0 + 0.58f;
    float haloX0 = -hw - 1.35f;
    float haloX1 = balcMidX + 0.55f;
    float haloZ0 = hd - 3.6f;
    float haloZ1 = hd + 1.65f;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{haloX0, haloY0, haloZ0},
        Vec3{haloX1, haloY1, haloZ1},
        blackAlum);
    // Continuous warm perimeter LED halo channel recessed along soffit underside
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{haloX0 + 0.45f, haloY0 - 0.04f, haloZ1 - 0.42f},
        Vec3{haloX1 - 0.35f, haloY0 + 0.02f, haloZ1 - 0.30f},
        warmGold);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{haloX0 + 0.45f, haloY0 - 0.04f, haloZ0 + 0.25f},
        Vec3{haloX0 + 0.57f, haloY0 + 0.02f, haloZ1 - 0.30f},
        warmGold);

    // Upper crisp white penthouse parapet crown projecting behind the dark halo roof
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.42f, haloY1, -hd * 0.65f},
        Vec3{ hw + 0.15f, haloY1 + 0.85f, hd - 0.55f},
        crispWhite);

    // 7. Rear & Side Windows + Tropical Palm Tree Frame
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.03f}, w * 0.82f, fy, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.85f, flH * 0.64f, 0.45f,
            0.0f, 0.0f, false, false);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.03f, 0.0f, -hd * 0.35f}, d * 0.45f, fy, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.45f, flH * 0.62f, 0.48f,
            0.0f, 0.0f, false, false);
    }

    // Tall palm tree on front-left yard
    float palmX = -hw - 1.55f;
    float palmZ = hd + 1.85f;
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{palmX, 0.08f, palmZ}, Vec3{palmX + 0.15f, totalH + 1.4f, palmZ},
        0.22f, 0.13f, 10, static_cast<uint32_t>(MaterialSlot::WallTimber), true, true
    );
    for (int f = 0; f < 8; ++f) {
        float ang = 6.2831853f * static_cast<float>(f) / 8.0f;
        Vec3 pMid{palmX + 0.15f + std::cos(ang) * 1.1f, totalH + 1.75f, palmZ + std::sin(ang) * 1.1f};
        Vec3 pTip{palmX + 0.15f + std::cos(ang) * 2.1f, totalH + 1.15f, palmZ + std::sin(ang) * 2.1f};
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices, Vec3{palmX + 0.15f, totalH + 1.4f, palmZ}, pMid,
            0.10f, 0.06f, 6, greenPlant, true, true
        );
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices, pMid, pTip,
            0.06f, 0.02f, 6, greenPlant, true, true
        );
    }
}

} // namespace procedural::buildings
