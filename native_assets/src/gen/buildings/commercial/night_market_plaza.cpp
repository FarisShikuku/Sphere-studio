#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateNightMarketPlaza(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(30.0f, params_.footprintWidth);
    float d = std::max(26.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t basaltMat   = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t concrete    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t glassGold   = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t glassEmer   = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t balconyGlas = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t signOrange  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);

    // 1. Dark Basalt & Cobblestone Night-Market Courtyard Deck
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.30f, -hd - 2.0f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 2.0f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.5f,  0.00f, -hd + 0.5f},
        Vec3{ hw - 0.5f,  0.15f,  hd - 0.5f},
        basaltMat);

    // 2. Two Facing Street-Food Kiosk Arcades (West & East Flanks)
    float kioskTopY = 3.8f;
    for (int side : {-1, 1}) {
        float kx = side * (hw - 3.6f);
        float innerEdge = (side < 0) ? (kx + 2.2f) : (kx - 2.2f);
        float awningOuter = (side < 0) ? (innerEdge + 1.9f) : (innerEdge - 1.9f);

        // Main food stall arcade block
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{kx - 2.2f, 0.15f, -hd + 3.0f},
            Vec3{kx + 2.2f, kioskTopY, hd - 5.5f},
            blackAlum);

        // Service counter strip facing central courtyard
        float cMinX = std::min(innerEdge, innerEdge + (side < 0 ? 0.65f : -0.65f));
        float cMaxX = std::max(innerEdge, innerEdge + (side < 0 ? 0.65f : -0.65f));
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cMinX, 0.15f, -hd + 3.2f},
            Vec3{cMaxX, 1.15f,  hd - 5.7f},
            pergolaMat);

        // Glazed service pass-through openings
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{std::min(innerEdge, innerEdge + (side < 0 ? 0.08f : -0.08f)), 1.15f, -hd + 3.8f},
            Vec3{std::max(innerEdge, innerEdge + (side < 0 ? 0.08f : -0.08f)), 2.85f,  hd - 6.3f},
            glassTinted);

        // Projecting stall canopy overhang
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{std::min(innerEdge, awningOuter), 2.85f, -hd + 2.8f},
            Vec3{std::max(innerEdge, awningOuter), 3.12f,  hd - 5.3f},
            (side < 0) ? signRed : signOrange);

        // Illuminated neon menu fascia boards along roof parapet
        for (int bay = 0; bay < 4; ++bay) {
            float bz = -hd + 4.4f + bay * ((d - 10.5f) / 3.5f);
            uint32_t boardMat = ((bay + (side > 0 ? 1 : 0)) % 3 == 0) ? signOrange
                              : (((bay % 2) == 0) ? signRed : glassEmer);
            float bMinX = std::min(innerEdge, innerEdge + (side < 0 ? 0.18f : -0.18f));
            float bMaxX = std::max(innerEdge, innerEdge + (side < 0 ? 0.18f : -0.18f));
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bMinX, 3.15f, bz - 1.3f},
                Vec3{bMaxX, 3.95f, bz + 1.3f},
                boardMat);
        }
    }

    // 3. Elevated Rear Rooftop Beer-Garden & Lantern Terrace (-Z Wing)
    float deckMinZ = -hd + 1.0f;
    float deckMaxZ = -hd + 8.5f;
    float terraceY = 3.85f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.4f, 0.15f, deckMinZ},
        Vec3{ hw - 1.4f, terraceY, deckMaxZ},
        concrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.6f, terraceY, deckMinZ + 0.2f},
        Vec3{ hw - 1.6f, terraceY + 0.18f, deckMaxZ - 0.2f},
        pergolaMat);

    // Wide Social Stadium Staircase ascending to terrace
    int stepCount = 9;
    float stairW = 8.4f;
    float stairStartZ = deckMaxZ + 4.5f;
    for (int s = 0; s < stepCount; ++s) {
        float y0 = 0.15f + s * ((terraceY - 0.15f) / stepCount);
        float y1 = 0.15f + (s + 1) * ((terraceY - 0.15f) / stepCount);
        float z0 = stairStartZ - (s + 1) * (4.5f / stepCount);
        float z1 = stairStartZ - s * (4.5f / stepCount);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-stairW * 0.5f, y0, z0},
            Vec3{ stairW * 0.5f, y1, z1},
            (s % 2 == 0) ? pergolaMat : basaltMat);
    }

    // Glass balcony railing along upper terrace edge (excluding central stair opening)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.6f, terraceY + 0.18f, deckMaxZ - 0.25f},
        Vec3{-stairW * 0.5f, terraceY + 1.25f, deckMaxZ - 0.12f},
        balconyGlas);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ stairW * 0.5f, terraceY + 0.18f, deckMaxZ - 0.25f},
        Vec3{ hw - 1.6f, terraceY + 1.25f, deckMaxZ - 0.12f},
        balconyGlas);

    // Overhead Timber Pergola Canopy on Rooftop Terrace
    float pergolaY = terraceY + 3.0f;
    for (float px : {-hw + 2.4f, -4.5f, 4.5f, hw - 2.4f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.15f, terraceY + 0.18f, deckMinZ + 0.6f},
            Vec3{px + 0.15f, pergolaY, deckMinZ + 0.9f},
            blackAlum);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.15f, terraceY + 0.18f, deckMaxZ - 0.9f},
            Vec3{px + 0.15f, pergolaY, deckMaxZ - 0.6f},
            blackAlum);
    }
    for (float pz = deckMinZ + 0.6f; pz <= deckMaxZ - 0.6f; pz += 1.4f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 2.0f, pergolaY, pz - 0.10f},
            Vec3{ hw - 2.0f, pergolaY + 0.24f, pz + 0.10f},
            pergolaMat);
    }

    // 4. Communal Courtyard Dining Tables & Planters
    for (float tz : {0.5f, 5.2f}) {
        for (float tx : {-3.6f, 3.6f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{tx - 1.5f, 0.85f, tz - 0.65f},
                Vec3{tx + 1.5f, 0.96f, tz + 0.65f},
                pergolaMat);
            // Benches
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{tx - 1.4f, 0.45f, tz - 1.25f},
                Vec3{tx + 1.4f, 0.54f, tz - 0.85f},
                pergolaMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{tx - 1.4f, 0.45f, tz + 0.85f},
                Vec3{tx + 1.4f, 0.54f, tz + 1.25f},
                pergolaMat);
            for (float lx : {tx - 1.2f, tx + 1.2f}) {
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{lx - 0.06f, 0.15f, tz - 1.15f},
                    Vec3{lx + 0.06f, 0.85f, tz + 1.15f},
                    frameMat);
            }
        }
    }

    // Green accent planters near courtyard entry
    for (float px : {-6.2f, 6.2f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 1.1f, 0.15f, hd - 4.2f},
            Vec3{px + 1.1f, 0.75f, hd - 2.4f},
            basaltMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.95f, 0.75f, hd - 4.05f},
            Vec3{px + 0.95f, 1.05f, hd - 2.55f},
            turfMat);
    }

    // 5. Overhead Catenary Glowing Night-Market Lantern Garlands across Courtyard
    for (int row = 0; row < 4; ++row) {
        float lz = -2.0f + row * 4.2f;
        Vec3 p0{-hw + 3.6f, kioskTopY + 1.3f, lz};
        Vec3 p1{ hw - 3.6f, kioskTopY + 1.3f, lz};

        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{p0.x, kioskTopY, p0.z}, p0, 0.08f, 0.08f, 6, blackAlum);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{p1.x, kioskTopY, p1.z}, p1, 0.08f, 0.08f, 6, blackAlum);

        BuildingComponents::appendBentCylinderCatenary(outVerts, outIndices,
            p0, p1, 1.1f, 0.04f, 12, 6, blackAlum);

        for (int b = 1; b <= 6; ++b) {
            float t = static_cast<float>(b) / 7.0f;
            float bx = p0.x + (p1.x - p0.x) * t;
            float sagY = p0.y - 4.0f * 1.1f * t * (1.0f - t);
            uint32_t lanternMat = ((b + row) % 4 == 0) ? glassEmer
                                : (((b + row) % 3 == 0) ? signRed
                                : (((b + row) % 2 == 0) ? signOrange : glassGold));
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx - 0.20f, sagY - 0.44f, lz - 0.20f},
                Vec3{bx + 0.20f, sagY - 0.04f, lz + 0.20f},
                lanternMat);
        }
    }

    // 6. Ceremonial Glowing Night-Market Entrance Torii Gate (+Z Plaza Front)
    float gateZ = hd - 1.0f;
    for (float gx : {-4.2f, 4.2f}) {
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{gx, 0.15f, gateZ}, Vec3{gx, 5.6f, gateZ},
            0.35f, 0.28f, 10, signRed);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-5.4f, 4.9f, gateZ - 0.35f},
        Vec3{ 5.4f, 5.55f, gateZ + 0.35f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-6.0f, 5.55f, gateZ - 0.45f},
        Vec3{ 6.0f, 6.05f, gateZ + 0.45f},
        signRed);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.6f, 4.65f, gateZ - 0.40f},
        Vec3{ 2.6f, 5.45f, gateZ + 0.40f},
        signOrange);
}

} // namespace procedural::buildings
