#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void CivicGen::generateAirportTerminal(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(34.0f, params_.mainWidth);
    float d = std::max(22.0f, params_.mainDepth);
    float h = std::max(10.0f, params_.stories * params_.storyHeight);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t concMat    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t glassMat   = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t tarmacMat  = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t yellowMat  = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t silverMat  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);

    // 1. Airside Tarmac Apron Foundation
    float apronW = w + 16.0f;
    float apronD = d + 16.0f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-apronW * 0.5f, -0.5f, -apronD * 0.5f},
        Vec3{ apronW * 0.5f,  0.0f,  apronD * 0.5f},
        tarmacMat
    );

    // Aircraft Stand Taxiway Centerline & Stop Bar (Yellow)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw + 6.0f, 0.01f, -apronD * 0.5f + 1.0f},
        Vec3{-hw + 6.4f, 0.03f, -hd - 1.0f},
        yellowMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{hw - 6.4f, 0.01f, -apronD * 0.5f + 1.0f},
        Vec3{hw - 6.0f, 0.03f, -hd - 1.0f},
        yellowMat
    );

    // 2. Main Airport Terminal Concourse Body
    float termW = w * 0.85f;
    float termD = d * 0.65f;
    float termH = h * 0.75f;
    float termMinX = -termW * 0.5f;
    float termMaxX =  termW * 0.5f;
    float termMinZ = -termD * 0.5f;
    float termMaxZ =  termD * 0.5f;

    // Terminal Foundation
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX - 0.5f, 0.0f, termMinZ - 0.5f},
        Vec3{termMaxX + 0.5f, 0.5f, termMaxZ + 0.5f},
        foundMat
    );

    // Concourse Rear Core Structure
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX, 0.5f, termMinZ},
        Vec3{termMaxX, termH, termMaxZ - 2.0f},
        concMat
    );

    // Airside Facade: Floor-to-Ceiling Panoramic Glass Curtain Wall (-Z)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX + 1.0f, 0.5f, termMinZ - 0.2f},
        Vec3{termMaxX - 1.0f, termH, termMinZ + 0.2f},
        glassMat
    );
    // Structural mullion columns on glass facade
    for (float mx = termMinX + 3.0f; mx <= termMaxX - 2.5f; mx += 4.5f) {
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{mx - 0.15f, 0.5f, termMinZ - 0.35f},
            Vec3{mx + 0.15f, termH, termMinZ + 0.35f},
            frameMat
        );
    }

    // Landside Facade (+Z): Departures / Arrivals Drop-off Portico
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX, 0.5f, termMaxZ - 0.4f},
        Vec3{termMaxX, termH, termMaxZ},
        silverMat
    );
    // Automatic glass entry portals
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{-termW * 0.25f, 0.5f, termMaxZ},
        3.5f, 2.8f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true
    );
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{termW * 0.25f, 0.5f, termMaxZ},
        3.5f, 2.8f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true
    );

    // 3. Sweeping Aerodynamic Gull-Wing Canopy Roof
    float wingRoofOverhang = 2.5f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX - wingRoofOverhang, termH, termMinZ - wingRoofOverhang},
        Vec3{termMaxX + wingRoofOverhang, termH + 0.8f, termMaxZ + wingRoofOverhang},
        roofMat
    );
    // Roof fascia trim
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{termMinX - wingRoofOverhang - 0.2f, termH + 0.6f, termMinZ - wingRoofOverhang - 0.2f},
        Vec3{termMaxX + wingRoofOverhang + 0.2f, termH + 0.95f, termMaxZ + wingRoofOverhang + 0.2f},
        silverMat
    );

    // 4. Passenger Boarding Jet Bridges (Air Bridges) extending to the tarmac
    float bridgeX[2] = {-termW * 0.3f, termW * 0.3f};
    for (int j = 0; j < 2; ++j) {
        float bx = bridgeX[j];
        float bridgeLen = 8.5f;
        float bridgeH = 3.6f;

        // Elevated Glass Boarding Tunnel Tube
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bx - 1.1f, 1.8f, termMinZ - bridgeLen},
            Vec3{bx + 1.1f, 1.8f + bridgeH, termMinZ},
            glassMat
        );
        // Steel frame ring ribs along tunnel
        for (float bz = termMinZ - bridgeLen + 1.5f; bz <= termMinZ; bz += 2.5f) {
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{bx - 1.25f, 1.7f, bz - 0.2f},
                Vec3{bx + 1.25f, 1.9f + bridgeH, bz + 0.2f},
                frameMat
            );
        }

        // Telescoping Aircraft Docking Hood / Rotunda at the outer end
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bx - 1.6f, 1.6f, termMinZ - bridgeLen - 2.2f},
            Vec3{bx + 1.6f, 2.0f + bridgeH, termMinZ - bridgeLen},
            silverMat
        );
        // Support Pedestal Pylon to ground
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{bx - 0.4f, 0.0f, termMinZ - bridgeLen * 0.65f - 0.4f},
            Vec3{bx + 0.4f, 1.8f, termMinZ - bridgeLen * 0.65f + 0.4f},
            frameMat
        );
    }

    // 5. Air Traffic Control (ATC) Tower
    if (params_.hasControlTower) {
        float towerX = hw - 2.5f;
        float towerZ = hd - 2.0f;
        float towerShaftH = h * 1.8f;
        float shaftW = 3.8f;

        // Tower Base Plinth
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{towerX - shaftW * 0.65f, 0.0f, towerZ - shaftW * 0.65f},
            Vec3{towerX + shaftW * 0.65f, 1.2f, towerZ + shaftW * 0.65f},
            foundMat
        );

        // Slender Concrete Tower Shaft
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{towerX - shaftW * 0.5f, 1.2f, towerZ - shaftW * 0.5f},
            Vec3{towerX + shaftW * 0.5f, towerShaftH, towerZ + shaftW * 0.5f},
            concMat
        );

        // Architectural flare / equipment gallery level
        float galleryW = shaftW + 2.2f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{towerX - galleryW * 0.5f, towerShaftH, towerZ - galleryW * 0.5f},
            Vec3{towerX + galleryW * 0.5f, towerShaftH + 1.2f, towerZ + galleryW * 0.5f},
            silverMat
        );

        // 360-degree Panoramic Glass Observation Cab
        float cabW = galleryW + 1.2f;
        float cabH = 2.8f;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{towerX - cabW * 0.5f, towerShaftH + 1.2f, towerZ - cabW * 0.5f},
            Vec3{towerX + cabW * 0.5f, towerShaftH + 1.2f + cabH, towerZ + cabW * 0.5f},
            glassMat
        );
        // Cab structural mullion posts
        for (int a = 0; a < 4; ++a) {
            float ox = (a % 2 == 0) ? -cabW * 0.5f : cabW * 0.5f;
            float oz = (a / 2 == 0) ? -cabW * 0.5f : cabW * 0.5f;
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{towerX + ox - 0.15f, towerShaftH + 1.2f, towerZ + oz - 0.15f},
                Vec3{towerX + ox + 0.15f, towerShaftH + 1.2f + cabH, towerZ + oz + 0.15f},
                frameMat
            );
        }

        // Radar Dome (Radome Cap) & Antenna Mast
        float domeY = towerShaftH + 1.2f + cabH;
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{towerX - cabW * 0.55f, domeY, towerZ - cabW * 0.55f},
            Vec3{towerX + cabW * 0.55f, domeY + 0.5f, towerZ + cabW * 0.55f},
            roofMat
        );
        // Spherical radome dome box
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{towerX - 1.2f, domeY + 0.5f, towerZ - 1.2f},
            Vec3{towerX + 1.2f, domeY + 2.2f, towerZ + 1.2f},
            silverMat
        );
        // Spire antenna
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{towerX - 0.1f, domeY + 2.2f, towerZ - 0.1f},
            Vec3{towerX + 0.1f, domeY + 4.5f, towerZ + 0.1f},
            frameMat
        );
    }
}

} // namespace procedural::buildings
