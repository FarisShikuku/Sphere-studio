#include "DesertCactusGenerator.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace desert {

namespace {

constexpr float PI_F = 3.14159265358979323846f;
constexpr float GOLDEN_ANGLE_F = 2.39996322972865332f;

struct FastRng {
    uint64_t state;
    explicit FastRng(uint64_t seed) : state(seed ? seed : 0x9E3779B97F4A7C15ULL) {}
    uint32_t nextU32() {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        return static_cast<uint32_t>((state >> 32) ^ state);
    }
    float next01() {
        return static_cast<float>(nextU32() & 0xFFFFFFu) / static_cast<float>(0x1000000u);
    }
    float range(float lo, float hi) {
        return lo + (hi - lo) * next01();
    }
};

Vec3 safeNorm(const Vec3& v, const Vec3& fallback = Vec3(0.0f, 1.0f, 0.0f)) {
    float lsq = v.lengthSq();
    if (lsq < 1e-10f) return fallback;
    return v * (1.0f / std::sqrt(lsq));
}

void orthoBasis(const Vec3& dir, Vec3& outRight, Vec3& outUp) {
    Vec3 d = safeNorm(dir);
    Vec3 ref = (std::fabs(d.y) < 0.92f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
    outRight = safeNorm(cross(ref, d), Vec3(1.0f, 0.0f, 0.0f));
    outUp = safeNorm(cross(d, outRight), Vec3(0.0f, 0.0f, 1.0f));
}

Vec3 cubicBezier(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t) {
    float u = 1.0f - t;
    float u2 = u * u;
    float t2 = t * t;
    return p0 * (u2 * u) + p1 * (3.0f * u2 * t) + p2 * (3.0f * u * t2) + p3 * (t2 * t);
}

} // namespace

CactusProceduralMesh DesertCactusGenerator::generateCactusMesh(const DesertSpeciesParams& params) {
    CactusProceduralMesh mesh;
    const int rings = 28;
    const int segments = std::max(params.stemRibCount * 4, 32);
    const float height = params.heightMeters;
    const float baseR = params.baseRadiusMeters;
    const int ribs = params.stemRibCount;
    const float fluteAmp = params.pleatingDepth;

    mesh.vertices.reserve((rings + 1) * (segments + 1));
    mesh.indices.reserve(rings * segments * 6);

    uint8_t cr = static_cast<uint8_t>(std::clamp(params.trunkGreenR * 255.0f, 0.0f, 255.0f));
    uint8_t cg = static_cast<uint8_t>(std::clamp(params.trunkGreenG * 255.0f, 0.0f, 255.0f));
    uint8_t cb = static_cast<uint8_t>(std::clamp(params.trunkGreenB * 255.0f, 0.0f, 255.0f));
    uint32_t baseColor = (0xFF << 24) | (cr << 16) | (cg << 8) | cb;

    for (int r = 0; r <= rings; ++r) {
        float v = static_cast<float>(r) / static_cast<float>(rings);
        float y = v * height;

        float profile = 1.0f;
        if (v > 0.85f) {
            float domeT = (v - 0.85f) / 0.15f;
            profile = std::sqrt(std::max(0.0f, 1.0f - domeT * domeT));
        } else if (v < 0.10f) {
            profile = 0.85f + 0.15f * (v / 0.10f);
        }

        float curR = baseR * profile;
        for (int s = 0; s <= segments; ++s) {
            float u = static_cast<float>(s) / static_cast<float>(segments);
            float theta = u * 2.0f * PI_F;
            float flute = fluteAmp * std::cos(ribs * theta);
            float rTheta = curR * (1.0f + flute);

            float px = rTheta * std::cos(theta);
            float pz = rTheta * std::sin(theta);

            Vec3 norm = safeNorm(Vec3(std::cos(theta), (v > 0.85f) ? (v - 0.85f) * 4.0f : 0.05f, std::sin(theta)));
            mesh.vertices.push_back({Vec3(px, y, pz), norm, u, v, baseColor});
        }
    }

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < segments; ++s) {
            uint32_t i0 = r * (segments + 1) + s;
            uint32_t i1 = (r + 1) * (segments + 1) + s;
            uint32_t i2 = (r + 1) * (segments + 1) + (s + 1);
            uint32_t i3 = r * (segments + 1) + (s + 1);
            mesh.indices.push_back(i0); mesh.indices.push_back(i1); mesh.indices.push_back(i2);
            mesh.indices.push_back(i0); mesh.indices.push_back(i2); mesh.indices.push_back(i3);
        }
    }

    mesh.areoleCount = static_cast<uint32_t>(ribs * (rings / 2));
    mesh.spineCount = mesh.areoleCount * static_cast<uint32_t>(std::max(1, params.areoleSpineDensity / 4));
    return mesh;
}

DesertSpecimenResult DesertCactusGenerator::synthesizeSpecimen(uint64_t seed) const {
    FastRng rng(seed);
    DesertSpecimenResult res;

    const float hM = std::clamp(m_params.heightMeters, 0.30f, 4.8f);
    const float radM = std::clamp(m_params.baseRadiusMeters, 0.14f, 1.8f);
    const int ribs = std::clamp(m_params.stemRibCount, 6, 24);
    const int spineDens = std::clamp(m_params.areoleSpineDensity, 10, 50);
    const float fluteAmp = std::clamp(m_params.pleatingDepth, 0.02f, 0.36f);
    const float windFlex = std::clamp(m_params.windFlexibility, 0.0f, 1.0f);
    const bool wantSpines = m_params.showSpines;
    const bool wantBlooms = m_params.showBlooms;

    res.ribCount = static_cast<uint32_t>(ribs);

    auto emitQuad = [&](const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
                        float shade, float windW, int32_t partKind) {
        Vec3 n = safeNorm(cross(p2 - p0, p1 - p0), Vec3(0.0f, 1.0f, 0.0f));
        res.quads.push_back({p0, p1, p2, p3, n, std::clamp(shade, 0.0f, 1.0f), windW, partKind});
    };

    auto emitBranch = [&](const Vec3& a, const Vec3& b, float r0, float r1, int32_t order, float windW) -> int32_t {
        int32_t idx = static_cast<int32_t>(res.branches.size());
        res.branches.push_back({a, b, r0, r1, order, windW});
        return idx;
    };

    auto emitLeaf = [&](const Vec3& pos, const Vec3& midDir, const Vec3& wingDir,
                        float len, float wid, float tint, int32_t brIdx) {
        res.leaves.push_back({pos, safeNorm(midDir), safeNorm(wingDir), len, wid, std::clamp(tint, 0.0f, 1.0f), brIdx});
    };

    auto emitSpineFan = [&](const Vec3& origin, const Vec3& surfNorm, const Vec3& tangA, const Vec3& tangB,
                            float baseLen, int countPerAreole, float curvature, float windW, int32_t style) {
        res.areoleCount++;
        if (!wantSpines) return;
        // Wool areole cushion base marker
        res.spines.push_back({origin, surfNorm, baseLen * 0.25f, 0.0f, windW, 1});
        for (int i = 0; i < countPerAreole; ++i) {
            float phi = (static_cast<float>(i) / static_cast<float>(std::max(1, countPerAreole))) * 2.0f * PI_F + rng.range(-0.15f, 0.15f);
            float spread = (i == 0 && countPerAreole >= 4) ? 0.14f : rng.range(0.55f, 1.05f);
            Vec3 d = safeNorm(surfNorm + (tangA * std::cos(phi) + tangB * std::sin(phi)) * spread);
            float sLen = baseLen * ((i == 0) ? 1.25f : rng.range(0.70f, 1.05f));
            res.spines.push_back({origin, d, sLen, (i == 0) ? curvature : curvature * 0.25f, windW, style});
        }
    };

    auto emitBloom = [&](const Vec3& pos, const Vec3& dir, float size, int32_t kind) {
        if (!wantBlooms) return;
        res.blooms.push_back({pos, safeNorm(dir), size, kind, rng.range(0.0f, 2.0f * PI_F)});
    };

    // Build parallel-transported 3D fluted tube along an arbitrary curve (2*ribs angular steps hit exact crests & valleys!)
    auto buildFlutedTube = [&](const std::vector<Vec3>& spinePts, const std::vector<float>& baseRadii,
                               int ribCount, float pleatAmp, float helicalTwist,
                               bool domeCap, bool isTubeBarrelFeltTop,
                               float spineLen, int spinesPerAreole, float spineCurvature,
                               int32_t partKind) {
        const int ringCount = static_cast<int>(spinePts.size());
        if (ringCount < 2) return;
        const int angSteps = std::clamp(ribCount, 6, 24) * 2; // even=crest, odd=valley

        std::vector<Vec3> tangents(ringCount);
        for (int r = 0; r < ringCount; ++r) {
            if (r == 0) tangents[r] = safeNorm(spinePts[1] - spinePts[0]);
            else if (r == ringCount - 1) tangents[r] = safeNorm(spinePts[r] - spinePts[r - 1]);
            else tangents[r] = safeNorm(spinePts[r + 1] - spinePts[r - 1]);
        }

        // Parallel transport Bishop frame to guarantee zero gimbal twist on curved elbows
        std::vector<Vec3> frameU(ringCount), frameV(ringCount);
        orthoBasis(tangents[0], frameU[0], frameV[0]);
        for (int r = 1; r < ringCount; ++r) {
            Vec3 prevU = frameU[r - 1];
            Vec3 t = tangents[r];
            Vec3 projU = prevU - t * dot(prevU, t);
            if (projU.lengthSq() < 1e-6f) {
                orthoBasis(t, frameU[r], frameV[r]);
            } else {
                frameU[r] = safeNorm(projU);
                frameV[r] = safeNorm(cross(t, frameU[r]));
            }
        }

        std::vector<std::vector<Vec3>> ringVerts(ringCount, std::vector<Vec3>(angSteps));
        std::vector<std::vector<Vec3>> ringNormals(ringCount, std::vector<Vec3>(angSteps));
        std::vector<float> crestShade(angSteps);

        for (int r = 0; r < ringCount; ++r) {
            float v = static_cast<float>(r) / static_cast<float>(ringCount - 1);
            float fluteFade = (domeCap && v > 0.85f) ? std::max(0.15f, 1.0f - (v - 0.85f) / 0.15f) : 1.0f;
            float baseR = baseRadii[r];

            for (int s = 0; s < angSteps; ++s) {
                float u = static_cast<float>(s) / static_cast<float>(angSteps);
                float theta = u * 2.0f * PI_F;
                float cosPhase = std::cos(ribCount * theta + helicalTwist * v);
                float modR = baseR * (1.0f + pleatAmp * fluteFade * cosPhase);

                Vec3 radial = frameU[r] * std::cos(theta) + frameV[r] * std::sin(theta);
                ringVerts[r][s] = spinePts[r] + radial * modR;
                ringNormals[r][s] = radial;
                if (r == 0) {
                    crestShade[s] = 0.42f + 0.58f * ((cosPhase + 1.0f) * 0.5f);
                }
            }
        }

        int spineRingStride = std::max(1, (ringCount * 10) / std::max(10, spineDens));
        int ribStride = (ribCount > 14) ? 2 : 1;

        for (int r = 0; r < ringCount - 1; ++r) {
            float vAvg = (static_cast<float>(r) + 0.5f) / static_cast<float>(ringCount - 1);
            float windW = std::clamp(spinePts[r].y / hM, 0.0f, 1.2f) * windFlex;
            int32_t pk = (isTubeBarrelFeltTop && r >= ringCount - 3) ? 3 : partKind;

            for (int s = 0; s < angSteps; ++s) {
                int sNext = (s + 1) % angSteps;
                float shadeAvg = 0.5f * (crestShade[s] + crestShade[sNext]);
                if (pk == 3) shadeAvg = std::min(1.0f, shadeAvg + 0.25f);
                // Winding outward: p0=(r, s), p1=(r, sNext), p2=(r+1, sNext), p3=(r+1, s)
                emitQuad(ringVerts[r][s], ringVerts[r][sNext], ringVerts[r + 1][sNext], ringVerts[r + 1][s],
                         shadeAvg, windW, pk);
            }

            // Place spines only at outer rib crests (even indices)
            if (r >= 1 && (r % spineRingStride == 0) && vAvg < 0.94f) {
                for (int s = 0; s < angSteps; s += 2 * ribStride) {
                    Vec3 radial = ringNormals[r][s];
                    Vec3 tang = tangents[r];
                    Vec3 side = safeNorm(cross(tang, radial));
                    emitSpineFan(ringVerts[r][s], radial, tang, side, spineLen, spinesPerAreole, spineCurvature, windW, 0);
                }
            }
        }

        if (domeCap) {
            Vec3 apex = spinePts.back() + tangents.back() * (baseRadii.back() * 0.35f);
            float windTop = std::clamp(apex.y / hM, 0.0f, 1.2f) * windFlex;
            int lastR = ringCount - 1;
            int32_t capKind = isTubeBarrelFeltTop ? 3 : partKind;
            for (int s = 0; s < angSteps; ++s) {
                int sNext = (s + 1) % angSteps;
                emitQuad(ringVerts[lastR][s], ringVerts[lastR][sNext], apex, apex, 0.85f, windTop, capKind);
            }
        }
    };

    switch (m_params.archetype) {
        case DesertCactusArchetype::GIANT_SAGUARO: {
            const float rStem = std::clamp(radM * 0.28f, 0.11f, 0.36f);
            const int trunkRings = 14;
            std::vector<Vec3> pts;
            std::vector<float> radii;
            pts.reserve(trunkRings);
            radii.reserve(trunkRings);
            for (int r = 0; r < trunkRings; ++r) {
                float v = static_cast<float>(r) / static_cast<float>(trunkRings - 1);
                float dome = (v > 0.86f) ? std::sqrt(std::max(0.12f, 1.0f - std::pow((v - 0.86f) / 0.14f, 2.0f))) : 1.0f;
                float waist = (v < 0.15f) ? (0.86f + 0.14f * (v / 0.15f)) : (1.0f + 0.08f * std::sin(v * PI_F));
                pts.push_back(Vec3(0.0f, v * hM, 0.0f));
                radii.push_back(rStem * waist * dome);
            }
            int sPerAreole = std::clamp(spineDens / 8, 2, 5);
            buildFlutedTube(pts, radii, ribs, fluteAmp, 0.0f, true, false, 0.048f, sPerAreole, 0.0f, 0);
            emitBloom(Vec3(0.0f, hM + rStem * 0.28f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), rStem * 0.72f, 0);
            emitBloom(Vec3(rStem * 0.45f, hM - 0.03f, 0.0f), Vec3(0.4f, 1.0f, 0.2f), rStem * 0.62f, 0);

            // Connected curved candelabra arms rooted at central axis
            const int armCount = std::clamp(2 + (ribs >= 12 ? 1 : 0) + (hM > 1.8f ? 1 : 0), 2, 4);
            for (int a = 0; a < armCount; ++a) {
                float az = a * (2.0f * PI_F / static_cast<float>(armCount)) + 0.42f;
                Vec3 dirOut(std::cos(az), 0.0f, std::sin(az));
                float attachY = hM * (0.36f + 0.09f * (a % 3));
                float armReach = radM * rng.range(0.62f, 0.86f);
                float armPeakY = std::min(hM * 0.96f, attachY + hM * rng.range(0.32f, 0.48f));

                Vec3 c0(0.0f, attachY, 0.0f);
                Vec3 c1 = dirOut * (armReach * 0.62f) + Vec3(0.0f, attachY - hM * 0.03f, 0.0f);
                Vec3 c2 = dirOut * armReach + Vec3(0.0f, attachY + (armPeakY - attachY) * 0.35f, 0.0f);
                Vec3 c3 = dirOut * (armReach * 0.96f) + Vec3(0.0f, armPeakY, 0.0f);

                std::vector<Vec3> aPts;
                std::vector<float> aRad;
                const int aSteps = 10;
                for (int k = 0; k < aSteps; ++k) {
                    float t = static_cast<float>(k) / static_cast<float>(aSteps - 1);
                    aPts.push_back(cubicBezier(c0, c1, c2, c3, t));
                    float neckPinch = (k <= 1) ? 0.62f : (t > 0.86f ? std::sqrt(std::max(0.15f, 1.0f - std::pow((t - 0.86f) / 0.14f, 2.0f))) : 0.82f);
                    aRad.push_back(rStem * 0.82f * neckPinch);
                }
                buildFlutedTube(aPts, aRad, std::max(6, (ribs * 3) / 4), fluteAmp, 0.0f, true, false, 0.042f, sPerAreole, 0.0f, 0);
                emitBloom(c3 + Vec3(0.0f, rStem * 0.22f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), rStem * 0.60f, 0);
            }
            break;
        }

        case DesertCactusArchetype::PRICKLY_PEAR: {
            // Hierarchical chain of 3D flattened obovate cladode pads with beveled rim
            auto addCladodePad3D = [&](const Vec3& basePos, const Vec3& axisUp, float padH, float padW, float rollRad, bool withFruit) {
                Vec3 uAxis = safeNorm(axisUp);
                Vec3 rBase, nBase;
                orthoBasis(uAxis, rBase, nBase);
                Vec3 rightDir = rBase * std::cos(rollRad) + nBase * std::sin(rollRad);
                Vec3 faceNorm = safeNorm(cross(rightDir, uAxis));

                const float thickness = std::clamp(padW * 0.18f, 0.018f, 0.055f);
                const float halfW = padW * 0.5f;
                const int segs = 10;
                // Obovate elliptical ring
                std::vector<Vec3> rimF(segs), rimB(segs);
                for (int i = 0; i < segs; ++i) {
                    float a = (static_cast<float>(i) / static_cast<float>(segs)) * 2.0f * PI_F;
                    float upFrac = 0.5f - 0.5f * std::cos(a); // 0 at base, 1 at top
                    float obovateScale = 0.55f + 0.55f * std::sin(upFrac * PI_F * 0.82f);
                    float wOff = std::sin(a) * halfW * obovateScale;
                    float hOff = upFrac * padH;
                    Vec3 p = basePos + uAxis * hOff + rightDir * wOff;
                    rimF[i] = p + faceNorm * (thickness * 0.5f);
                    rimB[i] = p - faceNorm * (thickness * 0.5f);
                }
                Vec3 centerF = basePos + uAxis * (padH * 0.52f) + faceNorm * (thickness * 0.88f);
                Vec3 centerB = basePos + uAxis * (padH * 0.52f) - faceNorm * (thickness * 0.88f);
                float windW = std::clamp((basePos.y + padH) / hM, 0.05f, 1.1f) * windFlex;

                for (int i = 0; i < segs; ++i) {
                    int nxt = (i + 1) % segs;
                    float edgeShade = 0.55f + 0.35f * (i % 2 == 0 ? 1.0f : 0.4f);
                    emitQuad(centerF, rimF[i], rimF[nxt], centerF, 0.90f, windW, 1);
                    emitQuad(centerB, rimB[nxt], rimB[i], centerB, 0.48f, windW, 1);
                    emitQuad(rimF[i], rimB[i], rimB[nxt], rimF[nxt], edgeShade, windW, 1);
                }

                // Areoles & glochid spine tufts on both pad faces
                int areolesPerFace = std::clamp(spineDens / 5, 3, 9);
                for (int k = 0; k < areolesPerFace; ++k) {
                    float f1 = 0.25f + 0.58f * rng.next01();
                    float f2 = rng.range(-0.62f, 0.62f);
                    Vec3 aPosF = basePos + uAxis * (padH * f1) + rightDir * (halfW * f2 * 0.65f) + faceNorm * (thickness * 0.72f);
                    Vec3 aPosB = basePos + uAxis * (padH * f1) + rightDir * (halfW * f2 * 0.65f) - faceNorm * (thickness * 0.72f);
                    emitSpineFan(aPosF, faceNorm, uAxis, rightDir, 0.040f, 3, 0.0f, windW, 0);
                    emitSpineFan(aPosB, faceNorm * -1.0f, uAxis, rightDir, 0.040f, 3, 0.0f, windW, 0);
                }

                if (withFruit) {
                    for (int fr = -1; fr <= 1; fr += 2) {
                        Vec3 frPos = basePos + uAxis * (padH * 0.94f) + rightDir * (halfW * fr * 0.42f);
                        emitBloom(frPos, safeNorm(uAxis + rightDir * (0.28f * fr)), padW * 0.16f, 1);
                    }
                }
                return basePos + uAxis * (padH * 0.88f);
            };

            // Root basal woody stemlet
            Vec3 rootTop(0.0f, hM * 0.08f, 0.0f);
            emitBranch(Vec3(0.0f, 0.0f, 0.0f), rootTop, radM * 0.14f, radM * 0.11f, 0, 0.02f);

            int basalPads = 2;
            for (int b = 0; b < basalPads; ++b) {
                float bYaw = b * 2.4f + 0.3f;
                Vec3 bDir = safeNorm(Vec3(std::cos(bYaw) * 0.25f, 1.0f, std::sin(bYaw) * 0.25f));
                float h1 = hM * 0.34f;
                float w1 = radM * 0.55f;
                Vec3 top1 = addCladodePad3D(rootTop, bDir, h1, w1, bYaw, false);

                for (int c = 0; c < 2; ++c) {
                    float cSide = (c == 0) ? -1.0f : 1.0f;
                    float cYaw = bYaw + cSide * 0.85f;
                    Vec3 cBase = top1 + Vec3(std::cos(cYaw) * w1 * 0.18f, -h1 * 0.06f, std::sin(cYaw) * w1 * 0.18f);
                    Vec3 cDir = safeNorm(Vec3(std::cos(cYaw) * 0.48f, 0.92f, std::sin(cYaw) * 0.48f));
                    float h2 = hM * 0.32f;
                    float w2 = radM * 0.48f;
                    Vec3 top2 = addCladodePad3D(cBase, cDir, h2, w2, cYaw + 0.55f, true);

                    if ((b + c) % 2 == 0) {
                        float dYaw = cYaw + cSide * 0.45f;
                        Vec3 dDir = safeNorm(Vec3(std::cos(dYaw) * 0.45f, 0.88f, std::sin(dYaw) * 0.45f));
                        addCladodePad3D(top2, dDir, hM * 0.26f, radM * 0.40f, dYaw + 0.7f, true);
                    }
                }
            }
            break;
        }

        case DesertCactusArchetype::GOLDEN_BARREL:
        case DesertCactusArchetype::FISHHOOK_BARREL: {
            const bool isGolden = (m_params.archetype == DesertCactusArchetype::GOLDEN_BARREL);
            const int heads = isGolden ? 3 : 2;
            const float curveSpine = isGolden ? 0.0f : 0.42f;
            for (int c = 0; c < heads; ++c) {
                float sc = (c == 0) ? 1.0f : (isGolden ? rng.range(0.42f, 0.58f) : 0.46f);
                float cAng = c * 2.25f + 0.55f;
                Vec3 cBase = (c == 0) ? Vec3(0.0f, 0.0f, 0.0f) : Vec3(std::cos(cAng) * radM * 0.75f, 0.0f, std::sin(cAng) * radM * 0.75f);
                float bh = hM * sc;
                float br = radM * 0.62f * sc;

                const int rings = 12;
                std::vector<Vec3> pts(rings);
                std::vector<float> radii(rings);
                for (int r = 0; r < rings; ++r) {
                    float v = static_cast<float>(r) / static_cast<float>(rings - 1);
                    // Barrel spheroid/cylinder profile
                    float sinArc = isGolden
                        ? std::sin(0.22f + v * (PI_F - 0.35f))
                        : std::pow(std::sin(0.20f + v * (PI_F - 0.32f)), 0.72f);
                    pts[r] = cBase + Vec3(0.0f, v * bh, 0.0f);
                    radii[r] = std::max(0.04f, br * sinArc);
                }
                int sPer = std::clamp(spineDens / 6, 3, 6);
                buildFlutedTube(pts, radii, ribs, fluteAmp * 1.15f, 0.0f, true, true,
                                (isGolden ? 0.068f : 0.088f) * sc, sPer, curveSpine, 0);

                // Crown ring of blooms
                int crownBlooms = isGolden ? 5 : 6;
                for (int bl = 0; bl < crownBlooms; ++bl) {
                    float ba = bl * (2.0f * PI_F / crownBlooms) + c;
                    Vec3 bPos = cBase + Vec3(std::cos(ba) * br * 0.25f, bh * 0.97f, std::sin(ba) * br * 0.25f);
                    emitBloom(bPos, safeNorm(Vec3(std::cos(ba) * 0.25f, 1.0f, std::sin(ba) * 0.25f)), br * 0.22f, 0);
                }
            }
            break;
        }

        case DesertCactusArchetype::ORGAN_PIPE: {
            const int pipes = std::clamp(ribs, 7, 15);
            for (int p = 0; p < pipes; ++p) {
                float pAng = (static_cast<float>(p) / static_cast<float>(pipes)) * 2.0f * PI_F + rng.range(-0.10f, 0.10f);
                Vec3 rDir(std::cos(pAng), 0.0f, std::sin(pAng));
                float reach = (p == 0) ? 0.0f : radM * rng.range(0.45f, 0.92f);
                float pHeight = (p == 0) ? hM : hM * rng.range(0.62f, 0.96f);

                Vec3 c0(0.0f, 0.0f, 0.0f);
                Vec3 c1 = rDir * (reach * 0.65f) + Vec3(0.0f, pHeight * 0.22f, 0.0f);
                Vec3 c2 = rDir * reach + Vec3(0.0f, pHeight * 0.65f, 0.0f);
                Vec3 c3 = rDir * (reach * 1.04f) + Vec3(0.0f, pHeight, 0.0f);

                float pipeR = std::clamp(radM * 0.14f, 0.045f, 0.11f);
                const int steps = 10;
                std::vector<Vec3> pts(steps);
                std::vector<float> radii(steps);
                for (int k = 0; k < steps; ++k) {
                    float t = static_cast<float>(k) / static_cast<float>(steps - 1);
                    pts[k] = cubicBezier(c0, c1, c2, c3, t);
                    float dome = (t > 0.85f) ? std::sqrt(std::max(0.18f, 1.0f - std::pow((t - 0.85f) / 0.15f, 2.0f))) : 1.0f;
                    radii[k] = pipeR * (0.78f + 0.22f * std::sin(t * PI_F * 0.6f)) * dome;
                }
                buildFlutedTube(pts, radii, std::clamp(ribs, 8, 16), fluteAmp, 0.0f, true, false, 0.036f, 3, 0.0f, 0);
                if (p % 2 == 0) {
                    emitBloom(c3 + Vec3(0.0f, pipeR * 0.35f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), pipeR * 0.95f, 0);
                }
            }
            break;
        }

        case DesertCactusArchetype::JUMPING_CHOLLA: {
            // Branching woody trunk + helical tuberculate cylinder joints densely armored in silvery spine stars
            Vec3 tBase(0.0f, 0.0f, 0.0f), tMid(0.02f, hM * 0.30f, -0.01f);
            emitBranch(tBase, tMid, radM * 0.14f, radM * 0.11f, 0, 0.03f * windFlex);
            const int boughs = 4;
            for (int b = 0; b < boughs; ++b) {
                float ba = (static_cast<float>(b) / boughs) * 2.0f * PI_F + 0.45f;
                Vec3 rDir(std::cos(ba), 0.0f, std::sin(ba));
                Vec3 j1 = tMid + safeNorm(rDir * 0.65f + Vec3(0.0f, 0.85f, 0.0f)) * (hM * 0.28f);
                emitBranch(tMid, j1, radM * 0.09f, radM * 0.072f, 1, 0.15f * windFlex);

                for (int ch = 0; ch < 2; ++ch) {
                    float ca = ba + (ch == 0 ? -0.58f : 0.58f);
                    Vec3 linkDir = safeNorm(Vec3(std::cos(ca) * 0.72f, 0.78f, std::sin(ca) * 0.72f));
                    Vec3 j2 = j1 + linkDir * (hM * 0.26f);

                    std::vector<Vec3> cPts = {j1, Vec3::lerp(j1, j2, 0.33f), Vec3::lerp(j1, j2, 0.66f), j2};
                    std::vector<float> cRad = {radM * 0.055f, radM * 0.088f, radM * 0.084f, radM * 0.048f};
                    buildFlutedTube(cPts, cRad, 8, fluteAmp * 1.4f, 1.8f, true, false, 0.058f, std::clamp(spineDens / 6, 4, 7), 0.0f, 0);

                    // Terminal drooping joint
                    float da = ca + (ch == 0 ? 0.35f : -0.35f);
                    Vec3 tDir = safeNorm(Vec3(std::cos(da) * 0.85f, 0.42f, std::sin(da) * 0.85f));
                    Vec3 j3 = j2 + tDir * (hM * 0.20f);
                    std::vector<Vec3> dPts = {j2, Vec3::lerp(j2, j3, 0.5f), j3};
                    std::vector<float> dRad = {radM * 0.050f, radM * 0.078f, radM * 0.042f};
                    buildFlutedTube(dPts, dRad, 8, fluteAmp * 1.3f, 1.4f, true, false, 0.062f, std::clamp(spineDens / 6, 4, 7), 0.0f, 0);
                    emitBloom(j3 + tDir * 0.04f, tDir, radM * 0.09f, 1);
                }
            }
            break;
        }

        case DesertCactusArchetype::JOSHUA_TREE: {
            // Dichotomous fibrous woody trunk + forked Y-limbs + dense 3D bayonet leaf tufts + creamy panicles
            Vec3 p0(0.0f, 0.0f, 0.0f), p1(0.02f, hM * 0.24f, -0.02f), p2(-0.02f, hM * 0.46f, 0.03f);
            emitBranch(p0, p1, radM * 0.22f, radM * 0.18f, 0, 0.02f * windFlex);
            emitBranch(p1, p2, radM * 0.18f, radM * 0.15f, 0, 0.06f * windFlex);

            const int limbs = 4;
            for (int l = 0; l < limbs; ++l) {
                float la = (static_cast<float>(l) / limbs) * 2.0f * PI_F + 0.35f;
                Vec3 dir1 = safeNorm(Vec3(std::cos(la) * 0.62f, 0.85f, std::sin(la) * 0.62f));
                Vec3 mid = p2 + dir1 * (hM * 0.25f);
                emitBranch(p2, mid, radM * 0.12f, radM * 0.095f, 1, 0.18f * windFlex);

                for (int f = 0; f < 2; ++f) {
                    float fa = la + (f == 0 ? -0.48f : 0.48f);
                    Vec3 dir2 = safeNorm(Vec3(std::cos(fa) * 0.56f, 0.94f, std::sin(fa) * 0.56f));
                    Vec3 tip = mid + dir2 * (hM * rng.range(0.18f, 0.28f));
                    int32_t tipSeg = emitBranch(mid, tip, radM * 0.088f, radM * 0.068f, 2, 0.34f * windFlex);

                    // 3D Bayonet/dagger leaf rosette head
                    int bladeCount = std::clamp(spineDens / 2 + 8, 16, 26);
                    Vec3 uVec, vVec;
                    orthoBasis(dir2, uVec, vVec);
                    for (int d = 0; d < bladeCount; ++d) {
                        float tier = static_cast<float>(d) / static_cast<float>(std::max(1, bladeCount - 1));
                        float phi = static_cast<float>(d) * GOLDEN_ANGLE_F;
                        Vec3 radial = uVec * std::cos(phi) + vVec * std::sin(phi);
                        float elevation = -0.35f + tier * 1.25f;
                        Vec3 bladeDir = safeNorm(radial * (1.0f - 0.32f * tier) + dir2 * elevation);
                        Vec3 wing = safeNorm(cross(bladeDir, dir2), vVec);
                        float bLen = (0.20f + 0.12f * (1.0f - std::fabs(tier - 0.5f))) * std::clamp(radM * 0.65f, 0.22f, 0.55f);
                        emitLeaf(tip - dir2 * (0.04f * (1.0f - tier)), bladeDir, wing, bLen, bLen * 0.14f, 0.25f + 0.75f * tier, tipSeg);
                    }
                    emitBloom(tip + dir2 * 0.09f, dir2, radM * 0.18f, 3);
                }
            }
            break;
        }

        case DesertCactusArchetype::OCOTILLO: {
            // Fountain vase of 12..22 curved spiny whip canes + small axial leaves + scarlet flame spikes
            const int canes = std::clamp(ribs, 10, 22);
            for (int c = 0; c < canes; ++c) {
                float az = (static_cast<float>(c) / canes) * 2.0f * PI_F + rng.range(-0.10f, 0.10f);
                Vec3 rad(std::cos(az), 0.0f, std::sin(az));
                Vec3 tang(-std::sin(az), 0.0f, std::cos(az));
                float reach = radM * rng.range(0.68f, 1.05f);
                float caneH = hM * rng.range(0.84f, 1.06f);

                Vec3 p0 = rad * (0.04f * radM);
                Vec3 p1 = rad * (reach * 0.26f) + Vec3(0.0f, caneH * 0.34f, 0.0f);
                Vec3 p2 = rad * (reach * 0.64f) + Vec3(0.0f, caneH * 0.72f, 0.0f);
                Vec3 p3 = rad * reach + Vec3(0.0f, caneH, 0.0f);

                const int segs = 5;
                Vec3 prev = p0;
                int32_t lastSeg = 0;
                for (int k = 1; k <= segs; ++k) {
                    float tPrev = static_cast<float>(k - 1) / segs;
                    float t = static_cast<float>(k) / segs;
                    Vec3 cur = cubicBezier(p0, p1, p2, p3, t);
                    float windW = std::clamp(cur.y / hM, 0.0f, 1.4f) * windFlex;
                    lastSeg = emitBranch(prev, cur, 0.022f * (1.0f - 0.55f * tPrev), 0.022f * (1.0f - 0.65f * t), k <= 2 ? 0 : 1, windW);

                    if (k >= 2) {
                        Vec3 caneDir = safeNorm(cur - prev);
                        Vec3 mid = (prev + cur) * 0.5f;
                        Vec3 leafD1 = safeNorm(rad * 0.6f + tang * 0.6f + Vec3(0, 0.45f, 0));
                        Vec3 leafD2 = safeNorm(rad * 0.6f - tang * 0.6f + Vec3(0, 0.45f, 0));
                        emitLeaf(mid, leafD1, tang, 0.055f, 0.032f, 0.6f + 0.4f * t, lastSeg);
                        emitLeaf(mid, leafD2, tang, 0.055f, 0.032f, 0.6f + 0.4f * t, lastSeg);
                        emitSpineFan(mid, safeNorm(rad + Vec3(0, 0.2f, 0)), caneDir, tang, 0.032f, 1, 0.25f, windW, 0);
                    }
                    prev = cur;
                }
                Vec3 tipDir = safeNorm(p3 - p2);
                emitBloom(p3, tipDir, 0.20f, 2);
            }
            break;
        }

        case DesertCactusArchetype::KRANTZ_ALOE: {
            // Branched candelabra woody heads holding fleshy curved toothed Aloe leaf rosettes + fiery raceme torches
            const int heads = 3;
            for (int h = 0; h < heads; ++h) {
                float hAz = (static_cast<float>(h) / heads) * 2.0f * PI_F + 0.4f;
                Vec3 hRadial(std::cos(hAz), 0.0f, std::sin(hAz));
                Vec3 headBase = (h == 0)
                    ? Vec3(0.0f, hM * 0.48f, 0.0f)
                    : hRadial * (radM * 0.45f) + Vec3(0.0f, hM * (0.34f + 0.08f * h), 0.0f);
                Vec3 trunkMid = headBase * 0.52f;
                emitBranch(Vec3(0, 0, 0), trunkMid, radM * 0.14f, radM * 0.11f, 0, 0.06f * windFlex);
                int32_t headBr = emitBranch(trunkMid, headBase, radM * 0.11f, radM * 0.085f, 1, 0.16f * windFlex);

                int leavesPerHead = std::clamp(spineDens / 2 + 6, 14, 24);
                for (int l = 0; l < leavesPerHead; ++l) {
                    float tier = static_cast<float>(l) / static_cast<float>(std::max(1, leavesPerHead - 1));
                    float phi = l * GOLDEN_ANGLE_F + h * 1.2f;
                    Vec3 radial(std::cos(phi), 0.0f, std::sin(phi));
                    Vec3 tang(-std::sin(phi), 0.0f, std::cos(phi));
                    float pitchUp = 0.20f + tier * 1.05f;
                    Vec3 leafDir = safeNorm(radial * std::cos(pitchUp) + Vec3(0.0f, std::sin(pitchUp), 0.0f));
                    float leafLen = radM * (0.68f - 0.26f * tier);
                    emitLeaf(headBase + radial * 0.02f, leafDir, tang, leafLen, leafLen * 0.20f, 0.35f + 0.65f * tier, headBr);

                    // Marginal hooked teeth on aloe blades
                    if (wantSpines && (l % 2 == 0)) {
                        for (float m : {0.42f, 0.72f}) {
                            Vec3 edgeBase = headBase + leafDir * (leafLen * m);
                            res.spines.push_back({edgeBase + tang * (leafLen * 0.09f), safeNorm(tang * 0.8f + leafDir * 0.5f), 0.024f, 0.35f, 0.2f * windFlex, 0});
                            res.spines.push_back({edgeBase - tang * (leafLen * 0.09f), safeNorm(tang * -0.8f + leafDir * 0.5f), 0.024f, 0.35f, 0.2f * windFlex, 0});
                        }
                    }
                }

                // Fiery orange-red nectar torch rising from rosette crown
                Vec3 sMid = headBase + Vec3(0.02f, hM * 0.26f, -0.01f);
                Vec3 sTop = sMid + Vec3(0.01f, hM * 0.22f, 0.02f);
                emitBranch(headBase, sMid, 0.020f, 0.014f, 2, 0.35f * windFlex);
                emitBranch(sMid, sTop, 0.014f, 0.009f, 2, 0.62f * windFlex);
                emitBloom(sTop, Vec3(0.0f, 1.0f, 0.0f), 0.18f * hM, 2);
            }
            break;
        }

        case DesertCactusArchetype::ZEBRA_HAWORTHIA: {
            // Central rosette + 2 basal pups with thick triangular 3D leaves and white porcelain zebra bands
            const int rosettes = 3;
            for (int ros = 0; ros < rosettes; ++ros) {
                float rScale = (ros == 0) ? 1.0f : 0.48f;
                float rAng = ros * 2.5f + 0.5f;
                Vec3 origin = (ros == 0) ? Vec3(0, 0.0f, 0) : Vec3(std::cos(rAng) * radM * 0.72f, 0.0f, std::sin(rAng) * radM * 0.72f);

                int leafCount = (ros == 0) ? std::clamp(spineDens / 2 + 6, 16, 26) : 10;
                for (int l = 0; l < leafCount; ++l) {
                    float tier = static_cast<float>(l) / static_cast<float>(std::max(1, leafCount - 1));
                    float phi = l * GOLDEN_ANGLE_F + ros;
                    Vec3 radial(std::cos(phi), 0.0f, std::sin(phi));
                    Vec3 tang(-std::sin(phi), 0.0f, std::cos(phi));
                    float pitchUp = 0.28f + tier * 1.10f;
                    Vec3 midDir = safeNorm(radial * std::cos(pitchUp) + Vec3(0.0f, std::sin(pitchUp), 0.0f));
                    float leafLen = std::max(0.18f, radM * 0.95f * rScale * (1.05f - 0.45f * tier));
                    emitLeaf(origin, midDir, tang, leafLen, leafLen * 0.28f, 0.30f + 0.65f * tier, -1);

                    // Raised porcelain white zebra transverse tubercle stripes on abaxial face
                    if (wantSpines) {
                        Vec3 abaxNormal = safeNorm(cross(tang, midDir));
                        if (abaxNormal.y > 0.0f) abaxNormal = abaxNormal * -1.0f;
                        int bandCount = std::clamp(spineDens / 8, 3, 6);
                        for (int b = 1; b <= bandCount; ++b) {
                            float frac = static_cast<float>(b) / static_cast<float>(bandCount + 1);
                            float bandHalfW = leafLen * 0.13f * (1.0f - frac * 0.75f);
                            Vec3 bPos = origin + midDir * (leafLen * frac) + abaxNormal * 0.004f;
                            res.spines.push_back({bPos, tang, bandHalfW * 2.0f, 0.0f, 0.02f, 2});
                        }
                    }
                }
            }
            if (wantBlooms) {
                Vec3 spTop(0.04f, hM * 1.35f, -0.02f);
                emitBranch(Vec3(0, 0.0f, 0), spTop, 0.008f, 0.004f, 2, 0.55f * windFlex);
                emitBloom(spTop, Vec3(0, 1, 0), 0.055f, 3);
            }
            break;
        }
    }

    return res;
}

} // namespace desert
} // namespace vegetation
