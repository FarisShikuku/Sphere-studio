#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void WarehouseGen::generateAerospaceHangar(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float l = std::max(46.0f, params_.length);
    float w = std::max(32.0f, params_.width);
    float h = 26.0f; // High-bay rocket assembly clearance
    float hl = l * 0.5f;
    float hw = w * 0.5f;

    uint32_t whitePanels = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t steelSpaceFrame = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkCharcoal = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t foundMat     = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t glassMat     = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t cryogenicMat = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t roofMat      = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    // 1. Heavy reinforced concrete foundation apron
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.5f, -0.6f, -hl - 2.5f},
        Vec3{hw + 2.5f, 0.0f, hl + 2.5f},
        foundMat);

    // 2. High-Bay Assembly Hall Enclosure (Insulated architectural white cassette panels)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hl},
        Vec3{hw, h, hl},
        whitePanels);

    // 3. External Structural Steel Space-Frame Exoskeleton Trusses
    int trussCount = 7;
    for (int t = 0; t < trussCount; ++t) {
        float tz = -hl + t * ((2.0f * hl) / (trussCount - 1));

        // Left vertical exoskeleton column
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 1.6f, 0.0f, tz - 0.4f},
            Vec3{-hw, h + 2.2f, tz + 0.4f},
            steelSpaceFrame);

        // Right vertical exoskeleton column
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw, 0.0f, tz - 0.4f},
            Vec3{hw + 1.6f, h + 2.2f, tz + 0.4f},
            steelSpaceFrame);

        // Overhead spanning roof truss arch
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 1.6f, h, tz - 0.45f},
            Vec3{hw + 1.6f, h + 2.4f, tz + 0.45f},
            steelSpaceFrame);
    }

    // 4. Giant Multi-Leaf Telescoping Hangar Door (Front, +Z: width 24m, height 19m)
    float doorW = w * 0.75f;
    float doorH = 19.0f;

    // Safety yellow & black chevron hazard surround frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-doorW * 0.5f - 0.8f, 0.0f, hl},
        Vec3{doorW * 0.5f + 0.8f, doorH + 0.8f, hl + 0.25f},
        safetyYellow);

    // Dark charcoal segmented sliding telescoping door leaves
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-doorW * 0.5f, 0.0f, hl + 0.26f},
        Vec3{doorW * 0.5f, doorH, hl + 0.55f},
        darkCharcoal);

    // Horizontal vision glazing strips across the door leaves
    for (float vy : {4.0f, 9.0f, 14.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-doorW * 0.45f, vy, hl + 0.56f},
            Vec3{doorW * 0.45f, vy + 0.9f, hl + 0.60f},
            glassMat);
    }

    // Personnel egress door beside the giant hangar door
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{hw - 2.2f, 0.0f, hl + 0.02f},
        1.4f, 2.5f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::SingleModern, true);

    // 5. Attached 4-Story Engineering, Avionics & Cleanroom Annex (Left side, -X)
    float annW = 8.0f;
    float annL = hl * 1.5f;
    float annH = 14.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - annW, 0.0f, -annL * 0.5f},
        Vec3{-hw, annH, annL * 0.5f},
        steelSpaceFrame);

    // 4 continuous horizontal ribbon glazing floors along the annex
    for (int fl = 0; fl < 4; ++fl) {
        float wy = 1.2f + fl * 3.3f;
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{-hw - annW - 0.02f, wy + 0.8f, 0.0f},
            annL * 0.85f, 1.6f,
            Vec3{-1, 0, 0}, Vec3{0, 0, 1},
            WindowType::RibbonStrip, GlassType::SolarTintedSmoke);
    }

    // 6. Cryogenic LOX & Liquid Hydrogen Spherical Propellant Storage Tanks (Rear right)
    float cryoX = hw * 0.65f;
    float cryoZ = -hl - 5.0f;
    for (float cx : {cryoX - 4.0f, cryoX + 4.0f}) {
        float tankR = 3.6f;
        float tankY = 4.8f;
        // Concrete foundation ring
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 2.8f, 0.0f, cryoZ - 2.8f},
            Vec3{cx + 2.8f, 0.6f, cryoZ + 2.8f},
            foundMat);
        // 6 structural steel support struts
        for (int i = 0; i < 6; ++i) {
            float ang = i * (2.0f * 3.14159f / 6.0f);
            float lx = cx + std::cos(ang) * 2.5f;
            float lz = cryoZ + std::sin(ang) * 2.5f;
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{lx, 0.6f, lz},
                Vec3{cx + std::cos(ang) * 1.5f, tankY, cryoZ + std::sin(ang) * 1.5f},
                0.18f, 0.18f, 10, darkCharcoal, true, true);
        }
        // Spherical insulated cryogenic tank body
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{cx, tankY - tankR * 0.8f, cryoZ},
            Vec3{cx, tankY + tankR * 0.8f, cryoZ},
            tankR, tankR, 20, whitePanels, true, true);
    }

    // Vacuum-insulated cryogenic propellant pipeline connecting tanks into hangar
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{cryoX, 3.5f, cryoZ},
        Vec3{cryoX, 3.5f, -hl},
        0.28f, 0.28f, 14, steelSpaceFrame, true, true);

    // 7. Rooftop High-Mast Lightning Protection Spire (apex height 36m)
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, h + 2.4f, 0.0f},
        Vec3{0.0f, h + 10.0f, 0.0f},
        0.18f, 0.04f, 12, steelSpaceFrame, true, true);
}

} // namespace procedural::buildings
