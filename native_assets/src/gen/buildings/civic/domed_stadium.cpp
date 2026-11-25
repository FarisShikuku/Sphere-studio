#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

void CivicGen::generateDomedStadium(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(42.0f, params_.mainWidth);
    float d = std::max(34.0f, params_.mainDepth);
    float baseH = std::max(12.0f, params_.stories * params_.storyHeight);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t concMat    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t courtMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t seatMat    = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t glassMat   = static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t silverMat  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t jumbotronMat = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);

    // 1. Vast Perimeter Plaza Plinth & Pedestrian Concourse
    float plinthW = w + 12.0f;
    float plinthD = d + 12.0f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-plinthW * 0.5f, -0.6f, -plinthD * 0.5f},
        Vec3{ plinthW * 0.5f,  0.0f,  plinthD * 0.5f},
        foundMat
    );

    // 2. Central Multi-Sport Hardwood Arena Court
    float courtW = w * 0.44f;
    float courtD = d * 0.38f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-courtW * 0.5f, 0.02f, -courtD * 0.5f},
        Vec3{ courtW * 0.5f, 0.08f,  courtD * 0.5f},
        courtMat
    );

    // Court Boundary Lines & Center Circle
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-0.15f, 0.09f, -courtD * 0.5f + 0.5f},
        Vec3{ 0.15f, 0.10f,  courtD * 0.5f - 0.5f},
        concMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-1.8f, 0.09f, -1.8f},
        Vec3{ 1.8f, 0.10f,  1.8f},
        silverMat
    );

    // 3. Multi-Tier Stepped Arena Seating Bowl (Lower, Suites, Upper)
    int tiers = 7;
    float innerW = courtW + 2.0f;
    float innerD = courtD + 2.0f;
    float stepW = (w * 0.88f - innerW) / (2.0f * tiers);
    float stepD = (d * 0.88f - innerD) / (2.0f * tiers);
    float stepH = (baseH * 0.65f) / tiers;

    for (int t = 0; t < tiers; ++t) {
        float curTierH = (t + 1) * stepH;
        float curHalfW = (innerW * 0.5f) + t * stepW;
        float curHalfD = (innerD * 0.5f) + t * stepD;
        float nextHalfW = curHalfW + stepW;
        float nextHalfD = curHalfD + stepD;

        uint32_t tSeatMat = (t == 3 || t == 4) ? static_cast<uint32_t>(MaterialSlot::StadiumSeatBlue) : seatMat;

        // North & South grandstand steps
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-nextHalfW, 0.0f, curHalfD}, Vec3{ nextHalfW, curTierH, nextHalfD}, tSeatMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-nextHalfW, 0.0f, -nextHalfD}, Vec3{ nextHalfW, curTierH, -curHalfD}, tSeatMat);
        // East & West grandstand steps
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{curHalfW, 0.0f, -curHalfD}, Vec3{nextHalfW, curTierH, curHalfD}, tSeatMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-nextHalfW, 0.0f, -curHalfD}, Vec3{-curHalfW, curTierH, curHalfD}, tSeatMat);
    }

    // 4. VIP Luxury Suites Band (Floor-to-ceiling glass ring above bowl)
    float suiteY = tiers * stepH;
    float suiteH = 2.2f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw * 0.88f, suiteY, -hd * 0.88f},
        Vec3{ hw * 0.88f, suiteY + suiteH, hd * 0.88f},
        glassMat
    );

    // 5. Suspended Center-Hung 4-Sided Jumbotron Multimedia Scoreboard Cube
    float jumboY = baseH * 0.95f;
    float jw = 5.2f, jd = 5.2f, jh = 3.6f;
    // Core housing
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-jw * 0.5f, jumboY, -jd * 0.5f},
        Vec3{ jw * 0.5f, jumboY + jh, jd * 0.5f},
        frameMat
    );
    // 4 glowing Jumbotron display screens
    // North screen
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-jw * 0.45f, jumboY + 0.4f, jd * 0.5f},
        Vec3{ jw * 0.45f, jumboY + jh - 0.4f, jd * 0.5f + 0.1f},
        jumbotronMat
    );
    // South screen
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-jw * 0.45f, jumboY + 0.4f, -jd * 0.5f - 0.1f},
        Vec3{ jw * 0.45f, jumboY + jh - 0.4f, -jd * 0.5f},
        jumbotronMat
    );
    // East screen
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{jw * 0.5f, jumboY + 0.4f, -jd * 0.45f},
        Vec3{jw * 0.5f + 0.1f, jumboY + jh - 0.4f, jd * 0.45f},
        jumbotronMat
    );
    // West screen
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-jw * 0.5f - 0.1f, jumboY + 0.4f, -jd * 0.45f},
        Vec3{-jw * 0.5f, jumboY + jh - 0.4f, jd * 0.45f},
        jumbotronMat
    );

    // Suspension cables from dome apex to Jumbotron corners
    float domeApexY = baseH + 8.5f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{-jw * 0.5f, jumboY + jh, -jd * 0.5f}, Vec3{-2.0f, domeApexY, -2.0f}, 0.05f, 0.05f, 6, silverMat, true, true);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{ jw * 0.5f, jumboY + jh, -jd * 0.5f}, Vec3{ 2.0f, domeApexY, -2.0f}, 0.05f, 0.05f, 6, silverMat, true, true);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{ jw * 0.5f, jumboY + jh,  jd * 0.5f}, Vec3{ 2.0f, domeApexY,  2.0f}, 0.05f, 0.05f, 6, silverMat, true, true);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{-jw * 0.5f, jumboY + jh,  jd * 0.5f}, Vec3{-2.0f, domeApexY,  2.0f}, 0.05f, 0.05f, 6, silverMat, true, true);

    // 6. Elliptical Exterior Facade Shell with Ribbon Glazing & Structural Buttress Fins
    const int segments = 32;
    std::vector<Vec3> outerRing(segments);
    for (int i = 0; i < segments; ++i) {
        float angle = (static_cast<float>(i) / segments) * 2.0f * kPi;
        outerRing[i] = Vec3{std::cos(angle) * hw, 0.0f, std::sin(angle) * hd};
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        Vec3 p0 = outerRing[i];
        Vec3 p1 = outerRing[next];
        Vec3 d = p1 - p0;
        Vec3 norm = d.normalized().cross(Vec3{0, 1, 0}).normalized();

        // Lower masonry/composite wall
        BuildingComponents::appendQuad(outVerts, outIndices,
            p0, p1, p1 + Vec3{0, baseH * 0.55f, 0}, p0 + Vec3{0, baseH * 0.55f, 0},
            norm, silverMat);

        // Upper panoramic architectural glass ribbon
        BuildingComponents::appendQuad(outVerts, outIndices,
            p0 + Vec3{0, baseH * 0.55f, 0}, p1 + Vec3{0, baseH * 0.55f, 0},
            p1 + Vec3{0, baseH, 0}, p0 + Vec3{0, baseH, 0},
            norm, glassMat);

        // Exterior structural column rib
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            p0 + norm * 0.15f, p0 + norm * 0.35f + Vec3{0, baseH, 0},
            0.24f, 0.18f, 8, frameMat, true, true);
    }

    // 7. Geodesic Retractable Space-Frame Dome Roof with Central Skylight Oculus
    float oculusR = 6.2f;
    Vec3 oculusCenter{0.0f, domeApexY, 0.0f};

    // Circular Oculus Rim Ring
    for (int i = 0; i < segments; ++i) {
        float a0 = (static_cast<float>(i) / segments) * 2.0f * kPi;
        float a1 = (static_cast<float>((i + 1) % segments) / segments) * 2.0f * kPi;
        Vec3 r0 = oculusCenter + Vec3{std::cos(a0) * oculusR, 0.0f, std::sin(a0) * oculusR};
        Vec3 r1 = oculusCenter + Vec3{std::cos(a1) * oculusR, 0.0f, std::sin(a1) * oculusR};

        // Oculus rim structural beam
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices, r0, r1, 0.22f, 0.22f, 8, frameMat, true, true);

        // Radial arch truss from outer perimeter ring to oculus ring
        Vec3 pPerim = outerRing[i] + Vec3{0, baseH, 0};
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices, pPerim, r0, 0.20f, 0.16f, 8, silverMat, true, true);

        // Roof skin quads between adjacent radial ribs
        Vec3 pPerimNext = outerRing[(i + 1) % segments] + Vec3{0, baseH, 0};
        Vec3 faceNorm = ((pPerimNext - pPerim).cross(r0 - pPerim)).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices, pPerim, pPerimNext, r1, r0, faceNorm, roofMat);

        // Oculus glass skylight fan
        BuildingComponents::appendQuad(outVerts, outIndices, oculusCenter, r0, r1, oculusCenter, Vec3{0, 1, 0}, glassMat);
    }

    // 8. Monumental Public Entrance Portals (North & South Plazas)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-7.5f, 0.0f, hd + 0.2f},
        Vec3{ 7.5f, 4.2f, hd + 3.8f},
        concMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-7.5f, 0.0f, -hd - 3.8f},
        Vec3{ 7.5f, 4.2f, -hd - 0.2f},
        concMat
    );

    // Glass entrance doors inside portals
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-6.5f, 0.0f, hd + 3.82f},
        Vec3{ 6.5f, 2.8f, hd + 3.85f},
        glassMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-6.5f, 0.0f, -hd - 3.85f},
        Vec3{ 6.5f, 2.8f, -hd - 3.82f},
        glassMat
    );
}

} // namespace procedural::buildings
