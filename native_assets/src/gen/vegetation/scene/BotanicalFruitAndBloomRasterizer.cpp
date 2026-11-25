// ============================================================================
// BotanicalFruitAndBloomRasterizer.cpp
// Modular 3D Botanical Reproductive Organ Rasterizer & Placement Engine:
//  - Seasonal Blossoms (racemes, bottlebrush spikes, bracts, florets)
//  - Cauliflorous Trunk Fruits (Papaya trunk collar, Cacao pods, Jackfruit)
//  - Crownshaft Collar Fruits (Coconut 3-nut clusters, Date Palm panicles)
//  - Infructescence & Outer-Twig Fruits (Banana hands+bell, Mango, Breadfruit,
//    Cashew apple+nut, Avocado, Citrus, Macadamia, Lychee, Pome/Stone fruits)
//  - Shrub Accents (berries, bracts, acacia puffs, thorns, protea, flame)
// ============================================================================

#include "Botanical3DSceneRenderer.h"
#include "../trees/tree_types/TreeTypeBase.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace vegetation {
namespace scene {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kGoldenAngle = 2.399963229728653f;

inline RgbF mixRgb(const RgbF& a, const RgbF& b, float t) {
    float u = std::clamp(t, 0.0f, 1.0f);
    return {a.r + (b.r - a.r) * u, a.g + (b.g - a.g) * u, a.b + (b.b - a.b) * u};
}

inline Vec3 perpAxis(const Vec3& d) {
    Vec3 ref = (std::abs(d.y) < 0.90f) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
    return cross(d, ref).normalized();
}
} // namespace

void rasterizeTreeBlossom3D(
    DepthFrameBuffer& fb,
    const Vec3& pos,
    const Vec3& dir,
    float sizeMeters,
    int bloomStyleId,
    const RgbF& primaryRgb,
    const RgbF& secondaryRgb,
    const Vec3& sunDir,
    uint32_t seedIdx,
    bool isCloseup
) {
    if (bloomStyleId <= 0) return;

    Vec3 d = dir.normalized();
    Vec3 side = perpAxis(d);
    Vec3 up = cross(side, d).normalized();

    float r = std::max(0.018f, sizeMeters);
    float phase = static_cast<float>(seedIdx) * kGoldenAngle;

    // 11: BOTTLEBRUSH_SPIKES (cylindrical crimson stamen brush)
    if (bloomStyleId == 11) {
        Vec3 spikeTip = pos + d * (r * 2.1f);
        fb.drawAaLine3D(pos, spikeTip, isCloseup ? 2.8f : 1.6f, 95, 70, 50, 0.92f);
        for (int k = 0; k < 10; ++k) {
            float t = 0.15f + (static_cast<float>(k) / 9.0f) * 0.78f;
            float ang = phase + k * 2.1f;
            Vec3 pBase = pos + d * (r * 2.1f * t);
            Vec3 pRim = pBase + (side * std::cos(ang) + up * std::sin(ang)) * (r * 0.68f);
            fb.drawAaLine3D(pBase, pRim, isCloseup ? 2.0f : 1.1f,
                            DepthFrameBuffer::toByte(primaryRgb.r),
                            DepthFrameBuffer::toByte(primaryRgb.g),
                            DepthFrameBuffer::toByte(primaryRgb.b),
                            0.95f);
            fb.drawShadedSphere3D(pRim, r * 0.10f, secondaryRgb, sunDir, 0.1f, 1.0f, 3.5f);
        }
        return;
    }

    // 9: GOLDEN_SHOWER_RACEMES / 4: JACARANDA / 8: CREPE_MYRTLE (hanging panicle racemes)
    if (bloomStyleId == 9 || bloomStyleId == 4 || bloomStyleId == 8) {
        int drops = (bloomStyleId == 9) ? 6 : 5;
        Vec3 dropAxis = (bloomStyleId == 9) ? Vec3(0.0f, -1.0f, 0.0f) : d;
        for (int i = 0; i < drops; ++i) {
            float frac = static_cast<float>(i) / static_cast<float>(drops - 1);
            float ang = phase + i * 2.094f;
            Vec3 p = pos + dropAxis * (r * (0.35f + frac * 1.75f))
                   + (side * std::cos(ang) + up * std::sin(ang)) * (r * 0.38f * (1.0f - frac * 0.5f));
            RgbF c = mixRgb(primaryRgb, secondaryRgb, frac * 0.35f);
            fb.drawShadedSphere3D(p, r * (0.38f - 0.12f * frac), c, sunDir, 0.18f, 1.8f, 14.0f);
        }
        return;
    }

    // 14: DOGWOOD_FOUR_BRACTS (4 symmetrical broad white bracts) or 13: BOUGAINVILLEA_BRACTS (3 bracts)
    int petalCount = (bloomStyleId == 14) ? 4 : (bloomStyleId == 13) ? 3 : (bloomStyleId == 7) ? 6 : 5;
    float petalDist = r * ((bloomStyleId == 7) ? 0.58f : 0.62f);
    float petalRad = r * ((bloomStyleId == 14 || bloomStyleId == 7) ? 0.46f : 0.38f);
    for (int p = 0; p < petalCount; ++p) {
        float ang = phase + static_cast<float>(p) * (2.0f * kPi / static_cast<float>(petalCount));
        Vec3 offset = (side * std::cos(ang) + up * std::sin(ang)) * petalDist + d * (r * 0.15f);
        RgbF pCol = (p % 2 == 0) ? primaryRgb : mixRgb(primaryRgb, {1.0f, 1.0f, 1.0f}, 0.14f);
        fb.drawShadedSphere3D(pos + offset, petalRad, pCol, sunDir, 0.15f, 1.6f, 15.0f);
    }
    fb.drawShadedSphere3D(pos + d * (r * 0.22f), r * 0.24f, secondaryRgb, sunDir, 0.30f, 1.4f, 8.0f);
}

void rasterizeTreeFruit3D(
    DepthFrameBuffer& fb,
    const Vec3& pos,
    const Vec3& dir,
    float sizeMeters,
    int fruitStyleId,
    const RgbF& primaryRgb,
    const RgbF& secondaryRgb,
    const Vec3& sunDir,
    uint32_t seedIdx,
    bool isCloseup
) {
    if (fruitStyleId <= 0) return;

    float r = std::max(0.03f, sizeMeters);
    Vec3 d = dir.normalized();
    Vec3 side = perpAxis(d);
    Vec3 hangPos = pos + Vec3(0.0f, -r * 0.75f, 0.0f);

    // Non-papaya species get a slim pedicel stalk (papayas hug the trunk node directly with stubby stem)
    if (fruitStyleId != 6) {
        fb.drawAaLine3D(pos, hangPos, isCloseup ? 2.0f : 1.1f, 92, 68, 42, 0.90f);
    }

    switch (fruitStyleId) {
        case 2: {
            // COCONUT_CLUSTERS: Tight cluster of 3 fibrous ovoid coconuts right at trunk crown collar
            fb.drawShadedCylinder3D(pos, pos + d * (r * 0.55f) + Vec3(0.0f, -r * 0.45f, 0.0f),
                                    r * 0.12f, r * 0.08f, {0.42f, 0.34f, 0.20f}, sunDir, 0.85f, false, true);
            Vec3 clusterCenter = pos + d * (r * 0.55f) + Vec3(0.0f, -r * 0.75f, 0.0f);
            for (int c = 0; c < 3; ++c) {
                float a = static_cast<float>(seedIdx) + static_cast<float>(c) * 2.094f;
                Vec3 off = side * (std::cos(a) * r * 0.62f) + cross(d, side).normalized() * (std::sin(a) * r * 0.62f);
                Vec3 nutTop = clusterCenter + off;
                Vec3 nutBot = nutTop + Vec3(0.0f, -r * 0.55f, 0.0f);
                RgbF nutCol = mixRgb(primaryRgb, secondaryRgb, (c == 1) ? 0.28f : 0.0f);
                fb.drawShadedSphere3D(nutTop, r * 0.54f, nutCol, sunDir, 0.34f, 2.2f, 22.0f);
                fb.drawShadedSphere3D(nutBot, r * 0.48f, mixRgb(nutCol, secondaryRgb, 0.18f), sunDir, 0.30f, 2.0f, 20.0f);
            }
            break;
        }
        case 3: {
            // DATE_BUNCHES: Thick arched amber panicle stalk with drooping strands of golden date drupes
            Vec3 stalkOut = pos + d * (r * 1.15f) + Vec3(0.0f, -r * 0.35f, 0.0f);
            fb.drawShadedCylinder3D(pos, stalkOut, r * 0.14f, r * 0.09f, {0.82f, 0.62f, 0.20f}, sunDir, 0.90f, false, true);
            for (int k = 0; k < 8; ++k) {
                float a = static_cast<float>(k) * 0.785f + static_cast<float>(seedIdx);
                float dy = -r * (0.25f + static_cast<float>(k % 4) * 0.38f);
                Vec3 fp = stalkOut + Vec3(std::cos(a) * r * 0.52f, dy, std::sin(a) * r * 0.52f);
                RgbF fc = mixRgb(primaryRgb, secondaryRgb, (k % 2 == 0) ? 0.0f : 0.32f);
                fb.drawShadedSphere3D(fp, r * 0.32f, fc, sunDir, 0.38f, 1.8f, 14.0f);
            }
            break;
        }
        case 4: {
            // BANANA_HANDS fallback
            for (int k = -2; k <= 2; ++k) {
                float fk = static_cast<float>(k);
                Vec3 p0 = hangPos + Vec3(fk * r * 0.34f, 0.0f, 0.0f);
                Vec3 pMid = p0 + Vec3(fk * r * 0.22f, -r * 0.42f, r * 0.56f);
                Vec3 pTip = pMid + Vec3(fk * r * 0.10f, r * 0.62f, r * 0.48f);
                fb.drawShadedCylinder3D(p0, pMid, r * 0.20f, r * 0.22f, primaryRgb, sunDir, 0.95f, false, true);
                fb.drawShadedCylinder3D(pMid, pTip, r * 0.22f, r * 0.12f, primaryRgb, sunDir, 0.95f, false, true);
            }
            break;
        }
        case 5: {
            // MANGO_DRUPES: Long pendulous floral panicle wire + kidney-oval blushed golden-crimson drupe
            Vec3 mHang = pos + Vec3(0.0f, -r * 1.45f, 0.0f);
            fb.drawAaLine3D(pos, mHang, isCloseup ? 2.0f : 1.0f, 108, 86, 46, 0.88f);
            Vec3 beakOff = side * (r * 0.22f) + Vec3(0.0f, -r * 0.58f, 0.0f);
            RgbF shoulderCol = mixRgb(primaryRgb, secondaryRgb, 0.32f);
            fb.drawShadedSphere3D(mHang, r * 0.68f, shoulderCol, sunDir, 0.44f, 2.2f, 24.0f);
            fb.drawShadedSphere3D(mHang + beakOff, r * 0.54f, primaryRgb, sunDir, 0.42f, 2.0f, 20.0f);
            break;
        }
        case 6: {
            // PAPAYA_CLUSTERS: Elongated 5-ribbed pyriform-oblong melon sessile on upper trunk collar!
            // Stubby horizontal-downward stalk emerging from trunk leaf-scar node
            Vec3 tiltDir = (d * 0.55f + Vec3(0.0f, -0.84f, 0.0f)).normalized();
            Vec3 neck = pos + tiltDir * (r * 0.28f);
            Vec3 body = pos + tiltDir * (r * 0.82f);
            Vec3 belly = pos + tiltDir * (r * 1.38f);
            fb.drawShadedCylinder3D(pos, neck, r * 0.14f, r * 0.16f, {0.36f, 0.48f, 0.18f}, sunDir, 0.88f, false, true);
            fb.drawShadedSphere3D(neck + tiltDir * (r * 0.18f), r * 0.46f, mixRgb(primaryRgb, secondaryRgb, 0.30f), sunDir, 0.38f, 2.0f, 22.0f);
            fb.drawShadedCylinder3D(neck + tiltDir * (r * 0.18f), body, r * 0.48f, r * 0.68f, primaryRgb, sunDir, 0.92f, false, false);
            fb.drawShadedSphere3D(belly, r * 0.68f, primaryRgb, sunDir, 0.42f, 2.4f, 28.0f);
            break;
        }
        case 10:
        case 20: {
            // MACADAMIA_RACEMES / LYCHEE_PANICLES: Multi-drupe hanging panicle bunch
            for (int k = 0; k < 6; ++k) {
                float a = static_cast<float>(k) * 1.047f + static_cast<float>(seedIdx);
                float dy = -static_cast<float>(k) * r * 0.35f;
                Vec3 fp = hangPos + Vec3(std::cos(a) * r * 0.42f, dy, std::sin(a) * r * 0.42f);
                RgbF fc = mixRgb(primaryRgb, secondaryRgb, (k % 2 == 0) ? 0.0f : 0.35f);
                fb.drawShadedSphere3D(fp, r * 0.42f, fc, sunDir, 0.38f, 2.0f, 18.0f);
            }
            break;
        }
        case 7:
        case 9:
        case 16: {
            // Pyriform fruit: AVOCADO_PEARS, CITRUS_LEMONS, PEAR_PYRIFORM
            Vec3 bot = hangPos + Vec3(0.0f, -r * 0.72f, 0.0f);
            fb.drawShadedSphere3D(hangPos + Vec3(0.0f, -r * 0.22f, 0.0f), r * 0.54f, primaryRgb, sunDir, 0.40f, 2.0f, 24.0f);
            fb.drawShadedSphere3D(bot, r * 0.78f, mixRgb(primaryRgb, secondaryRgb, 0.22f), sunDir, 0.44f, 2.5f, 30.0f);
            break;
        }
        case 18: {
            // CACAO_PODS: Elongated furrowed spindle/ellipsoid pod hanging on trunk & jorquette limbs
            Vec3 podTop = hangPos;
            Vec3 podMid = hangPos + Vec3(0.0f, -r * 0.72f, 0.0f);
            Vec3 podTip = hangPos + Vec3(0.0f, -r * 1.48f, 0.0f);
            RgbF furrowCol = mixRgb(primaryRgb, secondaryRgb, 0.38f);
            fb.drawShadedCylinder3D(podTop, podMid, r * 0.34f, r * 0.56f, primaryRgb, sunDir, 0.92f, false, true);
            fb.drawShadedCylinder3D(podMid, podTip, r * 0.56f, r * 0.14f, furrowCol, sunDir, 0.88f, false, true);
            break;
        }
        case 11: {
            // JACKFRUIT_CAULIFLOROUS: Heavy bumpy oblong barrel hung straight off trunk/limb bark
            Vec3 top = hangPos;
            Vec3 bot = hangPos + Vec3(0.0f, -r * 1.45f, 0.0f);
            fb.drawShadedSphere3D(top, r * 0.78f, primaryRgb, sunDir, 0.25f, 2.5f, 30.0f);
            fb.drawShadedCylinder3D(top, bot, r * 0.82f, r * 0.76f, primaryRgb, sunDir, 0.90f, false, true);
            fb.drawShadedSphere3D(bot, r * 0.76f, mixRgb(primaryRgb, secondaryRgb, 0.25f), sunDir, 0.22f, 2.5f, 30.0f);
            break;
        }
        case 19: {
            // BREADFRUIT_ORBS: Large knobby round-ovoid lime-green fruit clustered on outer twigs
            RgbF orbCol = mixRgb(primaryRgb, secondaryRgb, (seedIdx % 2 == 0) ? 0.16f : 0.0f);
            fb.drawShadedSphere3D(hangPos + Vec3(0.0f, -r * 0.35f, 0.0f), r * 1.05f, orbCol, sunDir, 0.24f, 3.0f, 34.0f);
            break;
        }
        case 21: {
            // CASHEW_APPLES: Glossy scarlet-gold hypocarp apple + curved grey-brown kidney nut suspended underneath
            fb.drawShadedSphere3D(hangPos + Vec3(0.0f, -r * 0.15f, 0.0f), r * 0.68f, primaryRgb, sunDir, 0.45f, 2.5f, 22.0f);
            fb.drawShadedSphere3D(hangPos + Vec3(r * 0.16f, -r * 0.92f, 0.0f), r * 0.32f, {0.42f, 0.34f, 0.26f}, sunDir, 0.2f, 1.5f, 10.0f);
            break;
        }
        default: {
            // Spherical fruits: OLIVE, ORANGE, GUAVA, PASSIONFRUIT, POMEGRANATE, APPLE, PEACH
            RgbF fc = mixRgb(primaryRgb, secondaryRgb, (seedIdx % 3 == 0) ? 0.24f : 0.0f);
            fb.drawShadedSphere3D(hangPos, r * 0.82f, fc, sunDir, 0.48f, 2.2f, 26.0f);
            // Tiny green calyx button where pedicel meets citrus / apple / peach top
            if (fruitStyleId == 8 || fruitStyleId == 12 || fruitStyleId == 15 || fruitStyleId == 17) {
                fb.drawShadedSphere3D(hangPos + Vec3(0.0f, r * 0.72f, 0.0f), r * 0.22f, {0.16f, 0.44f, 0.14f}, sunDir, 0.20f, 1.5f, 8.0f);
            }
            if (fruitStyleId == 14) {
                fb.drawShadedSphere3D(hangPos + Vec3(0.0f, -r * 0.82f, 0.0f), r * 0.26f, primaryRgb, sunDir, 0.2f, 1.5f, 8.0f);
            }
            break;
        }
    }
}

void rasterizeBananaInfructescence3D(
    DepthFrameBuffer& fb,
    const Vec3& stemCrownPos,
    float outAzRad,
    float bunchScaleMeters,
    const RgbF& fruitPrimary,
    const RgbF& bellSecondary,
    const RgbF& stemColor,
    const Vec3& sunDir,
    bool isCloseup
) {
    float s = std::clamp(bunchScaleMeters, 0.55f, 1.65f);
    Vec3 outDir(std::cos(outAzRad), 0.0f, std::sin(outAzRad));
    RgbF pedColor = mixRgb(stemColor, {0.26f, 0.44f, 0.16f}, 0.55f);
    RgbF rachisDark = mixRgb(pedColor, bellSecondary, 0.35f);
    RgbF floralTip = {0.22f, 0.14f, 0.08f};

    const Vec3 pPts[5] = {
        stemCrownPos,
        stemCrownPos + outDir * (0.14f * s) + Vec3(0.0f,  0.07f * s, 0.0f),
        stemCrownPos + outDir * (0.28f * s) + Vec3(0.0f,  0.03f * s, 0.0f),
        stemCrownPos + outDir * (0.36f * s) + Vec3(0.0f, -0.09f * s, 0.0f),
        stemCrownPos + outDir * (0.37f * s) + Vec3(0.0f, -0.22f * s, 0.0f)
    };
    float pedR = 0.036f * s;
    for (int i = 0; i < 4; ++i) {
        float r0 = pedR * (1.0f - 0.06f * i);
        float r1 = pedR * (1.0f - 0.06f * (i + 1));
        fb.drawShadedCylinder3D(pPts[i], pPts[i + 1], r0, r1, pedColor, sunDir, 0.92f, false, true);
    }

    Vec3 rachisTop = pPts[4];
    float handsSpan = 0.56f * s;
    float bareSpan = 0.28f * s;
    Vec3 handsBot = rachisTop + Vec3(0.0f, -handsSpan, 0.0f);
    Vec3 bellTop = handsBot + Vec3(0.0f, -bareSpan, 0.0f);

    fb.drawShadedCylinder3D(rachisTop, handsBot, pedR * 0.78f, pedR * 0.58f, pedColor, sunDir, 0.90f, false, true);
    fb.drawShadedCylinder3D(handsBot, bellTop, pedR * 0.58f, pedR * 0.38f, rachisDark, sunDir, 0.88f, false, true);

    for (int b = 1; b <= 2; ++b) {
        Vec3 nodePos = handsBot + Vec3(0.0f, -bareSpan * (static_cast<float>(b) / 3.0f), 0.0f);
        fb.drawShadedSphere3D(nodePos, pedR * 0.52f, rachisDark, sunDir, 0.22f, 1.5f, 10.0f);
    }

    constexpr int kHandCount = 6;
    for (int h = 0; h < kHandCount; ++h) {
        float hFrac = static_cast<float>(h) / static_cast<float>(kHandCount - 1);
        Vec3 handCenter = rachisTop + Vec3(0.0f, -hFrac * handsSpan, 0.0f);
        float handAz = outAzRad + ((h % 2 == 0) ? 0.38f : -0.38f) + (hFrac - 0.5f) * 0.30f;

        Vec3 handDir(std::cos(handAz), 0.0f, std::sin(handAz));
        Vec3 cushionPos = handCenter + handDir * (pedR * 0.85f);
        fb.drawShadedSphere3D(cushionPos, 0.032f * s, pedColor, sunDir, 0.24f, 1.8f, 14.0f);

        int fingers = (h < 4) ? 8 : 7;
        float fingerLen = (0.145f - 0.032f * hFrac) * s;
        float fingerR = (0.022f - 0.004f * hFrac) * s;

        for (int f = 0; f < fingers; ++f) {
            float fSpan = (static_cast<float>(f) / static_cast<float>(fingers - 1)) - 0.5f;
            float fAz = handAz + fSpan * 2.25f;
            Vec3 radial(std::cos(fAz), 0.0f, std::sin(fAz));

            Vec3 fBase = cushionPos + radial * (0.018f * s);
            Vec3 fMid = fBase + radial * (fingerLen * 0.52f) + Vec3(0.0f, -fingerLen * 0.24f, 0.0f);
            Vec3 fTip = fMid + radial * (fingerLen * 0.38f) + Vec3(0.0f, fingerLen * 0.44f, 0.0f);

            RgbF fBaseCol = mixRgb(fruitPrimary, pedColor, 0.28f - 0.14f * hFrac);
            RgbF fTipCol = mixRgb(fruitPrimary, {0.98f, 0.88f, 0.26f}, 0.35f);

            fb.drawShadedCylinder3D(fBase, fMid, fingerR * 0.88f, fingerR, fBaseCol, sunDir, 0.96f, false, true);
            fb.drawShadedCylinder3D(fMid, fTip, fingerR, fingerR * 0.55f, fTipCol, sunDir, 0.98f, false, true);
            fb.drawShadedSphere3D(fTip, fingerR * (isCloseup ? 0.52f : 0.46f), floralTip, sunDir, 0.12f, 1.0f, 5.0f);
        }
    }

    float bellR = 0.072f * s;
    Vec3 bellShoulder = bellTop + Vec3(0.0f, -bellR * 0.55f, 0.0f);
    Vec3 bellMid = bellShoulder + Vec3(0.0f, -bellR * 0.95f, 0.0f);
    Vec3 bellApex = bellMid + Vec3(0.0f, -bellR * 0.95f, 0.0f);

    RgbF bellBody = bellSecondary;
    RgbF bellTip = mixRgb(bellSecondary, {0.32f, 0.08f, 0.12f}, 0.45f);
    fb.drawShadedSphere3D(bellShoulder, bellR * 0.88f, bellBody, sunDir, 0.34f, 2.8f, 34.0f);
    fb.drawShadedCylinder3D(bellShoulder, bellMid, bellR * 0.88f, bellR * 0.60f, bellBody, sunDir, 0.92f, false, false);
    fb.drawShadedCylinder3D(bellMid, bellApex, bellR * 0.60f, bellR * 0.16f, bellTip, sunDir, 0.88f, false, true);
}

void rasterizeShrubAccent3D(
    DepthFrameBuffer& fb,
    const Vec3& pos,
    const Vec3& dir,
    float sizeMeters,
    int accentKindId,
    float phase,
    const RgbF& accentRgb,
    const RgbF& foliageRgb,
    const Vec3& sunDir,
    bool isCloseup
) {
    if (accentKindId <= 0) return;
    Vec3 d = dir.normalized();
    Vec3 side = perpAxis(d);
    Vec3 up = cross(side, d).normalized();
    float r = std::max(0.025f, sizeMeters);

    switch (accentKindId) {
        case 1: {
            const Vec3 offs[3] = {side * (-r * 0.55f), side * (r * 0.55f), up * (r * 0.48f)};
            for (const auto& o : offs) {
                fb.drawShadedSphere3D(pos + o, r * 0.42f, accentRgb, sunDir, 0.58f, 1.8f, 14.0f);
            }
            break;
        }
        case 2: {
            for (int b = 0; b < 3; ++b) {
                float a = phase + static_cast<float>(b) * (2.0f * kPi / 3.0f);
                Vec3 o = (side * std::cos(a) + up * std::sin(a)) * (r * 0.62f);
                fb.drawShadedSphere3D(pos + o, r * 0.48f, accentRgb, sunDir, 0.14f, 1.8f, 16.0f);
            }
            fb.drawShadedSphere3D(pos + d * (r * 0.2f), r * 0.20f, {1.0f, 0.97f, 0.85f}, sunDir, 0.2f, 1.2f, 6.0f);
            break;
        }
        case 3:
        case 6: {
            int count = (accentKindId == 6) ? 6 : 5;
            for (int k = 0; k < count; ++k) {
                float a = phase + static_cast<float>(k) * (2.0f * kPi / static_cast<float>(count));
                Vec3 pPos = pos + (side * std::cos(a) + up * std::sin(a)) * (r * 0.58f);
                RgbF pc = (k % 2 == 0) ? accentRgb : mixRgb(accentRgb, {1.0f, 1.0f, 1.0f}, 0.20f);
                fb.drawShadedSphere3D(pPos, r * 0.44f, pc, sunDir, 0.16f, 1.8f, 18.0f);
            }
            if (accentKindId == 6) {
                fb.drawShadedSphere3D(pos, r * 0.48f, accentRgb, sunDir, 0.16f, 2.0f, 18.0f);
            } else {
                Vec3 sTip = pos + d * (r * 0.72f);
                fb.drawAaLine3D(pos, sTip, isCloseup ? 2.0f : 1.2f, 255, 195, 65, 0.95f);
                fb.drawShadedSphere3D(sTip, r * 0.18f, {1.0f, 0.92f, 0.28f}, sunDir, 0.25f, 1.5f, 8.0f);
            }
            break;
        }
        case 4: {
            fb.drawShadedSphere3D(pos, r * 0.68f, accentRgb, sunDir, 0.22f, 2.2f, 16.0f);
            for (int k = 0; k < 6; ++k) {
                float a = phase + static_cast<float>(k) * (kPi / 3.0f);
                Vec3 fTip = pos + (side * std::cos(a) + up * std::sin(a)) * (r * 0.95f);
                fb.drawAaLine3D(pos, fTip, 1.1f, 255, 245, 150, 0.88f);
            }
            break;
        }
        case 5: {
            Vec3 sTip = pos + d * (r * 1.45f);
            fb.drawShadedCylinder3D(pos, sTip, r * 0.12f, r * 0.015f, accentRgb, sunDir, 0.95f, false, false);
            break;
        }
        case 7: {
            fb.drawShadedSphere3D(pos + d * (r * 0.55f), r * 0.45f, mixRgb(foliageRgb, accentRgb, 0.65f), sunDir, 0.2f, 2.0f, 16.0f);
            break;
        }
        case 8: {
            for (int b = 0; b < 6; ++b) {
                float a = phase + static_cast<float>(b) * (kPi / 3.0f);
                Vec3 bPos = pos + (side * std::cos(a) + up * std::sin(a)) * (r * 0.48f) + d * (r * 0.45f);
                fb.drawShadedSphere3D(bPos, r * 0.36f, accentRgb, sunDir, 0.2f, 2.0f, 16.0f);
            }
            fb.drawShadedSphere3D(pos + d * (r * 0.52f), r * 0.42f, {0.98f, 0.95f, 0.85f}, sunDir, 0.18f, 2.0f, 16.0f);
            break;
        }
        case 9: {
            fb.drawShadedCylinder3D(pos, pos + d * (r * 1.65f), r * 0.34f, r * 0.08f, accentRgb, sunDir, 0.98f, false, true);
            break;
        }
        case 10:
        default: {
            fb.drawShadedSphere3D(pos, r * 0.48f, accentRgb, sunDir, 0.22f, 1.8f, 14.0f);
            break;
        }
    }
}

void renderTreeReproductiveOrgans(
    DepthFrameBuffer& fb,
    const BotanicalTreeRenderParams& params,
    const std::vector<TreeBranchSegment>& branches,
    const std::vector<TreeLeafData>& rawLeaves,
    float heightSpan,
    const Vec3& sunDir,
    bool isCloseup
) {
    // 1. Seasonal 3D Blossoms anchored delicately along outer flowering shoots
    if (params.showBlossoms && params.bloomStyleId > 0 && !rawLeaves.empty()) {
        // Large showy corollas vs compact orchard florets vs ornamental panicles
        const bool isLargeShowyFlower = (params.bloomStyleId == 1 || params.bloomStyleId == 7 ||
                                         params.bloomStyleId == 11 || params.bloomStyleId == 18);
        const bool isPanicleOrBract = (params.bloomStyleId == 4 || params.bloomStyleId == 5 ||
                                       params.bloomStyleId == 6 || params.bloomStyleId == 8 ||
                                       params.bloomStyleId == 9 || params.bloomStyleId == 16 ||
                                       params.bloomStyleId == 17);
        float bScale = isLargeShowyFlower
            ? std::clamp(heightSpan * 0.0095f, 0.052f, 0.088f)
            : (isPanicleOrBract
                ? std::clamp(heightSpan * 0.0068f, 0.036f, 0.058f)
                : std::clamp(heightSpan * 0.0052f, 0.024f, 0.040f));

        // When ripe fruit is also active (e.g. Sweet Orange, Apple, Lemon), blossoms are sparse secondary accents
        // rather than dense white spheres obscuring the green foliage & ripe fruit!
        const bool hasActiveFruits = (params.showFruits && params.fruitStyleId > 0);
        int targetBlossoms = hasActiveFruits ? 14 : (isLargeShowyFlower ? 36 : 58);
        int stride = std::max(1, static_cast<int>(rawLeaves.size()) / std::max(1, targetBlossoms));
        size_t startIdx = hasActiveFruits ? std::min(rawLeaves.size() - 1, rawLeaves.size() / 6) : 0;

        for (size_t i = startIdx, placed = 0; i < rawLeaves.size() && placed < static_cast<size_t>(targetBlossoms); i += stride, ++placed) {
            const auto& l = rawLeaves[i];
            rasterizeTreeBlossom3D(
                fb,
                l.position + l.normal * (l.size * 0.22f),
                l.normal,
                bScale,
                params.bloomStyleId,
                params.flowerPrimary,
                params.flowerSecondary,
                sunDir,
                static_cast<uint32_t>(i),
                isCloseup
            );
        }
    }

    // 2. Biologically Accurate 3D Fruit Attachment & Silhouettes
    if (!params.showFruits || params.fruitStyleId <= 0 || branches.empty()) return;

    // 2a. BANANA_HANDS (4): Single heavy curved peduncle + tiered hands + maroon bell on upper pseudostem collar
    if (params.fruitStyleId == 4) {
        Vec3 bunchOrigin(0.0f, heightSpan * 0.48f, 0.0f);
        int stemCount = 0;
        for (const auto& b : branches) {
            if (b.recursionDepth == 0) stemCount++;
        }
        if (stemCount >= 2) {
            int targetCollar = std::max(1, stemCount - 2);
            int seen = 0;
            for (const auto& b : branches) {
                if (b.recursionDepth == 0) {
                    if (seen == targetCollar) {
                        bunchOrigin = b.endPos;
                        break;
                    }
                    seen++;
                }
            }
        }
        float bunchScale = std::clamp(heightSpan * 0.25f, 0.78f, 1.40f);
        float bunchAz = 0.78f;
        RgbF pedStemCol = mixRgb(params.barkColor, params.crownBaseColor, 0.35f);
        rasterizeBananaInfructescence3D(
            fb,
            bunchOrigin,
            bunchAz,
            bunchScale,
            params.fruitPrimary,
            params.fruitSecondary,
            pedStemCol,
            sunDir,
            isCloseup
        );
        return;
    }

    // 2b. PAPAYA_CLUSTERS (6): Dense spiral collar hugging the upper trunk (recursionDepth == 0)
    //     directly underneath the crown petioles — NEVER out along leaf petioles!
    if (params.fruitStyleId == 6) {
        std::vector<size_t> collarTrunkSegs;
        for (size_t i = 0; i < branches.size(); ++i) {
            const auto& b = branches[i];
            if (b.recursionDepth != 0) continue;
            float midY = 0.5f * (b.startPos.y + b.endPos.y);
            if (midY >= heightSpan * 0.40f && midY <= heightSpan * 0.68f) {
                collarTrunkSegs.push_back(i);
            }
        }
        if (collarTrunkSegs.empty()) {
            for (size_t i = 0; i < branches.size(); ++i) {
                if (branches[i].recursionDepth == 0 && branches[i].endPos.y >= heightSpan * 0.35f) {
                    collarTrunkSegs.push_back(i);
                }
            }
        }
        if (collarTrunkSegs.empty()) return;

        constexpr int kPapayaCount = 16;
        float baseSize = std::clamp(heightSpan * 0.032f, 0.095f, 0.175f);
        for (int p = 0; p < kPapayaCount; ++p) {
            float frac = static_cast<float>(p) / static_cast<float>(kPapayaCount - 1);
            size_t segIdx = collarTrunkSegs[std::min(collarTrunkSegs.size() - 1,
                static_cast<size_t>(frac * static_cast<float>(collarTrunkSegs.size())))];
            const auto& b = branches[segIdx];
            float tLocal = 0.20f + 0.65f * std::fmod(frac * static_cast<float>(collarTrunkSegs.size()), 1.0f);
            Vec3 axisCenter = lerp(b.startPos, b.endPos, tLocal);
            float rTrunk = lerp(b.radiusStart, b.radiusEnd, tLocal);
            float az = static_cast<float>(p) * kGoldenAngle;
            Vec3 radial(std::cos(az), 0.0f, std::sin(az));
            Vec3 attach = axisCenter + radial * (rTrunk * 0.94f);

            // Lower older papayas are larger and golden-ripe; upper younger ones smaller & greener
            float fruitR = baseSize * (1.12f - 0.34f * frac);
            RgbF ripePrimary = mixRgb(params.fruitPrimary, params.fruitSecondary, frac * 0.58f);
            rasterizeTreeFruit3D(
                fb,
                attach,
                radial,
                fruitR,
                6,
                ripePrimary,
                params.fruitSecondary,
                sunDir,
                static_cast<uint32_t>(p),
                isCloseup
            );
        }
        return;
    }

    // 2c. PALM CROWNS — COCONUT_CLUSTERS (2) & DATE_BUNCHES (3):
    //     Hung directly off the top trunk crown collar (recursionDepth == 0) below lowest frond sheaths!
    if (params.fruitStyleId == 2 || params.fruitStyleId == 3) {
        std::vector<size_t> crownTrunkSegs;
        for (size_t i = 0; i < branches.size(); ++i) {
            const auto& b = branches[i];
            if (b.recursionDepth == 0 && b.endPos.y >= heightSpan * 0.56f) {
                crownTrunkSegs.push_back(i);
            }
        }
        if (crownTrunkSegs.empty()) {
            for (size_t i = branches.size(); i > 0; --i) {
                if (branches[i - 1].recursionDepth == 0) {
                    crownTrunkSegs.push_back(i - 1);
                    break;
                }
            }
        }
        if (crownTrunkSegs.empty()) return;

        int clusterCount = (params.fruitStyleId == 2) ? 5 : 6;
        float fScale = std::clamp(heightSpan * 0.022f, 0.11f, 0.24f);
        for (int c = 0; c < clusterCount; ++c) {
            const auto& b = branches[crownTrunkSegs[static_cast<size_t>(c) % crownTrunkSegs.size()]];
            Vec3 collarPos = lerp(b.startPos, b.endPos, 0.85f);
            float rStem = lerp(b.radiusStart, b.radiusEnd, 0.85f);
            float az = static_cast<float>(c) * (2.0f * kPi / static_cast<float>(clusterCount)) + 0.4f;
            Vec3 radial(std::cos(az), 0.0f, std::sin(az));
            Vec3 attach = collarPos + radial * (rStem * 0.96f);
            rasterizeTreeFruit3D(
                fb,
                attach,
                radial,
                fScale,
                params.fruitStyleId,
                params.fruitPrimary,
                params.fruitSecondary,
                sunDir,
                static_cast<uint32_t>(c),
                isCloseup
            );
        }
        return;
    }

    // 2d. Cauliflorous Rainforest Trees (11 = Jackfruit, 18 = Cacao) vs Outer-Twig Fruiting Species
    const bool isCauliflorous = (params.fruitStyleId == 11 || params.fruitStyleId == 18);
    const bool isTinyOlive = (params.fruitStyleId == 1);
    const bool isOrchardGlobe = (params.fruitStyleId == 8 || params.fruitStyleId == 9 ||
                                 params.fruitStyleId == 12 || params.fruitStyleId == 13 ||
                                 params.fruitStyleId == 14 || params.fruitStyleId == 15 ||
                                 params.fruitStyleId == 16 || params.fruitStyleId == 17);
    int fruitLimit = isCauliflorous ? 12 : (isTinyOlive ? 34 : (isOrchardGlobe ? 30 : 22));
    float fScale;
    if (isCauliflorous) {
        fScale = std::clamp(heightSpan * 0.026f, 0.10f, 0.24f);
    } else if (params.fruitStyleId == 19) {
        fScale = std::clamp(heightSpan * 0.015f, 0.095f, 0.145f);
    } else if (isTinyOlive) {
        fScale = std::clamp(heightSpan * 0.0045f, 0.024f, 0.038f);
    } else if (isOrchardGlobe) {
        fScale = std::clamp(heightSpan * 0.0085f, 0.048f, 0.072f);
    } else if (params.fruitStyleId == 10 || params.fruitStyleId == 20) {
        fScale = std::clamp(heightSpan * 0.0075f, 0.042f, 0.062f);
    } else {
        // Mango, Avocado, Cashew
        fScale = std::clamp(heightSpan * 0.011f, 0.062f, 0.094f);
    }

    int minFruitDepth = 2;
    if (!isCauliflorous) {
        for (const auto& b : branches) {
            if (b.recursionDepth >= 3) {
                minFruitDepth = 3;
                break;
            }
        }
    }

    std::vector<size_t> candidateIndices;
    candidateIndices.reserve(branches.size());
    for (size_t i = 0; i < branches.size(); ++i) {
        const auto& b = branches[i];
        if (isCauliflorous) {
            if (b.recursionDepth > 1 || b.endPos.y < heightSpan * 0.22f) continue;
        } else {
            if (b.recursionDepth < minFruitDepth) continue;
        }
        candidateIndices.push_back(i);
    }

    int stride = std::max(1, static_cast<int>(candidateIndices.size()) / std::max(1, fruitLimit));
    int placed = 0;
    for (size_t k = 0; k < candidateIndices.size() && placed < fruitLimit; k += stride) {
        size_t idx = candidateIndices[k];
        const auto& b = branches[idx];
        float tPos = isCauliflorous ? 0.50f : 0.68f;
        Vec3 attach = lerp(b.startPos, b.endPos, tPos);
        if (isCauliflorous) {
            // Offset cauliflorous jackfruits & cacao pods directly onto the bark surface!
            float rBark = lerp(b.radiusStart, b.radiusEnd, tPos);
            float az = static_cast<float>(placed) * kGoldenAngle;
            Vec3 radial(std::cos(az), 0.0f, std::sin(az));
            attach = attach + radial * (rBark * 0.92f);
        }
        rasterizeTreeFruit3D(
            fb,
            attach,
            b.normal,
            fScale,
            params.fruitStyleId,
            params.fruitPrimary,
            params.fruitSecondary,
            sunDir,
            static_cast<uint32_t>(idx),
            isCloseup
        );
        placed++;
    }
}

} // namespace scene
} // namespace vegetation
