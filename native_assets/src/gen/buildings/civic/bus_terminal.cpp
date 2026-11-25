#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void CivicGen::generateBusTerminal(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    int stories = std::max(1, params_.stories);
    float w = std::max(28.0f, params_.mainWidth);
    float d = std::max(18.0f, params_.mainDepth);
    float h = stories * params_.storyHeight;
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t concMat   = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat  = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat  = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat   = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t glassMat  = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t hvacMat   = static_cast<uint32_t>(MaterialSlot::HvacMetal);

    // 1. Vast Asphalt / Tarmac Ground Apron with bus lanes
    float apronW = w + 16.0f;
    float apronD = d + 16.0f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-apronW * 0.5f, -0.4f, -apronD * 0.5f},
        Vec3{ apronW * 0.5f,  0.0f,  apronD * 0.5f},
        tarmacMat
    );

    // 2. Main Terminal Concourse Building (Ticket lobby, waiting hall, cafe)
    // Sited on rear-left portion of the site
    float termW = w * 0.45f;
    float termD = d * 0.75f;
    float termMinX = -hw + 1.0f;
    float termMaxX = termMinX + termW;
    float termMinZ = -hd + 1.0f;
    float termMaxZ = termMinZ + termD;

    // Terminal Foundation
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX - 0.3f, 0.0f, termMinZ - 0.3f},
        Vec3{termMaxX + 0.3f, 0.4f, termMaxZ + 0.3f},
        foundMat
    );

    // Terminal Walls (Rear, Left, Solid sections)
    // Back wall (-Z)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX, 0.4f, termMinZ},
        Vec3{termMaxX, h, termMinZ + 0.5f},
        concMat
    );
    // Outer side wall (-X)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX, 0.4f, termMinZ},
        Vec3{termMinX + 0.5f, h, termMaxZ},
        concMat
    );
    // Right interior wall (+X facing platforms)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMaxX - 0.5f, 0.4f, termMinZ},
        Vec3{termMaxX, h, termMaxZ},
        concMat
    );

    // Front facade (+Z) - Double height glass curtain wall
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX, 0.4f, termMaxZ - 0.2f},
        Vec3{termMaxX, h, termMaxZ},
        glassMat
    );
    // Horizontal & vertical aluminum mullion grid
    for (int st = 1; st < stories; ++st) {
        float slabY = 0.4f + st * params_.storyHeight;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{termMinX, slabY - 0.15f, termMaxZ - 0.3f},
            Vec3{termMaxX, slabY + 0.15f, termMaxZ + 0.1f},
            frameMat
        );
    }
    // Main entrance automatic sliding doors
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{(termMinX + termMaxX) * 0.5f, 0.4f, termMaxZ},
        3.2f, 2.8f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true
    );

    // Terminal Roof with parapet & HVAC units
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX - 0.2f, h, termMinZ - 0.2f},
        Vec3{termMaxX + 0.2f, h + 0.5f, termMaxZ + 0.2f},
        roofMat
    );
    // Rooftop HVAC units
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX + 2.0f, h + 0.5f, termMinZ + 2.0f},
        Vec3{termMinX + 5.0f, h + 1.8f, termMinZ + 4.5f},
        hvacMat
    );

    // 3. Passenger Boarding Platforms & Sawtooth Canopies (Bays 1 to 4)
    int bays = std::max(3, params_.busBays);
    float bayStartX = termMaxX + 2.5f;
    float baySpacing = std::min(5.5f, (hw - bayStartX + 4.0f) / std::max(1, bays - 1));
    float platLen = termD + 4.0f;
    float platMinZ = termMinZ;
    float platMaxZ = platMinZ + platLen;

    for (int b = 0; b < bays; ++b) {
        float bayX = bayStartX + b * baySpacing;

        // Raised Platform Island
        float platW = 2.4f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - platW * 0.5f, 0.0f, platMinZ},
            Vec3{bayX + platW * 0.5f, 0.35f, platMaxZ},
            foundMat
        );

        // Tactile Safety Yellow Hazard Stripe on both platform edges
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - platW * 0.5f, 0.35f, platMinZ},
            Vec3{bayX - platW * 0.5f + 0.25f, 0.36f, platMaxZ},
            yellowMat
        );
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX + platW * 0.5f - 0.25f, 0.35f, platMinZ},
            Vec3{bayX + platW * 0.5f, 0.36f, platMaxZ},
            yellowMat
        );

        // Steel Pylon Columns supporting the Canopy
        float canopyH = 4.4f;
        for (float pz = platMinZ + 2.5f; pz <= platMaxZ - 2.0f; pz += 5.0f) {
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{bayX - 0.18f, 0.35f, pz - 0.18f},
                Vec3{bayX + 0.18f, canopyH, pz + 0.18f},
                frameMat
            );
        }

        // Aerodynamic Cantilevered Canopy Roof
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - platW * 0.85f, canopyH, platMinZ - 1.0f},
            Vec3{bayX + platW * 0.85f, canopyH + 0.35f, platMaxZ + 1.0f},
            roofMat
        );

        // Individual Bay Information Totem / Digital Display (e.g. "Bay 1", "Bay 2")
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - 0.35f, 0.35f, platMaxZ - 1.5f},
            Vec3{bayX + 0.35f, 2.6f, platMaxZ - 1.1f},
            frameMat
        );
        // Illuminated route screen face
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bayX - 0.3f, 1.8f, platMaxZ - 1.05f},
            Vec3{bayX + 0.3f, 2.5f, platMaxZ - 1.0f},
            glassMat
        );

        // Bus Parking Bay Guide Striping on Tarmac (White stall boxes)
        float busStallX = bayX + platW * 0.5f + 1.6f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{busStallX - 1.3f, 0.01f, platMinZ + 1.0f},
            Vec3{busStallX + 1.3f, 0.02f, platMaxZ - 1.0f},
            concMat
        );
    }

    // 4. Pedestrian Crosswalk & Safety Bollards between terminal and bays
    for (float bx = termMaxX + 0.8f; bx <= bayStartX + bays * baySpacing; bx += 3.0f) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bx - 0.12f, 0.0f, termMaxZ + 1.2f},
            Vec3{bx + 0.12f, 0.9f, termMaxZ + 1.44f},
            yellowMat
        );
    }
}

} // namespace procedural::buildings
