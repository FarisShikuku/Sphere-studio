#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

void CivicGen::generateOlympicVelodrome(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(48.0f, params_.mainWidth);
    float d = std::max(34.0f, params_.mainDepth);
    float h = std::max(14.0f, params_.stories * params_.storyHeight);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t concMat    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t trackMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t turfMat    = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t seatMat    = static_cast<uint32_t>(MaterialSlot::StadiumSeatBlue);
    uint32_t silverMat  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t flameMat   = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t glassMat   = static_cast<uint32_t>(MaterialSlot::Glass);

    // 1. Broad Elliptical Podium Promenade
    float plinthW = w + 10.0f;
    float plinthD = d + 10.0f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-plinthW * 0.5f, -0.6f, -plinthD * 0.5f},
        Vec3{ plinthW * 0.5f,  0.0f,  plinthD * 0.5f},
        foundMat
    );

    // 2. Central Infield Pit (Turf & Equipment Area)
    float trackInnerW = w * 0.42f;
    float trackInnerD = d * 0.38f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-trackInnerW * 0.5f, 0.02f, -trackInnerD * 0.5f},
        Vec3{ trackInnerW * 0.5f, 0.06f,  trackInnerD * 0.5f},
        turfMat
    );

    // 3. Banked 35° Oval Competition Velodrome Track (Continuous Siberian Pine)
    const int trackSegs = 32;
    float trackWidth = 4.2f;
    std::vector<Vec3> trackInner(trackSegs);
    std::vector<Vec3> trackOuter(trackSegs);

    for (int i = 0; i < trackSegs; ++i) {
        float angle = (static_cast<float>(i) / trackSegs) * 2.0f * kPi;
        float rxIn = std::cos(angle) * (trackInnerW * 0.5f);
        float rzIn = std::sin(angle) * (trackInnerD * 0.5f);

        // Banked elevation: higher on turns (ends in X), lower on straights
        float bankFactor = std::abs(std::cos(angle)); // 1.0 at ends, 0.0 at center
        float bankElevation = 0.4f + bankFactor * 1.8f;

        float rxOut = std::cos(angle) * (trackInnerW * 0.5f + trackWidth);
        float rzOut = std::sin(angle) * (trackInnerD * 0.5f + trackWidth);

        trackInner[i] = Vec3{rxIn, 0.06f, rzIn};
        trackOuter[i] = Vec3{rxOut, 0.06f + bankElevation, rzOut};
    }

    for (int i = 0; i < trackSegs; ++i) {
        int next = (i + 1) % trackSegs;
        Vec3 norm = ((trackOuter[i] - trackInner[i]).cross(trackInner[next] - trackInner[i])).normalized();

        // Banked wooden track surface
        BuildingComponents::appendQuad(outVerts, outIndices,
            trackInner[i], trackInner[next], trackOuter[next], trackOuter[i],
            norm, trackMat);

        // Safety blue perimeter band along inside edge
        BuildingComponents::appendBox(outVerts, outIndices,
            trackInner[i] - Vec3{0.15f, 0, 0.15f}, trackInner[next] + Vec3{0.15f, 0.08f, 0.15f},
            seatMat);
    }

    // 4. Steep Elliptical Spectator Grandstand Seating Bowl
    int bowlTiers = 7;
    float bowlStartR_W = trackInnerW * 0.5f + trackWidth + 0.8f;
    float bowlStartR_D = trackInnerD * 0.5f + trackWidth + 0.8f;
    float bowlEndR_W = hw * 0.86f;
    float bowlEndR_D = hd * 0.86f;
    float tierStepH = (h * 0.65f) / bowlTiers;

    for (int t = 0; t < bowlTiers; ++t) {
        float f0 = static_cast<float>(t) / bowlTiers;
        float f1 = static_cast<float>(t + 1) / bowlTiers;
        float rw0 = bowlStartR_W * (1.0f - f0) + bowlEndR_W * f0;
        float rd0 = bowlStartR_D * (1.0f - f0) + bowlEndR_D * f0;
        float rw1 = bowlStartR_W * (1.0f - f1) + bowlEndR_W * f1;
        float rd1 = bowlStartR_D * (1.0f - f1) + bowlEndR_D * f1;
        float y0 = (t + 1) * tierStepH;

        for (int i = 0; i < trackSegs; ++i) {
            float a0 = (static_cast<float>(i) / trackSegs) * 2.0f * kPi;
            float a1 = (static_cast<float>((i + 1) % trackSegs) / trackSegs) * 2.0f * kPi;

            Vec3 b0{std::cos(a0) * rw0, y0, std::sin(a0) * rd0};
            Vec3 b1{std::cos(a1) * rw0, y0, std::sin(a1) * rd0};
            Vec3 b2{std::cos(a1) * rw1, y0, std::sin(a1) * rd1};
            Vec3 b3{std::cos(a0) * rw1, y0, std::sin(a0) * rd1};

            BuildingComponents::appendQuad(outVerts, outIndices, b0, b1, b2, b3, Vec3{0, 1, 0}, seatMat);

            // Step riser
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{b0.x, y0 - tierStepH, b0.z}, Vec3{b1.x, y0 - tierStepH, b1.z}, b1, b0,
                ((b0 + b1) * 0.5f).normalized(), concMat);
        }
    }

    // 5. Exterior Interwoven Diagrid / Bird's-Nest Lattice Facade Shell
    // Outer elliptical rim
    std::vector<Vec3> perimBot(trackSegs);
    std::vector<Vec3> perimTop(trackSegs);
    for (int i = 0; i < trackSegs; ++i) {
        float a = (static_cast<float>(i) / trackSegs) * 2.0f * kPi;
        // Undulating roofline height: higher at apex sides
        float waveH = h * (0.85f + 0.25f * std::sin(a * 2.0f));
        perimBot[i] = Vec3{std::cos(a) * hw, 0.0f, std::sin(a) * hd};
        perimTop[i] = Vec3{std::cos(a) * hw, waveH, std::sin(a) * hd};
    }

    // Inner reflective glass skin behind the lattice
    for (int i = 0; i < trackSegs; ++i) {
        int next = (i + 1) % trackSegs;
        Vec3 norm = ((perimBot[i] + perimBot[next]) * 0.5f).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices,
            perimBot[i], perimBot[next], perimTop[next], perimTop[i],
            norm, glassMat);
    }

    // Diagrid interlocking lattice steel columns (Bird's Nest weave)
    for (int i = 0; i < trackSegs; ++i) {
        int nextFwd = (i + 4) % trackSegs;
        int nextBack = (i - 4 + trackSegs) % trackSegs;

        // Diagonal forward strut
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            perimBot[i] + perimBot[i].normalized() * 0.2f,
            perimTop[nextFwd] + perimTop[nextFwd].normalized() * 0.2f,
            0.22f, 0.18f, 6, silverMat, true, true);

        // Diagonal backward crossing strut
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            perimBot[i] + perimBot[i].normalized() * 0.2f,
            perimTop[nextBack] + perimTop[nextBack].normalized() * 0.2f,
            0.22f, 0.18f, 6, silverMat, true, true);

        // Vertical perimeter rib
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            perimBot[i], perimTop[i],
            0.26f, 0.22f, 8, frameMat, true, true);
    }

    // 6. Sweeping Saddle-Shaped Tensile Roof Canopy Ring
    float canopyInnerW = hw * 0.62f;
    float canopyInnerD = hd * 0.62f;

    for (int i = 0; i < trackSegs; ++i) {
        int next = (i + 1) % trackSegs;
        float a0 = (static_cast<float>(i) / trackSegs) * 2.0f * kPi;
        float a1 = (static_cast<float>(next) / trackSegs) * 2.0f * kPi;

        Vec3 in0{std::cos(a0) * canopyInnerW, perimTop[i].y * 0.95f, std::sin(a0) * canopyInnerD};
        Vec3 in1{std::cos(a1) * canopyInnerW, perimTop[next].y * 0.95f, std::sin(a1) * canopyInnerD};

        Vec3 norm = ((in1 - in0).cross(perimTop[i] - in0)).normalized();
        BuildingComponents::appendQuad(outVerts, outIndices, in0, in1, perimTop[next], perimTop[i], norm, roofMat);

        // Cantilever structural tension cable
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices, perimTop[i], in0, 0.08f, 0.06f, 6, silverMat, true, true);
    }

    // 7. Monumental Olympic Flame Cauldron Tower Spire at East End (+X)
    Vec3 cauldronPos{hw + 3.5f, 0.0f, 0.0f};
    float cauldronH = h * 1.6f;

    // Tapered sculptural pylon
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        cauldronPos, cauldronPos + Vec3{0, cauldronH, 0},
        1.2f, 0.45f, 12, silverMat, true, true);

    // Cauldron bowl
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        cauldronPos + Vec3{0, cauldronH, 0}, cauldronPos + Vec3{0, cauldronH + 1.2f, 0},
        0.55f, 1.8f, 16, frameMat, true, true);

    // Glowing Olympic Flame
    BuildingComponents::appendBox(outVerts, outIndices,
        cauldronPos + Vec3{-0.6f, cauldronH + 1.2f, -0.6f},
        cauldronPos + Vec3{ 0.6f, cauldronH + 2.8f,  0.6f},
        flameMat);

    // 8. Grand Entrance Stairs & Walkway Ramp (West End -X)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.5f, 0.0f, -6.0f},
        Vec3{-hw - 0.2f, 2.4f,  6.0f},
        foundMat
    );
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 8.5f, 0.0f, -4.5f},
        Vec3{-hw - 6.5f, 1.2f,  4.5f},
        foundMat
    );
}

} // namespace procedural::buildings
