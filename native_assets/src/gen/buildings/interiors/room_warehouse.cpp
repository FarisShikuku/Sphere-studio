#include "RoomGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

static void appendTireWheelSimple(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    const Vec3& center,
    const Vec3& axis,
    float radius,
    float width
) {
    uint32_t rubberMat = static_cast<uint32_t>(MaterialSlot::TireRubber);
    uint32_t rimMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    Vec3 half = axis * (width * 0.5f);
    BuildingComponents::appendOrientedCylinder(verts, indices, center - half, center + half, radius, radius, 14, rubberMat);
    Vec3 rimHalf = axis * (width * 0.54f);
    BuildingComponents::appendOrientedCylinder(verts, indices, center - rimHalf, center + rimHalf, radius * 0.56f, radius * 0.56f, 12, rimMat);
}

static void appendPalletWithCargo(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float fy, float cz,
    float palletW, float palletD, float boxH,
    uint32_t cargoMat
) {
    uint32_t woodMat = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);
    uint32_t strapMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    float hw = palletW * 0.5f;
    float hd = palletD * 0.5f;

    // 3 bottom pallet stringers
    for (int s = -1; s <= 1; ++s) {
        float sx = cx + s * (hw - 0.08f);
        BuildingComponents::appendBox(verts, indices,
            Vec3{sx - 0.06f, fy, cz - hd},
            Vec3{sx + 0.06f, fy + 0.10f, cz + hd}, woodMat);
    }
    // Top pallet deck
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - hw, fy + 0.10f, cz - hd},
        Vec3{cx + hw, fy + 0.16f, cz + hd}, woodMat);

    // Stacked cargo boxes (two split cartons or single wrapped cube)
    float splitH = boxH * 0.52f;
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - hw + 0.05f, fy + 0.16f, cz - hd + 0.05f},
        Vec3{cx + hw - 0.05f, fy + 0.16f + splitH, cz + hd - 0.05f}, cargoMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - hw + 0.08f, fy + 0.16f + splitH + 0.02f, cz - hd + 0.08f},
        Vec3{cx + hw - 0.08f, fy + 0.16f + boxH, cz + hd - 0.08f}, cargoMat);
    // Strapping band
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.04f, fy + 0.16f, cz - hd + 0.04f},
        Vec3{cx + 0.04f, fy + 0.16f + boxH + 0.02f, cz + hd - 0.04f}, strapMat);
}

static void appendForklift(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float fy, float cz,
    float facingSignZ
) {
    uint32_t orangeMat = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t blackMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t steelMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t seatMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t cargoMat = static_cast<uint32_t>(MaterialSlot::CardboardTan);

    float s = (facingSignZ >= 0.0f) ? 1.0f : -1.0f;
    // Main chassis body
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.58f, fy + 0.24f, cz - 0.95f},
        Vec3{cx + 0.58f, fy + 1.05f, cz + 0.95f}, orangeMat);
    // Rear counterweight
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.56f, fy + 0.30f, cz - s * 1.15f},
        Vec3{cx + 0.56f, fy + 1.20f, cz - s * 0.55f}, blackMat);
    // Seat & steering column
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.26f, fy + 1.05f, cz - s * 0.35f},
        Vec3{cx + 0.26f, fy + 1.55f, cz + s * 0.15f}, seatMat);
    // Overhead ROPS safety cage posts & roof grill
    for (float sx : {-0.50f, 0.50f}) {
        for (float sz : {-0.55f, 0.45f}) {
            BuildingComponents::appendBox(verts, indices,
                Vec3{cx + sx - 0.04f, fy + 1.05f, cz + sz - 0.04f},
                Vec3{cx + sx + 0.04f, fy + 2.25f, cz + sz + 0.04f}, blackMat);
        }
    }
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.54f, fy + 2.22f, cz - 0.60f},
        Vec3{cx + 0.54f, fy + 2.30f, cz + 0.50f}, blackMat);

    // 4 wheels
    appendTireWheelSimple(verts, indices, Vec3{cx - 0.58f, fy + 0.34f, cz + s * 0.70f}, Vec3{1, 0, 0}, 0.34f, 0.22f);
    appendTireWheelSimple(verts, indices, Vec3{cx + 0.58f, fy + 0.34f, cz + s * 0.70f}, Vec3{1, 0, 0}, 0.34f, 0.22f);
    appendTireWheelSimple(verts, indices, Vec3{cx - 0.52f, fy + 0.26f, cz - s * 0.75f}, Vec3{1, 0, 0}, 0.26f, 0.18f);
    appendTireWheelSimple(verts, indices, Vec3{cx + 0.52f, fy + 0.26f, cz - s * 0.75f}, Vec3{1, 0, 0}, 0.26f, 0.18f);

    // Lift mast rails
    float mastZ = cz + s * 1.05f;
    for (float mx : {-0.38f, 0.38f}) {
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx + mx - 0.06f, fy + 0.15f, mastZ - 0.06f},
            Vec3{cx + mx + 0.06f, fy + 3.05f, mastZ + 0.06f}, blackMat);
    }
    // Mast cross-brace & forks
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.44f, fy + 0.35f, mastZ + s * 0.05f},
        Vec3{cx + 0.44f, fy + 0.85f, mastZ + s * 0.15f}, steelMat);
    for (float fx : {-0.28f, 0.28f}) {
        float fz0 = mastZ + s * 0.15f;
        float fz1 = fz0 + s * 1.15f;
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx + fx - 0.05f, fy + 0.22f, std::min(fz0, fz1)},
            Vec3{cx + fx + 0.05f, fy + 0.28f, std::max(fz0, fz1)}, steelMat);
    }
    // Carried pallet on forks
    appendPalletWithCargo(verts, indices, cx, fy + 0.28f, mastZ + s * 0.75f, 1.05f, 1.05f, 0.85f, cargoMat);
}

static void appendLoadingDockBay(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float bayCenterX,
    float floorY,
    float dockWallZ,
    float bayW,
    float doorW,
    float doorH,
    float inwardZDir, // +1.0f if wall is at -Z facing +Z inside; -1.0f if at +Z facing -Z
    bool isOpenBay,
    bool hasSemiTrailer
) {
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t rubberMat = static_cast<uint32_t>(MaterialSlot::TireRubber);
    uint32_t silverMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t greenMat = static_cast<uint32_t>(MaterialSlot::HydraulicGreen);
    uint32_t redMat = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t trailerMat = static_cast<uint32_t>(MaterialSlot::VehicleWhite);

    float hwDoor = doorW * 0.5f;
    float s = (inwardZDir >= 0.0f) ? 1.0f : -1.0f;

    // 1. Exterior/Portal Dock Shelter Seal Cushions
    float sealZ0 = dockWallZ - s * 0.42f;
    float sealZ1 = dockWallZ + s * 0.12f;
    float szMin = std::min(sealZ0, sealZ1);
    float szMax = std::max(sealZ0, sealZ1);
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - hwDoor - 0.28f, floorY + 0.05f, szMin},
        Vec3{bayCenterX - hwDoor, floorY + doorH + 0.35f, szMax}, darkMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX + hwDoor, floorY + 0.05f, szMin},
        Vec3{bayCenterX + hwDoor + 0.28f, floorY + doorH + 0.35f, szMax}, darkMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - hwDoor - 0.28f, floorY + doorH, szMin},
        Vec3{bayCenterX + hwDoor + 0.28f, floorY + doorH + 0.38f, szMax}, darkMat);

    // 2. Twin Heavy Rubber Dock Bumpers below dock sill on outer truck side
    float bumpZ0 = dockWallZ - s * 0.36f;
    float bumpZ1 = dockWallZ - s * 0.05f;
    float bzMin = std::min(bumpZ0, bumpZ1);
    float bzMax = std::max(bumpZ0, bumpZ1);
    for (float bx : {bayCenterX - hwDoor + 0.35f, bayCenterX + hwDoor - 0.35f}) {
        BuildingComponents::appendBox(verts, indices,
            Vec3{bx - 0.22f, floorY - 0.35f, bzMin},
            Vec3{bx + 0.22f, floorY + 0.12f, bzMax}, rubberMat);
    }

    // 3. Roll-Up Sectional Dock Door (raised high when open so dock apron & trailer show clearly)
    float shutterBottom = isOpenBay ? (floorY + doorH * 0.72f) : (floorY + 0.35f);
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - hwDoor + 0.04f, shutterBottom, dockWallZ - 0.06f},
        Vec3{bayCenterX + hwDoor - 0.04f, floorY + doorH, dockWallZ + 0.06f}, silverMat);
    // Vision glass strip inside sectional door
    float vY = isOpenBay ? (shutterBottom + 0.12f) : (floorY + doorH * 0.56f);
    for (int vl = -1; vl <= 1; ++vl) {
        float vx = bayCenterX + vl * (doorW * 0.26f);
        BuildingComponents::appendBox(verts, indices,
            Vec3{vx - 0.24f, vY, dockWallZ - 0.08f},
            Vec3{vx + 0.24f, vY + 0.34f, dockWallZ + 0.08f}, glassMat);
    }

    // 4. Interior Hydraulic Dock Leveler Pit, Steel Diamond Plate & Safety Yellow Toe Guards
    float levLen = 2.35f;
    float levW = std::min(doorW - 0.35f, 2.25f);
    float levZ0 = dockWallZ + s * 0.05f;
    float levZ1 = dockWallZ + s * (0.05f + levLen);
    float lzMin = std::min(levZ0, levZ1);
    float lzMax = std::max(levZ0, levZ1);

    // Dock leveler pit trim frame
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - levW * 0.5f - 0.08f, floorY + 0.005f, lzMin - 0.06f},
        Vec3{bayCenterX + levW * 0.5f + 0.08f, floorY + 0.03f, lzMax + 0.06f}, darkMat);
    // Hinged steel dock leveler deck plate (slightly sloped toward trailer lip when active)
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - levW * 0.5f, floorY + 0.02f, lzMin},
        Vec3{bayCenterX + levW * 0.5f, floorY + 0.07f, lzMax}, silverMat);
    // Hinged front lip bridge extending out toward trailer threshold
    float lipZ0 = dockWallZ - s * 0.38f;
    float lipZ1 = dockWallZ + s * 0.08f;
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - levW * 0.48f, floorY + 0.02f, std::min(lipZ0, lipZ1)},
        Vec3{bayCenterX + levW * 0.48f, floorY + 0.065f, std::max(lipZ0, lipZ1)}, yellowMat);
    // Yellow side safety toe-guards along leveler deck
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - levW * 0.5f - 0.05f, floorY + 0.02f, lzMin},
        Vec3{bayCenterX - levW * 0.5f + 0.03f, floorY + 0.14f, lzMax}, yellowMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX + levW * 0.5f - 0.03f, floorY + 0.02f, lzMin},
        Vec3{bayCenterX + levW * 0.5f + 0.05f, floorY + 0.14f, lzMax}, yellowMat);

    // 5. Interior Dock Lock Status Signal Light & Control Box beside doorway
    float ctrlX = bayCenterX + hwDoor + 0.32f;
    float ctrlZ = dockWallZ + s * 0.25f;
    BuildingComponents::appendBox(verts, indices,
        Vec3{ctrlX - 0.12f, floorY + 1.25f, ctrlZ - 0.10f},
        Vec3{ctrlX + 0.12f, floorY + 1.85f, ctrlZ + 0.10f}, frameMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{ctrlX - 0.07f, floorY + 1.58f, ctrlZ - 0.12f},
        Vec3{ctrlX + 0.07f, floorY + 1.74f, ctrlZ + 0.12f}, isOpenBay ? greenMat : redMat);

    // 6. Safety Yellow Staging Bay Lane Striping inside warehouse
    float laneHalfW = std::min(bayW * 0.44f, doorW * 0.58f);
    float laneLen = 4.6f;
    float laneZ0 = dockWallZ + s * 0.2f;
    float laneZ1 = dockWallZ + s * (0.2f + laneLen);
    float lanMinZ = std::min(laneZ0, laneZ1);
    float lanMaxZ = std::max(laneZ0, laneZ1);
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX - laneHalfW - 0.05f, floorY + 0.005f, lanMinZ},
        Vec3{bayCenterX - laneHalfW + 0.05f, floorY + 0.025f, lanMaxZ}, yellowMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{bayCenterX + laneHalfW - 0.05f, floorY + 0.005f, lanMinZ},
        Vec3{bayCenterX + laneHalfW + 0.05f, floorY + 0.025f, lanMaxZ}, yellowMat);

    // 7. Optional Docked 53ft Semi-Trailer Backed Up Outside Open Dock Door
    if (hasSemiTrailer) {
        float trLen = 7.2f;
        float trW = 2.50f;
        float trFloorY = floorY;
        float trRoofY = floorY + 2.85f;
        float trZ0 = dockWallZ - s * 0.25f;
        float trZ1 = dockWallZ - s * (0.25f + trLen);
        float tMinZ = std::min(trZ0, trZ1);
        float tMaxZ = std::max(trZ0, trZ1);

        // Trailer floor deck
        BuildingComponents::appendBox(verts, indices,
            Vec3{bayCenterX - trW * 0.5f, trFloorY - 0.18f, tMinZ},
            Vec3{bayCenterX + trW * 0.5f, trFloorY, tMaxZ}, frameMat);
        // Side walls & roof of trailer (open rear facing dock)
        BuildingComponents::appendBox(verts, indices,
            Vec3{bayCenterX - trW * 0.5f, trFloorY, tMinZ},
            Vec3{bayCenterX - trW * 0.5f + 0.10f, trRoofY, tMaxZ}, trailerMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{bayCenterX + trW * 0.5f - 0.10f, trFloorY, tMinZ},
            Vec3{bayCenterX + trW * 0.5f, trRoofY, tMaxZ}, trailerMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{bayCenterX - trW * 0.5f, trRoofY - 0.10f, tMinZ},
            Vec3{bayCenterX + trW * 0.5f, trRoofY, tMaxZ}, trailerMat);
        // Tandem trailer rear wheels
        for (float wzOff : {1.4f, 2.7f}) {
            float wz = dockWallZ - s * wzOff;
            appendTireWheelSimple(verts, indices, Vec3{bayCenterX - trW * 0.44f, trFloorY - 0.58f, wz}, Vec3{1, 0, 0}, 0.48f, 0.30f);
            appendTireWheelSimple(verts, indices, Vec3{bayCenterX + trW * 0.44f, trFloorY - 0.58f, wz}, Vec3{1, 0, 0}, 0.48f, 0.30f);
        }
        // Freight pallet inside the trailer visible through the open dock door
        appendPalletWithCargo(verts, indices, bayCenterX, trFloorY, dockWallZ - s * 1.4f, 1.1f, 1.1f, 1.1f,
            static_cast<uint32_t>(MaterialSlot::CardboardTan));
    }
}

static void appendPalletRackRow(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float startX, float floorY, float centerZ,
    float bayW, float rackD, float bayH,
    int32_t numBays, int32_t tiers
) {
    uint32_t postMat = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t beamMat = static_cast<uint32_t>(MaterialSlot::VehicleBlue);
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    float hd = rackD * 0.5f;

    // End guard bollard protectors
    for (float bx : {startX - 0.35f, startX + numBays * bayW + 0.35f}) {
        for (float bz : {centerZ - hd, centerZ + hd}) {
            BuildingComponents::appendOrientedCylinder(verts, indices,
                Vec3{bx, floorY, bz}, Vec3{bx, floorY + 1.05f, bz}, 0.09f, 0.09f, 8, yellowMat);
        }
    }

    // Uprights
    for (int b = 0; b <= numBays; ++b) {
        float px = startX + b * bayW;
        BuildingComponents::appendBox(verts, indices,
            Vec3{px - 0.06f, floorY, centerZ - hd},
            Vec3{px + 0.06f, floorY + bayH, centerZ - hd + 0.10f}, postMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{px - 0.06f, floorY, centerZ + hd - 0.10f},
            Vec3{px + 0.06f, floorY + bayH, centerZ + hd}, postMat);
    }

    // Shelf beams + pallets across tiers
    float tierSpacing = (bayH - 0.6f) / static_cast<float>(tiers);
    for (int t = 0; t < tiers; ++t) {
        float beamY = floorY + (t == 0 ? 0.02f : t * tierSpacing);
        if (t > 0) {
            BuildingComponents::appendBox(verts, indices,
                Vec3{startX, beamY - 0.10f, centerZ - hd},
                Vec3{startX + numBays * bayW, beamY, centerZ - hd + 0.08f}, beamMat);
            BuildingComponents::appendBox(verts, indices,
                Vec3{startX, beamY - 0.10f, centerZ + hd - 0.08f},
                Vec3{startX + numBays * bayW, beamY, centerZ + hd}, beamMat);
        }
        for (int b = 0; b < numBays; ++b) {
            float bx = startX + (b + 0.5f) * bayW;
            uint32_t cargoM = ((b + t) % 3 == 0) ? static_cast<uint32_t>(MaterialSlot::CardboardTan) :
                              ((b + t) % 3 == 1) ? static_cast<uint32_t>(MaterialSlot::VehicleBlue) :
                                                   static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
            appendPalletWithCargo(verts, indices, bx - bayW * 0.24f, beamY, centerZ, 1.05f, rackD * 0.78f, std::min(1.1f, tierSpacing * 0.65f), cargoM);
            if ((b + t) % 2 == 0) {
                appendPalletWithCargo(verts, indices, bx + bayW * 0.24f, beamY, centerZ, 1.05f, rackD * 0.78f, std::min(0.95f, tierSpacing * 0.58f), static_cast<uint32_t>(MaterialSlot::CardboardTan));
            }
        }
    }
}

} // namespace

void RoomGen::appendWarehouseInteriorContent(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float originX, float floorY, float originZ,
    float width, float length, float height,
    int32_t designIndex, int32_t loadingBays
) {
    float w = std::max(14.0f, width);
    float l = std::max(12.0f, length);
    float h = std::max(4.8f, height);
    float hw = w * 0.5f;
    float hl = l * 0.5f;
    int32_t design = ((designIndex % 4) + 4) % 4;
    int32_t bays = std::clamp(loadingBays, 2, 6);

    uint32_t epoxyMat = static_cast<uint32_t>(MaterialSlot::ShopFloorEpoxy);
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t frostedMat = static_cast<uint32_t>(MaterialSlot::GlassFrosted);
    uint32_t silverMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t cargoMat = static_cast<uint32_t>(MaterialSlot::CardboardTan);
    uint32_t blueCargo = static_cast<uint32_t>(MaterialSlot::VehicleBlue);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallLimestone);

    // Clean industrial concrete/epoxy warehouse floor slab overlay
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{originX - hw + 0.15f, floorY - 0.05f, originZ - hl + 0.15f},
        Vec3{originX + hw - 0.15f, floorY, originZ + hl - 0.15f}, epoxyMat);

    // Safety yellow main central forklift aisle striping
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{originX - hw + 1.2f, floorY + 0.005f, originZ - hl + 5.3f},
        Vec3{originX + hw - 1.2f, floorY + 0.02f, originZ - hl + 5.42f}, yellowMat);

    // Dispatch Supervisor Office Booth (Left-Front corner, low glazing)
    float boothW = std::min(5.2f, w * 0.22f);
    float boothD = std::min(3.8f, l * 0.22f);
    float bx0 = originX - hw + 0.5f;
    float bx1 = bx0 + boothW;
    float bz1 = originZ + hl - 0.5f;
    float bz0 = bz1 - boothD;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{bx0, floorY, bz0}, Vec3{bx1, floorY + 1.05f, bz1}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{bx0 + 0.12f, floorY + 1.05f, bz0 + 0.12f}, Vec3{bx1 - 0.12f, floorY + 2.55f, bz1 - 0.12f}, glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{bx0, floorY + 2.55f, bz0}, Vec3{bx1, floorY + 2.75f, bz1}, frameMat);

    // Primary Rear (-Z) Bank of Loading Docks (facing camera in default view!)
    float dockStartX = originX - hw + 2.0f;
    float dockEndX = originX + hw - 2.0f;
    float dockSpan = dockEndX - dockStartX;
    float bayW = dockSpan / static_cast<float>(bays);
    float doorW = std::clamp(bayW - 1.1f, 2.4f, 3.4f);
    float doorH = std::min(3.8f, h - 1.1f);
    float rearWallZ = originZ - hl + 0.22f;

    for (int b = 0; b < bays; ++b) {
        float cx = dockStartX + (b + 0.5f) * bayW;
        bool openBay = (b % 2 == 0) || (design == 0);
        bool trailerDocked = (design == 0 && b == 1) || (design == 1 && b == 2);
        appendLoadingDockBay(outVerts, outIndices, cx, floorY, rearWallZ, bayW, doorW, doorH, +1.0f, openBay, trailerDocked);
    }

    if (design == 0) {
        // DESIGN 0: Cross-Dock Loading Bays (Multi-lane freight staging + secondary dock wall + forklifts)
        // Secondary dock leveler bays along -X left wall
        int sideDocks = std::clamp(static_cast<int>((l - 7.0f) / 4.8f), 1, 3);
        for (int sb = 0; sb < sideDocks; ++sb) {
            float sz = originZ - hl + 6.8f + sb * 4.6f;
            if (sz + 1.5f > bz0 - 0.5f) break;
            float sx = originX - hw + 0.22f;
            // Dock leveler plate extending +X from left wall
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx, floorY + 0.02f, sz - 1.1f},
                Vec3{sx + 2.2f, floorY + 0.07f, sz + 1.1f}, silverMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx, floorY + 0.02f, sz - 1.15f},
                Vec3{sx + 2.2f, floorY + 0.13f, sz - 1.07f}, yellowMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx, floorY + 0.02f, sz + 1.07f},
                Vec3{sx + 2.2f, floorY + 0.13f, sz + 1.15f}, yellowMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx - 0.08f, floorY + doorH * 0.7f, sz - 1.45f},
                Vec3{sx + 0.08f, floorY + doorH, sz + 1.45f}, silverMat);
        }

        // Cross-dock staging pallet islands in front of each loading dock bay
        for (int b = 0; b < bays; ++b) {
            float cx = dockStartX + (b + 0.5f) * bayW;
            appendPalletWithCargo(outVerts, outIndices, cx - 0.7f, floorY, originZ + 0.2f, 1.1f, 1.1f, 1.25f, cargoMat);
            appendPalletWithCargo(outVerts, outIndices, cx + 0.7f, floorY, originZ + 0.2f, 1.1f, 1.1f, 0.95f, blueCargo);
            if (b % 2 == 0) {
                appendPalletWithCargo(outVerts, outIndices, cx, floorY, originZ + 2.2f, 1.15f, 1.15f, 1.40f,
                    static_cast<uint32_t>(MaterialSlot::CommercialAccentRed));
            }
        }
        // Active forklifts working the cross-dock floor
        appendForklift(outVerts, outIndices, originX - hw * 0.22f, floorY, originZ - hl + 6.4f, -1.0f);
        appendForklift(outVerts, outIndices, originX + hw * 0.28f, floorY, originZ + 3.2f, 1.0f);
    } else if (design == 1) {
        // DESIGN 1: Dual-Bank Conveyor & Cross-Dock Sorting Spine
        float convY = floorY + 0.92f;
        float convZ = originZ + 0.8f;
        float convX0 = originX - hw + 3.2f;
        float convX1 = originX + hw - 2.4f;
        // Main powered roller conveyor belt spine
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{convX0, convY - 0.12f, convZ - 0.65f},
            Vec3{convX1, convY, convZ + 0.65f}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{convX0 + 0.1f, convY, convZ - 0.55f},
            Vec3{convX1 - 0.1f, convY + 0.04f, convZ + 0.55f}, silverMat);
        // Support legs & diverter spurs toward loading docks
        for (float lx = convX0 + 0.6f; lx <= convX1 - 0.4f; lx += 3.2f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - 0.07f, floorY, convZ - 0.55f},
                Vec3{lx + 0.07f, convY - 0.10f, convZ - 0.43f}, darkMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - 0.07f, floorY, convZ + 0.43f},
                Vec3{lx + 0.07f, convY - 0.10f, convZ + 0.55f}, darkMat);
        }
        // Overhead barcode scanner gantry arches on conveyor
        for (float gx : {originX - hw * 0.25f, originX + hw * 0.25f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{gx - 0.18f, convY, convZ - 0.72f},
                Vec3{gx + 0.18f, convY + 1.45f, convZ - 0.58f}, yellowMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{gx - 0.18f, convY, convZ + 0.58f},
                Vec3{gx + 0.18f, convY + 1.45f, convZ + 0.72f}, yellowMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{gx - 0.22f, convY + 1.35f, convZ - 0.74f},
                Vec3{gx + 0.22f, convY + 1.68f, convZ + 0.74f}, darkMat);
        }
        // Parcel boxes riding along the conveyor belt
        for (float px = convX0 + 1.2f; px < convX1 - 1.2f; px += 2.3f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.40f, convY + 0.04f, convZ - 0.35f},
                Vec3{px + 0.40f, convY + 0.64f, convZ + 0.35f}, cargoMat);
        }
        // Diverter spurs feeding dock staging
        for (int b = 0; b < bays; ++b) {
            float sx = dockStartX + (b + 0.5f) * bayW;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx - 0.45f, convY - 0.10f, convZ - 3.6f},
                Vec3{sx + 0.45f, convY, convZ - 0.65f}, frameMat);
            appendPalletWithCargo(outVerts, outIndices, sx, floorY, originZ + hl - 2.6f, 1.1f, 1.1f, 1.15f, blueCargo);
        }
        appendForklift(outVerts, outIndices, originX + hw * 0.32f, floorY, originZ + 4.2f, -1.0f);
    } else if (design == 2) {
        // DESIGN 2: High-Bay Multi-Tier Pallet Racking & Dock Staging
        float rackStartX = originX - hw + 5.8f;
        float rackSpanX = std::max(8.0f, w - 8.0f);
        int rackBays = std::clamp(static_cast<int>(rackSpanX / 3.4f), 2, 6);
        float rackBayW = rackSpanX / static_cast<float>(rackBays);
        float rackH = std::min(h - 0.8f, 5.4f);
        int rackTiers = (rackH >= 5.0f) ? 4 : 3;

        float rackZ1 = originZ - 1.2f;
        float rackZ2 = originZ + 3.2f;
        if (rackZ1 > originZ - hl + 5.2f) {
            appendPalletRackRow(outVerts, outIndices, rackStartX, floorY, rackZ1, rackBayW, 1.5f, rackH, rackBays, rackTiers);
        }
        if (rackZ2 + 1.0f < originZ + hl - 1.2f) {
            appendPalletRackRow(outVerts, outIndices, rackStartX, floorY, rackZ2, rackBayW, 1.5f, rackH, rackBays, rackTiers);
        }
        appendForklift(outVerts, outIndices, originX - hw + 2.8f, floorY, originZ + 1.0f, 1.0f);
        appendPalletWithCargo(outVerts, outIndices, originX + hw - 3.2f, floorY, originZ - hl + 4.6f, 1.15f, 1.15f, 1.15f, cargoMat);
    } else {
        // DESIGN 3: Cold-Chain Dock Airlocks & Elevated Steel Mezzanine Deck
        // Translucent frosted cold-storage strip curtains across dock openings
        for (int b = 0; b < bays; ++b) {
            float cx = dockStartX + (b + 0.5f) * bayW;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - doorW * 0.46f, floorY + 0.15f, rearWallZ + 0.16f},
                Vec3{cx + doorW * 0.46f, floorY + doorH - 0.1f, rearWallZ + 0.22f}, frostedMat);
        }
        // Elevated Steel Mezzanine Platform along right-center span
        float mezY = floorY + std::min(3.3f, h * 0.48f);
        float mx0 = originX - hw * 0.15f;
        float mx1 = originX + hw - 1.2f;
        float mz0 = originZ - 1.8f;
        float mz1 = originZ + hl - 1.2f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx0, mezY - 0.18f, mz0}, Vec3{mx1, mezY, mz1}, frameMat);
        // Mezzanine support columns
        for (float px : {mx0 + 0.2f, (mx0 + mx1) * 0.5f, mx1 - 0.2f}) {
            for (float pz : {mz0 + 0.2f, mz1 - 0.2f}) {
                BuildingComponents::appendBox(outVerts, outIndices,
                    Vec3{px - 0.14f, floorY, pz - 0.14f}, Vec3{px + 0.14f, mezY - 0.18f, pz + 0.14f}, frameMat);
            }
        }
        // Safety yellow mezzanine perimeter guardrails
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{mx0, mezY + 0.95f, mz0}, Vec3{mx1, mezY + 1.03f, mz0 + 0.08f}, yellowMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{mx0, mezY + 0.95f, mz0}, Vec3{mx0 + 0.08f, mezY + 1.03f, mz1}, yellowMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{mx0, mezY + 0.50f, mz0}, Vec3{mx1, mezY + 0.56f, mz0 + 0.06f}, yellowMat);
        // Steel stair flight leading up to mezzanine
        int steps = 10;
        for (int st = 0; st < steps; ++st) {
            float t0 = static_cast<float>(st) / steps;
            float t1 = static_cast<float>(st + 1) / steps;
            float sx0 = mx0 - 3.2f + t0 * 3.2f;
            float sx1 = mx0 - 3.2f + t1 * 3.2f;
            float sy = floorY + t1 * mezY;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx0, sy - 0.12f, mz0 + 0.2f}, Vec3{sx1, sy, mz0 + 1.3f}, silverMat);
        }
        // Packing benches & pallets on mezzanine + under mezzanine
        appendPalletWithCargo(outVerts, outIndices, mx0 + 2.2f, mezY, (mz0 + mz1) * 0.5f, 1.1f, 1.1f, 1.0f, blueCargo);
        appendPalletWithCargo(outVerts, outIndices, mx0 + 4.8f, mezY, (mz0 + mz1) * 0.5f, 1.1f, 1.1f, 1.1f, cargoMat);
        appendPalletRackRow(outVerts, outIndices, originX - hw + 6.0f, floorY, originZ - 0.4f, 3.0f, 1.4f, std::min(3.0f, mezY - 0.2f), 2, 2);
        appendForklift(outVerts, outIndices, mx0 - 3.6f, floorY, originZ - 2.6f, -1.0f);
    }
}

void RoomGen::generateWarehouseInterior(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(14.0f, params_.width);
    float l = std::max(12.0f, params_.length);
    float h = std::max(4.8f, params_.ceilingHeight);
    float hw = w * 0.5f;
    float hl = l * 0.5f;

    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    // 1. Exterior Truck Apron Yard behind -Z Loading Docks (-0.95m lower than dock deck!)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -1.25f, -hl - 7.5f},
        Vec3{ hw + 2.0f, -0.95f,  hl + 1.5f}, tarmacMat);

    // Raised concrete warehouse floor foundation plinth (1.0m dock height above truck yard!)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.2f, -0.95f, -hl - 0.2f},
        Vec3{ hw + 0.2f,  0.00f,  hl + 0.2f}, foundMat);

    // 2. Perimeter Enclosure (Cutaway when showInside == true; Full walls + roof when showInside == false)
    int32_t bays = std::clamp(params_.loadingBays, 2, 6);
    float dockStartX = -hw + 2.0f;
    float dockEndX = hw - 2.0f;
    float bayW = (dockEndX - dockStartX) / static_cast<float>(bays);
    float doorW = std::clamp(bayW - 1.1f, 2.4f, 3.4f);
    float doorH = std::min(3.8f, h - 1.1f);

    // Rear (-Z) Dock Wall pierced with open loading dock bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hl}, Vec3{dockStartX, h, -hl + 0.25f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{dockEndX, 0.0f, -hl}, Vec3{hw, h, -hl + 0.25f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{dockStartX, doorH, -hl}, Vec3{dockEndX, h, -hl + 0.25f}, wallMat);
    for (int b = 0; b <= bays; ++b) {
        float px = dockStartX + b * bayW;
        float x0 = std::max(dockStartX, px - (bayW - doorW) * 0.5f);
        float x1 = std::min(dockEndX, px + (bayW - doorW) * 0.5f);
        if (x1 > x0 + 0.02f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{x0, 0.0f, -hl}, Vec3{x1, doorH, -hl + 0.25f}, wallMat);
        }
    }
    // Clerestory daylight ribbon above rear dock bays
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.85f, std::max(doorH + 0.35f, h - 1.3f), -hl - 0.04f},
        Vec3{ hw * 0.85f, h - 0.45f, -hl + 0.28f}, glassMat);

    // Left (-X) Wall with high clerestory windows
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hl}, Vec3{-hw + 0.25f, h, hl}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.04f, h - 1.4f, -hl * 0.80f},
        Vec3{-hw + 0.28f, h - 0.45f,  hl * 0.80f}, glassMat);

    if (params_.showInside) {
        // Low cutaway front (+Z) & right (+X) walls + sleek steel perimeter columns for unobstructed interior view
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, hl - 0.25f}, Vec3{hw, 0.42f, hl}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.25f, 0.0f, -hl}, Vec3{hw, 0.42f, hl}, wallMat);
        for (float px : {hw - 0.22f}) {
            for (float pz : {-hl + 0.2f, 0.0f, hl - 0.2f}) {
                BuildingComponents::appendBox(outVerts, outIndices, Vec3{px - 0.12f, 0.0f, pz - 0.12f}, Vec3{px + 0.12f, h, pz + 0.12f}, frameMat);
            }
        }
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, h - 0.18f, hl - 0.25f}, Vec3{hw, h, hl}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.25f, h - 0.18f, -hl}, Vec3{hw, h, hl}, frameMat);
    } else {
        // Enclosed front & right walls + pitched roof deck
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.0f, hl - 0.25f}, Vec3{hw, h, hl}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.25f, 0.0f, -hl}, Vec3{hw, h, hl}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.3f, h, -hl - 0.3f}, Vec3{hw + 0.3f, h + 0.35f, hl + 0.3f}, roofMat);
    }

    // 3. Populate Warehouse Interior Fitout (Loading Docks, Levelers, Pallets, Racks, Forklifts)
    appendWarehouseInteriorContent(outVerts, outIndices, 0.0f, 0.0f, 0.0f, w, l, h, params_.interiorDesign, bays);

    // 4. Industrial High-Bay 3D Luminaires along perimeter wall & columns
    float highMountY = std::min(h - 0.65f, 4.2f);
    BuildingComponents::generateWallSconceLamp(outVerts, outIndices, Vec3{-hw + 0.25f, highMountY, -hl * 0.45f}, Vec3{1.0f, 0.0f, 0.0f}, true);
    BuildingComponents::generateWallSconceLamp(outVerts, outIndices, Vec3{-hw + 0.25f, highMountY,  hl * 0.45f}, Vec3{1.0f, 0.0f, 0.0f}, true);
    if (!params_.showInside) {
        BuildingComponents::generateCeilingPendantBulb(outVerts, outIndices, Vec3{0.0f, h, -hl * 0.35f}, 0.75f, 0.20f, true);
        BuildingComponents::generateCeilingPendantBulb(outVerts, outIndices, Vec3{0.0f, h,  hl * 0.35f}, 0.75f, 0.20f, true);
    }
}

} // namespace procedural::buildings
