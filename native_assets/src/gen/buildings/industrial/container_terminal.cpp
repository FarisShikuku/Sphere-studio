#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

void WarehouseGen::generateContainerTerminal(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(50.0f, std::max(params_.width, params_.length));
    float d = std::max(34.0f, std::min(params_.width, params_.length));
    float h = std::max(16.0f, params_.height * 1.8f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMetal = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t signOrange = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t signRed = static_cast<uint32_t>(MaterialSlot::TrackRed);
    uint32_t containerBlue = static_cast<uint32_t>(MaterialSlot::StadiumSeatBlue);
    uint32_t containerGreen = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);

    // 1. Heavy Intermodal Asphalt Pavement & Truck Transfer Yard
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.35f, -hd - 4.0f},
        Vec3{ hw + 6.0f,  0.0f,   hd + 4.0f},
        tarmacMat);

    // Ground Crane Rails (running along Z axis)
    float railLeftX = -13.0f;
    float railRightX = 13.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{railLeftX - 0.25f, 0.0f, -hd - 2.0f},
        Vec3{railLeftX + 0.25f, 0.2f,  hd + 2.0f},
        frameMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{railRightX - 0.25f, 0.0f, -hd - 2.0f},
        Vec3{railRightX + 0.25f, 0.2f,  hd + 2.0f},
        frameMat);

    // Painted White / Yellow Truck Lane Striping
    for (float tx = -hw + 4.0f; tx < hw - 4.0f; tx += 6.5f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx, 0.01f, -hd + 1.0f},
            Vec3{tx + 0.15f, 0.03f, hd - 1.0f},
            silverAlum);
    }

    // 2. Giant Rail-Mounted Gantry Crane (RMG / RTG)
    float craneZ = 2.0f;
    float craneH = h;
    float legW = 1.2f;

    // Dual Box-Girder Overhead Bridge Beams (X: railLeftX to railRightX)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{railLeftX - 1.5f, craneH - 1.8f, craneZ - 1.6f},
        Vec3{railRightX + 1.5f, craneH, craneZ - 0.4f},
        safetyYellow);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{railLeftX - 1.5f, craneH - 1.8f, craneZ + 0.4f},
        Vec3{railRightX + 1.5f, craneH, craneZ + 1.6f},
        safetyYellow);

    // Hazard Black Stripes along girder sides
    for (float hx = railLeftX; hx < railRightX; hx += 4.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hx, craneH - 1.6f, craneZ - 1.62f},
            Vec3{hx + 1.2f, craneH - 0.2f, craneZ - 1.58f},
            darkMetal);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hx, craneH - 1.6f, craneZ + 1.58f},
            Vec3{hx + 1.2f, craneH - 0.2f, craneZ + 1.62f},
            darkMetal);
    }

    // 4 Heavy Vertical Steel Crane Legs
    for (float lx : {railLeftX, railRightX}) {
        for (float lz : {craneZ - 1.2f, craneZ + 1.2f}) {
            // Main vertical leg
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - legW * 0.5f, 0.2f, lz - legW * 0.5f},
                Vec3{lx + legW * 0.5f, craneH - 1.8f, lz + legW * 0.5f},
                safetyYellow);
            // Motorized Steel Wheel Bogies on ground rail
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{lx - 1.2f, 0.0f, lz - 0.8f},
                Vec3{lx + 1.2f, 0.9f, lz + 0.8f},
                darkMetal);
        }
    }

    // Top Moving Trolley Winch & Operator Cab
    float trolleyX = 0.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{trolleyX - 2.2f, craneH, craneZ - 1.8f},
        Vec3{trolleyX + 2.2f, craneH + 1.6f, craneZ + 1.8f},
        darkMetal);

    // Suspended Operator Glass Cab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{trolleyX - 1.6f, craneH - 3.4f, craneZ - 2.8f},
        Vec3{trolleyX + 0.4f, craneH - 1.8f, craneZ - 1.6f},
        glassMat);

    // Container Spreader Bar suspended below trolley
    float spreaderY = craneH * 0.55f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{trolleyX - 3.2f, spreaderY, craneZ - 1.4f},
        Vec3{trolleyX + 3.2f, spreaderY + 0.6f, craneZ + 1.4f},
        safetyYellow);
    // Suspension Cables
    for (float cx : {trolleyX - 2.8f, trolleyX + 2.8f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.05f, spreaderY + 0.6f, craneZ - 0.05f},
            Vec3{cx + 0.05f, craneH, craneZ + 0.05f},
            silverAlum);
    }

    // 3. Multi-Tier ISO Shipping Container Stacks (20ft & 40ft containers)
    // Container Dimensions: 40ft container is approx 12m length x 2.4m width x 2.6m height
    // Container stack blocks under and around crane:
    struct ContainerSlot {
        float x, y, z, w, d, h;
        uint32_t mat;
    };

    std::vector<ContainerSlot> containers = {
        // Central Stack A (Under Crane) - Tier 1
        {-5.0f, 0.0f, craneZ - 3.0f, 6.0f, 2.4f, 2.6f, containerBlue},
        { 5.0f, 0.0f, craneZ - 3.0f, 6.0f, 2.4f, 2.6f, signRed},
        {-5.0f, 0.0f, craneZ + 3.0f, 6.0f, 2.4f, 2.6f, signOrange},
        { 5.0f, 0.0f, craneZ + 3.0f, 6.0f, 2.4f, 2.6f, containerGreen},
        // Central Stack A - Tier 2
        {-5.0f, 2.6f, craneZ - 3.0f, 6.0f, 2.4f, 2.6f, signRed},
        { 5.0f, 2.6f, craneZ - 3.0f, 6.0f, 2.4f, 2.6f, containerBlue},
        {-5.0f, 2.6f, craneZ + 3.0f, 6.0f, 2.4f, 2.6f, containerGreen},
        { 5.0f, 2.6f, craneZ + 3.0f, 6.0f, 2.4f, 2.6f, signOrange},
        // Central Stack A - Tier 3
        { 0.0f, 5.2f, craneZ - 3.0f, 6.0f, 2.4f, 2.6f, containerBlue},
        { 0.0f, 5.2f, craneZ + 3.0f, 6.0f, 2.4f, 2.6f, containerGreen},

        // West Storage Bay (Outside Left Rail) - 40ft Stacks
        {-hw + 8.0f, 0.0f, -6.0f, 10.0f, 2.4f, 2.6f, signOrange},
        {-hw + 8.0f, 2.6f, -6.0f, 10.0f, 2.4f, 2.6f, containerBlue},
        {-hw + 8.0f, 0.0f,  6.0f, 10.0f, 2.4f, 2.6f, containerGreen},
        {-hw + 8.0f, 2.6f,  6.0f, 10.0f, 2.4f, 2.6f, signRed},

        // East Storage Bay (Outside Right Rail)
        {hw - 8.0f, 0.0f, -6.0f, 10.0f, 2.4f, 2.6f, containerBlue},
        {hw - 8.0f, 2.6f, -6.0f, 10.0f, 2.4f, 2.6f, signOrange},
        {hw - 8.0f, 0.0f,  6.0f, 10.0f, 2.4f, 2.6f, signRed},
        {hw - 8.0f, 2.6f,  6.0f, 10.0f, 2.4f, 2.6f, containerGreen}
    };

    for (const auto& c : containers) {
        // Container Main Box
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{c.x - c.w * 0.5f, c.y, c.z - c.d * 0.5f},
            Vec3{c.x + c.w * 0.5f, c.y + c.h, c.z + c.d * 0.5f},
            c.mat);
        // Container Corner Castings (Black corner steel blocks)
        for (float cx : {c.x - c.w * 0.5f, c.x + c.w * 0.5f - 0.35f}) {
            for (float cz : {c.z - c.d * 0.5f, c.z + c.d * 0.5f - 0.35f}) {
                for (float cy : {c.y, c.y + c.h - 0.35f}) {
                    BuildingComponents::appendBox(outVerts, outIndices,
                        Vec3{cx, cy, cz},
                        Vec3{cx + 0.35f, cy + 0.35f, cz + 0.35f},
                        darkMetal);
                }
            }
        }
    }

    // 4. Terminal Security Gatehouse & Inspection Barrier
    float gateX = hw - 4.0f;
    float gateZ = hd - 4.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{gateX - 3.5f, 0.0f, gateZ - 2.5f},
        Vec3{gateX + 3.5f, 3.8f, gateZ + 2.5f},
        wallConcrete);
    // Gatehouse Inspection Canopy
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{gateX - 5.0f, 3.8f, gateZ - 3.5f},
        Vec3{gateX + 5.0f, 4.1f, gateZ + 3.5f},
        silverAlum);
    // Boom barrier arm
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{gateX - 7.5f, 0.9f, gateZ},
        Vec3{gateX - 3.5f, 1.1f, gateZ + 0.2f},
        signRed);

    // 5. High-Mast Perimeter Floodlight Towers (Height 18m)
    for (float fx : {-hw + 2.0f, hw - 2.0f}) {
        float fz = -hd + 2.0f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.35f, 0.0f, fz - 0.35f},
            Vec3{fx + 0.35f, 18.0f, fz + 0.35f},
            silverAlum);
        // Head array platform & floodlight lamps
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 1.8f, 17.5f, fz - 0.6f},
            Vec3{fx + 1.8f, 18.5f, fz + 0.6f},
            darkMetal);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 1.6f, 17.7f, fz + 0.62f},
            Vec3{fx + 1.6f, 18.3f, fz + 0.85f},
            silverAlum);
    }
}

} // namespace procedural::buildings
