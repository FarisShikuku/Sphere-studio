#include "HouseGen.h"
#include "HouseScaleHelpers.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void HouseGen::generateGeodesicEcoDome(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = res_scale::width(params_, 8.0f, 14.0f);
    float d = res_scale::depth(params_, 8.0f, 14.0f);
    float flH = res_scale::floorHeight(params_, 3.0f);
    int stories = res_scale::stories(params_, 1, 3);
    float totalDomeH = stories * flH + res_scale::roofHeight(params_, 2.2f);
    float radius = std::max(4.0f, std::min(w, d) * 0.5f);

    uint32_t domeWall    = res_scale::wallMat(params_, MaterialSlot::WallStucco);
    uint32_t cedarDeck   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t strutMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    GlassType gt         = res_scale::glassType(params_, GlassType::ClearFloat);
    uint32_t glassMat    = BuildingComponents::getGlassMaterial(gt);
    WindowType wt        = res_scale::windowType(params_, WindowType::PictureGlass);
    bool hasDeck         = res_scale::showBalconies(params_);

    // 1. Elevated Octagonal/Circular Timber & Stone Eco-Deck
    float deckY = 0.45f;
    float deckR = radius + (hasDeck ? 1.6f : 0.4f);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, -0.35f, 0.0f},
        Vec3{0.0f, deckY, 0.0f},
        deckR, deckR, 12, cedarDeck, true, true);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, -0.35f, 0.0f},
        Vec3{0.0f, 0.05f, 0.0f},
        deckR + 0.25f, deckR + 0.25f, 12, foundMat, true, true);

    // 2. Geodesic Tiered Spherical Shell (4 tapered 10-sided structural dome bands + apex glass oculus)
    constexpr int rings = 4;
    constexpr int segs = 10;
    float prevY = deckY;
    float prevR = radius;
    for (int r = 1; r <= rings; ++r) {
        float t = static_cast<float>(r) / rings;
        float angle = t * 1.48f; // ~85 deg dome profile
        float nextY = deckY + std::sin(angle) * totalDomeH;
        float nextR = std::max(0.95f, std::cos(angle) * radius);
        uint32_t ringMat = (r == 2) ? glassMat : domeWall;

        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{0.0f, prevY, 0.0f},
            Vec3{0.0f, nextY, 0.0f},
            prevR, nextR, segs, ringMat, false, (r == rings));

        // Exposed geodesic rib struts at segment seams
        for (int s = 0; s < segs; ++s) {
            float phi = (2.0f * 3.14159265f * s) / segs;
            float cx0 = std::cos(phi) * prevR;
            float cz0 = std::sin(phi) * prevR;
            float cx1 = std::cos(phi) * nextR;
            float cz1 = std::sin(phi) * nextR;
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{cx0, prevY, cz0}, Vec3{cx1, nextY, cz1},
                0.08f, 0.08f, 6, strutMat, true, true);
        }

        // Intermediate floor observation deck ring inside dome perimeter
        if (r == 1 && stories >= 2) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{0.0f, nextY - 0.12f, 0.0f},
                Vec3{0.0f, nextY, 0.0f},
                nextR + 0.35f, nextR + 0.35f, segs, cedarDeck, true, true);
        }

        prevY = nextY;
        prevR = nextR;
    }

    // 3. Apex Skylight Glass Oculus & Dome Cap
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, prevY, 0.0f},
        Vec3{0.0f, prevY + 0.45f, 0.0f},
        prevR * 0.92f, prevR * 0.60f, segs, glassMat, true, true);

    // 4. Projecting Timber Vestibule Entry Tunnel (+Z) & Scaled Front Door
    float vestW = std::clamp(radius * 0.48f, 2.2f, 3.6f);
    float vestH = std::min(flH, 3.0f);
    float vestZ0 = radius * 0.72f;
    float vestZ1 = radius + 1.15f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-vestW * 0.5f, deckY, vestZ0},
        Vec3{vestW * 0.5f, deckY + vestH, vestZ1},
        domeWall);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-vestW * 0.5f - 0.15f, deckY + vestH, vestZ0 - 0.1f},
        Vec3{vestW * 0.5f + 0.15f, deckY + vestH + 0.25f, vestZ1 + 0.25f},
        cedarDeck);

    res_scale::appendScaledDoor(
        outVerts, outIndices, params_,
        Vec3{0.0f, deckY, vestZ1 + 0.02f}, vestH, vestW,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::FrenchDoublePatio, 1.35f, 2.25f, true);

    // 5. Glazed Dormer View Bays around ground ring (+X, -X, -Z)
    float bayW = std::clamp(radius * 0.44f, 2.0f, 3.2f);
    float bayH = std::min(flH * 0.78f, 2.3f);
    // Right bay (+X)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{radius * 0.72f, deckY, -bayW * 0.5f},
        Vec3{radius + 0.45f, deckY + bayH, bayW * 0.5f},
        domeWall);
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{radius + 0.47f, deckY + bayH * 0.5f, 0.0f},
        bayW * 0.78f, bayH * 0.70f, Vec3{1, 0, 0}, Vec3{0, 0, -1}, wt, gt);

    // Left bay (-X)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-radius - 0.45f, deckY, -bayW * 0.5f},
        Vec3{-radius * 0.72f, deckY + bayH, bayW * 0.5f},
        domeWall);
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{-radius - 0.47f, deckY + bayH * 0.5f, 0.0f},
        bayW * 0.78f, bayH * 0.70f, Vec3{-1, 0, 0}, Vec3{0, 0, 1}, wt, gt);

    // Rear bay (-Z)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-bayW * 0.5f, deckY, -radius - 0.45f},
        Vec3{bayW * 0.5f, deckY + bayH, -radius * 0.72f},
        domeWall);
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{0.0f, deckY + bayH * 0.5f, -radius - 0.47f},
        bayW * 0.78f, bayH * 0.70f, Vec3{0, 0, -1}, Vec3{-1, 0, 0}, wt, gt);
}

} // namespace procedural::buildings
