#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kAmpPi = 3.14159265358979323846f;
}

void CivicGen::generateCommunityAmphitheater(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(28.0f, params_.mainWidth);
    float d = std::max(24.0f, params_.mainDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);

    // 1. Civic Social Park Plaza Pad
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.35f, -hd - 2.0f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 2.0f},
        foundMat);

    // 2. Sculptural Acoustic Bandshell Stage Pavilion (-Z Rear Focal Stage)
    float stageW = std::min(16.0f, w * 0.56f);
    float stageZ0 = -hd + 1.5f;
    float stageZ1 = -hd + 8.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-stageW * 0.5f, 0.0f, stageZ0},
        Vec3{ stageW * 0.5f, 1.1f, stageZ1},
        limestone);
    // Warm timber performance deck top
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-stageW * 0.48f, 1.1f, stageZ0 + 0.2f},
        Vec3{ stageW * 0.48f, 1.22f, stageZ1 - 0.1f},
        pergolaMat);

    // Curved Ribbed Acoustic Canopy Shell arcing over Stage
    int arcRibs = 7;
    float shellH = 8.2f;
    for (int i = 0; i < arcRibs; ++i) {
        float t = -1.0f + 2.0f * (i / static_cast<float>(arcRibs - 1));
        float rx = t * (stageW * 0.48f);
        float tipY = shellH - std::abs(t) * 1.4f;
        // Rear curved structural support column + overhanging acoustic shell rib
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{rx, 1.1f, stageZ0 + 0.3f},
            Vec3{rx, tipY, stageZ0 + 1.4f},
            0.22f, 0.18f, 8, blackAlum);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{rx, tipY, stageZ0 + 1.4f},
            Vec3{rx, tipY + 0.8f, stageZ1 + 1.8f},
            0.18f, 0.12f, 8, silverAlum);
    }
    // Acoustic roof shell deck spanning ribs
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-stageW * 0.50f, shellH + 0.7f, stageZ1 + 1.6f},
        Vec3{ stageW * 0.50f, shellH + 0.7f, stageZ1 + 1.6f},
        Vec3{ stageW * 0.50f, shellH - 0.2f, stageZ0 + 1.0f},
        Vec3{-stageW * 0.50f, shellH - 0.2f, stageZ0 + 1.0f},
        Vec3{0.0f, 0.98f, 0.18f}, silverAlum);
    // Backstage acoustic baffle timber wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-stageW * 0.46f, 1.1f, stageZ0 + 0.4f},
        Vec3{ stageW * 0.46f, shellH - 0.5f, stageZ0 + 0.8f},
        pergolaMat);

    // 3. Concentric Semicircular Stepped Amphitheater Seating Tiers (facing -Z Stage!)
    Vec3 center{0.0f, 0.0f, stageZ1 + 1.2f};
    int tiers = 7;
    int segs  = 16;
    float rStart = 5.5f;
    float tierD  = std::min(1.45f, (hd + 1.0f - stageZ1 - rStart) / static_cast<float>(tiers));
    float tierH  = 0.48f;

    for (int t = 0; t < tiers; ++t) {
        float r0 = rStart + t * tierD;
        float r1 = r0 + tierD;
        float yTop = (t + 1) * tierH;

        for (int s = 0; s < segs; ++s) {
            // Semicircle sweeping from 10 deg to 170 deg in +Z half-space
            float a0 = 0.08f * kAmpPi + (0.84f * kAmpPi * s) / segs;
            float a1 = 0.08f * kAmpPi + (0.84f * kAmpPi * (s + 1)) / segs;
            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            Vec3 in0{center.x + c0 * r0, yTop, center.z + s0 * r0};
            Vec3 in1{center.x + c1 * r0, yTop, center.z + s1 * r0};
            Vec3 out1{center.x + c1 * r1, yTop, center.z + s1 * r1};
            Vec3 out0{center.x + c0 * r1, yTop, center.z + s0 * r1};

            // Step riser face facing inward toward stage
            Vec3 rNorm{-std::cos((a0 + a1) * 0.5f), 0.0f, -std::sin((a0 + a1) * 0.5f)};
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{in1.x, t * tierH, in1.z},
                Vec3{in0.x, t * tierH, in0.z},
                in0, in1, rNorm, limestone);
            // Wooden bench seat top tread
            BuildingComponents::appendQuad(outVerts, outIndices,
                in0, out0, out1, in1,
                Vec3{0, 1, 0}, (t % 2 == 0) ? pergolaMat : limestone);
            // Outer retaining wall on last tier
            if (t == tiers - 1) {
                Vec3 oNorm{std::cos((a0 + a1) * 0.5f), 0.0f, std::sin((a0 + a1) * 0.5f)};
                BuildingComponents::appendQuad(outVerts, outIndices,
                    Vec3{out0.x, 0.0f, out0.z},
                    Vec3{out1.x, 0.0f, out1.z},
                    out1, out0, oNorm, limestone);
            }
        }
    }

    // 4. Flanking Park Lawn Terraces & Shaded Social Pergola Pavilions (-X and +X)
    for (int side : {-1, 1}) {
        float px0 = (side < 0) ? (-hw + 0.5f) : (hw - 4.2f);
        float px1 = (side < 0) ? (-hw + 4.2f) : (hw - 0.5f);
        // Lawn terrace planter
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0, 0.0f, stageZ0},
            Vec3{px1, 0.55f, stageZ1},
            planterMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0 + 0.15f, 0.55f, stageZ0 + 0.15f},
            Vec3{px1 - 0.15f, 0.90f, stageZ1 - 0.15f},
            turfMat);
        // Community Coffee/Ticket Pergola Kiosk
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0 + 0.3f, 0.0f, stageZ1 + 2.0f},
            Vec3{px1 - 0.3f, 2.8f, stageZ1 + 6.0f},
            glassTinted);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px0, 2.8f, stageZ1 + 1.5f},
            Vec3{px1, 3.1f, stageZ1 + 6.5f},
            pergolaMat);
    }
}

} // namespace procedural::buildings
