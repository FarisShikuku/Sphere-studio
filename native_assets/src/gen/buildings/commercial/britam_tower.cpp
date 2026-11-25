#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

Vec3 lerp3(const Vec3& a, const Vec3& b, float t) {
    return {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
}

Vec3 computeTriNormal(const Vec3& a, const Vec3& b, const Vec3& c) {
    Vec3 u{b.x - a.x, b.y - a.y, b.z - a.z};
    Vec3 v{c.x - a.x, c.y - a.y, c.z - a.z};
    Vec3 n = u.cross(v);
    float len = n.length();
    return (len > 1e-5f) ? (n / len) : Vec3{0.0f, 1.0f, 0.0f};
}

void appendFacetQuad(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
    uint32_t mat
) {
    Vec3 n = computeTriNormal(p0, p1, p2);
    BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, n, mat);
}

void appendTri(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& p0, const Vec3& p1, const Vec3& p2,
    uint32_t mat
) {
    Vec3 n = computeTriNormal(p0, p1, p2);
    uint32_t base = static_cast<uint32_t>(outVerts.size());
    outVerts.push_back({p0, n, {0.0f, 0.0f}, mat});
    outVerts.push_back({p1, n, {1.0f, 0.0f}, mat});
    outVerts.push_back({p2, n, {0.5f, 1.0f}, mat});
    outIndices.push_back(base);
    outIndices.push_back(base + 1);
    outIndices.push_back(base + 2);
}

} // namespace

void TowerGen::generateBritamTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Britam Tower — Upper Hill, Nairobi (200m architectural landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 34.0f, 22.0f, 52.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 30.0f, 20.0f, 48.0f);
    int32_t stories = std::clamp(params_.stories, 18, 60);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 4.0f, 3.0f, 5.5f);
    float officeHeight = stories * storyH;
    float crownStartH = officeHeight;
    float crownPeakY = std::max(officeHeight + 52.0f, 180.0f);
    float spireTipY = std::max(crownPeakY + 20.0f, 200.0f);

    uint32_t louverCream   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t silverAlum    = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkBasalt    = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t emeraldGlass  = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t spandrelDark  = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t frameDark     = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t turfGreen     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t cobaltTeal    = static_cast<uint32_t>(MaterialSlot::Glass);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Landscaped Forecourt Plaza, Iconic Britam Totem Pylon & Dark Slate Lower Wing
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 12.0f, -0.25f, -hd - 10.0f},
        Vec3{ hw + 18.0f,  0.02f,  hd + 14.0f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 9.0f, 0.02f, hd + 3.0f},
        Vec3{-3.5f,      0.55f, hd + 11.5f},
        turfGreen);
    // Signature Teal Britam Entrance Totem Blade
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-5.5f, 0.02f, hd + 7.5f},
        Vec3{-3.9f, 6.8f,  hd + 8.3f},
        cobaltTeal);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-5.35f, 6.8f, hd + 7.58f},
        Vec3{-4.05f, 7.15f, hd + 8.22f},
        silverAlum);

    // Adjacent dark slate angled office annex wing on right (+X)
    float wingH = std::min(officeHeight * 0.42f, 54.0f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw * 0.35f, 0.02f, -hd * 0.92f},
        Vec3{hw + 12.0f, wingH,  hd * 0.62f},
        darkBasalt);
    for (int wf = 1; wf < 10; ++wf) {
        float wy = (wingH / 10.0f) * wf;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{hw * 0.38f, wy - 0.18f, -hd * 0.93f},
            Vec3{hw + 12.05f, wy + 0.18f, hd * 0.63f},
            frameDark);
    }

    // 2. Ground Entrance Arcade & Emerald Glazed Podium Base
    float podiumH = storyH * 2.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.2f, 0.02f, -hd - 0.8f},
        Vec3{ hw + 1.2f, podiumH, hd + 1.0f},
        spandrelDark);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.55f, podiumH - 0.45f, hd + 0.8f},
        Vec3{ hw * 0.35f, podiumH + 0.25f, hd + 4.4f},
        frameDark);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{-hw * 0.1f, 0.02f, hd + 1.05f},
        3.6f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::RevolvingDoor, true
    );

    // 3. Signature Asymmetric Faceted Helical Tapering Prism (0m -> 180m)
    // Cross-section evolves smoothly from wide asymmetric polygon at base to compact sculpted diamond crown
    const int segs = 28;
    auto ringAt = [&](float t) -> std::array<Vec3, 6> {
        // t in [0, 1] from podiumH up to crownPeakY
        float y = podiumH + t * (crownPeakY - podiumH);
        float scaleX = 1.0f - 0.72f * std::pow(t, 1.08f);
        float scaleZ = 1.0f - 0.74f * std::pow(t, 1.05f);
        // Subtle twist + diagonal helical drift of vertices creating Britam's sculptural facets
        float shiftX = -hw * 0.16f * std::sin(t * 1.4f);
        float drift = t * 4.2f;

        // 6-vertex asymmetric polygon capturing the wide front face split by the signature turquoise V-cleft:
        // v0: front-left, v1: cleft-left ridge, v2: cleft-right ridge, v3: front-right, v4: rear-right, v5: rear-left
        float cleftWidth = hw * (0.28f * (1.0f - t) + 0.04f);
        float cleftCenter = lerp3(Vec3{-hw * 0.18f, 0, 0}, Vec3{hw * 0.08f, 0, 0}, t).x;
        float frontZ = hd * scaleZ;
        float rearZ  = -hd * scaleZ;

        Vec3 v0{-hw * scaleX + shiftX, y, frontZ * 0.88f};
        Vec3 v1{cleftCenter - cleftWidth * 0.5f, y, frontZ};
        Vec3 v2{cleftCenter + cleftWidth * 0.5f, y, frontZ};
        Vec3 v3{ hw * scaleX + shiftX * 0.4f - drift * 0.35f, y, frontZ * 0.84f};
        Vec3 v4{ hw * scaleX * 0.86f - drift * 0.25f, y, rearZ};
        Vec3 v5{-hw * scaleX * 0.90f + shiftX * 0.6f, y, rearZ * 0.96f};
        return {v0, v1, v2, v3, v4, v5};
    };

    auto prevRing = ringAt(0.0f);
    for (int s = 1; s <= segs; ++s) {
        float t = static_cast<float>(s) / static_cast<float>(segs);
        auto currRing = ringAt(t);

        // Facet 0: Front-Left Louvered Sail Wing (v0 -> v1)
        appendFacetQuad(outVerts, outIndices, prevRing[0], prevRing[1], currRing[1], currRing[0], louverCream);
        // Facet 1: SIGNATURE TURQUOISE-EMERALD GLAZED V-CLEFT (v1 -> v2, slightly recessed)
        {
            Vec3 r1p{prevRing[1].x, prevRing[1].y, prevRing[1].z - 0.75f};
            Vec3 r2p{prevRing[2].x, prevRing[2].y, prevRing[2].z - 0.75f};
            Vec3 r1c{currRing[1].x, currRing[1].y, currRing[1].z - 0.75f};
            Vec3 r2c{currRing[2].x, currRing[2].y, currRing[2].z - 0.75f};
            // Inner recessed glass plane
            appendFacetQuad(outVerts, outIndices, r1p, r2p, r2c, r1c, emeraldGlass);
            // Left and right chamfer returns of the cleft
            appendFacetQuad(outVerts, outIndices, prevRing[1], r1p, r1c, currRing[1], cobaltTeal);
            appendFacetQuad(outVerts, outIndices, r2p, prevRing[2], currRing[2], r2c, cobaltTeal);
        }
        // Facet 2: Front-Right Louvered Sail Wing (v2 -> v3)
        appendFacetQuad(outVerts, outIndices, prevRing[2], prevRing[3], currRing[3], currRing[2], louverCream);
        // Facet 3: Right Stepped Taper Wall (v3 -> v4)
        appendFacetQuad(outVerts, outIndices, prevRing[3], prevRing[4], currRing[4], currRing[3], silverAlum);
        // Facet 4: Rear Taper Wall (v4 -> v5)
        appendFacetQuad(outVerts, outIndices, prevRing[4], prevRing[5], currRing[5], currRing[4], spandrelDark);
        // Facet 5: Left Taper Wall (v5 -> v0)
        appendFacetQuad(outVerts, outIndices, prevRing[5], prevRing[0], currRing[0], currRing[5], silverAlum);

        // Crisp exterior horizontal brise-soleil louver blades across the front wings & cleft mullions
        if (s % 1 == 0 && s < segs) {
            Vec3 ribL0 = lerp3(currRing[0], currRing[1], 0.02f);
            Vec3 ribL1 = lerp3(currRing[0], currRing[1], 0.98f);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{ribL0.x, ribL0.y, ribL0.z + 0.14f},
                Vec3{ribL1.x, ribL1.y, ribL1.z + 0.14f},
                0.16f, 0.16f, 6, silverAlum, true, true
            );
            Vec3 ribR0 = lerp3(currRing[2], currRing[3], 0.02f);
            Vec3 ribR1 = lerp3(currRing[2], currRing[3], 0.98f);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{ribR0.x, ribR0.y, ribR0.z + 0.14f},
                Vec3{ribR1.x, ribR1.y, ribR1.z + 0.14f},
                0.16f, 0.16f, 6, silverAlum, true, true
            );
            // Horizontal mullion bar across turquoise cleft
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{currRing[1].x, currRing[1].y, currRing[1].z - 0.55f},
                Vec3{currRing[2].x, currRing[2].y, currRing[2].z - 0.55f},
                0.11f, 0.11f, 6, frameDark, true, true
            );
        }

        prevRing = currRing;
    }

    // 4. Sculpted Slanted Roof Cap & Iconic Helical-Mast Apex Spire reaching 200.0m
    auto topRing = ringAt(1.0f);
    Vec3 apexCenter{
        (topRing[0].x + topRing[3].x) * 0.5f,
        crownPeakY + 2.5f,
        (topRing[1].z + topRing[4].z) * 0.5f
    };
    for (int i = 0; i < 6; ++i) {
        appendTri(outVerts, outIndices, topRing[i], topRing[(i + 1) % 6], apexCenter, silverAlum);
    }

    // Main spire mast & sculpted wind-turbine rings up to Y = 200.0m
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{apexCenter.x, apexCenter.y - 1.5f, apexCenter.z},
        Vec3{apexCenter.x, spireTipY, apexCenter.z},
        0.55f, 0.10f, 10, silverAlum, true, true
    );
    // Spire beacon collar rings
    for (int r = 0; r < 3; ++r) {
        float ry = apexCenter.y + 4.5f + r * 4.2f;
        float rad = 1.25f - r * 0.28f;
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{apexCenter.x, ry - 0.25f, apexCenter.z},
            Vec3{apexCenter.x, ry + 0.25f, apexCenter.z},
            rad, rad, 12, cobaltTeal, true, true
        );
    }
    // Corner seam structural mullions running edge-to-apex
    auto baseRing = ringAt(0.0f);
    for (int edge : {0, 1, 2, 3}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            baseRing[edge], topRing[edge],
            0.28f, 0.18f, 6, frameDark, true, true
        );
    }
    (void)crownStartH;
}

} // namespace procedural::buildings
