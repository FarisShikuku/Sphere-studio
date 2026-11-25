#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateParkingGarage(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(28.0f, params_.footprintWidth);
    float d = std::max(22.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    int32_t levels = std::clamp(params_.stories, 2, 10);
    float flH = std::max(2.9f, params_.storyHeight);

    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat     = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t evEmerald   = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t yellowMat   = static_cast<uint32_t>(MaterialSlot::SafetyYellow);

    // 1. Entry/Exit Plaza Asphalt Apron
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, -0.35f, -hd - 3.0f},
        Vec3{ hw + 4.0f,  0.00f,  hd + 6.0f},
        tarmacMat);

    // 2. Ground Floor Concrete Deck Pad
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.3f, 0.0f, -hd - 0.3f},
        Vec3{ hw + 0.3f, 0.25f, hd + 0.3f},
        foundMat);

    // Structural column grid spacing
    int colsX = std::clamp(static_cast<int>(w / 7.5f), 3, 7);
    int colsZ = std::clamp(static_cast<int>(d / 7.5f), 3, 6);
    float stepX = (w - 1.2f) / static_cast<float>(colsX);
    float stepZ = (d - 1.2f) / static_cast<float>(colsZ);

    // Reserve front-left bay for sloped vehicle ramp between floors!
    float rampX0 = -hw + 0.6f;
    float rampX1 = rampX0 + std::clamp(w * 0.28f, 6.5f, 10.5f);
    float rampZ0 = -hd + 3.5f;
    float rampZ1 =  hd - 3.5f;

    // 3. Multi-Level Open Parking Decks, Precast Spandrel Parapets, & Cable Guardrails
    for (int lv = 0; lv <= levels; ++lv) {
        float y = lv * flH;
        float slabThick = 0.32f;

        if (lv > 0) {
            // Main floor slab right of the ramp shaft
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rampX1, y - slabThick, -hd},
                Vec3{hw,     y,              hd},
                wallMat);
            // Front & Rear landing slabs aligned with ramp ends
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw,    y - slabThick, -hd},
                Vec3{rampX1, y,              rampZ0},
                wallMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw,    y - slabThick,  rampZ1},
                Vec3{rampX1, y,              hd},
                wallMat);

            // Sloped Concrete Vehicle Ramp Deck ascending from floor (lv-1) to lv
            float prevY = (lv - 1) * flH;
            bool rampUpForward = (lv % 2 == 1);
            float yStart = rampUpForward ? prevY : y;
            float yEnd   = rampUpForward ? y : prevY;

            // Top driving surface of ramp
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{rampX0, yEnd,   rampZ1},
                Vec3{rampX1, yEnd,   rampZ1},
                Vec3{rampX1, yStart, rampZ0},
                Vec3{rampX0, yStart, rampZ0},
                Vec3{0.0f, 0.95f, rampUpForward ? -0.30f : 0.30f},
                foundMat);
            // Bottom soffit surface of ramp
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{rampX0, yStart - 0.28f, rampZ0},
                Vec3{rampX1, yStart - 0.28f, rampZ0},
                Vec3{rampX1, yEnd   - 0.28f, rampZ1},
                Vec3{rampX0, yEnd   - 0.28f, rampZ1},
                Vec3{0.0f, -0.95f, 0.0f},
                wallMat);
            // Side ramp curb beam
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{rampX1, yStart - 0.28f, rampZ0},
                Vec3{rampX1, yEnd   - 0.28f, rampZ1},
                Vec3{rampX1, yEnd   + 0.45f, rampZ1},
                Vec3{rampX1, yStart + 0.45f, rampZ0},
                Vec3{1.0f, 0.0f, 0.0f},
                yellowMat);

            // Perimeter Precast Concrete Safety Spandrel Parapets (1.05m high) + Steel Cable Guardrails
            float parapetH = 1.05f;
            float spandrelTop = y + parapetH;
            // Front (+Z) except entry lane on level 1
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw - 0.12f, y - slabThick, hd - 0.25f},
                Vec3{ hw + 0.12f, spandrelTop,   hd + 0.12f},
                wallMat);
            // Back (-Z)
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw - 0.12f, y - slabThick, -hd - 0.12f},
                Vec3{ hw + 0.12f, spandrelTop,   -hd + 0.25f},
                wallMat);
            // Left (-X) & Right (+X)
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw - 0.12f, y - slabThick, -hd},
                Vec3{-hw + 0.25f, spandrelTop,    hd},
                wallMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{ hw - 0.25f, y - slabThick, -hd},
                Vec3{ hw + 0.12f, spandrelTop,    hd},
                wallMat);

            // Brushed aluminum horizontal safety rail above parapet spandrel
            float railY = spandrelTop + 0.25f;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw - 0.08f, railY, hd - 0.08f},
                Vec3{ hw + 0.08f, railY + 0.08f, hd + 0.08f},
                silverAlum);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{-hw - 0.08f, railY, -hd - 0.08f},
                Vec3{ hw + 0.08f, railY + 0.08f, -hd + 0.08f},
                silverAlum);
        }

        // Structural Concrete Columns & Architectural Perforated Sun-Screen Fins between levels
        if (lv < levels) {
            float yBot = (lv == 0) ? 0.25f : y;
            float yTop = (lv + 1) * flH - slabThick;

            for (int ix = 0; ix <= colsX; ++ix) {
                float cx = -hw + 0.6f + ix * stepX;
                for (int iz = 0; iz <= colsZ; ++iz) {
                    float cz = -hd + 0.6f + iz * stepZ;
                    // Skip columns inside the ramp lane
                    if (cx > rampX0 + 0.4f && cx < rampX1 - 0.4f && cz > rampZ0 + 0.4f && cz < rampZ1 - 0.4f) {
                        continue;
                    }
                    // Skip front ground entry vehicle aisle
                    if (lv == 0 && iz == colsZ && cx > -4.5f && cx < 4.5f) {
                        continue;
                    }
                    BuildingComponents::appendBox(outVerts, outIndices,
                        Vec3{cx - 0.26f, yBot, cz - 0.26f},
                        Vec3{cx + 0.26f, yTop, cz + 0.26f},
                        wallMat);
                }
            }

            // Architectural vertical aluminum louver fins along front-right facade
            if (lv > 0) {
                for (float fx = 2.0f; fx <= hw - 1.8f; fx += 1.4f) {
                    BuildingComponents::appendBox(outVerts, outIndices,
                        Vec3{fx - 0.05f, yBot + 1.0f, hd - 0.05f},
                        Vec3{fx + 0.05f, yTop, hd + 0.28f},
                        silverAlum);
                }
            }
        }
    }

    // 4. Rooftop Deck EV Charging Canopy & Parking Striping
    float roofY = levels * flH;
    // Painted parking stalls on top roof deck
    for (float sx = rampX1 + 1.5f; sx <= hw - 2.0f; sx += 2.9f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx, roofY, -hd + 1.5f},
            Vec3{sx + 0.10f, roofY + 0.03f, -hd + 6.2f},
            silverAlum);
    }
    // Rooftop Solar PV Shade Canopy over top-level EV stalls
    float shadeX0 = rampX1 + 1.2f;
    float shadeX1 = hw - 1.5f;
    float shadeZ0 = -hd + 1.2f;
    float shadeZ1 = -hd + 6.8f;
    float shadeH  = roofY + 3.2f;
    if (shadeX1 > shadeX0 + 4.0f) {
        for (float px : {shadeX0 + 0.8f, (shadeX0 + shadeX1) * 0.5f, shadeX1 - 0.8f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{px, roofY, (shadeZ0 + shadeZ1) * 0.5f},
                Vec3{px, shadeH, (shadeZ0 + shadeZ1) * 0.5f},
                0.15f, 0.15f, 8, blackAlum);
        }
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{shadeX0, shadeH, shadeZ0},
            Vec3{shadeX1, shadeH + 0.18f, shadeZ1},
            glassTinted);
        // Emerald EV signage strip on rooftop shade canopy
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{shadeX0, shadeH - 0.05f, shadeZ1},
            Vec3{shadeX1, shadeH + 0.15f, shadeZ1 + 0.06f},
            evEmerald);
    }

    // 5. Corner Glazed Elevator & Stairwell Core Tower (Front-Right Corner, rising above roof)
    float coreX0 = hw - 5.2f;
    float coreX1 = hw + 0.4f;
    float coreZ0 = hd - 5.2f;
    float coreZ1 = hd + 0.6f;
    float coreTopY = roofY + 3.8f;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{coreX0, 0.0f, coreZ0},
        Vec3{coreX1, coreTopY, coreZ1},
        wallMat);
    // Vertical glazed curtain wall strip running up full height of elevator core
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{coreX0 + 0.8f, 0.5f, coreZ1 - 0.15f},
        Vec3{coreX1 - 0.8f, coreTopY - 0.5f, coreZ1 + 0.08f},
        glassTinted);
    // Elevator machine penthouse cap + illuminated 'P' accent band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{coreX0 - 0.15f, coreTopY, coreZ0 - 0.15f},
        Vec3{coreX1 + 0.15f, coreTopY + 0.45f, coreZ1 + 0.15f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{coreX0 + 0.6f, coreTopY - 0.65f, coreZ1 + 0.08f},
        Vec3{coreX1 - 0.6f, coreTopY - 0.10f, coreZ1 + 0.22f},
        evEmerald);

    // 6. Ground-Level Entry/Exit Ticket Booth & Boom Barrier Gates (+Z Plaza)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.1f, 0.25f, hd + 1.2f},
        Vec3{ 1.1f, 2.75f, hd + 3.4f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.9f, 1.15f, hd + 1.15f},
        Vec3{ 0.9f, 2.25f, hd + 3.45f},
        glassTinted);
    // Left & Right Boom Barrier Gate Housings + Striped Barrier Arms
    for (float gx : {-3.6f, 3.6f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{gx - 0.22f, 0.0f, hd + 2.0f},
            Vec3{gx + 0.22f, 1.10f, hd + 2.45f},
            yellowMat);
        // Horizontal boom gate arm
        float dirSign = (gx < 0.0f) ? 1.0f : -1.0f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{std::min(gx, gx + dirSign * 2.3f), 0.92f, hd + 2.18f},
            Vec3{std::max(gx, gx + dirSign * 2.3f), 1.04f, hd + 2.28f},
            signRed);
    }
    // Overhead "PARKING / CLEARANCE" portal beam at ground entry
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-4.4f, flH - 0.85f, hd - 0.1f},
        Vec3{ 4.4f, flH - 0.25f, hd + 0.25f},
        yellowMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-4.0f, flH - 0.72f, hd + 0.25f},
        Vec3{ 4.0f, flH - 0.38f, hd + 0.30f},
        frameMat);
}

} // namespace procedural::buildings
