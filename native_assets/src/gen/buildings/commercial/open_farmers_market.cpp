#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kMarketPi = 3.14159265358979323846f;
}

void TowerGen::generateOpenFarmersMarket(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(30.0f, params_.footprintWidth);
    float d = std::max(24.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t brickMat    = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t signOrange  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t glassGold   = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t brassMat    = static_cast<uint32_t>(MaterialSlot::BrassHardware);

    // 1. Stone & Cobblestone Market Plaza Deck
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.30f, -hd - 2.0f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 2.0f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.6f,  0.00f, -hd + 0.6f},
        Vec3{ hw - 0.6f,  0.18f,  hd - 0.6f},
        limestone);

    // Timber boardwalk promenade running through the center
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-3.4f, 0.18f, -hd + 0.6f},
        Vec3{ 3.4f, 0.24f,  hd - 0.6f},
        pergolaMat);

    // 2. Central Octagonal Market Bandstand & Social Gazebo
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, 0.20f, 0.0f}, Vec3{0.0f, 0.65f, 0.0f},
        4.6f, 4.4f, 8, limestone);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, 0.65f, 0.0f}, Vec3{0.0f, 1.05f, 0.0f},
        3.9f, 3.9f, 8, pergolaMat);

    float gazeboR = 3.8f;
    float gazeboTopY = 4.4f;
    for (int i = 0; i < 8; ++i) {
        float ang = (2.0f * kMarketPi * i) / 8.0f;
        float px = gazeboR * std::cos(ang);
        float pz = gazeboR * std::sin(ang);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{px, 0.65f, pz}, Vec3{px, gazeboTopY, pz},
            0.12f, 0.10f, 8, blackAlum);
    }
    // Gazebo cupola roof + spire finial
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, gazeboTopY, 0.0f}, Vec3{0.0f, gazeboTopY + 1.9f, 0.0f},
        4.8f, 0.6f, 8, roofMat);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, gazeboTopY + 1.9f, 0.0f}, Vec3{0.0f, gazeboTopY + 3.0f, 0.0f},
        0.55f, 0.04f, 8, brassMat);

    // 3. Eight Colorful Striped Farmers Market Vendor Stalls (4 West + 4 East)
    float stallXOffsets[2] = {-hw * 0.56f, hw * 0.56f};
    float stallW = 4.6f;
    float stallD = 3.6f;
    uint32_t stallColors[4] = {signRed, signOrange, turfMat, glassGold};

    for (int side = 0; side < 2; ++side) {
        float cx = stallXOffsets[side];
        for (int row = 0; row < 4; ++row) {
            float cz = -hd * 0.66f + row * (d * 0.44f);
            // Skip if too close to center gazebo
            if (std::abs(cx) < 5.5f && std::abs(cz) < 4.5f) continue;

            uint32_t awningPrimary = stallColors[(row + side) % 4];
            uint32_t awningAccent = (awningPrimary == signRed) ? limestone : signOrange;

            // Timber display counter
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - stallW * 0.45f, 0.18f, cz - stallD * 0.40f},
                Vec3{cx + stallW * 0.45f, 1.10f, cz + stallD * 0.40f},
                pergolaMat);

            // Produce crates on counter
            for (int c = -1; c <= 1; ++c) {
                uint32_t crateMat = (c == 0) ? turfMat : ((c < 0) ? signOrange : signRed);
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{cx + c * 1.15f - 0.45f, 1.10f, cz - 0.55f},
                    Vec3{cx + c * 1.15f + 0.45f, 1.48f, cz + 0.55f},
                    crateMat);
            }

            // 4 corner canopy posts
            for (float sx : {-stallW * 0.46f, stallW * 0.46f}) {
                for (float sz : {-stallD * 0.44f, stallD * 0.44f}) {
                    BuildingComponents::appendBox(outVerts, outIndices,
                        Vec3{cx + sx - 0.07f, 0.18f, cz + sz - 0.07f},
                        Vec3{cx + sx + 0.07f, 2.90f, cz + sz + 0.07f},
                        frameMat);
                }
            }

            // Striped gabled market canopy
            int stripes = 5;
            float stripeW = (stallW + 0.6f) / static_cast<float>(stripes);
            float startX = cx - (stallW + 0.6f) * 0.5f;
            for (int s = 0; s < stripes; ++s) {
                uint32_t sMat = (s % 2 == 0) ? awningPrimary : awningAccent;
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{startX + s * stripeW, 2.85f, cz - stallD * 0.54f},
                    Vec3{startX + (s + 1) * stripeW, 3.12f, cz + stallD * 0.54f},
                    sMat);
            }
            // Pitched canopy ridge peak
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - stallW * 0.5f, 3.12f, cz - 0.35f},
                Vec3{cx + stallW * 0.5f, 3.55f, cz + 0.35f},
                awningPrimary);
        }
    }

    // 4. Landscaped Courtyard Planter Benches
    for (float px : {-4.4f, 4.4f}) {
        for (float pz : {-hd * 0.55f, hd * 0.55f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.9f, 0.18f, pz - 1.6f},
                Vec3{px + 0.9f, 0.72f, pz + 1.6f},
                planterMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.75f, 0.72f, pz - 1.45f},
                Vec3{px + 0.75f, 0.98f, pz + 1.45f},
                turfMat);
        }
    }

    // 5. Perimeter Light Poles & Festive Catenary String-Light Cables
    float poleH = 5.4f;
    std::vector<Vec3> leftPoles;
    std::vector<Vec3> rightPoles;
    for (int i = 0; i < 4; ++i) {
        float pz = -hd + 2.5f + i * ((d - 5.0f) / 3.0f);
        Vec3 lp{-hw + 1.6f, poleH, pz};
        Vec3 rp{ hw - 1.6f, poleH, pz};
        leftPoles.push_back(lp);
        rightPoles.push_back(rp);

        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{lp.x, 0.18f, lp.z}, lp, 0.14f, 0.10f, 8, blackAlum);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{rp.x, 0.18f, rp.z}, rp, 0.14f, 0.10f, 8, blackAlum);

        // Glowing warm festoon lantern atop each pole
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{lp.x - 0.22f, poleH, lp.z - 0.22f},
            Vec3{lp.x + 0.22f, poleH + 0.42f, lp.z + 0.22f},
            glassGold);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rp.x - 0.22f, poleH, rp.z - 0.22f},
            Vec3{rp.x + 0.22f, poleH + 0.42f, rp.z + 0.22f},
            glassGold);

        // Overhead catenary string cable spanning across market
        BuildingComponents::appendBentCylinderCatenary(outVerts, outIndices,
            Vec3{lp.x, poleH - 0.2f, lp.z},
            Vec3{rp.x, poleH - 0.2f, rp.z},
            1.15f, 0.045f, 12, 6, blackAlum);

        // Suspended lantern bulbs along the catenary arc
        for (int b = 1; b <= 5; ++b) {
            float t = static_cast<float>(b) / 6.0f;
            float bx = lp.x + (rp.x - lp.x) * t;
            float sagY = (poleH - 0.2f) - 4.0f * 1.15f * t * (1.0f - t);
            uint32_t bulbMat = (b % 2 == 0) ? signOrange : glassGold;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx - 0.14f, sagY - 0.30f, lp.z - 0.14f},
                Vec3{bx + 0.14f, sagY - 0.04f, lp.z + 0.14f},
                bulbMat);
        }
    }

    // 6. Grand Arched Market Entrance Portal Sign (+Z Plaza Front)
    float gateZ = hd - 0.8f;
    for (float gx : {-4.5f, 4.5f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{gx - 0.65f, 0.18f, gateZ - 0.65f},
            Vec3{gx + 0.65f, 5.20f, gateZ + 0.65f},
            brickMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{gx - 0.78f, 5.20f, gateZ - 0.78f},
            Vec3{gx + 0.78f, 5.55f, gateZ + 0.78f},
            limestone);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-4.8f, 4.70f, gateZ - 0.45f},
        Vec3{ 4.8f, 5.85f, gateZ + 0.45f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-4.2f, 4.90f, gateZ - 0.50f},
        Vec3{ 4.2f, 5.65f, gateZ + 0.50f},
        signOrange);
}

} // namespace procedural::buildings
