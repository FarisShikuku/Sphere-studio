#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateCoffeeKiosk(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(10.0f, params_.footprintWidth);
    float d = std::max(9.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float roofH = std::max(3.4f, params_.storyHeight);

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t brickMat    = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t stuccoMat   = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t champAlum   = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);

    // 1. Urban Sidewalk Plaza & Timber Cafe Terrace Deck
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.30f, -hd - 1.5f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 3.5f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, 0.00f, -hd - 0.4f},
        Vec3{ hw + 0.5f, 0.22f,  hd + 2.4f},
        pergolaMat);

    // 2. Boutique Cafe Kiosk Building Footprint (Rear 62% of terrace)
    float cafeX0 = -hw * 0.72f;
    float cafeX1 =  hw * 0.72f;
    float cafeZ0 = -hd * 0.72f;
    float cafeZ1 =  hd * 0.22f;

    // Cafe structural rear brick wall + timber clad side walls
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0, 0.22f, cafeZ0},
        Vec3{cafeX1, roofH, cafeZ0 + 0.45f},
        brickMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0, 0.22f, cafeZ0},
        Vec3{cafeX0 + 0.40f, roofH, cafeZ1},
        timberMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX1 - 0.40f, 0.22f, cafeZ0},
        Vec3{cafeX1, roofH, cafeZ1},
        timberMat);

    // Brick Espresso Roaster Chimney Stack on Rear-Left Corner
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 + 0.4f, 0.22f, cafeZ0 - 0.35f},
        Vec3{cafeX0 + 1.8f, roofH + 2.1f, cafeZ0 + 0.75f},
        brickMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 + 0.3f, roofH + 2.1f, cafeZ0 - 0.45f},
        Vec3{cafeX0 + 1.9f, roofH + 2.35f, cafeZ0 + 0.85f},
        foundMat);

    // Matte-Black Framed Glazed Cafe Storefront & Interior Espresso Bar (+Z front)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 + 0.40f, 0.22f, cafeZ0 + 0.45f},
        Vec3{cafeX1 - 0.40f, roofH - 0.35f, cafeZ1 - 0.05f},
        glassTinted);
    // Warm champagne aluminum espresso counter inside glass
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 + 1.0f, 0.22f, cafeZ1 - 1.2f},
        Vec3{cafeX1 - 2.6f, 1.20f, cafeZ1 - 0.55f},
        champAlum);
    // Exterior walk-up takeaway counter ledge + storefront mullions
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 + 0.6f, 1.05f, cafeZ1},
        Vec3{cafeX1 - 2.4f, 1.16f, cafeZ1 + 0.48f},
        pergolaMat);
    for (float mx = cafeX0 + 0.45f; mx <= cafeX1 - 0.45f; mx += 1.8f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.06f, 0.22f, cafeZ1 - 0.08f},
            Vec3{mx + 0.06f, roofH - 0.35f, cafeZ1 + 0.06f},
            blackAlum);
    }
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{cafeX1 - 1.45f, 0.22f, cafeZ1},
        1.15f, 2.35f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, true);

    // 3. Signature Striped Bistro Awning Canopy & Monopitch Shed Roof with Skylight Clerestory
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 - 0.35f, roofH - 0.25f, cafeZ0 - 0.35f},
        Vec3{cafeX1 + 0.35f, roofH + 0.30f, cafeZ1 + 0.35f},
        blackAlum);
    // Sloped cafe facade awning projecting over sidewalk seating
    float awY0 = 2.85f;
    float awY1 = 2.35f;
    float awZ0 = cafeZ1 + 0.02f;
    float awZ1 = cafeZ1 + 1.85f;
    int stripes = 5;
    float awSpan = (cafeX1 - cafeX0) / static_cast<float>(stripes);
    for (int s = 0; s < stripes; ++s) {
        float sx0 = cafeX0 + s * awSpan;
        float sx1 = sx0 + awSpan;
        uint32_t stripeCol = (s % 2 == 0) ? signRed : stuccoMat;
        BuildingComponents::appendQuad(outVerts, outIndices,
            Vec3{sx0, awY1, awZ1}, Vec3{sx1, awY1, awZ1},
            Vec3{sx1, awY0, awZ0}, Vec3{sx0, awY0, awZ0},
            Vec3{0.0f, 0.92f, 0.38f}, stripeCol);
        BuildingComponents::appendQuad(outVerts, outIndices,
            Vec3{sx0, awY0, awZ0}, Vec3{sx1, awY0, awZ0},
            Vec3{sx1, awY1, awZ1}, Vec3{sx0, awY1, awZ1},
            Vec3{0.0f, -0.92f, -0.38f}, stripeCol);
    }

    // Rooftop Clerestory Skylight Monitor Lantern & Illuminated "CAFE" Signboard
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 + 1.5f, roofH + 0.30f, cafeZ0 + 1.0f},
        Vec3{cafeX1 - 1.5f, roofH + 1.05f, cafeZ1 - 0.8f},
        glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cafeX0 + 1.4f, roofH + 1.05f, cafeZ0 + 0.9f},
        Vec3{cafeX1 - 1.4f, roofH + 1.25f, cafeZ1 - 0.7f},
        blackAlum);
    // Front Cafe Menu / Roastery Marquee Sign
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.4f, roofH + 0.30f, cafeZ1 - 0.1f},
        Vec3{ 2.4f, roofH + 1.10f, cafeZ1 + 0.25f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.2f, roofH + 0.45f, cafeZ1 + 0.25f},
        Vec3{ 2.2f, roofH + 0.95f, cafeZ1 + 0.30f},
        champAlum);

    // 4. Outdoor Sidewalk Bistro Patio Tables, Chairs & Planter Boxes (+Z Terrace)
    float patioZ = (cafeZ1 + hd + 2.2f) * 0.5f + 0.3f;
    for (int t = -1; t <= 1; ++t) {
        float tx = t * (w * 0.30f);
        // Bistro pedestal & round tabletop
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{tx, 0.22f, patioZ},
            Vec3{tx, 0.95f, patioZ},
            0.06f, 0.06f, 8, blackAlum);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{tx, 0.95f, patioZ},
            Vec3{tx, 1.02f, patioZ},
            0.55f, 0.55f, 12, pergolaMat);
        // Flanking bistro stools
        for (int s : {-1, 1}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{tx + s * 0.85f, 0.22f, patioZ},
                Vec3{tx + s * 0.85f, 0.65f, patioZ},
                0.22f, 0.22f, 8, blackAlum);
        }
    }

    // Perimeter Living Herb Planter Boxes at Patio Edge
    for (float px : {-hw * 0.65f, hw * 0.65f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 1.2f, 0.22f, hd + 1.6f},
            Vec3{px + 1.2f, 0.82f, hd + 2.3f},
            planterMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 1.15f, 0.82f, hd + 1.65f},
            Vec3{px + 1.15f, 1.15f, hd + 2.25f},
            turfMat);
    }
}

} // namespace procedural::buildings
