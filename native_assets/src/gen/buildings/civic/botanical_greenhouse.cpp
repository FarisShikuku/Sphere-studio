#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kGhPi = 3.14159265358979323846f;
}

void CivicGen::generateBotanicalGreenhouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(28.0f, params_.mainWidth);
    float d = std::max(16.0f, params_.mainDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassEmer   = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t woodMat     = static_cast<uint32_t>(MaterialSlot::PergolaWood);

    // 1. Landscaped Botanical Plaza & Raised Limestone Knee-Wall Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.0f, -0.35f, -hd - 2.5f},
        Vec3{ hw + 3.0f,  0.00f,  hd + 3.5f},
        foundMat);

    // Exterior Lawn & Hedge Borders
    for (float sx : {-hw - 2.2f, hw + 0.6f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx, 0.0f, -hd},
            Vec3{sx + 1.6f, 0.35f, hd},
            planterMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx + 0.1f, 0.35f, -hd + 0.1f},
            Vec3{sx + 1.5f, 0.70f, hd - 0.1f},
            turfMat);
    }

    // 2. Central Rotunda Palm House Glass Dome & Limestone Drum
    float rotundaR = std::min(7.5f, hd * 0.85f);
    float drumH = 5.6f;
    float domeH = 6.2f;

    // Octagonal/16-sided limestone dwarf base wall + tall wrought-iron glass panels + dome cap
    int segs = 16;
    for (int i = 0; i < segs; ++i) {
        float a0 = (2.0f * kGhPi * i) / segs;
        float a1 = (2.0f * kGhPi * (i + 1)) / segs;
        float c0 = std::cos(a0), s0 = std::sin(a0);
        float c1 = std::cos(a1), s1 = std::sin(a1);
        Vec3 norm{std::cos((a0 + a1) * 0.5f), 0.0f, std::sin((a0 + a1) * 0.5f)};

        // Limestone base plinth wall (0 to 1.2m)
        BuildingComponents::appendQuad(outVerts, outIndices,
            Vec3{c0 * rotundaR, 0.0f,  s0 * rotundaR},
            Vec3{c1 * rotundaR, 0.0f,  s1 * rotundaR},
            Vec3{c1 * rotundaR, 1.2f,  s1 * rotundaR},
            Vec3{c0 * rotundaR, 1.2f,  s0 * rotundaR},
            norm, limestone);
        // Tall glazed conservatory wall (1.2m to drumH)
        BuildingComponents::appendQuad(outVerts, outIndices,
            Vec3{c0 * rotundaR, 1.2f,  s0 * rotundaR},
            Vec3{c1 * rotundaR, 1.2f,  s1 * rotundaR},
            Vec3{c1 * rotundaR, drumH, s1 * rotundaR},
            Vec3{c0 * rotundaR, drumH, s0 * rotundaR},
            norm, glassMat);
        // Slender black wrought-iron mullion rib
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{c0 * rotundaR, 0.0f, s0 * rotundaR},
            Vec3{c0 * rotundaR, drumH, s0 * rotundaR},
            0.09f, 0.09f, 6, blackAlum);

        // Stepped Segmented Glass Dome Cap (4 latitude rings)
        int rings = 4;
        for (int r = 0; r < rings; ++r) {
            float phi0 = (0.5f * kGhPi * r) / rings;
            float phi1 = (0.5f * kGhPi * (r + 1)) / rings;
            float rr0 = rotundaR * std::cos(phi0);
            float rr1 = rotundaR * std::cos(phi1);
            float yy0 = drumH + domeH * std::sin(phi0);
            float yy1 = drumH + domeH * std::sin(phi1);
            Vec3 dNorm{
                std::cos((a0 + a1) * 0.5f) * std::cos((phi0 + phi1) * 0.5f),
                std::sin((phi0 + phi1) * 0.5f),
                std::sin((a0 + a1) * 0.5f) * std::cos((phi0 + phi1) * 0.5f)
            };
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{c0 * rr0, yy0, s0 * rr0},
                Vec3{c1 * rr0, yy0, s1 * rr0},
                Vec3{c1 * rr1, yy1, s1 * rr1},
                Vec3{c0 * rr1, yy1, s0 * rr1},
                dNorm, (r == rings - 1) ? silverAlum : glassEmer);
        }
    }
    // Finial lantern spire atop conservatory dome
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, drumH + domeH, 0.0f},
        Vec3{0.0f, drumH + domeH + 1.8f, 0.0f},
        0.55f, 0.08f, 10, blackAlum);

    // 3. Flanking Glasshouse Wing Conservatories (-X and +X wings)
    float wingSpan = hw - rotundaR + 0.8f;
    float wingD = hd * 1.15f;
    float wingH = 4.2f;
    for (int side : {-1, 1}) {
        float wx0 = (side < 0) ? -hw : (rotundaR - 0.8f);
        float wx1 = (side < 0) ? (-rotundaR + 0.8f) : hw;
        if (wx1 - wx0 < 3.0f) continue;

        // Masonry knee wall plinth + glass walls
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0, 0.0f, -wingD * 0.5f},
            Vec3{wx1, 1.0f,  wingD * 0.5f},
            limestone);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0 + 0.1f, 1.0f, -wingD * 0.5f + 0.1f},
            Vec3{wx1 - 0.1f, wingH, wingD * 0.5f - 0.1f},
            glassMat);

        // Interior lush botanical foliage tiers visible through glasshouse!
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0 + 0.8f, 1.0f, -wingD * 0.35f},
            Vec3{wx1 - 0.8f, 1.5f,  wingD * 0.35f},
            planterMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{wx0 + 0.9f, 1.5f, -wingD * 0.32f},
            Vec3{wx1 - 0.9f, 2.7f,  wingD * 0.32f},
            turfMat);

        // Exterior black iron rib trusses along glasshouse wings
        for (float rx = wx0 + 0.6f; rx <= wx1 - 0.4f; rx += 2.2f) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rx - 0.07f, 1.0f, -wingD * 0.5f - 0.05f},
                Vec3{rx + 0.07f, wingH + 0.1f, wingD * 0.5f + 0.05f},
                blackAlum);
        }

        // Glazed pitched conservatory roof on each wing
        RoofGenParams wingRoof;
        wingRoof.type = RoofType::Gable;
        wingRoof.baseWidth = (wx1 - wx0);
        wingRoof.baseDepth = wingD;
        wingRoof.baseTopY = wingH;
        wingRoof.roofHeight = 2.0f;
        wingRoof.overhang = 0.25f;
        wingRoof.showRooftopClutter = false;
        std::vector<BuildingVertex> rwVerts;
        std::vector<uint32_t> rwIdx;
        RoofGen::generate(rwVerts, rwIdx, wingRoof);
        float centerX = (wx0 + wx1) * 0.5f;
        uint32_t vOff = static_cast<uint32_t>(outVerts.size());
        for (auto v : rwVerts) {
            v.position.x += centerX;
            v.materialId = glassEmer;
            outVerts.push_back(v);
        }
        for (uint32_t idx : rwIdx) outIndices.push_back(vOff + idx);
    }

    // 4. Grand Glazed Front Entrance Vestibule (+Z)
    float entZ0 = rotundaR - 0.8f;
    float entZ1 = rotundaR + 2.6f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-3.0f, 0.0f, entZ0},
        Vec3{ 3.0f, 4.2f, entZ1},
        blackAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.7f, 0.2f, entZ0 + 0.1f},
        Vec3{ 2.7f, 3.8f, entZ1 - 0.05f},
        glassMat);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.0f, entZ1},
        2.6f, 2.8f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::ArchedWooden, true);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-3.2f, 4.2f, entZ0},
        Vec3{ 3.2f, 4.55f, entZ1 + 0.3f},
        woodMat);
}

} // namespace procedural::buildings
