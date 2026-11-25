#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateContainerShop(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(14.0f, params_.footprintWidth);
    float d = std::max(10.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t cortenMat   = static_cast<uint32_t>(MaterialSlot::WallCorten);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t signOrange  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    // 1. Urban Pop-Up Timber Deck & Concrete Pier Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.30f, -hd - 1.5f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 3.5f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, 0.00f, -hd - 0.4f},
        Vec3{ hw + 0.5f, 0.22f,  hd + 2.8f},
        timberMat);

    // 2. Ground Level 40ft Shipping Container Retail & Espresso Module (Back-Center)
    float c1W = std::min(12.2f, w * 0.82f);
    float c1D = 3.2f;
    float c1H = 2.95f;
    float c1X0 = -c1W * 0.5f - 0.8f;
    float c1X1 =  c1W * 0.5f - 0.8f;
    float c1Z0 = -2.2f;
    float c1Z1 = c1Z0 + c1D;

    // Container steel corner castings & main corrugated body
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{c1X0, 0.22f, c1Z0},
        Vec3{c1X1, 0.22f + c1H, c1Z1},
        cortenMat);
    // Heavy steel ISO corner posts
    for (float px : {c1X0, c1X1 - 0.18f}) {
        for (float pz : {c1Z0, c1Z1 - 0.18f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.03f, 0.22f, pz - 0.03f},
                Vec3{px + 0.21f, 0.22f + c1H + 0.03f, pz + 0.21f},
                blackAlum);
        }
    }
    // Vertical corrugated ribs along rear (-Z) and ends (-X, +X)
    for (float rx = c1X0 + 0.35f; rx < c1X1 - 0.35f; rx += 0.48f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rx - 0.06f, 0.32f, c1Z0 - 0.06f},
            Vec3{rx + 0.06f, 0.22f + c1H - 0.10f, c1Z0},
            cortenMat);
    }

    // Cut-out Floor-to-Ceiling Glass Storefront & Hydraulic Fold-Up Service Awning (+Z)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{c1X0 + 0.5f, 0.35f, c1Z1 - 0.08f},
        Vec3{c1X1 - 0.5f, 0.22f + c1H - 0.25f, c1Z1 + 0.06f},
        glassTinted);
    // Walk-up timber service counter shelf & outdoor bar stools
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{c1X0 + 0.8f, 1.05f, c1Z1 + 0.06f},
        Vec3{c1X0 + 5.2f, 1.15f, c1Z1 + 0.65f},
        timberMat);
    for (float sx = c1X0 + 1.3f; sx <= c1X0 + 4.8f; sx += 1.1f) {
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{sx, 0.22f, c1Z1 + 0.95f},
            Vec3{sx, 0.78f, c1Z1 + 0.95f},
            0.05f, 0.05f, 8, blackAlum);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{sx, 0.78f, c1Z1 + 0.95f},
            Vec3{sx, 0.84f, c1Z1 + 0.95f},
            0.22f, 0.22f, 10, timberMat);
    }

    // Hydraulic Folded-Up Awning Flap Canopy projecting from top of ground container
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{c1X0 + 0.4f, 0.22f + c1H - 0.15f, c1Z1},
        Vec3{c1X1 - 0.4f, 0.22f + c1H - 0.15f, c1Z1},
        Vec3{c1X1 - 0.4f, 0.22f + c1H + 0.45f, c1Z1 + 2.2f},
        Vec3{c1X0 + 0.4f, 0.22f + c1H + 0.45f, c1Z1 + 2.2f},
        Vec3{0.0f, 0.95f, -0.30f},
        blackAlum);
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{c1X0 + 0.4f, 0.22f + c1H + 0.45f, c1Z1 + 2.2f},
        Vec3{c1X1 - 0.4f, 0.22f + c1H + 0.45f, c1Z1 + 2.2f},
        Vec3{c1X1 - 0.4f, 0.22f + c1H - 0.15f, c1Z1},
        Vec3{c1X0 + 0.4f, 0.22f + c1H - 0.15f, c1Z1},
        Vec3{0.0f, -0.95f, 0.30f},
        timberMat);
    // Hydraulic steel struts holding awning open
    for (float ax : {c1X0 + 0.8f, c1X1 - 0.8f}) {
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{ax, 0.22f + c1H - 1.1f, c1Z1 + 0.05f},
            Vec3{ax, 0.22f + c1H + 0.40f, c1Z1 + 2.0f},
            0.04f, 0.04f, 8, silverAlum);
    }

    // 3. Second Perpendicular Ground 20ft Pop-Up Boutique Container (Right Wing)
    float c2X0 = c1X1 + 0.3f;
    float c2X1 = std::min(hw - 0.4f, c2X0 + 3.2f);
    float c2Z0 = -2.2f;
    float c2Z1 =  3.4f;
    if (c2X1 > c2X0 + 2.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{c2X0, 0.22f, c2Z0},
            Vec3{c2X1, 0.22f + c1H, c2Z1},
            blackAlum);
        // Large retail showcase glass window facing court (-X) and front (+Z)
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{c2X0 + 0.25f, 0.45f, c2Z1 - 0.06f},
            Vec3{c2X1 - 0.25f, 0.22f + c1H - 0.30f, c2Z1 + 0.06f},
            glassMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{c2X0 - 0.06f, 0.45f, c2Z0 + 0.6f},
            Vec3{c2X0 + 0.06f, 0.22f + c1H - 0.30f, c2Z1 - 0.6f},
            glassTinted);
    }

    // 4. Stacked Upper-Tier 20ft Cantilever Container Lounge & Rooftop Deck Terrace
    float topY0 = 0.22f + c1H;
    float topY1 = topY0 + 2.85f;
    float uX0 = c1X0 + 1.6f;
    float uX1 = uX0 + 7.2f;
    float uZ0 = c1Z0 - 0.4f;
    float uZ1 = c1Z1 + 1.0f; // Cantilevers forward over courtyard!

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{uX0, topY0, uZ0},
        Vec3{uX1, topY1, uZ1},
        signOrange);
    // Vertical steel corrugation ribs on upper orange container
    for (float rx = uX0 + 0.4f; rx < uX1 - 0.4f; rx += 0.45f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rx - 0.05f, topY0 + 0.12f, uZ0 - 0.05f},
            Vec3{rx + 0.05f, topY1 - 0.12f, uZ0},
            signOrange);
    }
    // Panoramic upper glass window + framing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{uX0 + 0.6f, topY0 + 0.45f, uZ1 - 0.05f},
        Vec3{uX1 - 0.6f, topY1 - 0.35f, uZ1 + 0.06f},
        glassTinted);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{uX1, topY0, (uZ0 + uZ1) * 0.5f},
        1.0f, 2.2f, Vec3{1, 0, 0}, Vec3{0, 0, 1},
        DoorType::SingleModern, false);

    // Rooftop Deck Terrace on top of Ground Container (Right of Upper Container)
    float deckX0 = uX1;
    float deckX1 = std::max(uX1 + 2.5f, c1X1);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{deckX0, topY0, c1Z0},
        Vec3{deckX1, topY0 + 0.10f, c1Z1},
        timberMat);
    // Terrace Glass Balustrades & Planter Box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{deckX0, topY0 + 0.10f, c1Z1 - 0.08f},
        Vec3{deckX1, topY0 + 1.10f, c1Z1},
        glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{deckX1 - 0.08f, topY0 + 0.10f, c1Z0},
        Vec3{deckX1, topY0 + 1.10f, c1Z1},
        glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{deckX0 + 0.3f, topY0 + 0.10f, c1Z0 + 0.2f},
        Vec3{deckX1 - 0.3f, topY0 + 0.55f, c1Z0 + 0.85f},
        planterMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{deckX0 + 0.35f, topY0 + 0.55f, c1Z0 + 0.25f},
        Vec3{deckX1 - 0.35f, topY0 + 0.85f, c1Z0 + 0.80f},
        turfMat);

    // Exterior Industrial Steel Staircase rising to Upper Terrace Deck (-X side)
    int steps = 10;
    float stairX0 = c1X0 - 1.2f;
    float stairX1 = c1X0 - 0.1f;
    for (int s = 0; s < steps; ++s) {
        float sy = 0.22f + (s + 1) * ((topY0 - 0.22f) / static_cast<float>(steps));
        float sz = c1Z1 + 0.5f - s * 0.32f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{stairX0, sy - 0.08f, sz - 0.14f},
            Vec3{stairX1, sy, sz + 0.14f},
            frameMat);
    }
    // Handrail along stairs
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stairX0, 1.15f, c1Z1 + 0.5f},
        Vec3{stairX0, topY0 + 0.95f, c1Z1 + 0.5f - steps * 0.32f},
        0.04f, 0.04f, 8, blackAlum);

    // Illuminated Rooftop Box Signage Board
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{uX0 + 0.8f, topY1, uZ1 - 0.25f},
        Vec3{uX1 - 0.8f, topY1 + 0.90f, uZ1 - 0.05f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{uX0 + 1.0f, topY1 + 0.15f, uZ1 - 0.05f},
        Vec3{uX1 - 1.0f, topY1 + 0.75f, uZ1 + 0.02f},
        signOrange);
}

} // namespace procedural::buildings
