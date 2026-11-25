#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void CivicGen::generateStadium(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(36.0f, params_.mainWidth);
    float d = std::max(28.0f, params_.mainDepth);
    float h = std::max(12.0f, params_.stories * params_.storyHeight);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t concMat    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t turfMat    = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t trackMat   = static_cast<uint32_t>(MaterialSlot::TrackRed);
    uint32_t seatMat    = static_cast<uint32_t>(MaterialSlot::StadiumSeatBlue);
    uint32_t glassMat   = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t hvacMat    = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t silverMat  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);

    // 1. Surrounding Plaza Plinth & Promenade
    float plinthW = w + 8.0f;
    float plinthD = d + 8.0f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-plinthW * 0.5f, -0.6f, -plinthD * 0.5f},
        Vec3{ plinthW * 0.5f,  0.0f,  plinthD * 0.5f},
        foundMat
    );

    // 2. Central Athletic Running Track (Terracotta Red)
    float trackW = w * 0.58f;
    float trackD = d * 0.58f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-trackW * 0.5f, 0.01f, -trackD * 0.5f},
        Vec3{ trackW * 0.5f, 0.05f,  trackD * 0.5f},
        trackMat
    );

    // 3. Central Regulation Sports Field / Playing Pitch (Vibrant Green Turf)
    float pitchW = trackW * 0.78f;
    float pitchD = trackD * 0.78f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-pitchW * 0.5f, 0.06f, -pitchD * 0.5f},
        Vec3{ pitchW * 0.5f, 0.12f,  pitchD * 0.5f},
        turfMat
    );

    // Pitch Line Markings (Perimeter white lines, Center line, Penalty boxes)
    // Center Line
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-0.15f, 0.13f, -pitchD * 0.5f + 0.3f},
        Vec3{ 0.15f, 0.14f,  pitchD * 0.5f - 0.3f},
        concMat
    );
    // Center Circle Box representation
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-1.8f, 0.13f, -1.8f},
        Vec3{ 1.8f, 0.14f,  1.8f},
        turfMat
    );
    // Outer white perimeter lines
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-pitchW * 0.5f + 0.2f, 0.13f, -pitchD * 0.5f + 0.2f},
        Vec3{ pitchW * 0.5f - 0.2f, 0.14f, -pitchD * 0.5f + 0.4f},
        concMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-pitchW * 0.5f + 0.2f, 0.13f,  pitchD * 0.5f - 0.4f},
        Vec3{ pitchW * 0.5f - 0.2f, 0.14f,  pitchD * 0.5f - 0.2f},
        concMat
    );

    // Goal Frame Structures at -X and +X ends
    float goalW = 3.6f;
    float goalH = 2.0f;
    // Goal -X
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-pitchW * 0.5f - 0.1f, 0.12f, -goalW * 0.5f},
        Vec3{-pitchW * 0.5f, 0.12f + goalH, goalW * 0.5f},
        silverMat
    );
    // Goal +X
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{ pitchW * 0.5f, 0.12f, -goalW * 0.5f},
        Vec3{ pitchW * 0.5f + 0.1f, 0.12f + goalH, goalW * 0.5f},
        silverMat
    );

    // 4. Concentric Stepped Spectator Seating Grandstands (Rising from Field to Rim)
    // 6 concentric stepped seating tiers
    int tiers = 6;
    float innerW = trackW + 1.5f;
    float innerD = trackD + 1.5f;
    float stepW = (w - innerW) / (2.0f * tiers);
    float stepD = (d - innerD) / (2.0f * tiers);
    float stepH = (h * 0.65f) / tiers;

    for (int t = 0; t < tiers; ++t) {
        float curTierH = (t + 1) * stepH;
        float curHalfW = (innerW * 0.5f) + t * stepW;
        float curHalfD = (innerD * 0.5f) + t * stepD;
        float nextHalfW = curHalfW + stepW;
        float nextHalfD = curHalfD + stepD;

        // North stand (+Z)
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-nextHalfW, 0.0f, curHalfD},
            Vec3{ nextHalfW, curTierH, nextHalfD},
            seatMat
        );
        // South stand (-Z)
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-nextHalfW, 0.0f, -nextHalfD},
            Vec3{ nextHalfW, curTierH, -curHalfD},
            seatMat
        );
        // East stand (+X)
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{curHalfW, 0.0f, -curHalfD},
            Vec3{nextHalfW, curTierH, curHalfD},
            seatMat
        );
        // West stand (-X)
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-nextHalfW, 0.0f, -curHalfD},
            Vec3{-curHalfW, curTierH, curHalfD},
            seatMat
        );
    }

    // 5. VIP Luxury Corporate Suites (Middle band ring of reflective glass)
    float suiteY = tiers * stepH;
    float suiteH = 2.4f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw + 0.2f, suiteY, -hd + 0.2f},
        Vec3{ hw - 0.2f, suiteY + suiteH, hd - 0.2f},
        glassMat
    );

    // 6. Exterior Facade Shell with Structural Flying Buttress Pylons
    // Exterior perimeter concrete wall
    float outerTopY = h;
    // North and South outer walls
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, hd - 1.0f},
        Vec3{ hw, outerTopY, hd},
        concMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{ hw, outerTopY, -hd + 1.0f},
        concMat
    );
    // East and West outer walls
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{hw - 1.0f, 0.0f, -hd},
        Vec3{hw, outerTopY, hd},
        concMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{-hw + 1.0f, outerTopY, hd},
        concMat
    );

    // External Vertical Flying Buttress Fins / Columns around the perimeter
    int finsX = 8;
    float finSpacingX = w / (finsX - 1);
    for (int i = 0; i < finsX; ++i) {
        float fx = -hw + i * finSpacingX;
        // North fin
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{fx - 0.45f, 0.0f, hd},
            Vec3{fx + 0.45f, outerTopY + 0.6f, hd + 1.2f},
            frameMat
        );
        // South fin
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{fx - 0.45f, 0.0f, -hd - 1.2f},
            Vec3{fx + 0.45f, outerTopY + 0.6f, -hd},
            frameMat
        );
    }
    int finsZ = 6;
    float finSpacingZ = d / (finsZ - 1);
    for (int i = 0; i < finsZ; ++i) {
        float fz = -hd + i * finSpacingZ;
        // East fin
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{hw, 0.0f, fz - 0.45f},
            Vec3{hw + 1.2f, outerTopY + 0.6f, fz + 0.45f},
            frameMat
        );
        // West fin
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{-hw - 1.2f, 0.0f, fz - 0.45f},
            Vec3{-hw, outerTopY + 0.6f, fz + 0.45f},
            frameMat
        );
    }

    // 7. Cantilevered Tension Ring Canopy Roof (Shades stands, leaves field open)
    float roofInwardDepth = 6.5f;
    float canopyY = outerTopY + 0.6f;
    float canopyThick = 0.55f;

    // North canopy rim
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 1.2f, canopyY, hd - roofInwardDepth},
        Vec3{ hw + 1.2f, canopyY + canopyThick, hd + 1.2f},
        roofMat
    );
    // South canopy rim
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 1.2f, canopyY, -hd - 1.2f},
        Vec3{ hw + 1.2f, canopyY + canopyThick, -hd + roofInwardDepth},
        roofMat
    );
    // East canopy rim
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{hw - roofInwardDepth, canopyY, -hd + roofInwardDepth},
        Vec3{hw + 1.2f, canopyY + canopyThick, hd - roofInwardDepth},
        roofMat
    );
    // West canopy rim
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 1.2f, canopyY, -hd + roofInwardDepth},
        Vec3{-hw + roofInwardDepth, canopyY + canopyThick, hd - roofInwardDepth},
        roofMat
    );

    // 8. 4 Soaring Corner Floodlight Towers with Multi-Lamp Grids
    if (params_.hasFloodlights) {
        float towerH = h * 1.55f;
        float cornerOffsets[4][2] = {
            {-hw - 2.8f, -hd - 2.8f},
            { hw + 2.8f, -hd - 2.8f},
            { hw + 2.8f,  hd + 2.8f},
            {-hw - 2.8f,  hd + 2.8f}
        };

        for (int c = 0; c < 4; ++c) {
            float tx = cornerOffsets[c][0];
            float tz = cornerOffsets[c][1];

            // Pylon Mast Tower (Steel lattice column)
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{tx - 0.5f, 0.0f, tz - 0.5f},
                Vec3{tx + 0.5f, towerH, tz + 0.5f},
                frameMat
            );

            // Cross-arms and floodlight head rack
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{tx - 1.8f, towerH, tz - 1.8f},
                Vec3{tx + 1.8f, towerH + 2.2f, tz + 1.8f},
                silverMat
            );

            // High-luminance lamp bulbs array (Glass reflector panels)
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{tx - 1.6f, towerH + 0.3f, tz - 1.6f},
                Vec3{tx + 1.6f, towerH + 1.9f, tz + 1.6f},
                glassMat
            );
        }
    }

    // 9. Grand Spectator Entrance Staircases (North & South portals)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-6.0f, 0.0f, hd + 1.2f},
        Vec3{ 6.0f, 2.0f, hd + 3.8f},
        foundMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-6.0f, 0.0f, -hd - 3.8f},
        Vec3{ 6.0f, 2.0f, -hd - 1.2f},
        foundMat
    );
}

} // namespace procedural::buildings
