// ============================================================================
// FlowerGen.cpp - 10 Wildflowers 3D Mesh & Clump Synthesis Implementation
// ============================================================================

#include "FlowerGen.h"
#include <algorithm>
#include <cmath>

namespace vegetation {

namespace {
constexpr float kPi = 3.1415926535f;

inline Vec3 safeNorm(const Vec3& v, const Vec3& fallback = Vec3(0.0f, 1.0f, 0.0f)) {
    float l2 = v.lengthSq();
    if (l2 < 1e-8f) return fallback;
    return v * (1.0f / std::sqrt(l2));
}

inline void makeOrthonormalBasis(const Vec3& normalDir, Vec3& outTangent, Vec3& outBitangent) {
    Vec3 n = safeNorm(normalDir);
    Vec3 ref = (std::abs(n.y) < 0.90f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
    outTangent = safeNorm(cross(ref, n), Vec3(1.0f, 0.0f, 0.0f));
    outBitangent = safeNorm(cross(n, outTangent), Vec3(0.0f, 0.0f, 1.0f));
}
} // namespace

FlowerSynthesisParams getPredefinedFlowerParams(FlowerSpecies species) {
    FlowerSynthesisParams p;
    p.species = species;
    switch (species) {
        case FlowerSpecies::RED_POPPY:
            p.petalCount = 6;
            p.stemCount = 7;
            p.petalLengthMeters = 0.085f;
            p.petalWidthMeters = 0.072f;
            p.petalCurvature = 0.52f;
            p.stemHeightMeters = 0.62f;
            p.stemRadiusMeters = 0.006f;
            p.centerDiskRadiusMeters = 0.020f;
            p.clumpSpreadMeters = 0.58f;
            p.petalColorR = 0.92f; p.petalColorG = 0.12f; p.petalColorB = 0.10f;
            p.centerColorR = 0.14f; p.centerColorG = 0.13f; p.centerColorB = 0.16f;
            break;
        case FlowerSpecies::OXEYE_DAISY:
            p.petalCount = 21;
            p.stemCount = 9;
            p.petalLengthMeters = 0.075f;
            p.petalWidthMeters = 0.022f;
            p.petalCurvature = 0.18f;
            p.stemHeightMeters = 0.58f;
            p.stemRadiusMeters = 0.006f;
            p.centerDiskRadiusMeters = 0.024f;
            p.clumpSpreadMeters = 0.60f;
            p.petalColorR = 0.98f; p.petalColorG = 0.98f; p.petalColorB = 0.94f;
            p.centerColorR = 0.94f; p.centerColorG = 0.76f; p.centerColorB = 0.12f;
            break;
        case FlowerSpecies::ENGLISH_LAVENDER:
            p.petalCount = 16;
            p.stemCount = 14;
            p.petalLengthMeters = 0.032f;
            p.petalWidthMeters = 0.016f;
            p.petalCurvature = 0.15f;
            p.stemHeightMeters = 0.68f;
            p.stemRadiusMeters = 0.0045f;
            p.centerDiskRadiusMeters = 0.016f;
            p.clumpSpreadMeters = 0.65f;
            p.petalColorR = 0.58f; p.petalColorG = 0.36f; p.petalColorB = 0.88f;
            p.centerColorR = 0.42f; p.centerColorG = 0.24f; p.centerColorB = 0.72f;
            break;
        case FlowerSpecies::COMMON_SUNFLOWER:
            p.petalCount = 30;
            p.stemCount = 4;
            p.petalLengthMeters = 0.165f;
            p.petalWidthMeters = 0.044f;
            p.petalCurvature = 0.22f;
            p.stemHeightMeters = 1.65f;
            p.stemRadiusMeters = 0.018f;
            p.centerDiskRadiusMeters = 0.095f;
            p.clumpSpreadMeters = 0.72f;
            p.petalColorR = 0.98f; p.petalColorG = 0.80f; p.petalColorB = 0.06f;
            p.centerColorR = 0.34f; p.centerColorG = 0.20f; p.centerColorB = 0.10f;
            break;
        case FlowerSpecies::MOUNTAIN_BLUEBELL:
            p.petalCount = 5;
            p.stemCount = 10;
            p.petalLengthMeters = 0.055f;
            p.petalWidthMeters = 0.036f;
            p.petalCurvature = 0.65f;
            p.stemHeightMeters = 0.48f;
            p.stemRadiusMeters = 0.0045f;
            p.centerDiskRadiusMeters = 0.014f;
            p.clumpSpreadMeters = 0.52f;
            p.petalColorR = 0.24f; p.petalColorG = 0.48f; p.petalColorB = 0.96f;
            p.centerColorR = 0.86f; p.centerColorG = 0.92f; p.centerColorB = 0.98f;
            break;
        case FlowerSpecies::AFRICAN_MARIGOLD:
            p.petalCount = 28;
            p.stemCount = 7;
            p.petalLengthMeters = 0.072f;
            p.petalWidthMeters = 0.036f;
            p.petalCurvature = 0.55f;
            p.stemHeightMeters = 0.56f;
            p.stemRadiusMeters = 0.008f;
            p.centerDiskRadiusMeters = 0.020f;
            p.clumpSpreadMeters = 0.56f;
            p.petalColorR = 0.98f; p.petalColorG = 0.56f; p.petalColorB = 0.06f;
            p.centerColorR = 0.90f; p.centerColorG = 0.36f; p.centerColorB = 0.04f;
            break;
        case FlowerSpecies::PURPLE_CONEFLOWER:
            p.petalCount = 18;
            p.stemCount = 8;
            p.petalLengthMeters = 0.105f;
            p.petalWidthMeters = 0.026f;
            p.petalCurvature = -0.38f; // Reflexed drooping petals
            p.stemHeightMeters = 0.85f;
            p.stemRadiusMeters = 0.007f;
            p.centerDiskRadiusMeters = 0.032f;
            p.clumpSpreadMeters = 0.62f;
            p.petalColorR = 0.86f; p.petalColorG = 0.26f; p.petalColorB = 0.66f;
            p.centerColorR = 0.72f; p.centerColorG = 0.30f; p.centerColorB = 0.08f;
            break;
        case FlowerSpecies::KENYA_PYRETHRUM:
            p.petalCount = 22;
            p.stemCount = 11;
            p.petalLengthMeters = 0.070f;
            p.petalWidthMeters = 0.020f;
            p.petalCurvature = 0.14f;
            p.stemHeightMeters = 0.65f;
            p.stemRadiusMeters = 0.0055f;
            p.centerDiskRadiusMeters = 0.024f;
            p.clumpSpreadMeters = 0.64f;
            p.petalColorR = 0.99f; p.petalColorG = 0.98f; p.petalColorB = 0.96f;
            p.centerColorR = 0.96f; p.centerColorG = 0.82f; p.centerColorB = 0.10f;
            break;
        case FlowerSpecies::BIRD_OF_PARADISE_WILD:
            p.petalCount = 6;
            p.stemCount = 5;
            p.petalLengthMeters = 0.140f;
            p.petalWidthMeters = 0.034f;
            p.petalCurvature = 0.30f;
            p.stemHeightMeters = 1.20f;
            p.stemRadiusMeters = 0.014f;
            p.centerDiskRadiusMeters = 0.028f;
            p.clumpSpreadMeters = 0.68f;
            p.petalColorR = 0.98f; p.petalColorG = 0.42f; p.petalColorB = 0.06f;
            p.centerColorR = 0.14f; p.centerColorG = 0.30f; p.centerColorB = 0.92f;
            break;
        case FlowerSpecies::FOXGLOVE_SPIRE:
            p.petalCount = 14;
            p.stemCount = 5;
            p.petalLengthMeters = 0.060f;
            p.petalWidthMeters = 0.034f;
            p.petalCurvature = 0.58f;
            p.stemHeightMeters = 1.25f;
            p.stemRadiusMeters = 0.010f;
            p.centerDiskRadiusMeters = 0.018f;
            p.clumpSpreadMeters = 0.56f;
            p.petalColorR = 0.88f; p.petalColorG = 0.34f; p.petalColorB = 0.64f;
            p.centerColorR = 0.56f; p.centerColorG = 0.12f; p.centerColorB = 0.36f;
            break;
    }
    return p;
}

FlowerMeshData FlowerGen::generateFlower(const FlowerSynthesisParams& params, uint64_t seed) {
    FastRng rng(seed);
    FlowerMeshData mesh;

    // 1. Stem vertical cylinder spline
    constexpr int STEM_SEGS = 4;
    Vec3 stemBase(0, 0, 0);
    Vec3 stemHead(0, params.stemHeightMeters, 0);

    for (int i = 0; i < STEM_SEGS; ++i) {
        float t = static_cast<float>(i) / (STEM_SEGS - 1);
        Vec3 p = lerp(stemBase, stemHead, t);
        mesh.vertices.push_back({p.toC(), {0, 1, 0}, {0, t}, {0.25f, 0.55f, 0.18f, 1.0f}, t * 0.4f});
    }

    // 2. Central Floral Receptacle Disk
    uint32_t centerIdx = static_cast<uint32_t>(mesh.vertices.size());
    mesh.vertices.push_back({stemHead.toC(), {0, 1, 0}, {0.5f, 0.5f}, {params.centerColorR, params.centerColorG, params.centerColorB, 1.0f}, 0.5f});

    constexpr int DISK_SEGS = 8;
    for (int i = 0; i < DISK_SEGS; ++i) {
        float angle = static_cast<float>(i) * (2.0f * kPi / DISK_SEGS);
        Vec3 ringP = stemHead + Vec3(std::cos(angle) * params.centerDiskRadiusMeters, 0.005f, std::sin(angle) * params.centerDiskRadiusMeters);
        mesh.vertices.push_back({ringP.toC(), {0, 1, 0}, {0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)}, {params.centerColorR, params.centerColorG, params.centerColorB, 1.0f}, 0.5f});

        uint32_t curr = centerIdx + 1 + i;
        uint32_t next = centerIdx + 1 + ((i + 1) % DISK_SEGS);
        mesh.indices.push_back(centerIdx);
        mesh.indices.push_back(curr);
        mesh.indices.push_back(next);
    }

    // 3. Petals arranged radially
    int pCount = std::max(3, params.petalCount);
    for (int p = 0; p < pCount; ++p) {
        float theta = static_cast<float>(p) * (2.0f * kPi / pCount);
        float cosT = std::cos(theta);
        float sinT = std::sin(theta);
        Vec3 outDir(cosT, 0.0f, sinT);
        Vec3 sideDir(-sinT, 0.0f, cosT);

        Vec3 root = stemHead + outDir * params.centerDiskRadiusMeters;
        Vec3 tip = root + outDir * params.petalLengthMeters + Vec3(0, params.petalCurvature * params.petalLengthMeters, 0);
        Vec3 midLeft = root + outDir * (params.petalLengthMeters * 0.5f) - sideDir * (params.petalWidthMeters * 0.5f);
        Vec3 midRight = root + outDir * (params.petalLengthMeters * 0.5f) + sideDir * (params.petalWidthMeters * 0.5f);

        uint32_t pIdx = static_cast<uint32_t>(mesh.vertices.size());
        Vec3 normal(0, 1, 0);

        float r = params.petalColorR * rng.nextRange(0.92f, 1.05f);
        float g = params.petalColorG * rng.nextRange(0.92f, 1.05f);
        float b = params.petalColorB * rng.nextRange(0.92f, 1.05f);

        mesh.vertices.push_back({root.toC(), normal.toC(), {0.5f, 0.0f}, {r, g, b, 1.0f}, 0.5f});
        mesh.vertices.push_back({midLeft.toC(), normal.toC(), {0.0f, 0.5f}, {r, g, b, 1.0f}, 0.6f});
        mesh.vertices.push_back({tip.toC(), normal.toC(), {0.5f, 1.0f}, {r, g, b, 1.0f}, 0.7f});
        mesh.vertices.push_back({midRight.toC(), normal.toC(), {1.0f, 0.5f}, {r, g, b, 1.0f}, 0.6f});

        mesh.indices.push_back(pIdx + 0);
        mesh.indices.push_back(pIdx + 1);
        mesh.indices.push_back(pIdx + 2);

        mesh.indices.push_back(pIdx + 0);
        mesh.indices.push_back(pIdx + 2);
        mesh.indices.push_back(pIdx + 3);
    }

    return mesh;
}

FlowerClusterResult FlowerGen::synthesizeFlowerCluster(const FlowerSynthesisParams& params, uint64_t seed) {
    FastRng rng(seed);
    FlowerClusterResult res;

    const int stemTotal = std::clamp(params.stemCount, 1, 20);
    const float spread = std::clamp(params.clumpSpreadMeters, 0.15f, 2.2f);
    const float baseHeight = std::clamp(params.stemHeightMeters, 0.20f, 2.8f);
    const float basePetalLen = std::clamp(params.petalLengthMeters, 0.02f, 0.35f);
    const float basePetalW = std::clamp(params.petalWidthMeters, 0.01f, 0.16f);
    const float baseDiskR = std::clamp(params.centerDiskRadiusMeters, 0.008f, 0.18f);
    const float stemBaseR = std::clamp(params.stemRadiusMeters, 0.003f, 0.032f);
    const int pCount = std::clamp(params.petalCount, 3, 42);

    // Helper to add a radial 3D corolla head (petals + disc/cone)
    auto emitRadialCorolla = [&](const Vec3& headPos, const Vec3& faceNormal, float scale, int ringCount, float cupTilt, int styleKind, float windW) {
        Vec3 tang, bitang;
        makeOrthonormalBasis(faceNormal, tang, bitang);
        Vec3 upN = safeNorm(faceNormal);

        float dR = baseDiskR * scale;
        FlowerDiskHead3D disk;
        disk.centerPos = headPos;
        disk.normalDir = upN;
        disk.radiusMeters = dR;
        disk.coneHeightMeters = (styleKind == 1) ? (dR * 1.45f) : (dR * 0.48f);
        disk.styleKind = styleKind;
        disk.colorR = params.centerColorR;
        disk.colorG = params.centerColorG;
        disk.colorB = params.centerColorB;
        disk.windWeight = windW;
        res.diskHeads.push_back(disk);
        res.totalBlossomHeads += 1;

        for (int ring = 0; ring < ringCount; ++ring) {
            float ringPhase = ring * (kPi / std::max(1, pCount)) + rng.nextRange(-0.08f, 0.08f);
            float ringLenScale = 1.0f - ring * 0.22f;
            float ringTilt = cupTilt + ring * 0.28f;
            int petalsInRing = (ring == 0) ? pCount : std::max(6, (pCount * 3) / 4);

            for (int p = 0; p < petalsInRing; ++p) {
                float theta = static_cast<float>(p) * (2.0f * kPi / static_cast<float>(petalsInRing)) + ringPhase;
                float c = std::cos(theta);
                float s = std::sin(theta);
                Vec3 outDir = safeNorm(tang * c + bitang * s);
                Vec3 sideDir = safeNorm(bitang * c - tang * s);

                float pLen = basePetalLen * scale * ringLenScale * rng.nextRange(0.92f, 1.08f);
                float pWid = basePetalW * scale * (0.95f - ring * 0.12f);

                Vec3 root = headPos + outDir * (dR * 0.78f) + upN * (ring * 0.006f * scale);
                Vec3 midBase = root + outDir * (pLen * 0.52f) + upN * (pLen * ringTilt * 0.45f);
                Vec3 tip = root + outDir * pLen + upN * (pLen * ringTilt);
                Vec3 midL = midBase - sideDir * (pWid * 0.52f);
                Vec3 midR = midBase + sideDir * (pWid * 0.52f);

                float tint = rng.nextRange(0.88f, 1.08f);
                FlowerPetalQuad3D q;
                q.p0 = root;
                q.p1 = midL;
                q.p2 = tip;
                q.p3 = midR;
                q.colorR = std::clamp(params.petalColorR * tint, 0.0f, 1.0f);
                q.colorG = std::clamp(params.petalColorG * tint, 0.0f, 1.0f);
                q.colorB = std::clamp(params.petalColorB * tint, 0.0f, 1.0f);
                q.shade = 0.85f + 0.15f * std::max(0.0f, upN.y);
                q.windWeight = windW;
                res.petalQuads.push_back(q);
            }
        }
    };

    // Helper to add a 3D nodding bell floret (Bluebell / Foxglove)
    auto emitBellCorolla = [&](const Vec3& attachPos, const Vec3& bellAxis, float scale, float windW) {
        Vec3 axis = safeNorm(bellAxis, Vec3(0.0f, -1.0f, 0.0f));
        Vec3 tang, bitang;
        makeOrthonormalBasis(axis, tang, bitang);

        float len = basePetalLen * scale;
        float flare = basePetalW * scale * 0.85f;
        int lobes = 5;
        for (int k = 0; k < lobes; ++k) {
            float a0 = static_cast<float>(k) * (2.0f * kPi / lobes);
            float a1 = a0 + (2.0f * kPi / lobes) * 0.88f;
            float am = 0.5f * (a0 + a1);
            Vec3 r0 = tang * std::cos(a0) + bitang * std::sin(a0);
            Vec3 r1 = tang * std::cos(a1) + bitang * std::sin(a1);
            Vec3 rm = tang * std::cos(am) + bitang * std::sin(am);

            FlowerPetalQuad3D q;
            q.p0 = attachPos + rm * (flare * 0.18f);
            q.p1 = attachPos + axis * (len * 0.65f) + r0 * (flare * 0.65f);
            q.p2 = attachPos + axis * len + rm * flare;
            q.p3 = attachPos + axis * (len * 0.65f) + r1 * (flare * 0.65f);
            float tint = rng.nextRange(0.90f, 1.06f);
            q.colorR = std::clamp(params.petalColorR * tint, 0.0f, 1.0f);
            q.colorG = std::clamp(params.petalColorG * tint, 0.0f, 1.0f);
            q.colorB = std::clamp(params.petalColorB * tint, 0.0f, 1.0f);
            q.shade = 0.82f + 0.18f * (k % 2);
            q.windWeight = windW;
            res.petalQuads.push_back(q);
        }

        FlowerDiskHead3D throat;
        throat.centerPos = attachPos + axis * (len * 0.45f);
        throat.normalDir = axis;
        throat.radiusMeters = flare * 0.35f;
        throat.coneHeightMeters = len * 0.35f;
        throat.styleKind = 3;
        throat.colorR = params.centerColorR;
        throat.colorG = params.centerColorG;
        throat.colorB = params.centerColorB;
        throat.windWeight = windW;
        res.diskHeads.push_back(throat);
        res.totalBlossomHeads += 1;
    };

    for (int st = 0; st < stemTotal; ++st) {
        // Golden-angle spiral distribution within meadow clump
        float rNorm = (stemTotal == 1) ? 0.0f : std::sqrt((static_cast<float>(st) + 0.35f) / static_cast<float>(stemTotal));
        float az = static_cast<float>(st) * 2.399963f + rng.nextRange(-0.22f, 0.22f);
        float baseR = rNorm * spread * 0.38f;
        Vec3 currPos(std::cos(az) * baseR, 0.0f, std::sin(az) * baseR);

        float stemH = baseHeight * (1.0f - 0.18f * rNorm) * rng.nextRange(0.88f, 1.12f);
        float leanOut = (0.14f + 0.36f * rNorm) * spread / std::max(0.4f, stemH);
        Vec3 leanDir(std::cos(az) * leanOut, 1.0f, std::sin(az) * leanOut);
        leanDir = safeNorm(leanDir);

        constexpr int SEGS = 5;
        float segLen = stemH / static_cast<float>(SEGS);
        float currRad = stemBaseR * rng.nextRange(0.88f, 1.10f);

        Vec3 prevPos = currPos;
        Vec3 upperStemPts[SEGS + 1];
        upperStemPts[0] = currPos;

        for (int s = 0; s < SEGS; ++s) {
            float t0 = static_cast<float>(s) / static_cast<float>(SEGS);
            float t1 = static_cast<float>(s + 1) / static_cast<float>(SEGS);
            float arch = (params.species == FlowerSpecies::MOUNTAIN_BLUEBELL && s >= 3) ? 0.45f : 0.12f;
            Vec3 segDir = safeNorm(Vec3(
                leanDir.x * (1.0f + arch * t1),
                leanDir.y * ((params.species == FlowerSpecies::MOUNTAIN_BLUEBELL && s == SEGS - 1) ? 0.25f : 1.0f),
                leanDir.z * (1.0f + arch * t1)
            ));
            Vec3 nextPos = prevPos + segDir * segLen;
            float nextRad = std::max(0.002f, currRad * 0.85f);

            FlowerStemSegment3D seg;
            seg.startPos = prevPos;
            seg.endPos = nextPos;
            seg.radiusBottom = currRad;
            seg.radiusTop = nextRad;
            seg.kind = 0;
            seg.windWeight = params.windFlexibility * (t1 * t1);
            res.stems.push_back(seg);

            // Basal & cauline stem leaves
            if (params.showLeaves && (s == 0 || s == 1 || (s == 2 && params.species != FlowerSpecies::ENGLISH_LAVENDER))) {
                int leavesAtNode = (s == 0) ? 3 : 2;
                for (int lf = 0; lf < leavesAtNode; ++lf) {
                    float lAz = az + static_cast<float>(s * 2 + lf) * 2.15f + rng.nextRange(-0.25f, 0.25f);
                    float pitch = (s == 0) ? 0.38f : 0.28f;
                    Vec3 midDir = safeNorm(Vec3(std::cos(lAz), pitch, std::sin(lAz)));
                    Vec3 wingDir = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), midDir), Vec3(1.0f, 0.0f, 0.0f));
                    float leafScaleBase = std::clamp(stemH * 0.28f, 0.08f, 0.34f);

                    FlowerLeaf3D leaf;
                    leaf.attachPos = lerp(prevPos, nextPos, 0.35f + 0.25f * lf);
                    leaf.midribDir = midDir;
                    leaf.wingDir = wingDir;
                    leaf.lengthMeters = (params.species == FlowerSpecies::BIRD_OF_PARADISE_WILD)
                        ? leafScaleBase * 1.85f
                        : (params.species == FlowerSpecies::ENGLISH_LAVENDER ? leafScaleBase * 0.55f : leafScaleBase);
                    leaf.widthMeters = (params.species == FlowerSpecies::COMMON_SUNFLOWER || params.species == FlowerSpecies::BIRD_OF_PARADISE_WILD)
                        ? leaf.lengthMeters * 0.52f
                        : (params.species == FlowerSpecies::ENGLISH_LAVENDER ? leaf.lengthMeters * 0.16f : leaf.lengthMeters * 0.34f);
                    leaf.tint = (s == 0) ? 0.25f : 0.72f;
                    leaf.windWeight = params.windFlexibility * (t0 * 0.8f);
                    res.leaves.push_back(leaf);
                }
            }

            prevPos = nextPos;
            currRad = nextRad;
            upperStemPts[s + 1] = nextPos;
        }

        Vec3 apex = prevPos;
        Vec3 apexDir = safeNorm(apex - upperStemPts[SEGS - 1]);
        float apexWind = params.windFlexibility;

        switch (params.species) {
            case FlowerSpecies::COMMON_SUNFLOWER: {
                // Heliotropic tilt toward warm morning sun (+X/+Z/+Y)
                Vec3 sunFace = safeNorm(Vec3(0.38f, 0.85f, 0.36f) + apexDir * 0.25f);
                emitRadialCorolla(apex, sunFace, 1.0f, 1, 0.12f, 0, apexWind);
                break;
            }
            case FlowerSpecies::PURPLE_CONEFLOWER: {
                // Swept-back reflexed petals drooping below a tall bronze-orange cone
                Vec3 face = safeNorm(apexDir + Vec3(0.0f, 0.35f, 0.0f));
                emitRadialCorolla(apex, face, 1.0f, 1, -0.36f, 1, apexWind);
                break;
            }
            case FlowerSpecies::RED_POPPY: {
                // Silky deeply-cupped petals around dark coal stamen boss
                Vec3 face = safeNorm(apexDir + Vec3(0.0f, 0.45f, 0.0f));
                emitRadialCorolla(apex, face, 1.0f, 1, 0.56f, 2, apexWind);
                break;
            }
            case FlowerSpecies::AFRICAN_MARIGOLD: {
                // Multi-tiered ruffled pompon layers
                Vec3 face = safeNorm(apexDir + Vec3(0.0f, 0.40f, 0.0f));
                emitRadialCorolla(apex, face, 1.0f, 3, 0.38f, 0, apexWind);
                break;
            }
            case FlowerSpecies::OXEYE_DAISY:
            case FlowerSpecies::KENYA_PYRETHRUM: {
                Vec3 face = safeNorm(apexDir + Vec3(0.0f, 0.45f, 0.0f));
                emitRadialCorolla(apex, face, 1.0f, 1, 0.14f, 0, apexWind);
                break;
            }
            case FlowerSpecies::ENGLISH_LAVENDER: {
                // Dense terminal violet flower spike across upper stem segment
                Vec3 spikeStart = lerp(upperStemPts[SEGS - 1], apex, 0.15f);
                Vec3 spikeEnd = apex + apexDir * (basePetalLen * 1.8f);
                FlowerStemSegment3D spikeAxis;
                spikeAxis.startPos = upperStemPts[SEGS - 1];
                spikeAxis.endPos = spikeEnd;
                spikeAxis.radiusBottom = currRad;
                spikeAxis.radiusTop = 0.002f;
                spikeAxis.kind = 1;
                spikeAxis.windWeight = apexWind;
                res.stems.push_back(spikeAxis);

                int whorls = 7;
                for (int w = 0; w < whorls; ++w) {
                    float tw = static_cast<float>(w) / static_cast<float>(whorls - 1);
                    Vec3 wPos = lerp(spikeStart, spikeEnd, tw);
                    float wScale = 1.0f - 0.35f * tw;
                    for (int f = 0; f < 3; ++f) {
                        float fa = az + w * 1.4f + f * (2.0f * kPi / 3.0f);
                        Vec3 outD = safeNorm(Vec3(std::cos(fa), 0.35f, std::sin(fa)));
                        Vec3 sideD = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), outD));

                        FlowerPetalQuad3D pq;
                        pq.p0 = wPos;
                        pq.p1 = wPos + outD * (basePetalLen * 0.7f * wScale) - sideD * (basePetalW * 0.6f * wScale);
                        pq.p2 = wPos + outD * (basePetalLen * 1.35f * wScale) + Vec3(0.0f, 0.008f, 0.0f);
                        pq.p3 = wPos + outD * (basePetalLen * 0.7f * wScale) + sideD * (basePetalW * 0.6f * wScale);
                        pq.colorR = params.petalColorR * rng.nextRange(0.90f, 1.08f);
                        pq.colorG = params.petalColorG * rng.nextRange(0.90f, 1.08f);
                        pq.colorB = params.petalColorB * rng.nextRange(0.90f, 1.08f);
                        pq.shade = 0.84f + 0.16f * tw;
                        pq.windWeight = apexWind;
                        res.petalQuads.push_back(pq);
                    }
                    FlowerDiskHead3D bud;
                    bud.centerPos = wPos;
                    bud.normalDir = apexDir;
                    bud.radiusMeters = baseDiskR * 0.75f * wScale;
                    bud.coneHeightMeters = baseDiskR * 0.6f;
                    bud.styleKind = 3;
                    bud.colorR = params.centerColorR;
                    bud.colorG = params.centerColorG;
                    bud.colorB = params.centerColorB;
                    bud.windWeight = apexWind;
                    res.diskHeads.push_back(bud);
                }
                res.totalBlossomHeads += 1;
                break;
            }
            case FlowerSpecies::MOUNTAIN_BLUEBELL: {
                // Arching pedicels bearing 2-3 pendulous bluebell corollas
                for (int b = 0; b < 3; ++b) {
                    float tb = 0.25f + b * 0.35f;
                    Vec3 node = lerp(upperStemPts[SEGS - 1], apex, tb);
                    float pa = az + b * 1.35f;
                    Vec3 pedEnd = node + Vec3(std::cos(pa) * 0.055f, -0.025f, std::sin(pa) * 0.055f);

                    FlowerStemSegment3D ped;
                    ped.startPos = node;
                    ped.endPos = pedEnd;
                    ped.radiusBottom = 0.0022f;
                    ped.radiusTop = 0.0015f;
                    ped.kind = 1;
                    ped.windWeight = apexWind;
                    res.stems.push_back(ped);

                    emitBellCorolla(pedEnd, Vec3(std::cos(pa) * 0.22f, -1.0f, std::sin(pa) * 0.22f), 0.95f - b * 0.10f, apexWind);
                }
                break;
            }
            case FlowerSpecies::FOXGLOVE_SPIRE: {
                // Tall vertical raceme of nodding speckled tubular bells
                Vec3 racemeStart = lerp(upperStemPts[2], apex, 0.2f);
                int bells = 8;
                for (int b = 0; b < bells; ++b) {
                    float tb = static_cast<float>(b) / static_cast<float>(bells - 1);
                    Vec3 node = lerp(racemeStart, apex, tb);
                    float fa = az + std::sin(b * 0.9f) * 0.85f;
                    Vec3 outPed = node + Vec3(std::cos(fa) * 0.032f, 0.008f, std::sin(fa) * 0.032f);

                    FlowerStemSegment3D ped;
                    ped.startPos = node;
                    ped.endPos = outPed;
                    ped.radiusBottom = 0.003f;
                    ped.radiusTop = 0.002f;
                    ped.kind = 1;
                    ped.windWeight = params.windFlexibility * (0.5f + 0.5f * tb);
                    res.stems.push_back(ped);

                    Vec3 bellDir(std::cos(fa) * 0.55f, -0.78f, std::sin(fa) * 0.55f);
                    emitBellCorolla(outPed, bellDir, 1.0f - 0.35f * tb, ped.windWeight);
                }
                break;
            }
            case FlowerSpecies::BIRD_OF_PARADISE_WILD: {
                // Horizontal canoe spathe bract + ascending flaming orange-and-cobalt crest
                Vec3 spatheDir = safeNorm(Vec3(std::cos(az), 0.16f, std::sin(az)));
                Vec3 spatheTip = apex + spatheDir * (basePetalLen * 1.35f);
                FlowerStemSegment3D spathe;
                spathe.startPos = apex;
                spathe.endPos = spatheTip;
                spathe.radiusBottom = currRad * 1.45f;
                spathe.radiusTop = 0.004f;
                spathe.kind = 2; // Boat bract spathe
                spathe.windWeight = apexWind;
                res.stems.push_back(spathe);

                Vec3 sideDir = safeNorm(cross(Vec3(0.0f, 1.0f, 0.0f), spatheDir));
                // 3-4 flaming orange sepals bursting vertically from the boat bract
                for (int k = 0; k < 4; ++k) {
                    float tk = 0.18f + k * 0.18f;
                    Vec3 root = lerp(apex, spatheTip, tk);
                    Vec3 tip = root + Vec3(0.0f, basePetalLen * (0.95f - k * 0.08f), 0.0f) + spatheDir * (basePetalLen * 0.28f);
                    FlowerPetalQuad3D sep;
                    sep.p0 = root;
                    sep.p1 = lerp(root, tip, 0.45f) - sideDir * (basePetalW * 0.42f);
                    sep.p2 = tip;
                    sep.p3 = lerp(root, tip, 0.45f) + sideDir * (basePetalW * 0.42f);
                    sep.colorR = params.petalColorR;
                    sep.colorG = params.petalColorG;
                    sep.colorB = params.petalColorB;
                    sep.shade = 0.96f;
                    sep.windWeight = apexWind;
                    res.petalQuads.push_back(sep);
                }
                // Cobalt-blue arrow petal crest
                Vec3 blueRoot = lerp(apex, spatheTip, 0.48f);
                Vec3 blueTip = blueRoot + Vec3(0.0f, basePetalLen * 0.62f, 0.0f) + spatheDir * (basePetalLen * 0.45f);
                FlowerPetalQuad3D blueCrest;
                blueCrest.p0 = blueRoot;
                blueCrest.p1 = lerp(blueRoot, blueTip, 0.5f) - sideDir * (basePetalW * 0.35f);
                blueCrest.p2 = blueTip;
                blueCrest.p3 = lerp(blueRoot, blueTip, 0.5f) + sideDir * (basePetalW * 0.35f);
                blueCrest.colorR = params.centerColorR;
                blueCrest.colorG = params.centerColorG;
                blueCrest.colorB = params.centerColorB;
                blueCrest.shade = 1.0f;
                blueCrest.windWeight = apexWind;
                res.petalQuads.push_back(blueCrest);

                FlowerDiskHead3D crestBoss;
                crestBoss.centerPos = lerp(apex, spatheTip, 0.42f);
                crestBoss.normalDir = Vec3(0.0f, 1.0f, 0.0f);
                crestBoss.radiusMeters = baseDiskR * 0.70f;
                crestBoss.coneHeightMeters = baseDiskR * 0.4f;
                crestBoss.styleKind = 4;
                crestBoss.colorR = params.centerColorR;
                crestBoss.colorG = params.centerColorG;
                crestBoss.colorB = params.centerColorB;
                crestBoss.windWeight = apexWind;
                res.diskHeads.push_back(crestBoss);
                res.totalBlossomHeads += 1;
                break;
            }
        }
    }

    return res;
}

} // namespace vegetation
