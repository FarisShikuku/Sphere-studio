#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendWingWindowBand(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float startX,
    float endX,
    float planeZ,
    float floorBaseY,
    float flH,
    const Vec3& normal,
    const Vec3& tangent,
    int bays,
    WindowType winType,
    GlassType glassType,
    uint32_t pilasterMat,
    float fillRatioW,
    float winHeightRatio,
    float sillFrac
) {
    float span = std::abs(endX - startX);
    if (span < 1.2f || bays <= 0) return;
    float step = span / static_cast<float>(bays);
    float winW = std::max(0.85f, step * fillRatioW);
    float winH = std::max(0.90f, flH * winHeightRatio);
    float winCY = floorBaseY + flH * sillFrac + winH * 0.5f;

    float minX = std::min(startX, endX);
    float maxX = std::max(startX, endX);
    for (int i = 0; i < bays; ++i) {
        float cx = minX + (i + 0.5f) * ((maxX - minX) / static_cast<float>(bays));
        BuildingComponents::generateWindow(
            outVerts, outIndices,
            Vec3{cx, winCY, planeZ},
            winW, winH, normal, tangent, winType, glassType);
    }
    // Vertical architectural pilasters dividing bays cleanly
    for (int i = 0; i <= bays; ++i) {
        float px = minX + i * ((maxX - minX) / static_cast<float>(bays));
        float z0 = (normal.z > 0.0f) ? (planeZ - 0.02f) : (planeZ - 0.14f);
        float z1 = (normal.z > 0.0f) ? (planeZ + 0.14f) : (planeZ + 0.02f);
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{px - 0.12f, floorBaseY + 0.12f, z0},
            Vec3{px + 0.12f, floorBaseY + flH - 0.12f, z1},
            pilasterMat);
    }
}

void appendSideWindowBand(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float planeX,
    float startZ,
    float endZ,
    float floorBaseY,
    float flH,
    const Vec3& normal,
    const Vec3& tangent,
    int bays,
    WindowType winType,
    GlassType glassType,
    uint32_t pilasterMat,
    bool skipCenterDoorBay
) {
    float minZ = std::min(startZ, endZ);
    float maxZ = std::max(startZ, endZ);
    float span = maxZ - minZ;
    if (span < 1.2f || bays <= 0) return;

    float step = span / static_cast<float>(bays);
    float winW = std::max(0.95f, step * 0.74f);
    float winH = std::max(1.05f, flH * 0.64f);
    float winCY = floorBaseY + flH * 0.18f + winH * 0.5f;

    int midIdx = bays / 2;
    for (int i = 0; i < bays; ++i) {
        if (skipCenterDoorBay && i == midIdx) continue;
        float cz = minZ + (i + 0.5f) * step;
        BuildingComponents::generateWindow(
            outVerts, outIndices,
            Vec3{planeX, winCY, cz},
            winW, winH, normal, tangent, winType, glassType);
    }
    for (int i = 0; i <= bays; ++i) {
        float pz = minZ + i * step;
        float x0 = (normal.x > 0.0f) ? (planeX - 0.02f) : (planeX - 0.14f);
        float x1 = (normal.x > 0.0f) ? (planeX + 0.14f) : (planeX + 0.02f);
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{x0, floorBaseY + 0.12f, pz - 0.12f},
            Vec3{x1, floorBaseY + flH - 0.12f, pz + 0.12f},
            pilasterMat);
    }
}

} // namespace

void TowerGen::generateMall(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(32.0f, params_.footprintWidth);
    float d = std::max(24.0f, params_.footprintDepth);
    int32_t stories = std::clamp(params_.stories, 2, 12);
    float flH = std::clamp((params_.storyHeight > 0.5f) ? params_.storyHeight : 4.2f, 3.2f, 6.0f);
    float plinthY = 0.50f;
    float roofTopY = plinthY + stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete  = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t wallLimestone = params_.useExtendedParams
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t glassClear    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassReflect  = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t balcGlass     = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t spandrelGlass = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t frameMat      = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat      = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t aluminumMat   = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t signOrange    = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t signRed       = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t hvacMat       = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t greenPlant    = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat    = static_cast<uint32_t>(MaterialSlot::PlanterBox);

    GlassType gt = params_.useExtendedParams ? params_.glassType : GlassType::ClearFloat;
    WindowType mainUpperWin = params_.useExtendedParams ? params_.windowType : WindowType::ModernCasement;

    // 1. Site Forecourt Apron, Sidewalks & Plinth Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.5f, -0.25f, -hd - 2.5f},
        Vec3{ hw + 4.5f,  0.0f,   hd + 7.2f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.8f, 0.0f, -hd - 0.8f},
        Vec3{ hw + 0.8f, plinthY, hd + 1.2f},
        foundMat);

    // Landscaped Plaza Planter Beds flanking front walkway
    for (float sx : {-1.0f, 1.0f}) {
        float px0 = (sx < 0.0f) ? (-hw + 1.6f) : (9.2f);
        float px1 = (sx < 0.0f) ? (-9.2f) : (hw - 1.6f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0, 0.0f, hd + 2.4f},
            Vec3{px1, 0.42f, hd + 4.2f},
            planterMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0 + 0.25f, 0.42f, hd + 2.65f},
            Vec3{px1 - 0.25f, 0.92f, hd + 3.95f},
            greenPlant);
    }

    // 2. Wing & Atrium Partitioning
    float atriumW = std::clamp(w * 0.38f, 11.0f, 15.5f);
    float wingOuterX = hw;
    float wingInnerX = atriumW * 0.5f; // Each wing connects cleanly to central atrium
    float wingSpan = wingOuterX - wingInnerX;
    float atriumProjZ = hd + 2.4f;

    // West Wing Body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-wingOuterX, plinthY, -hd},
        Vec3{-wingInnerX, roofTopY, hd},
        wallLimestone);
    // East Wing Body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingInnerX, plinthY, -hd},
        Vec3{wingOuterX, roofTopY, hd},
        wallLimestone);
    // Connecting Rear Center Retail Concourse Body (behind atrium)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-wingInnerX, plinthY, -hd},
        Vec3{ wingInnerX, roofTopY - 0.2f, -hd * 0.15f},
        wallConcrete);

    // Horizontal Silver Aluminum Floor Spandrel Cornices Wrapping Both Wings on Every Floor
    for (int s = 1; s <= stories; ++s) {
        float sy = plinthY + s * flH;
        float beltH = (s == stories) ? 0.45f : 0.22f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-wingOuterX - 0.22f, sy - beltH, -hd - 0.22f},
            Vec3{-wingInnerX + 0.10f, sy + 0.14f, hd + 0.22f},
            aluminumMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wingInnerX - 0.10f, sy - beltH, -hd - 0.22f},
            Vec3{wingOuterX + 0.22f, sy + 0.14f, hd + 0.22f},
            aluminumMat);
    }

    // 3. Multi-Level Glazed Curtain Atrium with Interior Mezzanine Galleries & 360° Mullions
    float atriumH = roofTopY + 2.4f;
    float atriumBackZ = -hd * 0.20f;
    // Recessed Interior Floor Gallery Decks visible behind atrium glass
    for (int s = 1; s < stories; ++s) {
        float fy = plinthY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-wingInnerX + 0.8f, fy - 0.22f, atriumBackZ},
            Vec3{ wingInnerX - 0.8f, fy + 0.08f, hd + 0.45f},
            wallConcrete);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-wingInnerX + 1.2f, fy + 0.08f, hd + 0.35f},
            Vec3{ wingInnerX - 1.2f, fy + 1.05f, hd + 0.43f},
            balcGlass);
    }

    // Outer Curtain-Wall Glazing Shell of Atrium (Crisp Blue Reflective Glass + Floor Spandrels)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-wingInnerX, plinthY, atriumBackZ},
        Vec3{ wingInnerX, atriumH, atriumProjZ},
        glassReflect);

    // Full-Height Vertical Mullions on Atrium Front AND Projecting Side Cheeks
    int frontBays = 4;
    for (int i = 0; i <= frontBays; ++i) {
        float fx = -wingInnerX + i * (atriumW / static_cast<float>(frontBays));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.14f, plinthY, atriumProjZ - 0.05f},
            Vec3{fx + 0.14f, atriumH + 0.08f, atriumProjZ + 0.24f},
            frameMat);
    }
    // Side cheek vertical mullions on projecting glass sides (z = hd .. atriumProjZ)
    for (float sx : {-1.0f, 1.0f}) {
        float sideX = sx * wingInnerX;
        for (float mz : {hd + 0.15f, hd + 1.25f, atriumProjZ - 0.1f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sideX - 0.14f, plinthY, mz - 0.12f},
                Vec3{sideX + 0.14f, atriumH + 0.05f, mz + 0.12f},
                frameMat);
        }
    }

    // Atrium Horizontal Floor Spandrel Belts & Dark Glass Spandrels Wrapping All 3 Exposed Faces
    for (int s = 1; s <= stories; ++s) {
        float fy = plinthY + s * flH;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-wingInnerX - 0.18f, fy - 0.24f, hd - 0.05f},
            Vec3{ wingInnerX + 0.18f, fy + 0.22f, atriumProjZ + 0.22f},
            aluminumMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-wingInnerX - 0.12f, fy - 0.14f, hd - 0.02f},
            Vec3{ wingInnerX + 0.12f, fy + 0.12f, atriumProjZ + 0.24f},
            spandrelGlass);
    }
    // Atrium Crown Ring Beam
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-wingInnerX - 0.22f, atriumH - 0.35f, atriumBackZ - 0.1f},
        Vec3{ wingInnerX + 0.22f, atriumH + 0.20f, atriumProjZ + 0.28f},
        aluminumMat);

    // 4. Arched Barrel-Vault Glass & Steel Rib Skylight Crown Atop Atrium
    float vaultR = wingInnerX - 0.6f;
    float vaultBaseY = atriumH + 0.20f;
    int vaultSegs = 10;
    for (int i = 0; i < vaultSegs; ++i) {
        float a0 = (static_cast<float>(i) / vaultSegs) * 3.1415926f;
        float a1 = (static_cast<float>(i + 1) / vaultSegs) * 3.1415926f;
        float x0 = -vaultR * std::cos(a0);
        float y0 = vaultBaseY + vaultR * 0.38f * std::sin(a0);
        float x1 = -vaultR * std::cos(a1);
        float y1 = vaultBaseY + vaultR * 0.38f * std::sin(a1);

        Vec3 p0{x0, y0, atriumProjZ - 0.3f};
        Vec3 p1{x1, y1, atriumProjZ - 0.3f};
        Vec3 p2{x1, y1, atriumBackZ + 0.5f};
        Vec3 p3{x0, y0, atriumBackZ + 0.5f};
        Vec3 n = ((p1 - p0).cross(p3 - p0)).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, n, glassClear);
    }
    // Curved Barrel-Vault Steel Arch Ribs along Atrium Length
    for (int r = 0; r <= 5; ++r) {
        float rz = atriumBackZ + 0.6f + r * ((atriumProjZ - 0.4f - (atriumBackZ + 0.6f)) / 5.0f);
        for (int i = 0; i < vaultSegs; ++i) {
            float a0 = (static_cast<float>(i) / vaultSegs) * 3.1415926f;
            float a1 = (static_cast<float>(i + 1) / vaultSegs) * 3.1415926f;
            Vec3 q0{-vaultR * std::cos(a0), vaultBaseY + vaultR * 0.38f * std::sin(a0), rz};
            Vec3 q1{-vaultR * std::cos(a1), vaultBaseY + vaultR * 0.38f * std::sin(a1), rz};
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices, q0, q1, 0.12f, 0.12f, 6, frameMat, true, true);
        }
    }

    // 5. Entrance Grand Portico Canopy with True Angled Cylindrical Tension Rods & Triple Glazed Doors
    float canopyY = plinthY + flH - 0.42f;
    float canopyHalfW = std::min(wingInnerX + 1.4f, 8.6f);
    float canopyFrontZ = atriumProjZ + 3.4f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-canopyHalfW, canopyY, atriumProjZ - 0.2f},
        Vec3{ canopyHalfW, canopyY + 0.38f, canopyFrontZ},
        aluminumMat);
    // Dark fascia rim on portico canopy + portico drop-off columns
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-canopyHalfW - 0.06f, canopyY + 0.08f, canopyFrontZ - 0.12f},
        Vec3{ canopyHalfW + 0.06f, canopyY + 0.34f, canopyFrontZ + 0.06f},
        frameMat);
    for (float cx : {-canopyHalfW + 0.65f, canopyHalfW - 0.65f}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{cx, plinthY, canopyFrontZ - 0.45f},
            Vec3{cx, canopyY, canopyFrontZ - 0.45f},
            0.22f, 0.22f, 10, aluminumMat, true, true);
        // True angled tension tie rods (using cylinders, never inverted box!)
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{cx, canopyY + 0.38f, canopyFrontZ - 0.35f},
            Vec3{cx, canopyY + 3.15f, atriumProjZ + 0.10f},
            0.06f, 0.06f, 8, frameMat, true, true);
    }

    // Triple Grand Glazed Entrance Doors (Center Double Glass + Flanking Sidelight Doors)
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, plinthY, atriumProjZ},
        4.6f, 3.3f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, false);
    for (float dx : {-4.4f, 4.4f}) {
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{dx, plinthY + 1.75f, atriumProjZ},
            2.6f, 3.1f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::PictureGlass, gt);
    }

    // 6. Comprehensive Multi-Level Retail Windows on ALL Floors & ALL 4 Facades
    int wingFrontBays = std::clamp(static_cast<int>(std::round(wingSpan / 3.4f)), 3, 6);
    int sideBays = std::clamp(static_cast<int>(std::round(d / 4.4f)), 4, 7);
    int rearCenterBays = std::clamp(static_cast<int>(std::round(atriumW / 3.6f)), 3, 5);

    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        WindowType wt = (s == 0) ? WindowType::PictureGlass
                      : (s == stories - 1 ? WindowType::ModernCasement : mainUpperWin);
        float winFillW = (s == 0) ? 0.80f : 0.72f;
        float winFillH = (s == 0) ? 0.72f : 0.62f;
        float sillFrac = (s == 0) ? 0.10f : 0.20f;

        // West & East Wing Front Facade Windows (+Z)
        appendWingWindowBand(outVerts, outIndices,
            -wingOuterX + 0.75f, -wingInnerX - 0.55f, hd,
            fy, flH, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            wingFrontBays, wt, gt, aluminumMat, winFillW, winFillH, sillFrac);
        appendWingWindowBand(outVerts, outIndices,
            wingInnerX + 0.55f, wingOuterX - 0.75f, hd,
            fy, flH, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            wingFrontBays, wt, gt, aluminumMat, winFillW, winFillH, sillFrac);

        // West & East Wing Rear Facade Windows (-Z) + Rear Center Atrium Bay Windows (-Z)
        appendWingWindowBand(outVerts, outIndices,
            -wingOuterX + 0.75f, -wingInnerX - 0.55f, -hd,
            fy, flH, Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            wingFrontBays, wt, gt, aluminumMat, winFillW, winFillH, sillFrac);
        appendWingWindowBand(outVerts, outIndices,
            wingInnerX + 0.55f, wingOuterX - 0.75f, -hd,
            fy, flH, Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            wingFrontBays, wt, gt, aluminumMat, winFillW, winFillH, sillFrac);
        appendWingWindowBand(outVerts, outIndices,
            -wingInnerX + 0.6f, wingInnerX - 0.6f, -hd,
            fy, flH, Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            rearCenterBays, (s == 0 ? WindowType::PictureGlass : WindowType::CurtainWall),
            gt, frameMat, 0.78f, 0.68f, 0.14f);

        // Left (-X) & Right (+X) Side Elevation Windows on EVERY Floor
        bool hasSideDoor = (s == 0);
        appendSideWindowBand(outVerts, outIndices,
            -wingOuterX, -hd + 0.85f, hd - 0.85f,
            fy, flH, Vec3{-1, 0, 0}, Vec3{0, 0, -1},
            sideBays, wt, gt, aluminumMat, hasSideDoor);
        appendSideWindowBand(outVerts, outIndices,
            wingOuterX, -hd + 0.85f, hd - 0.85f,
            fy, flH, Vec3{1, 0, 0}, Vec3{0, 0, 1},
            sideBays, wt, gt, aluminumMat, hasSideDoor);

        if (hasSideDoor) {
            // Secondary Glazed Anchor Entrances on West & East Side Wings + Rear Entrance
            BuildingComponents::generateDoor(outVerts, outIndices,
                Vec3{-wingOuterX, plinthY, 0.0f},
                3.2f, 3.0f, Vec3{-1, 0, 0}, Vec3{0, 0, -1},
                DoorType::DoubleGlassCommercial, true);
            BuildingComponents::generateDoor(outVerts, outIndices,
                Vec3{wingOuterX, plinthY, 0.0f},
                3.2f, 3.0f, Vec3{1, 0, 0}, Vec3{0, 0, 1},
                DoorType::DoubleGlassCommercial, true);
            BuildingComponents::generateDoor(outVerts, outIndices,
                Vec3{0.0f, plinthY, -hd},
                4.2f, 3.1f, Vec3{0, 0, -1}, Vec3{-1, 0, 0},
                DoorType::DoubleGlassCommercial, true);
        }
    }

    // 7. Storefront Awning Canopies & Rooftop Parapet Brand Signage
    // Sleek storefront projecting metal awnings cleanly above Ground Floor display windows
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-wingOuterX + 0.5f, plinthY + flH - 0.42f, hd},
        Vec3{-wingInnerX - 0.4f, plinthY + flH - 0.24f, hd + 1.15f},
        signRed);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingInnerX + 0.4f, plinthY + flH - 0.42f, hd},
        Vec3{wingOuterX - 0.5f, plinthY + flH - 0.24f, hd + 1.15f},
        signOrange);

    // Anchor Brand Illuminated Fascia Sign Boxes mounted cleanly on top parapet above roof cornice
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-wingOuterX + 1.8f, roofTopY + 0.16f, hd + 0.04f},
        Vec3{-wingInnerX - 1.8f, roofTopY + 0.86f, hd + 0.28f},
        signRed);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingInnerX + 1.8f, roofTopY + 0.16f, hd + 0.04f},
        Vec3{wingOuterX - 1.8f, roofTopY + 0.86f, hd + 0.28f},
        signOrange);

    // 8. Full-Perimeter Rooftop Parapet, Wing Skylight Monitors & HVAC Chiller Units
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, roofTopY, -hd}, Vec3{hw, roofTopY + 0.85f, -hd + 0.38f}, wallConcrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, roofTopY, hd - 0.38f}, Vec3{-wingInnerX, roofTopY + 0.85f, hd}, wallConcrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingInnerX, roofTopY, hd - 0.38f}, Vec3{hw, roofTopY + 0.85f, hd}, wallConcrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, roofTopY, -hd}, Vec3{-hw + 0.38f, roofTopY + 0.85f, hd}, wallConcrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.38f, roofTopY, -hd}, Vec3{hw, roofTopY + 0.85f, hd}, wallConcrete);

    // Twin Wing Rooftop Daylight Skylight Monitors
    for (float sx : {-1.0f, 1.0f}) {
        float skCx = sx * (wingInnerX + wingSpan * 0.5f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{skCx - wingSpan * 0.28f, roofTopY, -hd * 0.35f},
            Vec3{skCx + wingSpan * 0.28f, roofTopY + 0.55f, hd * 0.35f},
            frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{skCx - wingSpan * 0.24f, roofTopY + 0.55f, -hd * 0.31f},
            Vec3{skCx + wingSpan * 0.24f, roofTopY + 0.95f, hd * 0.31f},
            glassClear);
        // Rooftop HVAC Chiller Packages rear of each wing skylight
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{skCx - 2.4f, roofTopY, -hd + 2.2f},
            Vec3{skCx + 2.4f, roofTopY + 1.45f, -hd + 5.6f},
            hvacMat);
    }
}

} // namespace procedural::buildings
