#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendVehicleSilhouette(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float yGround, float cz,
    float length, float width, float height,
    uint32_t bodyMat, uint32_t cabinMat, uint32_t tireMat
) {
    float hw = width * 0.5f;
    float hl = length * 0.5f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw, yGround + 0.24f, cz - hl},
        Vec3{cx + hw, yGround + height * 0.56f, cz + hl},
        bodyMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw * 0.85f, yGround + height * 0.56f, cz - hl * 0.48f},
        Vec3{cx + hw * 0.85f, yGround + height, cz + hl * 0.34f},
        cabinMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{cx - hw * 0.80f, yGround + height - 0.05f, cz - hl * 0.44f},
        Vec3{cx + hw * 0.80f, yGround + height + 0.02f, cz + hl * 0.30f},
        bodyMat
    );
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-hl * 0.58f, hl * 0.58f}) {
            float wx = cx + sx * (hw - 0.06f);
            float wy = yGround + 0.33f;
            float wz = cz + sz;
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx - 0.13f, wy, wz},
                Vec3{wx + 0.13f, wy, wz},
                0.33f, 0.33f, 12, tireMat, true, true
            );
        }
    }
}

void appendStoneWallFeature(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    uint32_t baseStone, uint32_t accentStone
) {
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{x0, y0, z0}, Vec3{x1, y1, z1}, baseStone);
    int rows = 6;
    float rowH = (y1 - y0) / static_cast<float>(rows);
    for (int r = 0; r < rows; ++r) {
        float ry = y0 + r * rowH + 0.04f;
        float span = (x1 - x0);
        int cols = std::max(3, static_cast<int>(span / 0.85f));
        float cw = span / static_cast<float>(cols);
        for (int c = 0; c < cols; ++c) {
            float shift = (r % 2 == 0) ? 0.0f : cw * 0.35f;
            float bx0 = std::max(x0, x0 + c * cw + shift * 0.15f + 0.03f);
            float bx1 = std::min(x1, bx0 + cw - 0.08f);
            if (bx1 - bx0 > 0.12f) {
                float dz = ((r + c) % 2 == 0) ? 0.07f : 0.03f;
                BuildingComponents::appendBox(
                    outVerts, outIndices,
                    Vec3{bx0, ry, z1 - 0.02f},
                    Vec3{bx1, ry + rowH - 0.08f, z1 + dz},
                    ((r + c) % 3 == 0) ? accentStone : baseStone
                );
            }
        }
    }
}

} // namespace

void HouseGen::generateCantileverVStrutMansion(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 13.0f, 20.5f);
    float d = res_scale::depth(params_, 9.0f, 13.5f);
    float flH = res_scale::floorHeight(params_, 3.25f);
    int stories = res_scale::stories(params_, 3, 4);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t crispWhite  = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t darkBasalt  = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t slateGrey   = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t stoneWall   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t warmGlass   = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t clearGlass  = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t balcGlass   = static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t frameMetal  = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t hvacMetal   = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t greenLawn   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterBox  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t tireMat     = static_cast<uint32_t>(MaterialSlot::TireRubber);

    GlassType gt = res_scale::glassType(params_, GlassType::GoldMirroredLuxury);
    WindowType wt = res_scale::windowType(params_, WindowType::PictureGlass);

    // Architectural Zone Coordinates
    float garageLeftX  = -hw + 1.6f;
    float garageRightX = hw * 0.24f;
    float towerLeftX   = garageRightX - 0.1f;
    float towerRightX  = hw * 0.54f;
    float rightWingX0  = towerRightX - 0.1f;
    float rightWingX1  = hw + 1.4f;
    float frontZ       = hd;
    float plinthY      = 0.22f;

    // 1. Landscaped Front Lawn, Paved Driveway & Floating Entry Stairs
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.2f, -0.18f, -hd - 1.2f},
        Vec3{ hw + 2.4f, 0.02f,  hd + 5.8f},
        greenLawn);
    // Paved driveway apron leading to 2-car garage portal
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garageLeftX - 0.4f, 0.02f, -1.0f},
        Vec3{rightWingX1 + 0.6f, 0.08f, hd + 4.8f},
        tarmacMat);
    // Foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.2f, 0.0f, -hd - 0.2f},
        Vec3{rightWingX1, plinthY, frontZ + 0.4f},
        stoneWall);

    // Floating stepped white entry stairs on front-left (-X) leading up to 1st floor entry platform
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.2f, 0.08f, hd - 2.6f},
        Vec3{garageLeftX + 0.2f, plinthY + 1.55f, hd + 0.6f},
        darkBasalt);
    for (int st = 0; st < 5; ++st) {
        float sy0 = 0.20f + st * 0.28f;
        float sz1 = hd + 2.6f - st * 0.46f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 1.05f, sy0, sz1 - 0.56f},
            Vec3{garageLeftX + 0.05f, sy0 + 0.12f, sz1},
            crispWhite);
    }
    // Glowing white spherical garden bollard by garage entry
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{garageLeftX + 0.85f, 0.08f, hd + 1.15f},
        Vec3{garageLeftX + 0.85f, 0.52f, hd + 1.15f},
        0.24f, 0.20f, 12, crispWhite, true, true
    );
    // Draped green hedge planter above left entry stair wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.25f, plinthY + 1.55f, hd - 2.4f},
        Vec3{garageLeftX + 0.15f, plinthY + 2.25f, hd + 0.72f},
        greenLawn);

    // 2. Ground Level: Open Crisp White Garage Portal & Right Patio Stone Wall
    float gTopY = plinthY + flH;
    // Recessed main ground structural volume behind open carport & patio
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.4f, plinthY, -hd},
        Vec3{garageLeftX + 0.55f, gTopY, frontZ - 1.2f},
        slateGrey);
    // Rugged stone rear wall inside open garage bay
    appendStoneWallFeature(
        outVerts, outIndices,
        garageLeftX + 0.45f, plinthY, frontZ - 4.4f,
        garageRightX - 0.35f, gTopY - 0.15f, frontZ - 4.0f,
        stoneWall, darkBasalt
    );
    // Warm vertical timber/slate rear door inside carport
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garageLeftX + 2.8f, plinthY, frontZ - 4.0f},
        Vec3{garageRightX - 1.4f, gTopY - 0.35f, frontZ - 3.65f},
        darkBasalt);

    // Crisp White Garage Portal Frame (Left pier, right pier, deep horizontal soffit canopy)
    float portZ1 = frontZ + 0.65f;
    float portThick = 0.42f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garageLeftX, plinthY, frontZ - 1.8f},
        Vec3{garageLeftX + portThick, gTopY, portZ1},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garageRightX - portThick, plinthY, frontZ - 2.6f},
        Vec3{garageRightX, gTopY, portZ1},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garageLeftX, gTopY - 0.46f, frontZ - 4.2f},
        Vec3{garageRightX + 0.15f, gTopY + 0.18f, portZ1},
        crispWhite);

    // Optional twin SUV / pickup silhouettes parked in open garage bay
    if (params_.hasGarage) {
        float span = garageRightX - garageLeftX;
        appendVehicleSilhouette(
            outVerts, outIndices,
            garageLeftX + span * 0.32f, 0.08f, frontZ - 0.95f,
            4.25f, 1.88f, 1.62f,
            static_cast<uint32_t>(MaterialSlot::CommercialAccentRed),
            static_cast<uint32_t>(MaterialSlot::GlassTinted), tireMat
        );
        appendVehicleSilhouette(
            outVerts, outIndices,
            garageLeftX + span * 0.68f, 0.08f, frontZ - 1.25f,
            4.35f, 1.90f, 1.68f,
            static_cast<uint32_t>(MaterialSlot::VehicleBlue),
            static_cast<uint32_t>(MaterialSlot::GlassTinted), tireMat
        );
    }

    // Right-Side Ground Garden Patio (Stone backdrop + slatted pergola canopy)
    appendStoneWallFeature(
        outVerts, outIndices,
        rightWingX0 + 0.45f, plinthY, -0.6f,
        rightWingX1 - 0.3f, gTopY - 0.25f, 0.35f,
        stoneWall, crispWhite
    );
    // Back patio room with warm glazing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0 + 0.2f, plinthY, -hd},
        Vec3{rightWingX1 - 0.2f, gTopY, -0.6f},
        darkBasalt);
    // Projecting slatted pergola overhang above right ground patio
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerRightX, gTopY - 0.26f, -0.2f},
        Vec3{rightWingX1 + 0.4f, gTopY + 0.08f, frontZ * 0.58f},
        slateGrey);
    for (int r = 0; r < 9; ++r) {
        float rx = towerRightX + 0.2f + r * ((rightWingX1 - towerRightX) / 9.0f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rx, gTopY - 0.34f, 0.1f},
            Vec3{rx + 0.10f, gTopY - 0.24f, frontZ * 0.58f},
            crispWhite);
    }
    // Hedge planter sitting along the pergola edge
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0 + 1.0f, gTopY + 0.08f, frontZ * 0.34f},
        Vec3{rightWingX1 - 0.3f, gTopY + 0.62f, frontZ * 0.56f},
        greenLawn);

    // 3. Signature 3-Story Vertical Dark Basalt Atrium / Stairwell Core Tower
    float towerTopY = plinthY + stories * flH + 0.55f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerLeftX, plinthY, -hd * 0.65f},
        Vec3{towerRightX, towerTopY, frontZ + 0.12f},
        darkBasalt);
    // Dark charcoal recessed reveal collar surrounding vertical curtain window
    float winPad = 0.45f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerLeftX + winPad, plinthY + 0.15f, frontZ + 0.08f},
        Vec3{towerRightX - winPad, towerTopY - 0.75f, frontZ + 0.22f},
        blackAlum);
    // Full-height glowing 3-story vertical curtain glass bay
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerLeftX + winPad + 0.14f, plinthY + 0.20f, frontZ + 0.14f},
        Vec3{towerRightX - winPad - 0.14f, towerTopY - 0.90f, frontZ + 0.24f},
        warmGlass);
    // Fine horizontal black transoms & vertical center mullion
    float atriumTopY = towerTopY - 0.90f;
    float atriumMidX = 0.5f * (towerLeftX + towerRightX);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{atriumMidX - 0.04f, plinthY + 0.20f, frontZ + 0.22f},
        Vec3{atriumMidX + 0.04f, atriumTopY, frontZ + 0.28f},
        blackAlum);
    for (float my = plinthY + 0.65f; my < atriumTopY - 0.2f; my += 0.56f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{towerLeftX + winPad + 0.12f, my - 0.025f, frontZ + 0.22f},
            Vec3{towerRightX - winPad - 0.12f, my + 0.025f, frontZ + 0.28f},
            blackAlum);
    }

    // 4. Second-Floor Wrap Terrace Deck & Dark Glass Balustrade Above Garage
    float l2Y = gTopY;
    float l3Y = gTopY + flH;
    // Recessed 2nd-story interior mass behind wraparound terrace
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, l2Y, -hd},
        Vec3{towerLeftX + 0.1f, l3Y, frontZ - 1.55f},
        slateGrey);
    // Large dark terrace glazing behind V-struts
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{garageLeftX + 0.6f, l2Y + 0.18f, frontZ - 1.58f},
        Vec3{towerLeftX - 0.25f, l2Y + flH * 0.78f, frontZ - 1.48f},
        warmGlass);
    // Wraparound dark glass terrace railing across front of 2nd-floor deck
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{garageLeftX + 0.1f, l2Y + 0.18f, portZ1 - 0.12f},
            Vec3{towerLeftX - 0.05f, l2Y + 1.12f, portZ1 - 0.06f},
            balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{garageLeftX + 0.1f, l2Y + 1.12f, portZ1 - 0.14f},
            Vec3{towerLeftX - 0.05f, l2Y + 1.18f, portZ1 - 0.04f},
            blackAlum);
    }

    // 5. SIGNATURE UPPER-LEFT CANTILEVERED WHITE BOX + ANGLED V-STRUTS
    float cantLeftX  = -hw - 0.35f;
    float cantRightX = towerLeftX - 0.05f;
    float cantBotY   = l3Y - 0.55f;
    float cantTopY   = l3Y + flH * 0.88f;
    float cantZBack  = -hd * 0.25f;
    float cantZFront = frontZ + 1.55f;

    // Cantilevered white floor slab & deep left return wall & ceiling soffit
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cantLeftX, cantBotY, cantZBack},
        Vec3{cantRightX, cantBotY + 0.34f, cantZFront},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cantLeftX, cantTopY - 0.32f, cantZBack},
        Vec3{cantRightX, cantTopY, cantZFront},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cantLeftX, cantBotY, cantZBack},
        Vec3{cantLeftX + 3.10f, cantTopY, cantZFront},
        crispWhite);
    // Right cheek wall of white cantilevered frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cantRightX - 0.42f, cantBotY, cantZBack},
        Vec3{cantRightX, cantTopY, cantZFront},
        crispWhite);

    // Recessed warm-lit lounge glazing inside cantilevered white box opening
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cantLeftX + 3.05f, cantBotY + 0.34f, cantZFront - 2.4f},
        Vec3{cantRightX - 0.42f, cantTopY - 0.32f, cantZFront - 2.25f},
        warmGlass);

    // Balcony glass balustrade inside the cantilevered white opening
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cantLeftX + 3.10f, cantBotY + 0.34f, cantZFront - 0.22f},
            Vec3{cantRightX - 0.42f, cantBotY + 1.28f, cantZFront - 0.16f},
            balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cantLeftX + 3.10f, cantBotY + 1.28f, cantZFront - 0.24f},
            Vec3{cantRightX - 0.42f, cantBotY + 1.34f, cantZFront - 0.14f},
            blackAlum);
    }

    // Twin Vertical Dark Basalt Architectural Feature Fins bisecting the Cantilever Box
    float finMidX = 0.5f * ((cantLeftX + 3.10f) + cantRightX);
    for (float off : {-0.26f, 0.26f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{finMidX + off - 0.12f, cantBotY - 0.25f, cantZFront - 0.35f},
            Vec3{finMidX + off + 0.12f, cantTopY + 0.25f, cantZFront + 0.18f},
            darkBasalt);
    }

    // SIGNATURE ANGLED WHITE V-STRUTS supporting Cantilevered Box down to 2nd Floor Terrace
    float vApexX = 0.5f * ((cantLeftX + 2.8f) + cantRightX);
    float vZ = cantZFront - 0.72f;
    float vBaseZ = portZ1 - 0.55f;
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{vApexX - 0.18f, l2Y + 0.16f, vBaseZ},
        Vec3{vApexX - 1.35f, cantBotY + 0.04f, vZ},
        0.16f, 0.16f, 12, crispWhite, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{vApexX + 0.18f, l2Y + 0.16f, vBaseZ},
        Vec3{vApexX + 1.35f, cantBotY + 0.04f, vZ},
        0.16f, 0.16f, 12, crispWhite, true, true
    );

    // 6. Left-Rear 3rd-Floor Stairwell Window Bay & Floating Rooftop Canopy
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.2f, plinthY, -hd},
        Vec3{cantLeftX + 3.2f, l3Y + flH * 1.15f, -hd * 0.15f},
        slateGrey);
    // Vertical left side staircase curtain window
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.04f, plinthY + 1.2f, -hd * 0.05f},
        Vec3{-hw + 0.22f, l3Y + flH * 0.75f, frontZ * 0.15f},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.06f, plinthY + 1.35f, -hd * 0.02f},
        Vec3{-hw + 0.18f, l3Y + flH * 0.65f, frontZ * 0.12f},
        clearGlass);

    // Floating top white rooftop pergola canopy slab above the cantilevered white box
    float topCanopyY = cantTopY + 1.45f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cantLeftX + 1.8f, topCanopyY - 0.22f, -hd * 0.20f},
        Vec3{towerLeftX + 0.15f, topCanopyY, frontZ + 0.45f},
        crispWhite);
    // Supporting return cheek wall connecting floating canopy back to basalt tower
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{cantLeftX + 1.8f, cantTopY, -hd * 0.20f},
        Vec3{cantLeftX + 2.25f, topCanopyY, frontZ - 0.55f},
        crispWhite);

    // 7. Right-Side Offset 2-Story Cantilevered White Wing
    float rwBotY = l2Y + 0.55f;
    float rwTopY = l3Y + flH * 0.65f;
    float rwZFront = frontZ * 0.64f;
    // Recessed dark glass interior box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0, rwBotY, -hd * 0.85f},
        Vec3{rightWingX1 - 0.1f, rwTopY, rwZFront - 1.25f},
        slateGrey);
    // Glazed openings on both levels of right wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0 + 1.1f, rwBotY + 0.25f, rwZFront - 1.28f},
        Vec3{rightWingX1 - 0.45f, rwTopY - 0.45f, rwZFront - 1.15f},
        warmGlass);
    // White cantilevered frame perimeter around right wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0 + 0.35f, rwBotY, -hd * 0.5f},
        Vec3{rightWingX1, rwBotY + 0.26f, rwZFront},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0 + 0.35f, rwTopY - 0.28f, -hd * 0.5f},
        Vec3{rightWingX1, rwTopY, rwZFront},
        crispWhite);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{rightWingX0 + 0.35f, rwBotY, rwZFront - 1.8f},
        Vec3{rightWingX0 + 1.65f, rwTopY, rwZFront},
        crispWhite);
    // Vertical architectural reveals on white wing front cheek
    for (int g = 0; g < 3; ++g) {
        float gx = rightWingX0 + 0.65f + g * 0.32f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{gx, rwBotY + 0.2f, rwZFront - 0.02f},
            Vec3{gx + 0.06f, rwTopY - 0.2f, rwZFront + 0.05f},
            slateGrey);
    }
    if (res_scale::showBalconies(params_)) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rightWingX0 + 1.65f, rwBotY + 0.26f, rwZFront - 0.14f},
            Vec3{rightWingX1 - 0.15f, rwBotY + 1.18f, rwZFront - 0.08f},
            balcGlass);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{rightWingX0 + 1.65f, rwBotY + 1.18f, rwZFront - 0.16f},
            Vec3{rightWingX1 - 0.15f, rwBotY + 1.24f, rwZFront - 0.06f},
            blackAlum);
    }

    // 8. Rear & Side Windows (360° Complete Envelope)
    for (int s = 0; s < stories; ++s) {
        float fy = plinthY + s * flH;
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{0.0f, 0.0f, -hd - 0.03f}, w * 0.80f, fy, flH,
            Vec3{0, 0, -1}, Vec3{-1, 0, 0},
            4, wt, gt, 1.80f, flH * 0.64f, 0.45f,
            0.0f, 0.0f, false, false);
        res_scale::appendScaledWindowRow(
            outVerts, outIndices, params_,
            Vec3{rightWingX1 + 0.03f, 0.0f, -hd * 0.2f}, d * 0.55f, fy, flH,
            Vec3{1, 0, 0}, Vec3{0, 0, -1},
            2, wt, gt, 1.50f, flH * 0.60f, 0.50f,
            0.0f, 0.0f, false, false);
    }

    // 9. Twin Cylindrical Rooftop Water Storage Tanks on Top of Basalt Core Tower
    if (res_scale::showClutter(params_, true)) {
        float tMidX = 0.5f * (towerLeftX + towerRightX);
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{tMidX - 1.15f, towerTopY, -0.4f},
            Vec3{tMidX - 1.15f, towerTopY + 1.45f, -0.4f},
            0.78f, 0.78f, 16, hvacMetal, true, true
        );
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{tMidX + 0.95f, towerTopY, 0.25f},
            Vec3{tMidX + 0.95f, towerTopY + 1.55f, 0.25f},
            0.82f, 0.82f, 16, hvacMetal, true, true
        );
    }

    // Flat Roof Caps
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerLeftX - 0.15f, towerTopY - 0.12f, -hd * 0.65f - 0.15f},
        Vec3{towerRightX + 0.15f, towerTopY + 0.06f, frontZ + 0.24f},
        slateGrey);
}

} // namespace procedural::buildings
