#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

void CivicGen::generateBaseballBallpark(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(44.0f, params_.mainWidth);
    float d = std::max(44.0f, params_.mainDepth);
    float h = std::max(14.0f, params_.stories * params_.storyHeight);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t brickMat   = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t concMat    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t turfMat    = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t dirtMat    = static_cast<uint32_t>(MaterialSlot::TrackRed);
    uint32_t seatMat    = static_cast<uint32_t>(MaterialSlot::StadiumSeatBlue);
    uint32_t silverMat  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t yellowMat  = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t scoreMat   = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);

    // 1. Surrounding Plaza Plinth (City sidewalk & street level)
    float plinthW = w + 8.0f;
    float plinthD = d + 8.0f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-plinthW * 0.5f, -0.6f, -plinthD * 0.5f},
        Vec3{ plinthW * 0.5f,  0.0f,  plinthD * 0.5f},
        foundMat
    );

    // 2. Full Outfield Grass Surface (Emerald Green Turf)
    float fieldW = w * 0.75f;
    float fieldD = d * 0.75f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-fieldW * 0.5f, 0.01f, -fieldD * 0.5f},
        Vec3{ fieldW * 0.5f, 0.05f,  fieldD * 0.5f},
        turfMat
    );

    // 3. Infield Clay Dirt Diamond & Basepaths
    // Home plate is positioned near (-hw * 0.25f, -hd * 0.25f), angling out toward (+X, +Z)
    float diamondR = 11.5f;
    Vec3 homePlate{-hw * 0.22f, 0.06f, -hd * 0.22f};
    Vec3 firstBase = homePlate + Vec3{diamondR, 0.0f, 0.0f};
    Vec3 thirdBase = homePlate + Vec3{0.0f, 0.0f, diamondR};
    Vec3 secondBase = homePlate + Vec3{diamondR, 0.0f, diamondR};
    Vec3 pitcherMound = homePlate + Vec3{diamondR * 0.55f, 0.0f, diamondR * 0.55f};

    // Clay Diamond Infield Base Pad
    BuildingComponents::appendBox(
        outVerts, outIndices,
        homePlate + Vec3{-1.5f, 0.0f, -1.5f},
        secondBase + Vec3{1.5f, 0.02f, 1.5f},
        dirtMat
    );

    // Infield grass cutout island in center of diamond
    BuildingComponents::appendBox(
        outVerts, outIndices,
        homePlate + Vec3{2.4f, 0.025f, 2.4f},
        secondBase - Vec3{2.4f, 0.0f, 2.4f},
        turfMat
    );

    // Raised Pitcher's Mound with Pitching Rubber
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        pitcherMound, pitcherMound + Vec3{0, 0.25f, 0},
        1.6f, 1.4f, 12, dirtMat, true, true
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        pitcherMound + Vec3{-0.15f, 0.26f, -0.4f},
        pitcherMound + Vec3{ 0.15f, 0.28f,  0.4f},
        concMat
    );

    // Base Bags (Pillows: 1st, 2nd, 3rd base and Home Plate pentagon)
    BuildingComponents::appendBox(outVerts, outIndices, firstBase - Vec3{0.35f, 0, 0.35f}, firstBase + Vec3{0.35f, 0.12f, 0.35f}, concMat);
    BuildingComponents::appendBox(outVerts, outIndices, secondBase - Vec3{0.35f, 0, 0.35f}, secondBase + Vec3{0.35f, 0.12f, 0.35f}, concMat);
    BuildingComponents::appendBox(outVerts, outIndices, thirdBase - Vec3{0.35f, 0, 0.35f}, thirdBase + Vec3{0.35f, 0.12f, 0.35f}, concMat);
    BuildingComponents::appendBox(outVerts, outIndices, homePlate - Vec3{0.35f, 0, 0.35f}, homePlate + Vec3{0.35f, 0.04f, 0.35f}, concMat);

    // White Foul Lines to Left and Right Outfield
    BuildingComponents::appendBox(outVerts, outIndices, homePlate, homePlate + Vec3{0.15f, 0.03f, fieldD * 0.85f}, concMat);
    BuildingComponents::appendBox(outVerts, outIndices, homePlate, homePlate + Vec3{fieldW * 0.85f, 0.03f, 0.15f}, concMat);

    // 4. Left-Field "Green Monster" Wall & Outfield Scoreboard
    float wallH = 7.5f;
    // High left-field barrier
    BuildingComponents::appendBox(
        outVerts, outIndices,
        homePlate + Vec3{-1.0f, 0.0f, fieldD * 0.82f},
        homePlate + Vec3{fieldW * 0.5f, wallH, fieldD * 0.82f + 1.2f},
        turfMat
    );
    // Outfield Scoreboard screen display mounted on wall
    BuildingComponents::appendBox(
        outVerts, outIndices,
        homePlate + Vec3{1.0f, wallH * 0.4f, fieldD * 0.82f - 0.1f},
        homePlate + Vec3{fieldW * 0.42f, wallH * 0.95f, fieldD * 0.82f},
        scoreMat
    );

    // Right-field lower perimeter wall
    BuildingComponents::appendBox(
        outVerts, outIndices,
        homePlate + Vec3{fieldW * 0.82f, 0.0f, -1.0f},
        homePlate + Vec3{fieldW * 0.82f + 1.2f, 3.2f, fieldD * 0.82f + 1.2f},
        brickMat
    );

    // Yellow Foul Poles at Left & Right Wall Intersections
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        homePlate + Vec3{0.0f, 0.0f, fieldD * 0.82f},
        homePlate + Vec3{0.0f, wallH + 6.0f, fieldD * 0.82f},
        0.18f, 0.12f, 8, yellowMat, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        homePlate + Vec3{fieldW * 0.82f, 0.0f, 0.0f},
        homePlate + Vec3{fieldW * 0.82f, wallH + 6.0f, 0.0f},
        0.18f, 0.12f, 8, yellowMat, true, true
    );

    // 5. Dual-Deck Grandstand Seating Bowl wrapping around Home Plate and Baseline
    int standTiers = 8;
    float standStartDist = 4.5f;
    float standStepW = 1.6f;
    float standStepH = 0.72f;

    for (int t = 0; t < standTiers; ++t) {
        float rInner = standStartDist + t * standStepW;
        float rOuter = rInner + standStepW;
        float tierH = (t + 1) * standStepH;

        // Angle sweep from -15° to 105° wrapping home plate
        const int arcSegs = 14;
        for (int a = 0; a < arcSegs; ++a) {
            float a0 = (-15.0f + a * (120.0f / arcSegs)) * (kPi / 180.0f);
            float a1 = (-15.0f + (a + 1) * (120.0f / arcSegs)) * (kPi / 180.0f);

            Vec3 c0 = homePlate - Vec3{std::cos(a0) * rInner, 0, std::sin(a0) * rInner};
            Vec3 c1 = homePlate - Vec3{std::cos(a1) * rInner, 0, std::sin(a1) * rInner};
            Vec3 c2 = homePlate - Vec3{std::cos(a1) * rOuter, 0, std::sin(a1) * rOuter};
            Vec3 c3 = homePlate - Vec3{std::cos(a0) * rOuter, 0, std::sin(a0) * rOuter};

            BuildingComponents::appendQuad(outVerts, outIndices,
                c0 + Vec3{0, tierH, 0}, c1 + Vec3{0, tierH, 0},
                c2 + Vec3{0, tierH, 0}, c3 + Vec3{0, tierH, 0},
                Vec3{0, 1, 0}, seatMat);

            BuildingComponents::appendQuad(outVerts, outIndices,
                c0, c1, c1 + Vec3{0, tierH, 0}, c0 + Vec3{0, tierH, 0},
                (c0 - homePlate).normalized(), concMat);
        }
    }

    // Upper Grandstand Cantilever Roof Canopy
    float canopyRadius = standStartDist + standTiers * standStepW + 1.5f;
    float canopyY = standTiers * standStepH + 3.2f;
    for (int a = 0; a < 14; ++a) {
        float a0 = (-15.0f + a * (120.0f / 14)) * (kPi / 180.0f);
        float a1 = (-15.0f + (a + 1) * (120.0f / 14)) * (kPi / 180.0f);

        Vec3 r0 = homePlate - Vec3{std::cos(a0) * (standStartDist + 4.0f), 0, std::sin(a0) * (standStartDist + 4.0f)};
        Vec3 r1 = homePlate - Vec3{std::cos(a1) * (standStartDist + 4.0f), 0, std::sin(a1) * (standStartDist + 4.0f)};
        Vec3 r2 = homePlate - Vec3{std::cos(a1) * canopyRadius, 0, std::sin(a1) * canopyRadius};
        Vec3 r3 = homePlate - Vec3{std::cos(a0) * canopyRadius, 0, std::sin(a0) * canopyRadius};

        BuildingComponents::appendQuad(outVerts, outIndices,
            r0 + Vec3{0, canopyY, 0}, r1 + Vec3{0, canopyY, 0},
            r2 + Vec3{0, canopyY + 1.0f, 0}, r3 + Vec3{0, canopyY + 1.0f, 0},
            Vec3{0, 1, 0}, roofMat);
    }

    // 6. Classic Red Brick Exterior Facade with Arched Loggia Portals
    // Exterior corner wrapping around home plate rear
    float outerR = canopyRadius + 0.8f;
    float facadeH = h * 0.85f;
    for (int a = 0; a < 16; ++a) {
        float a0 = (-20.0f + a * (130.0f / 16)) * (kPi / 180.0f);
        float a1 = (-20.0f + (a + 1) * (130.0f / 16)) * (kPi / 180.0f);

        Vec3 p0 = homePlate - Vec3{std::cos(a0) * outerR, 0, std::sin(a0) * outerR};
        Vec3 p1 = homePlate - Vec3{std::cos(a1) * outerR, 0, std::sin(a1) * outerR};
        Vec3 norm = ((p0 + p1) * 0.5f - homePlate).normalized();

        // Brick wall panel
        BuildingComponents::appendQuad(outVerts, outIndices,
            p0, p1, p1 + Vec3{0, facadeH, 0}, p0 + Vec3{0, facadeH, 0},
            norm, brickMat);

        // Architectural arched brick portals on ground level
        if (a % 2 == 0) {
            Vec3 archCenter = (p0 + p1) * 0.5f;
            BuildingComponents::appendBox(outVerts, outIndices,
                archCenter - norm * 0.1f - Vec3{0.6f, 0, 0.6f},
                archCenter + norm * 0.2f + Vec3{0.6f, 2.6f, 0.6f},
                frameMat);
        }

        // Decorative stone cornice header
        BuildingComponents::appendBox(outVerts, outIndices,
            p0 + Vec3{0, facadeH, 0} - norm * 0.1f,
            p1 + Vec3{0, facadeH + 0.5f, 0} + norm * 0.3f,
            concMat);
    }

    // 7. 6 Steel-Lattice Field Floodlight Towers
    float floodlightH = h * 1.55f;
    Vec3 lightPositions[6] = {
        homePlate - Vec3{outerR * 0.9f, 0, 0},
        homePlate - Vec3{outerR * 0.65f, 0, outerR * 0.65f},
        homePlate - Vec3{0, 0, outerR * 0.9f},
        homePlate + Vec3{fieldW * 0.82f, 0, fieldD * 0.4f},
        homePlate + Vec3{fieldW * 0.5f, 0, fieldD * 0.85f},
        homePlate + Vec3{fieldW * 0.84f, 0, fieldD * 0.84f}
    };

    for (int i = 0; i < 6; ++i) {
        Vec3 p = lightPositions[i];
        // Vertical steel lattice pylon
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            p, p + Vec3{0, floodlightH, 0},
            0.55f, 0.32f, 8, frameMat, true, true
        );
        // Multi-lamp rectangular array head
        BuildingComponents::appendBox(
            outVerts, outIndices,
            p + Vec3{-1.8f, floodlightH, -0.6f},
            p + Vec3{ 1.8f, floodlightH + 1.8f, 0.6f},
            silverMat
        );
        // Light bulbs surface
        BuildingComponents::appendBox(
            outVerts, outIndices,
            p + Vec3{-1.6f, floodlightH + 0.2f, -0.4f},
            p + Vec3{ 1.6f, floodlightH + 1.6f, 0.4f},
            concMat
        );
    }
}

} // namespace procedural::buildings
