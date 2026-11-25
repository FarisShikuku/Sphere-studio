#include "RoomGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

static void appendTireAssembly(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    const Vec3& center,
    const Vec3& axis,
    float outerRadius,
    float width
) {
    uint32_t rubberMat = static_cast<uint32_t>(MaterialSlot::TireRubber);
    uint32_t rimMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t hubMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);

    Vec3 nAxis = axis.normalized();
    Vec3 half = nAxis * (width * 0.5f);
    // Main black rubber tread barrel
    BuildingComponents::appendOrientedCylinder(verts, indices, center - half, center + half, outerRadius, outerRadius, 16, rubberMat);
    // Alloy wheel rim slightly protruding from both sidewalls
    Vec3 rimHalf = nAxis * (width * 0.53f);
    BuildingComponents::appendOrientedCylinder(verts, indices, center - rimHalf, center + rimHalf, outerRadius * 0.58f, outerRadius * 0.58f, 14, rimMat);
    // Center lug hub cap
    Vec3 hubHalf = nAxis * (width * 0.56f);
    BuildingComponents::appendOrientedCylinder(verts, indices, center - hubHalf, center + hubHalf, outerRadius * 0.22f, outerRadius * 0.22f, 10, hubMat);
}

static void appendTireStorageRack(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float floorY, float cz,
    float rackLenX, int32_t tiers, float tireRadius
) {
    uint32_t steelMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t beamMat = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    float hx = rackLenX * 0.5f;
    float rackDepth = tireRadius * 1.6f;
    float hd = rackDepth * 0.5f;
    float tierH = tireRadius * 2.25f;
    float totalH = 0.25f + tiers * tierH;

    // 4 upright corner posts (plus center post for wide racks)
    int numPosts = (rackLenX > 4.0f) ? 3 : 2;
    for (int p = 0; p < numPosts; ++p) {
        float px = cx - hx + p * (rackLenX / (numPosts - 1));
        BuildingComponents::appendBox(verts, indices,
            Vec3{px - 0.05f, floorY, cz - hd},
            Vec3{px + 0.05f, floorY + totalH, cz - hd + 0.08f}, steelMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{px - 0.05f, floorY, cz + hd - 0.08f},
            Vec3{px + 0.05f, floorY + totalH, cz + hd}, steelMat);
    }

    // Horizontal cradle rails + standing row of 3D tires per tier
    float tireThick = 0.26f;
    for (int t = 0; t < tiers; ++t) {
        float barY = floorY + 0.28f + t * tierH;
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - hx, barY, cz - hd + 0.14f},
            Vec3{cx + hx, barY + 0.07f, cz - hd + 0.22f}, beamMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - hx, barY, cz + hd - 0.22f},
            Vec3{cx + hx, barY + 0.07f, cz + hd - 0.14f}, beamMat);

        float tireCenterY = barY + tireRadius * 0.88f;
        for (float tx = cx - hx + 0.35f; tx <= cx + hx - 0.35f; tx += (tireThick + 0.09f)) {
            appendTireAssembly(verts, indices, Vec3{tx, tireCenterY, cz}, Vec3{1, 0, 0}, tireRadius, tireThick);
        }
    }
}

static void appendStackedTirePile(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float floorY, float cz,
    int32_t count, float outerRadius, float tireWidth
) {
    for (int i = 0; i < count; ++i) {
        float oy = floorY + (i + 0.5f) * tireWidth;
        float ox = (i % 2 == 0) ? 0.02f : -0.02f;
        appendTireAssembly(verts, indices, Vec3{cx + ox, oy, cz}, Vec3{0, 1, 0}, outerRadius, tireWidth * 0.96f);
    }
}

static void appendTireServiceStation(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float floorY, float cz
) {
    uint32_t machineMat = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t darkMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t steelMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t screenMat = static_cast<uint32_t>(MaterialSlot::Glass);

    // 1. Tire Changer Machine with turntable & bead-breaker column
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 1.4f, floorY, cz - 0.55f},
        Vec3{cx - 0.3f, floorY + 0.92f, cz + 0.55f}, machineMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 1.3f, floorY + 0.92f, cz - 0.45f},
        Vec3{cx - 1.1f, floorY + 1.95f, cz - 0.25f}, darkMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 1.25f, floorY + 1.78f, cz - 0.35f},
        Vec3{cx - 0.72f, floorY + 1.90f, cz + 0.15f}, steelMat);
    // Horizontal tire clamped onto changer turntable
    appendTireAssembly(verts, indices, Vec3{cx - 0.80f, floorY + 1.06f, cz + 0.05f}, Vec3{0, 1, 0}, 0.36f, 0.24f);

    // 2. Digital Wheel Balancer with hood & mounted vertical spinning tire
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx + 0.4f, floorY, cz - 0.50f},
        Vec3{cx + 1.4f, floorY + 1.02f, cz + 0.50f}, machineMat);
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx + 0.9f, floorY + 1.02f, cz - 0.38f},
        Vec3{cx + 1.35f, floorY + 1.65f, cz + 0.25f}, screenMat);
    appendTireAssembly(verts, indices, Vec3{cx + 0.25f, floorY + 0.78f, cz + 0.10f}, Vec3{1, 0, 0}, 0.36f, 0.24f);
}

static void appendHydraulic2PostLift(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float floorY, float cz,
    float liftY, float postSpreadX, float postHeight
) {
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t blackMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t steelMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);

    for (int s : {-1, 1}) {
        float px = cx + s * postSpreadX;
        // Heavy floor anchor baseplate
        BuildingComponents::appendBox(verts, indices,
            Vec3{px - 0.32f, floorY, cz - 0.35f},
            Vec3{px + 0.32f, floorY + 0.06f, cz + 0.35f}, blackMat);
        // Tall hydraulic lift column
        BuildingComponents::appendBox(verts, indices,
            Vec3{px - 0.16f, floorY + 0.06f, cz - 0.20f},
            Vec3{px + 0.16f, floorY + postHeight, cz + 0.20f}, yellowMat);
        // Hydraulic cylinder chrome ram
        BuildingComponents::appendOrientedCylinder(verts, indices,
            Vec3{px - s * 0.18f, floorY + 0.10f, cz},
            Vec3{px - s * 0.18f, floorY + liftY + 0.55f, cz},
            0.055f, 0.055f, 8, steelMat);
        // Lifting carriage sleeve
        BuildingComponents::appendBox(verts, indices,
            Vec3{px - 0.22f, floorY + liftY - 0.25f, cz - 0.25f},
            Vec3{px + 0.22f, floorY + liftY + 0.35f, cz + 0.25f}, blackMat);
        // Front & rear articulated swing arms with lift pads
        for (float az : {-0.95f, 0.95f}) {
            float armInX = cx + s * 0.65f;
            BuildingComponents::appendBox(verts, indices,
                Vec3{std::min(armInX, px), floorY + liftY - 0.07f, cz + az * 0.35f - 0.06f},
                Vec3{std::max(armInX, px), floorY + liftY + 0.02f, cz + az * 0.35f + 0.06f}, blackMat);
            BuildingComponents::appendBox(verts, indices,
                Vec3{armInX - 0.10f, floorY + liftY + 0.02f, cz + az - 0.10f},
                Vec3{armInX + 0.10f, floorY + liftY + 0.10f, cz + az + 0.10f}, yellowMat);
        }
    }
    // Overhead hydraulic equalizer crossbeam
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - postSpreadX - 0.18f, floorY + postHeight - 0.22f, cz - 0.14f},
        Vec3{cx + postSpreadX + 0.18f, floorY + postHeight, cz + 0.14f}, yellowMat);
}

static void appendHydraulic4PostLift(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float floorY, float cz,
    float liftY, float spanW, float runwayLen, float postHeight
) {
    uint32_t blueMat = static_cast<uint32_t>(MaterialSlot::VehicleBlue);
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t steelMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t redMat = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);

    float hw = spanW * 0.5f;
    float hl = runwayLen * 0.5f;

    // 4 heavy corner lift columns
    for (float px : {cx - hw, cx + hw}) {
        for (float pz : {cz - hl, cz + hl}) {
            BuildingComponents::appendBox(verts, indices,
                Vec3{px - 0.22f, floorY, pz - 0.22f},
                Vec3{px + 0.22f, floorY + 0.06f, pz + 0.22f}, steelMat);
            BuildingComponents::appendBox(verts, indices,
                Vec3{px - 0.15f, floorY + 0.06f, pz - 0.15f},
                Vec3{px + 0.15f, floorY + postHeight, pz + 0.15f}, blueMat);
        }
    }

    // Front & rear lift crossbeams
    for (float pz : {cz - hl, cz + hl}) {
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - hw - 0.16f, floorY + liftY - 0.20f, pz - 0.16f},
            Vec3{cx + hw + 0.16f, floorY + liftY, pz + 0.16f}, yellowMat);
    }

    // Two wide steel drive-on runways
    float trackOffset = std::min(hw - 0.65f, 1.05f);
    float runwayW = 0.72f;
    for (float rx : {cx - trackOffset, cx + trackOffset}) {
        BuildingComponents::appendBox(verts, indices,
            Vec3{rx - runwayW * 0.5f, floorY + liftY, cz - hl - 0.2f},
            Vec3{rx + runwayW * 0.5f, floorY + liftY + 0.14f, cz + hl + 0.2f}, yellowMat);
        // Approach ramp drop-down lip at +Z entry
        BuildingComponents::appendBox(verts, indices,
            Vec3{rx - runwayW * 0.5f, floorY + liftY - 0.24f, cz + hl + 0.2f},
            Vec3{rx + runwayW * 0.5f, floorY + liftY + 0.08f, cz + hl + 0.95f}, steelMat);
    }

    // Center rolling hydraulic bridge jack
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - trackOffset, floorY + liftY - 0.05f, cz - 0.35f},
        Vec3{cx + trackOffset, floorY + liftY + 0.12f, cz + 0.35f}, redMat);
}

static void appendScissorLift(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float floorY, float cz,
    float liftY, float width, float length
) {
    uint32_t redMat = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    float trackX = width * 0.36f;
    for (float sx : {cx - trackX, cx + trackX}) {
        // Base frame rail
        BuildingComponents::appendBox(verts, indices,
            Vec3{sx - 0.30f, floorY, cz - length * 0.45f},
            Vec3{sx + 0.30f, floorY + 0.08f, cz + length * 0.45f}, redMat);
        // Crossed hydraulic scissor legs
        BuildingComponents::appendOrientedCylinder(verts, indices,
            Vec3{sx, floorY + 0.06f, cz - length * 0.32f},
            Vec3{sx, floorY + liftY, cz + length * 0.32f}, 0.07f, 0.07f, 6, redMat);
        BuildingComponents::appendOrientedCylinder(verts, indices,
            Vec3{sx, floorY + 0.06f, cz + length * 0.32f},
            Vec3{sx, floorY + liftY, cz - length * 0.32f}, 0.07f, 0.07f, 6, redMat);
        // Top support platform pad
        BuildingComponents::appendBox(verts, indices,
            Vec3{sx - 0.34f, floorY + liftY, cz - length * 0.5f},
            Vec3{sx + 0.34f, floorY + liftY + 0.10f, cz + length * 0.5f}, yellowMat);
    }
}

enum class VehicleClass {
    Sedan = 0,
    SuvPickup = 1,
    BoxTruck = 2,
    TransitBus = 3
};

static void appendDetailedVehicle(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float tireBottomY, float cz,
    VehicleClass vClass,
    uint32_t bodyMat
) {
    uint32_t darkMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t silverMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t cargoMat = static_cast<uint32_t>(MaterialSlot::VehicleWhite);
    uint32_t redMat = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);

    if (vClass == VehicleClass::Sedan) {
        float w = 1.84f, l = 4.50f, wheelR = 0.34f;
        float chassisY = tireBottomY + wheelR * 0.72f;
        // Lower body
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.5f, chassisY, cz - l * 0.5f},
            Vec3{cx + w * 0.5f, chassisY + 0.62f, cz + l * 0.5f}, bodyMat);
        // Cabin greenhouse
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.44f, chassisY + 0.62f, cz - l * 0.24f},
            Vec3{cx + w * 0.44f, chassisY + 1.18f, cz + l * 0.20f}, glassMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.45f, chassisY + 1.14f, cz - l * 0.23f},
            Vec3{cx + w * 0.45f, chassisY + 1.22f, cz + l * 0.19f}, bodyMat);
        // Bumpers
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.51f, chassisY, cz + l * 0.46f},
            Vec3{cx + w * 0.51f, chassisY + 0.24f, cz + l * 0.53f}, darkMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.51f, chassisY, cz - l * 0.53f},
            Vec3{cx + w * 0.51f, chassisY + 0.24f, cz - l * 0.46f}, darkMat);
        // 4 wheels
        for (float wz : {cz - 1.38f, cz + 1.38f}) {
            appendTireAssembly(verts, indices, Vec3{cx - w * 0.46f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.24f);
            appendTireAssembly(verts, indices, Vec3{cx + w * 0.46f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.24f);
        }
    } else if (vClass == VehicleClass::SuvPickup) {
        float w = 2.06f, l = 5.05f, wheelR = 0.42f;
        float chassisY = tireBottomY + wheelR * 0.85f;
        // Rugged chassis + bed/body
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.5f, chassisY, cz - l * 0.5f},
            Vec3{cx + w * 0.5f, chassisY + 0.82f, cz + l * 0.5f}, bodyMat);
        // Tall SUV greenhouse
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.46f, chassisY + 0.82f, cz - l * 0.42f},
            Vec3{cx + w * 0.46f, chassisY + 1.48f, cz + l * 0.18f}, glassMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.47f, chassisY + 1.44f, cz - l * 0.43f},
            Vec3{cx + w * 0.47f, chassisY + 1.54f, cz + l * 0.19f}, bodyMat);
        // Spare tire mounted on rear tailgate
        appendTireAssembly(verts, indices, Vec3{cx, chassisY + 0.72f, cz - l * 0.52f}, Vec3{0, 0, 1}, 0.38f, 0.24f);
        for (float wz : {cz - 1.55f, cz + 1.55f}) {
            appendTireAssembly(verts, indices, Vec3{cx - w * 0.46f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.28f);
            appendTireAssembly(verts, indices, Vec3{cx + w * 0.46f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.28f);
        }
    } else if (vClass == VehicleClass::BoxTruck) {
        float w = 2.40f, l = 7.40f, wheelR = 0.48f;
        float chassisY = tireBottomY + wheelR * 0.90f;
        // Steel frame rails
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - 0.55f, chassisY, cz - l * 0.48f},
            Vec3{cx + 0.55f, chassisY + 0.28f, cz + l * 0.48f}, darkMat);
        // Forward cab (+Z front)
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.45f, chassisY + 0.22f, cz + l * 0.18f},
            Vec3{cx + w * 0.45f, chassisY + 1.45f, cz + l * 0.49f}, bodyMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.42f, chassisY + 1.25f, cz + l * 0.20f},
            Vec3{cx + w * 0.42f, chassisY + 2.15f, cz + l * 0.45f}, glassMat);
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.44f, chassisY + 2.12f, cz + l * 0.19f},
            Vec3{cx + w * 0.44f, chassisY + 2.26f, cz + l * 0.46f}, bodyMat);
        // Tall spacious commercial cargo box
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.5f, chassisY + 0.28f, cz - l * 0.49f},
            Vec3{cx + w * 0.5f, chassisY + 2.68f, cz + l * 0.14f}, cargoMat);
        // Aluminum box trim & rear roll-up shutter
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.42f, chassisY + 0.42f, cz - l * 0.51f},
            Vec3{cx + w * 0.42f, chassisY + 2.52f, cz - l * 0.47f}, silverMat);
        // Heavy truck tires
        for (float wz : {cz - 2.1f, cz + 2.45f}) {
            appendTireAssembly(verts, indices, Vec3{cx - w * 0.44f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.32f);
            appendTireAssembly(verts, indices, Vec3{cx + w * 0.44f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.32f);
        }
    } else {
        // Heavy Commercial Transit Bus / Coach (~10.5m long!)
        float w = 2.52f, l = 10.40f, wheelR = 0.52f;
        float chassisY = tireBottomY + wheelR * 0.85f;
        // Lower coach skirt
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.5f, chassisY, cz - l * 0.5f},
            Vec3{cx + w * 0.5f, chassisY + 1.25f, cz + l * 0.5f}, bodyMat);
        // Panoramic tinted side & front passenger window band
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.49f, chassisY + 1.25f, cz - l * 0.48f},
            Vec3{cx + w * 0.49f, chassisY + 2.42f, cz + l * 0.49f}, glassMat);
        // Coach roof & HVAC climate pods
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.50f, chassisY + 2.42f, cz - l * 0.50f},
            Vec3{cx + w * 0.50f, chassisY + 2.68f, cz + l * 0.50f}, cargoMat);
        for (float hz : {cz - 1.8f, cz + 1.8f}) {
            BuildingComponents::appendBox(verts, indices,
                Vec3{cx - 0.85f, chassisY + 2.68f, hz - 0.9f},
                Vec3{cx + 0.85f, chassisY + 2.98f, hz + 0.9f}, silverMat);
        }
        // Accent stripe
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - w * 0.51f, chassisY + 0.95f, cz - l * 0.50f},
            Vec3{cx + w * 0.51f, chassisY + 1.15f, cz + l * 0.50f}, redMat);
        // Dual commercial coach tires
        for (float wz : {cz - 3.2f, cz - 1.8f, cz + 3.1f}) {
            appendTireAssembly(verts, indices, Vec3{cx - w * 0.46f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.34f);
            appendTireAssembly(verts, indices, Vec3{cx + w * 0.46f, tireBottomY + wheelR, wz}, Vec3{1, 0, 0}, wheelR, 0.34f);
        }
    }
}

static void appendToolChestAndCart(
    std::vector<BuildingVertex>& verts,
    std::vector<uint32_t>& indices,
    float cx, float floorY, float cz
) {
    uint32_t redMat = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t silverMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);

    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.75f, floorY + 0.12f, cz - 0.32f},
        Vec3{cx + 0.75f, floorY + 1.15f, cz + 0.32f}, redMat);
    // Silver drawer pulls
    for (int d = 0; d < 4; ++d) {
        float dy = floorY + 0.28f + d * 0.21f;
        BuildingComponents::appendBox(verts, indices,
            Vec3{cx - 0.65f, dy, cz + 0.31f},
            Vec3{cx + 0.65f, dy + 0.05f, cz + 0.35f}, silverMat);
    }
    BuildingComponents::appendBox(verts, indices,
        Vec3{cx - 0.75f, floorY + 1.15f, cz - 0.32f},
        Vec3{cx + 0.75f, floorY + 1.21f, cz + 0.32f}, darkMat);
}

} // namespace

void RoomGen::appendVehicleRepairInteriorContent(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float originX, float floorY, float originZ,
    float width, float length, float height,
    int32_t designIndex
) {
    float w = std::max(22.0f, width);
    float l = std::max(14.0f, length);
    float h = std::max(5.5f, height);
    float hw = w * 0.5f;
    float hl = l * 0.5f;
    int32_t design = ((designIndex % 4) + 4) % 4;

    uint32_t epoxyMat = static_cast<uint32_t>(MaterialSlot::ShopFloorEpoxy);
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t darkMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t silverMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t redCar = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t blueCar = static_cast<uint32_t>(MaterialSlot::VehicleBlue);
    uint32_t orangeCar = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t greenEv = static_cast<uint32_t>(MaterialSlot::HydraulicGreen);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallLimestone);

    // Epoxied high-visibility repair floor slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{originX - hw + 0.15f, floorY - 0.04f, originZ - hl + 0.15f},
        Vec3{originX + hw - 0.15f, floorY, originZ + hl - 0.15f}, epoxyMat);

    // Wide drive-through maneuvering corridor stripe along +Z side
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{originX - hw + 1.0f, floorY + 0.005f, originZ + hl * 0.35f},
        Vec3{originX + hw - 1.0f, floorY + 0.025f, originZ + hl * 0.35f + 0.12f}, yellowMat);

    // Vertical shop air compressor tank & pipe manifold in rear-left corner
    float compX = originX - hw + 1.3f;
    float compZ = originZ - hl + 1.3f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{compX, floorY, compZ}, Vec3{compX, floorY + 2.25f, compZ}, 0.42f, 0.42f, 12, blueCar);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{compX - 0.35f, floorY + 2.25f, compZ - 0.25f},
        Vec3{compX + 0.35f, floorY + 2.65f, compZ + 0.25f}, darkMat);

    // Spacious Multi-Bay Layout (4 generous bays spanning the workshop!)
    float bayZoneX0 = originX - hw + 2.2f;
    float bayZoneX1 = originX + hw - 2.2f;
    float baySpan = (bayZoneX1 - bayZoneX0) / 4.0f;
    float bayCenterZ = originZ - 0.8f;

    // Yellow floor bay divider lines
    for (int b = 0; b <= 4; ++b) {
        float lx = bayZoneX0 + b * baySpan;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{lx - 0.06f, floorY + 0.005f, originZ - hl + 2.0f},
            Vec3{lx + 0.06f, floorY + 0.025f, originZ + hl * 0.32f}, yellowMat);
    }

    if (design == 0) {
        // DESIGN 0: All-Vehicle Multi-Bay Hub (Sedan + SUV + Box Truck + Heavy Bus Pit!)
        float bx0 = bayZoneX0 + baySpan * 0.5f;
        float bx1 = bayZoneX0 + baySpan * 1.5f;
        float bx2 = bayZoneX0 + baySpan * 2.5f;
        float bx3 = bayZoneX0 + baySpan * 3.5f;

        // Bay 1: 2-Post Lifter raising Sedan
        appendHydraulic2PostLift(outVerts, outIndices, bx0, floorY, bayCenterZ, 1.45f, 1.55f, std::min(4.0f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx0, floorY + 1.42f, bayCenterZ, VehicleClass::Sedan, redCar);
        appendToolChestAndCart(outVerts, outIndices, bx0, floorY, originZ - hl + 1.1f);

        // Bay 2: 2-Post Lifter raising SUV / Pickup
        appendHydraulic2PostLift(outVerts, outIndices, bx1, floorY, bayCenterZ, 1.38f, 1.68f, std::min(4.1f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx1, floorY + 1.35f, bayCenterZ, VehicleClass::SuvPickup, blueCar);
        appendToolChestAndCart(outVerts, outIndices, bx1, floorY, originZ - hl + 1.1f);

        // Bay 3: Heavy-Duty 4-Post Drive-On Hydraulic Lifter raising Commercial Box Truck!
        appendHydraulic4PostLift(outVerts, outIndices, bx2, floorY, bayCenterZ, 1.30f, std::min(4.1f, baySpan - 0.8f), std::min(8.2f, l * 0.45f), std::min(4.6f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx2, floorY + 1.44f, bayCenterZ, VehicleClass::BoxTruck, orangeCar);
        appendToolChestAndCart(outVerts, outIndices, bx2, floorY, originZ - hl + 1.1f);

        // Bay 4: Extra-Wide Heavy Bus & Fleet Inspection Pit + Overhead Bridge Crane
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx3 - 0.72f, floorY + 0.01f, bayCenterZ - 4.8f},
            Vec3{bx3 + 0.72f, floorY + 0.04f, bayCenterZ + 4.8f}, darkMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx3 - 0.85f, floorY + 0.02f, bayCenterZ - 4.9f},
            Vec3{bx3 - 0.72f, floorY + 0.08f, bayCenterZ + 4.9f}, yellowMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx3 + 0.72f, floorY + 0.02f, bayCenterZ - 4.9f},
            Vec3{bx3 + 0.85f, floorY + 0.08f, bayCenterZ + 4.9f}, yellowMat);
        appendDetailedVehicle(outVerts, outIndices, bx3, floorY, bayCenterZ + 0.2f, VehicleClass::TransitBus, blueCar);

        // Overhead yellow bridge crane beam over Heavy Bus Bay 4
        float craneY = std::min(h - 0.85f, 5.8f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx3 - baySpan * 0.46f, craneY, bayCenterZ - 1.5f},
            Vec3{bx3 + baySpan * 0.46f, craneY + 0.35f, bayCenterZ - 1.15f}, yellowMat);

        // Dedicated Tire Storage Racks along Left Wall & Rear + Tire Stacks & Changer Station
        appendTireStorageRack(outVerts, outIndices, originX - hw + 1.4f, floorY, originZ + 2.2f, 1.8f, 3, 0.38f);
        appendTireStorageRack(outVerts, outIndices, bx1, floorY, originZ - hl + 2.2f, 4.6f, 2, 0.38f);
        appendStackedTirePile(outVerts, outIndices, bx0 - 2.1f, floorY, originZ + 3.2f, 5, 0.38f, 0.24f);
        appendStackedTirePile(outVerts, outIndices, bx2 - 2.2f, floorY, originZ + 4.0f, 4, 0.44f, 0.28f);
        appendStackedTirePile(outVerts, outIndices, bx3 + 2.1f, floorY, originZ + 3.5f, 4, 0.50f, 0.32f);
        appendTireServiceStation(outVerts, outIndices, bx0 + 0.3f, floorY, originZ + hl - 2.6f);
    } else if (design == 1) {
        // DESIGN 1: Tire & Wheel Alignment Megashop (Massive Tire Racks + Stacks + Alignment Lifts)
        float bx0 = bayZoneX0 + baySpan * 0.5f;
        float bx1 = bayZoneX0 + baySpan * 1.5f;
        float bx2 = bayZoneX0 + baySpan * 2.5f;
        float bx3 = bayZoneX0 + baySpan * 3.5f;

        // Huge 3-tier tire rack wall along back wall
        appendTireStorageRack(outVerts, outIndices, bx1, floorY, originZ - hl + 1.3f, baySpan * 1.8f, 3, 0.40f);
        appendTireStorageRack(outVerts, outIndices, bx3, floorY, originZ - hl + 1.3f, baySpan * 0.9f, 3, 0.42f);

        // Dual 4-Post Alignment Platform Lifts with alignment sensor towers
        appendHydraulic4PostLift(outVerts, outIndices, bx0, floorY, bayCenterZ, 1.25f, std::min(3.8f, baySpan - 0.8f), 6.2f, std::min(4.2f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx0, floorY + 1.39f, bayCenterZ, VehicleClass::Sedan, redCar);
        appendHydraulic4PostLift(outVerts, outIndices, bx1, floorY, bayCenterZ, 1.35f, std::min(3.9f, baySpan - 0.8f), 6.5f, std::min(4.2f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx1, floorY + 1.49f, bayCenterZ, VehicleClass::SuvPickup, greenEv);

        // 2-Post Express Tire & Brake Lifter
        appendHydraulic2PostLift(outVerts, outIndices, bx2, floorY, bayCenterZ, 1.35f, 1.60f, std::min(4.0f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx2, floorY + 1.32f, bayCenterZ, VehicleClass::Sedan, blueCar);

        // Dedicated 3-Station Tire Mounting / Balancing / Display Zone in Bay 4 & Foreground
        appendTireServiceStation(outVerts, outIndices, bx3, floorY, bayCenterZ - 1.5f);
        appendTireServiceStation(outVerts, outIndices, bx1, floorY, originZ + hl - 2.4f);
        for (int p = 0; p < 5; ++p) {
            appendStackedTirePile(outVerts, outIndices, bx3 - 1.8f + p * 0.95f, floorY, bayCenterZ + 2.4f, 5 - (p % 2), 0.39f, 0.25f);
        }
        appendStackedTirePile(outVerts, outIndices, bx0 - 1.6f, floorY, originZ + hl - 2.3f, 6, 0.40f, 0.25f);
        appendStackedTirePile(outVerts, outIndices, bx0 - 0.6f, floorY, originZ + hl - 2.3f, 5, 0.40f, 0.25f);
    } else if (design == 2) {
        // DESIGN 2: Heavy Truck, Box Van & Bus Fleet Depot (Twin Heavy 4-Post Lifters + Bus Pit + Gantry Crane)
        float thirdSpan = (bayZoneX1 - bayZoneX0) / 3.0f;
        float bx0 = bayZoneX0 + thirdSpan * 0.5f;
        float bx1 = bayZoneX0 + thirdSpan * 1.5f;
        float bx2 = bayZoneX0 + thirdSpan * 2.5f;

        // Heavy 4-Post Truck Lifter #1 + Box Truck
        appendHydraulic4PostLift(outVerts, outIndices, bx0, floorY, bayCenterZ, 1.40f, std::min(4.4f, thirdSpan - 0.9f), std::min(9.0f, l * 0.48f), std::min(5.0f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx0, floorY + 1.54f, bayCenterZ, VehicleClass::BoxTruck, orangeCar);

        // Heavy 4-Post Truck Lifter #2 + Box Truck / Heavy Fleet Rig
        appendHydraulic4PostLift(outVerts, outIndices, bx1, floorY, bayCenterZ, 1.15f, std::min(4.4f, thirdSpan - 0.9f), std::min(9.0f, l * 0.48f), std::min(5.0f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx1, floorY + 1.29f, bayCenterZ, VehicleClass::BoxTruck, blueCar);

        // Heavy Bus & Coach Inspection Pit + Full-Length Transit Bus
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx2 - 0.85f, floorY + 0.01f, bayCenterZ - 5.2f},
            Vec3{bx2 + 0.85f, floorY + 0.06f, bayCenterZ + 5.2f}, darkMat);
        appendDetailedVehicle(outVerts, outIndices, bx2, floorY, bayCenterZ, VehicleClass::TransitBus, redCar);

        // Full-Width Yellow Overhead Bridge Crane Span
        float craneY = std::min(h - 0.75f, 6.1f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bayZoneX0, craneY, bayCenterZ - 0.5f},
            Vec3{bayZoneX1, craneY + 0.42f, bayCenterZ + 0.1f}, yellowMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx1 - 0.45f, craneY - 0.65f, bayCenterZ - 0.4f},
            Vec3{bx1 + 0.45f, craneY, bayCenterZ}, redCar);

        // Giant Heavy-Truck Tire Racks & Stacked Commercial Tires
        appendTireStorageRack(outVerts, outIndices, bx0, floorY, originZ - hl + 1.3f, 5.2f, 2, 0.52f);
        appendStackedTirePile(outVerts, outIndices, bx1 - 2.6f, floorY, originZ + hl - 2.4f, 4, 0.52f, 0.34f);
        appendStackedTirePile(outVerts, outIndices, bx1 - 1.2f, floorY, originZ + hl - 2.4f, 4, 0.52f, 0.34f);
        appendTireServiceStation(outVerts, outIndices, bx2 - 1.5f, floorY, originZ + hl - 2.5f);
        appendToolChestAndCart(outVerts, outIndices, bx0, floorY, originZ - hl + 1.1f);
        appendToolChestAndCart(outVerts, outIndices, bx1, floorY, originZ - hl + 1.1f);
    } else {
        // DESIGN 3: EV & Performance Diagnostics Workshop (2-Post + Scissor Battery Lift + 4-Post + Telemetry)
        float bx0 = bayZoneX0 + baySpan * 0.5f;
        float bx1 = bayZoneX0 + baySpan * 1.5f;
        float bx2 = bayZoneX0 + baySpan * 2.5f;
        float bx3 = bayZoneX0 + baySpan * 3.5f;

        appendHydraulic2PostLift(outVerts, outIndices, bx0, floorY, bayCenterZ, 1.50f, 1.55f, std::min(4.0f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx0, floorY + 1.47f, bayCenterZ, VehicleClass::Sedan, greenEv);

        // Hydraulic Scissor Lift holding EV / Performance car + battery table cart
        appendScissorLift(outVerts, outIndices, bx1, floorY, bayCenterZ, 1.15f, 2.0f, 4.2f);
        appendDetailedVehicle(outVerts, outIndices, bx1, floorY + 1.25f, bayCenterZ, VehicleClass::Sedan, redCar);

        // Heavy 4-Post Lift with Electric SUV
        appendHydraulic4PostLift(outVerts, outIndices, bx2, floorY, bayCenterZ, 1.30f, std::min(4.0f, baySpan - 0.8f), 6.4f, std::min(4.4f, h - 0.6f));
        appendDetailedVehicle(outVerts, outIndices, bx2, floorY + 1.44f, bayCenterZ, VehicleClass::SuvPickup, blueCar);

        // Glazed EV Telemetry & Parts Office in Bay 4 + Tire Racks
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx3 - 2.0f, floorY, originZ - hl + 0.8f},
            Vec3{bx3 + 2.0f, floorY + 1.0f, originZ - hl + 4.4f}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx3 - 1.9f, floorY + 1.0f, originZ - hl + 0.9f},
            Vec3{bx3 + 1.9f, floorY + 2.6f, originZ - hl + 4.3f}, glassMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx3 - 2.05f, floorY + 2.6f, originZ - hl + 0.75f},
            Vec3{bx3 + 2.05f, floorY + 2.8f, originZ - hl + 4.45f}, frameMat);

        appendTireStorageRack(outVerts, outIndices, bx3, floorY, originZ + 2.0f, 3.8f, 3, 0.38f);
        appendStackedTirePile(outVerts, outIndices, bx0 - 1.2f, floorY, originZ + hl - 2.2f, 5, 0.38f, 0.24f);
        appendTireServiceStation(outVerts, outIndices, bx2, floorY, originZ + hl - 2.4f);
    }

    // Customer Parts & Service Reception Counter in Front-Left Foreground
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{originX - hw + 0.7f, floorY, originZ + hl - 4.2f},
        Vec3{originX - hw + 4.2f, floorY + 1.12f, originZ + hl - 3.3f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{originX - hw + 0.65f, floorY + 1.12f, originZ + hl - 4.25f},
        Vec3{originX - hw + 4.25f, floorY + 1.20f, originZ + hl - 3.25f}, silverMat);
}

void RoomGen::generateVehicleRepairInterior(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(24.0f, params_.width);
    float l = std::max(16.0f, params_.length);
    float h = std::max(5.5f, params_.ceilingHeight);
    float hw = w * 0.5f;
    float hl = l * 0.5f;

    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t brickMat = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t silverMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t orangeMat = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);

    // 1. Exterior Asphalt Apron & Drive-In Approaches
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.5f, -0.35f, -hl - 1.5f},
        Vec3{ hw + 2.5f,  0.00f,  hl + 5.0f}, tarmacMat);

    // Concrete Workshop Foundation Slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.2f, 0.00f, -hl - 0.2f},
        Vec3{ hw + 0.2f, 0.16f,  hl + 0.2f}, foundMat);

    // 2. Rear (-Z) & Left (-X) Workshop Walls with Brick Wainscot & Clerestory Light Windows
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.16f, -hl}, Vec3{hw, h, -hl + 0.28f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.03f, 0.16f, -hl - 0.03f}, Vec3{hw + 0.03f, 1.30f, -hl + 0.30f}, brickMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.85f, h - 1.25f, -hl - 0.04f}, Vec3{hw * 0.85f, h - 0.35f, -hl + 0.32f}, glassMat);

    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.16f, -hl}, Vec3{-hw + 0.28f, h, hl}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.03f, 0.16f, -hl - 0.03f}, Vec3{-hw + 0.30f, 1.30f, hl + 0.03f}, brickMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.04f, h - 1.25f, -hl * 0.8f}, Vec3{-hw + 0.32f, h - 0.35f, hl * 0.8f}, glassMat);

    if (params_.showInside) {
        // Cutaway front (+Z) & right (+X) enclosure curbs + high bay steel portal frame so all lifts/tires/vehicles are 100% clear
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.16f, hl - 0.25f}, Vec3{hw, 0.45f, hl}, brickMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.25f, 0.16f, -hl}, Vec3{hw, 0.45f, hl}, brickMat);
        for (float px : {hw - 0.18f}) {
            for (float pz : {-hl + 0.2f, 0.0f, hl - 0.2f}) {
                BuildingComponents::appendBox(outVerts, outIndices, Vec3{px - 0.14f, 0.16f, pz - 0.14f}, Vec3{px + 0.14f, h, pz + 0.14f}, frameMat);
            }
        }
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, h - 0.22f, hl - 0.25f}, Vec3{hw, h, hl}, orangeMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.25f, h - 0.22f, -hl}, Vec3{hw, h, hl}, frameMat);
    } else {
        // Enclosed Garage with 4 tall drive-in service bay roll-up doors on +Z
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{hw - 0.28f, 0.16f, -hl}, Vec3{hw, h, hl}, wallMat);
        int bays = 4;
        float bayW = (w - 2.0f) / bays;
        float doorW = bayW - 0.9f;
        float doorH = std::min(h - 1.1f, 4.6f);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, 0.16f + doorH, hl - 0.28f}, Vec3{hw, h, hl}, wallMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.05f, h - 0.5f, hl + 0.02f}, Vec3{hw + 0.05f, h, hl + 0.12f}, orangeMat);
        for (int b = 0; b <= bays; ++b) {
            float px = -hw + 1.0f + b * bayW;
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{px - 0.45f, 0.16f, hl - 0.28f}, Vec3{px + 0.45f, 0.16f + doorH, hl}, brickMat);
        }
        for (int b = 0; b < bays; ++b) {
            float bx = -hw + 1.0f + (b + 0.5f) * bayW;
            float sBot = (b % 2 == 0) ? (0.16f + doorH * 0.65f) : 0.18f;
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{bx - doorW * 0.5f, sBot, hl - 0.18f}, Vec3{bx + doorW * 0.5f, 0.16f + doorH, hl - 0.05f}, silverMat);
        }
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.25f, h, -hl - 0.25f}, Vec3{hw + 0.25f, h + 0.40f, hl + 0.25f}, roofMat);
    }

    // 3. Populate Spacious Vehicle Repair Interior Content
    appendVehicleRepairInteriorContent(outVerts, outIndices, 0.0f, 0.16f, 0.0f, w, l, h, params_.interiorDesign);

    // 4. Physical 3D Workshop Luminaires along rear & left service walls + portable shop stand
    float shopLampY = std::min(h - 0.8f, 4.1f);
    BuildingComponents::generateWallSconceLamp(outVerts, outIndices, Vec3{-hw * 0.45f, shopLampY, -hl + 0.28f}, Vec3{0.0f, 0.0f, 1.0f}, true);
    BuildingComponents::generateWallSconceLamp(outVerts, outIndices, Vec3{ hw * 0.45f, shopLampY, -hl + 0.28f}, Vec3{0.0f, 0.0f, 1.0f}, true);
    BuildingComponents::generateFloorStandingLamp(outVerts, outIndices, Vec3{-hw * 0.72f, 0.16f, hl * 0.28f}, 2.1f, true);
    if (!params_.showInside) {
        BuildingComponents::generateCeilingPendantBulb(outVerts, outIndices, Vec3{-hw * 0.35f, h, 0.0f}, 0.70f, 0.18f, true);
        BuildingComponents::generateCeilingPendantBulb(outVerts, outIndices, Vec3{ hw * 0.35f, h, 0.0f}, 0.70f, 0.18f, true);
    }
}

} // namespace procedural::buildings
