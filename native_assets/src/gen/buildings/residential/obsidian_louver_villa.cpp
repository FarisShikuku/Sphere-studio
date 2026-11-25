#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendStackedStonePier(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    uint32_t coreMat,
    uint32_t courseMat
) {
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{x0, y0, z0}, Vec3{x1, y1, z1}, coreMat);
    float courseStep = 0.24f;
    int idx = 0;
    for (float cy = y0 + 0.14f; cy + 0.08f < y1; cy += courseStep, ++idx) {
        float proud = (idx % 2 == 0) ? 0.045f : 0.02f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{x0 - proud, cy, z0 - proud},
            Vec3{x1 + proud, cy + 0.09f, z1 + proud},
            (idx % 3 == 0) ? courseMat : coreMat
        );
    }
}

void appendStylizedPalmTree(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x, float y0, float z,
    float height,
    uint32_t trunkMat,
    uint32_t leafMat
) {
    int segs = 5;
    float segH = height / static_cast<float>(segs);
    float curX = x;
    float curY = y0;
    float curZ = z;
    for (int i = 0; i < segs; ++i) {
        float r0 = 0.16f - i * 0.018f;
        float r1 = 0.14f - i * 0.018f;
        float nextX = curX + 0.06f;
        float nextY = curY + segH;
        float nextZ = curZ + ((i % 2 == 0) ? 0.04f : -0.02f);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{curX, curY, curZ},
            Vec3{nextX, nextY, nextZ},
            r0, r1, 8, trunkMat, true, true
        );
        curX = nextX;
        curY = nextY;
        curZ = nextZ;
    }

    // Palm crown cluster + arching frond blades
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{curX, curY - 0.1f, curZ},
        Vec3{curX, curY + 0.35f, curZ},
        0.18f, 0.06f, 8, leafMat, true, true
    );
    const int fronds = 7;
    const float twoPi = 6.2831853f;
    for (int f = 0; f < fronds; ++f) {
        float ang = twoPi * static_cast<float>(f) / static_cast<float>(fronds);
        float dx = std::cos(ang);
        float dz = std::sin(ang);
        Vec3 pMid{curX + dx * 0.85f, curY + 0.38f, curZ + dz * 0.85f};
        Vec3 pTip{curX + dx * 1.65f, curY - 0.18f, curZ + dz * 1.65f};
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{curX, curY + 0.25f, curZ}, pMid,
            0.09f, 0.06f, 6, leafMat, true, true
        );
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            pMid, pTip,
            0.06f, 0.015f, 6, leafMat, true, true
        );
    }
}

void appendParkedLuxuryCarSilhouette(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float yGround, float cz,
    float length, float width, float height,
    uint32_t bodyMat, uint32_t glassMat, uint32_t tireMat, uint32_t headLightMat
) {
    float hw = width * 0.5f;
    float hl = length * 0.5f;
    // Lower chassis
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw, yGround + 0.22f, cz - hl},
        Vec3{cx + hw, yGround + height * 0.56f, cz + hl},
        bodyMat
    );
    // Upper greenhouse cabin
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw * 0.86f, yGround + height * 0.56f, cz - hl * 0.55f},
        Vec3{cx + hw * 0.86f, yGround + height, cz + hl * 0.32f},
        glassMat
    );
    // Roof cap
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw * 0.80f, yGround + height - 0.06f, cz - hl * 0.50f},
        Vec3{cx + hw * 0.80f, yGround + height + 0.02f, cz + hl * 0.26f},
        bodyMat
    );
    // Front grille & twin headlights
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw * 0.36f, yGround + 0.32f, cz + hl - 0.02f},
        Vec3{cx + hw * 0.36f, yGround + 0.62f, cz + hl + 0.04f},
        static_cast<uint32_t>(MaterialSlot::FrameMetal)
    );
    for (float side : {-1.0f, 1.0f}) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{cx + side * hw * 0.68f - 0.14f, yGround + 0.42f, cz + hl - 0.02f},
            Vec3{cx + side * hw * 0.68f + 0.14f, yGround + 0.56f, cz + hl + 0.05f},
            headLightMat
        );
    }
    // Four wheels
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.62f, hl * 0.62f}) {
            float wx = cx + sx * (hw - 0.05f);
            float wy = yGround + 0.31f;
            float wz = cz + sz;
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.12f, wy, wz},
                Vec3{wx + 0.12f, wy, wz},
                0.31f, 0.31f, 12, tireMat, true, true
            );
        }
    }
}

} // namespace

void HouseGen::generateObsidianLouverVilla(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 11.0f, 18.0f);
    float d = res_scale::depth(params_, 8.5f, 12.5f);
    float flH = res_scale::floorHeight(params_, 3.25f);
    int stories = res_scale::stories(params_, 2, 3);
    float totalH = stories * flH;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t basaltWall   = res_scale::wallMat(params_, MaterialSlot::WallBasalt);
    uint32_t blackAlum    = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t gunmetal     = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t bronzeFins   = static_cast<uint32_t>(MaterialSlot::WallBronzeAluminum);
    uint32_t crispWhite   = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t stonePlinth  = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t warmGlass    = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t balcGlass    = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t frameMetal   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t timberMat    = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t tarmacMat    = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t planterBox   = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t greenPlant   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t tireMat      = static_cast<uint32_t>(MaterialSlot::TireRubber);

    GlassType gt = res_scale::glassType(params_, GlassType::GoldMirroredLuxury);
    WindowType wt = res_scale::windowType(params_, WindowType::PictureGlass);

    // 1. Driveway Apron, Curb & Tropical Landscaped Borders
    float plinthY = 0.20f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.4f, -0.18f, -hd - 1.0f},
        Vec3{ hw + 1.6f, 0.02f,  hd + 5.2f},
        tarmacMat);
    // Front sidewalk curb strip
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.4f, 0.02f, hd + 4.35f},
        Vec3{ hw + 1.6f, 0.14f, hd + 5.20f},
        gunmetal);
    // Structural foundation plinth under house
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.3f, 0.0f, -hd - 0.3f},
        Vec3{ hw + 0.3f, plinthY, hd + 0.4f},
        stonePlinth);

    // Left front planter island
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.1f, 0.02f, hd + 1.8f},
        Vec3{-hw + 2.2f, 0.24f, hd + 3.9f},
        planterBox);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.95f, 0.24f, hd + 1.95f},
        Vec3{-hw + 2.05f, 0.48f, hd + 3.75f},
        greenPlant);

    // Right front tropical garden strip along right ground facade
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{0.4f, 0.02f, hd + 0.65f},
        Vec3{hw + 1.4f, 0.24f, hd + 2.20f},
        planterBox);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{0.55f, 0.24f, hd + 0.78f},
        Vec3{hw + 1.25f, 0.55f, hd + 2.05f},
        greenPlant);

    // 2. Architectural Bay Partitioning
    // Left bay (x: -hw .. 0.35): Deep recessed carport below + full-height louver screen above
    // Right-center bay (x: 0.35 .. hw * 0.56): Recessed ground entry + White C-Frame upper terrace portal
    // Far-right bay (x: hw * 0.56 .. hw): Stacked stone ground wall + vertical dark fin screen above
    float carportRightX = w * 0.04f;
    float portalLeftX   = carportRightX - 0.65f;
    float portalRightX  = hw * 0.56f;

    // 3. Textured Stacked-Stone Corner Piers (Signature Left Pier & Far-Right Perimeter Wall)
    appendStackedStonePier(
        outVerts, outIndices,
        -hw - 0.25f, plinthY, -hd - 0.1f,
        -hw + 1.35f, plinthY + totalH + 0.45f, hd + 0.85f,
        stonePlinth, gunmetal
    );
    // Low projecting foreground stone gate pier on left
    appendStackedStonePier(
        outVerts, outIndices,
        -hw - 0.35f, plinthY, hd + 0.75f,
        -hw + 0.95f, plinthY + flH * 0.96f, hd + 2.15f,
        stonePlinth, gunmetal
    );
    // Ground-right stacked stone base wall
    appendStackedStonePier(
        outVerts, outIndices,
        carportRightX - 0.4f, plinthY, hd - 0.85f,
        hw + 0.15f, plinthY + flH * 0.92f, hd + 0.25f,
        stonePlinth, gunmetal
    );
    // Far-right freestanding courtyard stone pier
    appendStackedStonePier(
        outVerts, outIndices,
        hw + 0.35f, plinthY, -hd * 0.35f,
        hw + 1.15f, plinthY + flH * 0.95f, hd * 0.40f,
        stonePlinth, gunmetal
    );

    // 4. Main Structural Rear & Core Massing (Watertight enclosure behind open bays)
    float recessDepth = std::min(5.2f, d * 0.46f);
    float carportBackZ = hd - recessDepth;

    // Ground rear enclosed mass behind carport & entry vestibule
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.4f, plinthY, -hd},
        Vec3{ hw, plinthY + flH, carportBackZ},
        basaltWall);

    // Right ground wing enclosure
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{carportRightX - 0.4f, plinthY, carportBackZ},
        Vec3{hw, plinthY + flH, hd - 0.25f},
        basaltWall);

    // Carport dividing pier + glazed entry vestibule back wall inside carport alcove
    appendStackedStonePier(
        outVerts, outIndices,
        -1.2f, plinthY, carportBackZ - 0.2f,
        0.55f, plinthY + flH, carportBackZ + 0.65f,
        stonePlinth, gunmetal
    );
    // Warm-lit recessed double glass entry door at rear-right of carport
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.55f, plinthY, carportBackZ + 0.04f},
        Vec3{ 1.55f, plinthY + 2.55f, carportBackZ + 0.20f},
        warmGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{0.46f, plinthY, carportBackZ + 0.02f},
        Vec3{0.56f, plinthY + 2.58f, carportBackZ + 0.24f},
        blackAlum);
    // Vertical timber accent screen adjacent to entry vestibule
    for (int s = 0; s < 6; ++s) {
        float sx = 1.75f + s * 0.16f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx, plinthY, carportBackZ + 0.06f},
            Vec3{sx + 0.07f, plinthY + flH - 0.25f, carportBackZ + 0.22f},
            timberMat);
    }

    // Garage carport side ceiling fascia + projecting cantilevered soffit canopy
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.35f, plinthY + flH - 0.48f, carportBackZ},
        Vec3{ hw + 0.15f, plinthY + flH + 0.18f, hd + 0.92f},
        blackAlum);
    // Signature warm recessed linear LED soffit glow strip under ground fascia
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.35f, plinthY + flH - 0.51f, hd + 0.55f},
        Vec3{ carportRightX + 1.2f, plinthY + flH - 0.44f, hd + 0.72f},
        warmGlass);

    // Optional two dark luxury SUV/sedan silhouettes parked inside open double carport
    if (params_.hasGarage) {
        float bayW = (carportRightX - (-hw + 1.35f));
        float car1X = -hw + 1.35f + bayW * 0.28f;
        float car2X = -hw + 1.35f + bayW * 0.68f;
        appendParkedLuxuryCarSilhouette(
            outVerts, outIndices,
            car1X, 0.02f, hd - 1.15f,
            4.35f, 1.92f, 1.62f,
            blackAlum, static_cast<uint32_t>(MaterialSlot::GlassTinted), tireMat, warmGlass
        );
        appendParkedLuxuryCarSilhouette(
            outVerts, outIndices,
            car2X, 0.02f, hd - 1.45f,
            4.25f, 1.88f, 1.54f,
            gunmetal, static_cast<uint32_t>(MaterialSlot::GlassTinted), tireMat, warmGlass
        );
    }

    // 5. Upper Level Volumes (Stories >= 2)
    float uY0 = plinthY + flH;
    float upperTotalH = (stories - 1) * flH;
    float uY1 = plinthY + totalH;

    // Enclosed main upper interior box set slightly behind the louvered & terrace zones
    float louverWallZ = hd - 1.05f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.35f, uY0, -hd},
        Vec3{ hw + 0.05f, uY1, louverWallZ},
        basaltWall);

    // Left upper floor warm golden glazing wall behind full-height vertical louvers
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.35f, uY0 + 0.20f, louverWallZ - 0.04f},
        Vec3{portalLeftX - 0.05f, uY0 + flH - 0.25f, louverWallZ + 0.14f},
        warmGlass);

    // Projecting Dark Charcoal Upper Roof Fascia Canopy over Left Louver Bay
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.15f, uY0 + flH - 0.36f, louverWallZ - 0.2f},
        Vec3{portalLeftX + 0.25f, uY0 + flH + 0.24f, hd + 1.05f},
        blackAlum);

    // Dense Vertical Architectural Louvers across Left Upper Bay
    float louverMinX = -hw + 1.45f;
    float louverMaxX = portalLeftX - 0.10f;
    float louverSpan = std::max(1.5f, louverMaxX - louverMinX);
    int louverCount = std::clamp(static_cast<int>(louverSpan / 0.34f), 8, 28);
    float louverZ = hd + 0.35f;
    for (int i = 0; i < louverCount; ++i) {
        float lx = louverMinX + (i + 0.5f) * (louverSpan / static_cast<float>(louverCount));
        uint32_t finMat = (i % 4 == 0) ? bronzeFins : blackAlum;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{lx - 0.035f, uY0 + 0.18f, louverZ - 0.16f},
            Vec3{lx + 0.035f, uY0 + flH - 0.34f, louverZ + 0.16f},
            finMat);
    }

    // 6. Signature White Crisp C-Frame Balcony Portal (Upper-Center-Right)
    float frameThick = 0.28f;
    float portalZBack = hd - 3.6f;
    float portalZFront = hd + 0.72f;
    float pY0 = uY0 - 0.15f;
    float pY1 = uY0 + flH * 0.92f;

    // Back wall of recessed balcony lounge (warm taupe / bronze stone + glazed sliding wall)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{portalLeftX, uY0, portalZBack - 0.2f},
        Vec3{portalRightX, pY1, portalZBack + 0.1f},
        warmGlass);
    // Textured stone inner side wall inside the C-frame terrace
    appendStackedStonePier(
        outVerts, outIndices,
        portalRightX - 0.45f, uY0 + 0.1f, portalZBack,
        portalRightX - 0.05f, pY1 - frameThick, portalZFront - 0.15f,
        stonePlinth, gunmetal
    );
    // White C-frame top soffit slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{portalLeftX, pY1 - frameThick, portalZBack},
        Vec3{portalRightX, pY1, portalZFront},
        crispWhite);
    // White C-frame right vertical cheek wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{portalRightX - frameThick, pY0, portalZBack},
        Vec3{portalRightX, pY1, portalZFront},
        crispWhite);
    // White C-frame bottom sill return beam
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{portalLeftX, pY0, portalZBack},
        Vec3{portalRightX, pY0 + frameThick, portalZFront},
        crispWhite);

    // Glowing X-shaped warm LED ceiling feature inside C-frame balcony soffit
    float bMidX = 0.5f * (portalLeftX + portalRightX - frameThick);
    float bMidZ = 0.5f * (portalZBack + portalZFront);
    float ceilY = pY1 - frameThick - 0.04f;
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{bMidX - 1.1f, ceilY, bMidZ - 0.8f},
        Vec3{bMidX + 1.1f, ceilY, bMidZ + 0.8f},
        0.04f, 0.04f, 6, warmGlass, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{bMidX - 1.1f, ceilY, bMidZ + 0.8f},
        Vec3{bMidX + 1.1f, ceilY, bMidZ - 0.8f},
        0.04f, 0.04f, 6, warmGlass, true, true
    );

    // Frameless glass balcony railing across the front opening of the C-frame
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{portalLeftX + 0.1f, pY0 + frameThick, portalZFront - 0.14f},
            Vec3{portalRightX - frameThick, pY0 + frameThick + 1.05f, portalZFront - 0.08f},
            balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{portalLeftX + 0.1f, pY0 + frameThick + 1.05f, portalZFront - 0.16f},
            Vec3{portalRightX - frameThick, pY0 + frameThick + 1.10f, portalZFront - 0.06f},
            frameMetal);
    }

    // 7. Upper-Right Dark Fin Cladding & Crown Fascia Band + Far-Right Projection
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{portalLeftX - 1.8f, uY0 + flH - 0.10f, hd - 0.45f},
        Vec3{hw + 0.18f, uY1 + 0.45f, hd + 0.52f},
        blackAlum);
    // Elevated parapet penthouse pop-up band slightly setback
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.18f, uY1 + 0.10f, -hd * 0.45f},
        Vec3{ hw - 0.8f, uY1 + 0.85f, hd - 1.35f},
        basaltWall);

    // Vertical dark slatted screen to the right of white C-frame
    float slatMinX = portalRightX + 0.12f;
    float slatMaxX = hw - 0.15f;
    int slatCount = std::max(6, static_cast<int>((slatMaxX - slatMinX) / 0.22f));
    for (int i = 0; i < slatCount; ++i) {
        float sx = slatMinX + (i + 0.5f) * ((slatMaxX - slatMinX) / static_cast<float>(slatCount));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - 0.035f, uY0 - 0.05f, hd - 0.05f},
            Vec3{sx + 0.035f, uY0 + flH - 0.08f, hd + 0.25f},
            blackAlum);
    }
    // Projecting dark architectural fin wing on far-right (+X) side facade
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - 0.10f, plinthY + 0.6f, -hd * 0.25f},
        Vec3{hw + 0.65f, uY1 + 0.10f, hd * 0.45f},
        blackAlum);

    // 8. Rear & Side Facade Windows (clean 360-degree view)
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.03f}, w * 0.85f, fy, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.85f, flH * 0.65f, 0.45f,
            0.0f, 0.0f, false, false);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{-hw - 0.03f, 0.0f, -hd * 0.25f}, d * 0.52f, fy, flH,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            2, wt, gt, 1.50f, flH * 0.62f, 0.50f,
            0.0f, 0.0f, false, false);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{hw + 0.03f, 0.0f, -hd * 0.25f}, d * 0.50f, fy, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.45f, flH * 0.62f, 0.50f,
            0.0f, 0.0f, false, false);
    }

    // 9. Tropical Palm Tree Accents (Right-front garden & Left front edge)
    appendStylizedPalmTree(outVerts, outIndices, hw + 0.75f, 0.35f, hd + 1.35f, 4.8f, timberMat, greenPlant);
    appendStylizedPalmTree(outVerts, outIndices, -hw - 0.75f, 0.35f, hd + 2.65f, 3.1f, timberMat, greenPlant);

    // 10. Flat Roof Deck & Coping Parapet
    res_scale::appendAdaptiveRoof(
        outVerts, outIndices, params_,
        w + 0.4f, d + 0.4f, uY1 + 0.15f,
        RoofType::FlatParapet, 0.45f, 0.0f,
        res_scale::showClutter(params_, false)
    );
}

} // namespace procedural::buildings
