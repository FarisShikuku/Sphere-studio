// ============================================================================
// BotanicalLeafRasterizer.cpp
// Analytical Sub-Pixel 3D Leaf, Needle, Blossom, Fruit & Accent Rasterizer
// ============================================================================

#include "Botanical3DSceneRenderer.h"
#include <algorithm>
#include <cmath>

namespace vegetation {
namespace scene {

namespace {
constexpr float kPi = 3.14159265358979323846f;

inline float smoothstepF(float edge0, float edge1, float x) {
    float denom = edge1 - edge0;
    if (std::abs(denom) < 1e-6f) return (x >= edge1) ? 1.0f : 0.0f;
    float t = std::clamp((x - edge0) / denom, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline RgbF mixRgb(const RgbF& a, const RgbF& b, float t) {
    float u = std::clamp(t, 0.0f, 1.0f);
    return {a.r + (b.r - a.r) * u, a.g + (b.g - a.g) * u, a.b + (b.b - a.b) * u};
}

inline RgbF scaleRgb(const RgbF& a, float s) {
    return {std::clamp(a.r * s, 0.0f, 1.0f), std::clamp(a.g * s, 0.0f, 1.0f), std::clamp(a.b * s, 0.0f, 1.0f)};
}

struct LeafVertexUV {
    Vec3 worldPos;
    float s{0.0f}; // [0, 1] longitudinal from blade base to apex
    float t{0.0f}; // [-1, 1] lateral across blade width
};

struct LeafEvalResult {
    float alpha{0.0f};
    float veinMix{0.0f};    // [0..1] midrib / lateral vein highlight
    float rachisMix{0.0f};  // [0..1] woody stem/rachis blend
    float glandMix{0.0f};   // [0..1] petiole gland / rust / silver back sheen
    float foldShade{0.0f};  // [-1..+1] V-fold directional lighting shift
};

// Evaluate true botanical lamina SDF + venation in (s in [0,1], t in [-1,1])
inline LeafEvalResult evaluateBotanicalBlade(
    int morphId,
    float s,
    float t,
    float edgeBlur,
    bool isUnderside,
    bool hasSplitSeed
) {
    LeafEvalResult res;
    if (s < -0.15f || s > 1.08f || std::abs(t) > 1.12f) return res;

    // Gentle natural lateral curl / wave along midrib
    float sClamped = std::clamp(s, 0.0001f, 0.9999f);
    float absT = std::abs(t);
    float signedFold = (t >= 0.0f) ? 1.0f : -1.0f;

    switch (morphId) {
        case 5: {
            // FEATHERY_BIPINNATE (Acacia, Jacaranda, Flamboyant, Whistling Thorn, Yellow Fever Tree):
            // Delicate arched central rachis + 7 opposite pairs of spreading pinnae,
            // each bearing 8 pairs of tiny distinct oblong pinnules (micro-leaflets)!
            float rachisBend = 0.035f * std::sin(s * kPi);
            float distRachis = std::abs(t - rachisBend);
            float rachisHalfW = lerp(0.030f, 0.011f, sClamped);
            float rachisCov = (s >= 0.0f && s <= 0.97f)
                ? smoothstepF(rachisHalfW + edgeBlur * 0.7f, rachisHalfW - edgeBlur * 0.7f, distRachis)
                : 0.0f;

            float bestLeafCov = 0.0f;
            float bestPinnaStem = 0.0f;
            float localShade = signedFold * 0.14f;

            // 7 opposite pinna pairs along the primary rachis
            constexpr int kPinnaPairs = 7;
            for (int side = -1; side <= 1; side += 2) {
                float fSide = static_cast<float>(side);
                for (int p = 0; p < kPinnaPairs; ++p) {
                    float sAttach = 0.11f + static_cast<float>(p) * 0.125f;
                    // Asymmetric slight stagger feels organic
                    float sRoot = sAttach + (side > 0 ? 0.012f : -0.008f);
                    if (s < sRoot - 0.14f || s > sRoot + 0.36f) continue;

                    // Pinna directional vector (forward + outward lateral)
                    float dirS = 0.30f;
                    float dirT = fSide * 0.954f;
                    float pinnaLen = 0.94f * std::sin(kPi * std::pow(std::clamp((sAttach - 0.02f) / 0.96f, 0.05f, 0.95f), 0.68f));

                    float ds = s - sRoot;
                    float dt = t - (0.035f * std::sin(sRoot * kPi));
                    float uAlong = ds * dirS + dt * dirT;           // distance along pinna [0..pinnaLen]
                    float vPerp = -ds * dirT + dt * dirS;           // signed perp from pinna rachilla

                    if (uAlong < -0.02f || uAlong > pinnaLen + 0.05f) continue;

                    // Secondary rachilla mid-vein inside pinna
                    if (uAlong >= 0.0f && uAlong <= pinnaLen * 0.96f) {
                        float rCov = smoothstepF(0.013f + edgeBlur * 0.6f, 0.007f, std::abs(vPerp));
                        bestPinnaStem = std::max(bestPinnaStem, rCov);
                    }

                    // 7-9 pairs of fine distinct rounded-oblong pinnules per pinna
                    float stepP = 0.112f;
                    if (uAlong >= 0.05f && uAlong <= pinnaLen) {
                        float pPos = (uAlong - 0.05f) / stepP;
                        float pFrac = pPos - std::floor(pPos) - 0.5f;
                        // Outer pinnules slightly smaller near tip
                        float tipTaper = smoothstepF(pinnaLen + 0.02f, pinnaLen * 0.72f, uAlong);
                        float pLen = (0.044f + 0.014f * tipTaper);
                        float absV = std::abs(vPerp);
                        // Pinnules sit cleanly on both sides of the pinna rachilla, tilted forward
                        float vCenter = 0.028f * (0.65f + 0.35f * tipTaper);
                        float uSkew = pFrac * stepP - absV * 0.34f;
                        float normDist = std::sqrt((uSkew / (pLen * 0.48f)) * (uSkew / (pLen * 0.48f)) +
                                                   ((absV - vCenter) / (0.020f * (0.6f + 0.4f * tipTaper))) *
                                                   ((absV - vCenter) / (0.020f * (0.6f + 0.4f * tipTaper))));
                        float blur = std::max(0.18f, edgeBlur * 16.0f);
                        float leafletCov = smoothstepF(1.0f + blur, 1.0f - blur, normDist);
                        // Distance attenuation avoids turning tiny distant crowns into solid blobs while keeping recognizable lacy air gaps
                        bestLeafCov = std::max(bestLeafCov, leafletCov);
                        if (leafletCov > 0.2f) {
                            localShade = (vPerp >= 0.0f ? 0.16f : -0.10f) * tipTaper;
                        }
                    }
                }
            }

            // Soft terminal tuft at rachis apex
            if (s > 0.88f && s <= 1.01f && absT < 0.14f) {
                float dTip = std::hypot((s - 0.945f) / 0.065f, t / 0.09f);
                bestLeafCov = std::max(bestLeafCov, smoothstepF(1.0f + edgeBlur * 4.0f, 0.82f, dTip));
            }

            float stemCov = std::max(rachisCov, bestPinnaStem * 0.85f);
            res.alpha = std::clamp(std::max(bestLeafCov, stemCov), 0.0f, 1.0f);
            res.rachisMix = std::clamp(stemCov * 0.90f, 0.0f, 1.0f);
            res.foldShade = localShade;
            return res;
        }

        case 4: {
            // GINKGO_FLABELLATE_FAN: Cuneate wedge base opening into bilobed scalloped fan with central sinus
            if (s < 0.0f || s > 1.0f) return res;
            float r = std::hypot(s, t * 0.95f);
            float ang = std::atan2(t * 0.95f, std::max(0.01f, s));
            float maxAng = 0.86f;
            float angMask = smoothstepF(maxAng + edgeBlur, maxAng - edgeBlur, std::abs(ang));

            float cleft = 0.25f * std::exp(-ang * ang * 55.0f);
            float scallop = 0.025f * std::cos(ang * 14.0f);
            float outerR = 0.94f - cleft + scallop;
            float rMask = smoothstepF(outerR + edgeBlur, outerR - edgeBlur, r) * smoothstepF(-0.02f, 0.04f, r);

            res.alpha = angMask * rMask;
            // Dichotomous radiating veins
            float fanRibs = std::pow(0.5f + 0.5f * std::cos(ang * 28.0f), 6.0f);
            res.veinMix = fanRibs * 0.35f * smoothstepF(0.10f, 0.75f, r);
            res.foldShade = signedFold * 0.18f;
            return res;
        }

        case 6: {
            // OAK_SINUATE_LOBED: Classical smooth 7-lobe oak blade with deep rounded sinuses
            if (s < 0.0f || s > 1.0f) return res;
            float env = std::pow(std::sin(kPi * std::pow(sClamped, 0.76f)), 0.80f);
            float lobes = 0.62f + 0.36f * std::cos(6.0f * kPi * sClamped - 0.55f);
            // Smooth rounded apex lobe
            float halfW = env * lobes * 0.86f;
            float sdf = halfW - absT;
            res.alpha = smoothstepF(-edgeBlur, edgeBlur, sdf);

            float midrib = smoothstepF(0.045f * (1.0f - 0.65f * sClamped) + edgeBlur, 0.010f, absT) * smoothstepF(0.95f, 0.84f, sClamped);
            // Lateral veins curved toward lobe peaks
            float veinFreq = 6.0f * kPi * sClamped - absT * 3.8f - 0.55f;
            float latVein = std::pow(std::max(0.0f, std::cos(veinFreq * 0.5f)), 14.0f) * smoothstepF(0.03f, 0.18f, absT) * smoothstepF(halfW, halfW * 0.72f, absT);
            res.veinMix = std::clamp(midrib * 0.72f + latVein * 0.42f, 0.0f, 1.0f);
            res.foldShade = signedFold * (0.26f + 0.10f * std::sin(veinFreq));
            return res;
        }

        case 7: {
            // PALMATE_MAPLE_LOBED: 5 sharp palmate acuminate lobes with deep sinuses
            float su = s - 0.14f;
            float tu = t;
            float r = std::hypot(su, tu);
            float ang = std::atan2(tu, su);
            // 5 lobes at 0, +/-0.72, +/-1.44 rad; truncate cleanly behind basal sinus
            float backCutoff = smoothstepF(-0.18f, 0.06f, su + 0.14f * std::cos(ang * 2.0f));
            float lobeWaves = std::pow(0.5f + 0.5f * std::cos(ang * 4.35f), 1.7f);
            float tooth = 0.035f * std::sin(ang * 26.0f);
            float maxR = (0.30f + 0.60f * lobeWaves + tooth) * (1.0f - 0.16f * std::min(1.8f, std::abs(ang)));
            if (std::abs(ang) > 2.05f) maxR *= smoothstepF(2.55f, 2.05f, std::abs(ang));

            res.alpha = backCutoff * smoothstepF(maxR + edgeBlur, maxR - edgeBlur, r);
            // 5 radiating primary veins
            float pVein = 0.0f;
            for (int k = -2; k <= 2; ++k) {
                float targetAng = k * 0.72f;
                float dAng = std::abs(ang - targetAng) * std::max(0.15f, r);
                pVein = std::max(pVein, smoothstepF(0.032f + edgeBlur, 0.008f, dAng));
            }
            res.veinMix = pVein * 0.75f * smoothstepF(maxR * 0.94f, maxR * 0.45f, r);
            res.foldShade = signedFold * 0.22f + 0.10f * std::sin(ang * 4.35f);
            return res;
        }

        case 15: {
            // PAPAYA_PALMATIFID_STAR: 7 deeply incised star-shaped palmatifid lobes
            float su = s - 0.26f;
            float tu = t * 1.05f;
            float r = std::hypot(su, tu);
            float ang = std::atan2(tu, su);
            float lobeWaves = std::pow(0.5f + 0.5f * std::cos(ang * 3.5f), 2.1f);
            float subTeeth = 0.045f * std::sin(ang * 17.5f);
            float maxR = 0.24f + 0.58f * lobeWaves + subTeeth;
            res.alpha = smoothstepF(maxR + edgeBlur, maxR - edgeBlur, r);
            float pVein = std::pow(0.5f + 0.5f * std::cos(ang * 3.5f), 16.0f);
            res.veinMix = pVein * 0.65f * smoothstepF(maxR * 0.92f, 0.05f, r);
            res.foldShade = signedFold * 0.20f;
            return res;
        }

        case 16: {
            // BAOBAB_DIGITATE_COMPOUND: 5 distinct obovate leaflets radiating from single petiole apex
            float bestCov = 0.0f;
            float bestVein = 0.0f;
            float bestShade = 0.0f;
            constexpr float angles[5] = {-1.08f, -0.54f, 0.0f, 0.54f, 1.08f};
            constexpr float lengths[5] = {0.58f, 0.78f, 0.96f, 0.78f, 0.58f};
            for (int k = 0; k < 5; ++k) {
                float ca = std::cos(angles[k]);
                float sa = std::sin(angles[k]);
                float ls = (s - 0.03f) * ca + t * sa;
                float lt = -(s - 0.03f) * sa + t * ca;
                float len = lengths[k];
                if (ls >= -0.02f && ls <= len + 0.04f) {
                    float u = std::clamp(ls / len, 0.001f, 0.999f);
                    // Obovate leaflet shape (wider towards 0.65)
                    float wMax = len * 0.24f * std::sin(kPi * std::pow(u, 1.25f));
                    float cov = smoothstepF(wMax + edgeBlur, wMax - edgeBlur, std::abs(lt));
                    if (cov > bestCov) {
                        bestCov = cov;
                        bestVein = smoothstepF(0.022f + edgeBlur, 0.005f, std::abs(lt)) * 0.65f;
                        bestShade = (lt >= 0.0f ? 0.20f : -0.18f);
                    }
                }
            }
            res.alpha = bestCov;
            res.veinMix = bestVein;
            res.foldShade = bestShade;
            return res;
        }

        case 17: {
            // PALM_PINNATE_FROND: Arching central woody rachis with 18 pairs of slender sword pinnae
            float rachisCurve = 0.045f * std::sin(s * kPi);
            float rachisW = lerp(0.042f, 0.012f, sClamped);
            float rachisCov = (s >= 0.0f && s <= 0.98f)
                ? smoothstepF(rachisW + edgeBlur, rachisW - edgeBlur, std::abs(t - rachisCurve))
                : 0.0f;

            float bestBlade = 0.0f;
            float fold = signedFold * 0.24f;
            // Periodic pinna ribbons radiating out on both sides
            if (s >= 0.06f && s <= 0.99f) {
                float span = 0.95f * std::pow(std::sin(kPi * std::pow(sClamped, 0.72f)), 0.75f);
                if (absT <= span + edgeBlur && absT >= rachisW * 0.5f) {
                    // Angled sweep forward along frond
                    float pinnaCoord = (s - absT * 0.38f) * 16.0f;
                    float frac = std::abs((pinnaCoord - std::floor(pinnaCoord)) - 0.5f);
                    // Narrow tapering ribbon per pinna
                    float pinnaWidth = 0.36f * (1.0f - 0.55f * (absT / std::max(0.15f, span)));
                    float bladeMask = smoothstepF(pinnaWidth + edgeBlur * 5.0f, pinnaWidth - edgeBlur * 4.0f, frac)
                                    * smoothstepF(span + edgeBlur, span - edgeBlur, absT);
                    bestBlade = bladeMask;
                    fold = (frac < pinnaWidth * 0.5f) ? 0.22f : -0.20f;
                }
            }
            res.alpha = std::clamp(std::max(rachisCov, bestBlade), 0.0f, 1.0f);
            res.rachisMix = rachisCov * 0.85f;
            res.foldShade = fold;
            return res;
        }

        case 18: {
            // DOUM_COSTAPALMATE_FAN: Rigid pleated circular fan split radially near periphery
            float su = s - 0.08f;
            float r = std::hypot(su, t);
            float ang = std::atan2(t, std::max(0.01f, su));
            float maxAng = 1.25f;
            float angMask = smoothstepF(maxAng + edgeBlur, maxAng - edgeBlur, std::abs(ang));
            float outerR = 0.90f * (1.0f - 0.14f * (std::abs(ang) / maxAng));
            float splitDepth = 0.50f;
            // Radial sword pleats: beyond splitDepth, gaps widen into points
            float rayPhase = std::abs(std::sin(ang * 11.0f));
            float rayThreshold = (r > splitDepth) ? lerp(0.15f, 0.82f, (r - splitDepth) / std::max(0.1f, outerR - splitDepth)) : 0.0f;
            float rayCov = smoothstepF(rayThreshold - edgeBlur * 3.0f, rayThreshold + edgeBlur * 3.0f, rayPhase);
            res.alpha = angMask * smoothstepF(outerR + edgeBlur, outerR - edgeBlur, r) * rayCov;
            // Strong accordion pleat shading
            res.foldShade = std::sin(ang * 22.0f) * 0.35f;
            res.veinMix = smoothstepF(0.28f, 0.05f, r) * 0.6f; // Costa center
            return res;
        }

        case 14: {
            // BANANA_PADDLE_MEGA: Rounded-cuneate oblong lamina, thick midrib, fine oblique transverse veins,
            // and natural vein-parallel wind splits on mature outer blades only (hasSplitSeed)
            if (s < 0.0f || s > 1.0f) return res;
            float env = std::pow(std::sin(kPi * std::pow(sClamped, 0.78f)), 0.44f)
                      * (0.86f + 0.22f * sClamped * (1.0f - sClamped) * 4.0f);
            float halfW = std::min(0.95f, env * 0.86f);
            float sdf = halfW - absT;

            // Organic wind-split slits parallel to transverse veins on mature outer blades only
            float splitGap = 0.0f;
            if (hasSplitSeed && absT > 0.14f && halfW > 0.28f) {
                float sVein = sClamped - absT * 0.16f;
                float slitBand1 = std::abs(sVein - 0.34f);
                float slitBand2 = std::abs(sVein - 0.66f);
                float slitBand3 = std::abs(sVein - 0.49f);
                float marginWiden = smoothstepF(0.14f, 0.78f, absT);
                if (t > 0.0f && slitBand1 < 0.018f) {
                    splitGap = smoothstepF(0.018f, 0.005f, slitBand1) * marginWiden;
                } else if (t > 0.0f && slitBand2 < 0.015f) {
                    splitGap = smoothstepF(0.015f, 0.004f, slitBand2) * marginWiden;
                } else if (t < 0.0f && slitBand3 < 0.017f) {
                    splitGap = smoothstepF(0.017f, 0.005f, slitBand3) * marginWiden;
                }
            }

            res.alpha = smoothstepF(-edgeBlur, edgeBlur, sdf) * (1.0f - splitGap);
            float midrib = smoothstepF(0.066f * (1.0f - 0.56f * sClamped) + edgeBlur, 0.014f, absT);
            float transverse = std::pow(0.5f + 0.5f * std::sin(sClamped * 58.0f - absT * 10.0f), 10.0f) * 0.20f;
            res.veinMix = std::clamp(midrib * 0.85f + transverse, 0.0f, 1.0f);
            res.foldShade = signedFold * 0.30f;
            return res;
        }

        case 9:
        case 20: {
            // CORDATE_BIRCH (9) & FIG_CORDATE_WAXY (20): Heart-shaped basal auricles + acuminate drip tip
            float basalNotch = 0.13f * std::sin(std::clamp(absT * 2.4f, 0.0f, kPi));
            float sShift = std::clamp((s + basalNotch) / (1.0f + basalNotch), 0.0001f, 0.9999f);
            if (s + basalNotch < -0.02f || s > 1.0f) return res;
            float baseW = std::pow(std::sin(kPi * std::pow(sShift, 0.56f)), 0.72f) * (1.06f - 0.24f * sShift);
            float serration = (morphId == 9) ? (0.026f * std::sin(sShift * 42.0f) * smoothstepF(0.15f, 0.85f, sShift)) : 0.0f;
            float halfW = std::max(0.0f, (baseW + serration) * 0.78f);
            res.alpha = smoothstepF(-edgeBlur, edgeBlur, halfW - absT) * smoothstepF(1.01f, 0.98f, s);

            float midrib = smoothstepF(0.036f * (1.0f - 0.6f * sShift) + edgeBlur, 0.008f, absT);
            float vDist = std::abs(absT - (sShift - 0.04f) * 0.72f);
            float actinodromous = (morphId == 20) ? smoothstepF(0.028f + edgeBlur, 0.006f, vDist) * smoothstepF(0.65f, 0.25f, sShift) * 0.65f : 0.0f;
            float herringbone = std::pow(std::max(0.0f, std::cos((sShift * 14.0f - absT * 6.5f) * kPi)), 12.0f) * smoothstepF(halfW, halfW * 0.5f, absT) * 0.34f;
            res.veinMix = std::clamp(midrib * 0.70f + actinodromous + herringbone, 0.0f, 1.0f);
            res.foldShade = signedFold * 0.22f;
            return res;
        }

        case 10:
        case 11: {
            // LANCEOLATE_WILLOW (10) & OBLONG_LEATHERY_OLIVE (11): Slender curved lance blade
            if (s < 0.0f || s > 1.0f) return res;
            float bowOffset = (morphId == 10) ? (0.06f * std::sin(s * kPi)) : 0.0f;
            float halfW = ((morphId == 10) ? 0.32f : 0.36f) * std::sin(kPi * std::pow(sClamped, 0.84f)) * (1.04f - 0.16f * sClamped);
            res.alpha = smoothstepF(-edgeBlur, edgeBlur, halfW - std::abs(t - bowOffset));
            res.veinMix = smoothstepF(0.032f + edgeBlur, 0.008f, std::abs(t - bowOffset)) * 0.72f;
            res.foldShade = ((t - bowOffset) >= 0.0f ? 0.22f : -0.22f);
            if (morphId == 11 && isUnderside) res.glandMix = 0.65f; // Silvery abaxial olive sheen
            return res;
        }

        case 19:
        case 22: {
            // MANGROVE_MUCRONATE_OBOVATE (19) & MAGNOLIA_GLOSSY_ELLIPTIC (22):
            // Broad obovate-elliptic blade widest above middle, rusty underside on Magnolia
            if (s < 0.0f || s > 1.0f) return res;
            float halfW = 0.62f * std::sin(kPi * std::pow(sClamped, 1.18f)) * (0.92f + 0.18f * sClamped);
            if (morphId == 19 && s > 0.92f) {
                // Sharp mucro spine tip
                halfW = std::max(halfW, 0.04f * (1.0f - (s - 0.92f) / 0.08f));
            }
            res.alpha = smoothstepF(-edgeBlur, edgeBlur, halfW - absT);
            float midrib = smoothstepF(0.044f * (1.0f - 0.5f * sClamped) + edgeBlur, 0.010f, absT);
            res.veinMix = midrib * 0.78f;
            res.foldShade = signedFold * 0.24f;
            if (morphId == 22 && isUnderside) res.glandMix = 0.80f; // Velvety cinnamon-rust underside
            return res;
        }

        case 8:
        case 12:
        case 13:
        default: {
            // OVATE_SERRATED (8), CHESTNUT_LANCE_SERRATE (12), CHERRY_ELLIPTIC (13)
            if (s < 0.0f || s > 1.0f) return res;
            float baseShape = (morphId == 12)
                ? 0.46f * std::sin(kPi * std::pow(sClamped, 0.78f))
                : 0.58f * std::sin(kPi * std::pow(sClamped, 0.70f)) * (1.06f - 0.18f * sClamped);
            float teethFreq = (morphId == 12) ? 28.0f : 38.0f;
            float teethAmp = (morphId == 12) ? 0.042f : 0.026f;
            float teeth = teethAmp * std::sin(sClamped * teethFreq) * smoothstepF(0.08f, 0.25f, sClamped) * smoothstepF(0.98f, 0.85f, sClamped);
            float halfW = std::max(0.0f, baseShape + teeth);

            res.alpha = smoothstepF(-edgeBlur, edgeBlur, halfW - absT);
            float midrib = smoothstepF(0.038f * (1.0f - 0.6f * sClamped) + edgeBlur, 0.008f, absT);
            float latVein = std::pow(std::max(0.0f, std::cos((sClamped * 12.0f - absT * 6.5f) * kPi)), 12.0f)
                          * smoothstepF(halfW, halfW * 0.5f, absT) * smoothstepF(0.04f, 0.15f, absT) * 0.36f;
            res.veinMix = std::clamp(midrib * 0.72f + latVein, 0.0f, 1.0f);
            res.foldShade = signedFold * 0.22f;
            if (morphId == 13 && s < 0.08f && absT < 0.09f && absT > 0.025f) {
                res.glandMix = 0.55f; // Paired cherry petiole amber glands
            }
            return res;
        }
    }
}

inline void rasterizeLeafTriangle(
    DepthFrameBuffer& fb,
    const ScreenProj& p0, const LeafVertexUV& v0,
    const ScreenProj& p1, const LeafVertexUV& v1,
    const ScreenProj& p2, const LeafVertexUV& v2,
    int morphId,
    const RgbF& leafLitCol,
    const RgbF& veinCol,
    const RgbF& stemCol,
    const RgbF& rustBackCol,
    bool isUnderside,
    bool hasSplitSeed,
    float alphaScale,
    bool isWireframe
) {
    if (!p0.valid || !p1.valid || !p2.valid) return;

    float area = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
    if (std::abs(area) < 0.25f) return;
    float invArea = 1.0f / area;

    int minX = std::max(0, static_cast<int>(std::floor(std::min({p0.x, p1.x, p2.x}))));
    int maxX = std::min(fb.width() - 1, static_cast<int>(std::ceil(std::max({p0.x, p1.x, p2.x}))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({p0.y, p1.y, p2.y}))));
    int maxY = std::min(fb.height() - 1, static_cast<int>(std::ceil(std::max({p0.y, p1.y, p2.y}))));
    if (minX > maxX || minY > maxY) return;

    float invZ0 = 1.0f / p0.depth;
    float invZ1 = 1.0f / p1.depth;
    float invZ2 = 1.0f / p2.depth;

    // Approximate UV step per pixel for screen-space anti-aliasing
    float dS_dx = std::abs(((v1.s - v0.s) * (p2.y - p0.y) - (v2.s - v0.s) * (p1.y - p0.y)) * invArea);
    float dS_dy = std::abs(((v2.s - v0.s) * (p1.x - p0.x) - (v1.s - v0.s) * (p2.x - p0.x)) * invArea);
    float dT_dx = std::abs(((v1.t - v0.t) * (p2.y - p0.y) - (v2.t - v0.t) * (p1.y - p0.y)) * invArea);
    float dT_dy = std::abs(((v2.t - v0.t) * (p1.x - p0.x) - (v1.t - v0.t) * (p2.x - p0.x)) * invArea);
    float edgeBlur = std::clamp(std::max(dS_dx + dS_dy, dT_dx + dT_dy) * 0.72f, 0.008f, 0.14f);

    if (isWireframe) {
        fb.drawAaLine3D(v0.worldPos, v1.worldPos, 0.9f, 50, 210, 150, 0.45f, true, 0.01f);
        fb.drawAaLine3D(v1.worldPos, v2.worldPos, 0.9f, 50, 210, 150, 0.45f, true, 0.01f);
        return;
    }

    for (int py = minY; py <= maxY; ++py) {
        float y = static_cast<float>(py) + 0.5f;
        for (int px = minX; px <= maxX; ++px) {
            float x = static_cast<float>(px) + 0.5f;
            float w0 = ((p1.x - x) * (p2.y - y) - (p1.y - y) * (p2.x - x)) * invArea;
            float w1 = ((p2.x - x) * (p0.y - y) - (p2.y - y) * (p0.x - x)) * invArea;
            float w2 = 1.0f - w0 - w1;

            if (w0 < -0.002f || w1 < -0.002f || w2 < -0.002f) continue;

            float invZ = w0 * invZ0 + w1 * invZ1 + w2 * invZ2;
            if (invZ <= 1e-5f) continue;
            float depth = 1.0f / invZ;

            // Early depth reject (branches in front stay in front!)
            if (depth > fb.getDepth(px, py) + 0.012f) continue;

            float s = (w0 * v0.s * invZ0 + w1 * v1.s * invZ1 + w2 * v2.s * invZ2) * depth;
            float t = (w0 * v0.t * invZ0 + w1 * v1.t * invZ1 + w2 * v2.t * invZ2) * depth;

            LeafEvalResult ev = evaluateBotanicalBlade(morphId, s, t, edgeBlur, isUnderside, hasSplitSeed);
            float cov = ev.alpha * alphaScale;
            if (cov < 0.03f) continue;

            // V-fold half-blade shading + venation + subtle woody rachis
            float foldMul = std::clamp(1.0f + ev.foldShade * 0.24f, 0.72f, 1.24f);
            RgbF col = scaleRgb(leafLitCol, foldMul);

            // Subtle margin darkening gives organic crispness without heavy cartoon black outlines
            float rimShade = lerp(0.88f, 1.02f, smoothstepF(0.25f, 0.90f, ev.alpha));
            col = scaleRgb(col, rimShade);

            if (ev.glandMix > 0.01f) {
                col = mixRgb(col, rustBackCol, ev.glandMix * 0.75f);
            }
            if (ev.veinMix > 0.01f) {
                col = mixRgb(col, veinCol, ev.veinMix * 0.65f);
            }
            if (ev.rachisMix > 0.01f) {
                col = mixRgb(col, stemCol, ev.rachisMix * 0.80f);
            }

            bool writeZ = (cov > 0.78f);
            fb.blendPixel(
                px, py, depth,
                DepthFrameBuffer::toByte(col.r),
                DepthFrameBuffer::toByte(col.g),
                DepthFrameBuffer::toByte(col.b),
                cov,
                writeZ
            );
        }
    }
}

void drawConiferOrSucculentLeaf(
    DepthFrameBuffer& fb,
    const LeafRenderItem& leaf,
    int morphId,
    const RgbF& baseColor,
    const RgbF& sunlitColor,
    const RgbF& barkColor,
    const Vec3& sunDir,
    float foliageAlphaScale,
    bool isCloseup,
    bool isWireframe
) {
    if (foliageAlphaScale <= 0.03f) return;
    Vec3 dir = leaf.midribDir.normalized();
    Vec3 wing = leaf.wingDir.normalized();
    Vec3 norm = leaf.normalDir.normalized();

    float sunFactor = std::clamp(leaf.sunlitFactor * (0.55f + 0.45f * std::abs(dot(norm, sunDir))), 0.15f, 1.0f);
    RgbF nCol = mixRgb(baseColor, sunlitColor, sunFactor);
    RgbF tipCol = scaleRgb(sunlitColor, 1.12f);

    if (morphId == 21) {
        // EUPHORBIA_SUCCULENT_WING: 4-winged succulent spine & scalloped rib flare along arm
        float segLen = std::max(0.08f, leaf.lengthMeters * 0.85f);
        Vec3 pEnd = leaf.twigAttach + dir * segLen;
        RgbF cactusCol = mixRgb(nCol, {0.26f, 0.52f, 0.24f}, 0.45f);
        fb.drawShadedCylinder3D(leaf.twigAttach, pEnd, segLen * 0.16f, segLen * 0.13f, cactusCol, sunDir, 0.85f, isWireframe, false);
        // Paired short stipular spines
        Vec3 s1 = leaf.twigAttach + dir * (segLen * 0.6f) + wing * (segLen * 0.28f);
        Vec3 s2 = leaf.twigAttach + dir * (segLen * 0.6f) - wing * (segLen * 0.28f);
        fb.drawAaLine3D(leaf.twigAttach + dir * (segLen * 0.55f), s1, isCloseup ? 1.8f : 1.0f, 185, 165, 120, 0.85f * foliageAlphaScale);
        fb.drawAaLine3D(leaf.twigAttach + dir * (segLen * 0.55f), s2, isCloseup ? 1.8f : 1.0f, 185, 165, 120, 0.85f * foliageAlphaScale);
        return;
    }

    // Pine / Spruce / Larch / Sequoia needles
    int count = (morphId == 0) ? 2 : (morphId == 1) ? 5 : (morphId == 2) ? 7 : 4;
    float spread = (morphId == 0) ? 0.26f : (morphId == 2) ? 0.58f : 0.46f;
    float needleLen = leaf.lengthMeters * ((morphId == 0) ? 1.45f : (morphId == 3) ? 0.72f : 1.15f);

    // Subtle basal woody peg sheath
    Vec3 sheathEnd = leaf.twigAttach + dir * (needleLen * 0.14f);
    fb.drawAaLine3D(leaf.twigAttach, sheathEnd, isCloseup ? 2.2f : 1.2f,
                    DepthFrameBuffer::toByte(barkColor.r), DepthFrameBuffer::toByte(barkColor.g), DepthFrameBuffer::toByte(barkColor.b),
                    0.90f * foliageAlphaScale);

    for (int k = 0; k < count; ++k) {
        float frac = (count == 1) ? 0.0f : (static_cast<float>(k) / static_cast<float>(count - 1)) * 2.0f - 1.0f;
        float lift = std::cos(frac * 1.4f) * 0.24f;
        Vec3 nDir = (dir + wing * (frac * spread) + norm * lift).normalized();
        Vec3 nTip = sheathEnd + nDir * needleLen;
        RgbF c = mixRgb(nCol, tipCol, 0.35f + 0.35f * (1.0f - std::abs(frac)));
        float pxW = isCloseup ? 2.1f : ((morphId == 3) ? 1.9f : 1.35f);
        fb.drawAaLine3D(sheathEnd, nTip, pxW,
                        DepthFrameBuffer::toByte(c.r), DepthFrameBuffer::toByte(c.g), DepthFrameBuffer::toByte(c.b),
                        0.92f * foliageAlphaScale);
    }
}

} // namespace

void rasterizeBotanicalLeaf(
    DepthFrameBuffer& fb,
    const LeafRenderItem& leaf,
    int leafMorphologyId,
    const RgbF& baseColor,
    const RgbF& sunlitColor,
    const RgbF& stemColor,
    const Vec3& sunDir,
    float foliageAlphaScale,
    bool isCloseup,
    bool isWireframe
) {
    if (foliageAlphaScale <= 0.03f) return;

    // 1. Conifer Needles & Succulent Wings
    if (leafMorphologyId <= 3 || leafMorphologyId == 21) {
        drawConiferOrSucculentLeaf(
            fb, leaf, leafMorphologyId, baseColor, sunlitColor, stemColor,
            sunDir, foliageAlphaScale, isCloseup, isWireframe
        );
        return;
    }

    Vec3 M = leaf.midribDir.normalized();
    Vec3 S = leaf.wingDir.normalized();
    Vec3 N = leaf.normalDir.normalized();

    // 2. Slender green-olive woody petiole connecting twig node -> blade base ONLY (never sticking out!)
    bool hasBranchPetiole = (leafMorphologyId == 14 || leafMorphologyId == 15 || leafMorphologyId == 17 || leafMorphologyId == 18);
    if (!hasBranchPetiole && leaf.petioleMeters > 0.005f) {
        RgbF petioleCol = mixRgb(stemColor, baseColor, 0.45f);
        float pThick = isCloseup ? 1.9f : 1.05f;
        fb.drawAaLine3D(
            leaf.twigAttach,
            leaf.bladeBase,
            pThick,
            DepthFrameBuffer::toByte(petioleCol.r),
            DepthFrameBuffer::toByte(petioleCol.g),
            DepthFrameBuffer::toByte(petioleCol.b),
            0.94f * foliageAlphaScale
        );
    }

    // 3. Aspect ratio width/length tuned authentically per morphology
    float widthRatio = 0.56f;
    switch (leafMorphologyId) {
        case 4:  widthRatio = 0.96f; break; // Ginkgo fan
        case 5:  widthRatio = 0.92f; break; // Feathery Bipinnate compound pinnae
        case 6:  widthRatio = 0.72f; break; // Oak lobed
        case 7:  widthRatio = 0.96f; break; // Maple palmate
        case 8:  widthRatio = 0.62f; break; // Ovate serrated
        case 9:  widthRatio = 0.78f; break; // Cordate birch
        case 10: widthRatio = 0.34f; break; // Willow lance
        case 11: widthRatio = 0.36f; break; // Olive oblong
        case 12: widthRatio = 0.46f; break; // Chestnut lance
        case 13: widthRatio = 0.58f; break; // Cherry elliptic
        case 14: widthRatio = 0.42f; break; // Banana giant paddle
        case 15: widthRatio = 0.98f; break; // Papaya / Passionfruit star
        case 16: widthRatio = 0.96f; break; // Baobab digitate compound
        case 17: widthRatio = 0.46f; break; // Palm pinnate frond
        case 18: widthRatio = 0.95f; break; // Doum fan
        case 19: widthRatio = 0.56f; break; // Mangrove obovate
        case 20: widthRatio = 0.80f; break; // Fig cordate waxy
        case 22: widthRatio = 0.60f; break; // Magnolia obovate
        default: break;
    }

    float L = leaf.lengthMeters;
    float halfW = L * widthRatio * 0.5f;

    // Back-face lighting & natural foliage hue variation
    Vec3 viewToLeaf = (leaf.bladeBase - fb.camPos()).normalized();
    float ndotV = dot(N, viewToLeaf);
    bool isUnderside = (ndotV > 0.0f);
    Vec3 shadedN = isUnderside ? (N * -1.0f) : N;

    float nDotS = dot(shadedN, sunDir);
    float diffuse = std::clamp(nDotS, 0.0f, 1.0f);
    float translucent = std::clamp(-nDotS, 0.0f, 1.0f) * 0.42f; // Backlit chlorophyll glow!
    float light = std::clamp(0.32f + diffuse * 0.68f + translucent, 0.24f, 1.15f) * (0.72f + 0.35f * leaf.sunlitFactor);

    RgbF leafCol = mixRgb(baseColor, sunlitColor, std::clamp(light * 0.72f + (leaf.tintVariation - 0.5f) * 0.18f, 0.0f, 1.0f));
    leafCol = scaleRgb(leafCol, std::clamp(0.75f + 0.35f * light, 0.65f, 1.12f));

    RgbF veinCol = mixRgb(sunlitColor, {0.82f, 0.92f, 0.52f}, 0.48f);
    RgbF rustBackCol = (leafMorphologyId == 22)
        ? RgbF{0.54f, 0.33f, 0.18f} // Velvety rusty Magnolia underside
        : mixRgb(leafCol, {0.66f, 0.74f, 0.64f}, 0.52f); // Silvery Olive/Willow underside

    bool splitSeed = (leafMorphologyId == 14) ? leaf.allowSplits : ((leaf.indexSeed & 1u) != 0u);
    float effAlpha = std::clamp(foliageAlphaScale * 0.96f, 0.0f, 1.0f);

    if (leafMorphologyId == 14) {
        // Multi-segment parabolic arched + 3D V-trough mesh for giant Musa paddle blades:
        // joins flush with petiole tangent at s=0, arches smoothly downward, and eliminates flat-cardboard creases
        constexpr int kSegs = 6;
        LeafVertexUV ringL[kSegs + 1], ringC[kSegs + 1], ringR[kSegs + 1];
        ScreenProj projL[kSegs + 1], projC[kSegs + 1], projR[kSegs + 1];

        for (int i = 0; i <= kSegs; ++i) {
            float s = static_cast<float>(i) / static_cast<float>(kSegs);
            // Smooth forward reach with quadratic gravity arch starting tangentially from M at s=0
            Vec3 archPos = leaf.bladeBase
                + M * (L * (s - 0.08f * s * s))
                + N * (0.045f * L * std::sin(kPi * s))
                + Vec3(0.0f, -0.24f * L * s * s, 0.0f);
            float troughDepth = 0.032f * halfW * std::sin(kPi * s);
            float wingLift = 0.095f * halfW * std::sin(kPi * s);

            Vec3 centerPos = archPos - N * troughDepth;
            Vec3 leftPos = archPos - S * halfW + N * wingLift;
            Vec3 rightPos = archPos + S * halfW + N * wingLift;

            ringL[i] = {leftPos, s, -1.0f};
            ringC[i] = {centerPos, s, 0.0f};
            ringR[i] = {rightPos, s, 1.0f};

            projL[i] = fb.project(leftPos, 0.20f);
            projC[i] = fb.project(centerPos, 0.20f);
            projR[i] = fb.project(rightPos, 0.20f);
        }

        for (int i = 0; i < kSegs; ++i) {
            if (projL[i].valid && projC[i].valid && projL[i + 1].valid && projC[i + 1].valid) {
                rasterizeLeafTriangle(fb, projL[i], ringL[i], projC[i], ringC[i], projC[i + 1], ringC[i + 1],
                                      leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
                rasterizeLeafTriangle(fb, projL[i], ringL[i], projC[i + 1], ringC[i + 1], projL[i + 1], ringL[i + 1],
                                      leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
            }
            if (projC[i].valid && projR[i].valid && projC[i + 1].valid && projR[i + 1].valid) {
                rasterizeLeafTriangle(fb, projC[i], ringC[i], projR[i], ringR[i], projR[i + 1], ringR[i + 1],
                                      leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
                rasterizeLeafTriangle(fb, projC[i], ringC[i], projR[i + 1], ringR[i + 1], projC[i + 1], ringC[i + 1],
                                      leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
            }
        }
        return;
    }

    // Two-quad slightly arched leaf mesh (base -> midhinge (+camber) -> apex (-droop))
    Vec3 camber = N * (0.07f * L);
    Vec3 droop = N * (-0.08f * L) + Vec3(0.0f, -0.05f * L, 0.0f);

    Vec3 midCenter = leaf.bladeBase + M * (0.50f * L) + camber;
    Vec3 apexCenter = leaf.bladeBase + M * L + droop;

    LeafVertexUV vBL{leaf.bladeBase - S * halfW, 0.0f, -1.0f};
    LeafVertexUV vBR{leaf.bladeBase + S * halfW, 0.0f,  1.0f};
    LeafVertexUV vML{midCenter - S * halfW, 0.50f, -1.0f};
    LeafVertexUV vMR{midCenter + S * halfW, 0.50f,  1.0f};
    LeafVertexUV vTL{apexCenter - S * halfW, 1.0f, -1.0f};
    LeafVertexUV vTR{apexCenter + S * halfW, 1.0f,  1.0f};

    ScreenProj sBL = fb.project(vBL.worldPos, 0.20f);
    ScreenProj sBR = fb.project(vBR.worldPos, 0.20f);
    ScreenProj sML = fb.project(vML.worldPos, 0.20f);
    ScreenProj sMR = fb.project(vMR.worldPos, 0.20f);
    ScreenProj sTL = fb.project(vTL.worldPos, 0.20f);
    ScreenProj sTR = fb.project(vTR.worldPos, 0.20f);

    // Quick visibility cull
    if (sBL.valid && sBR.valid && sML.valid && sMR.valid) {
        rasterizeLeafTriangle(fb, sBL, vBL, sBR, vBR, sMR, vMR, leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
        rasterizeLeafTriangle(fb, sBL, vBL, sMR, vMR, sML, vML, leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
    }
    if (sML.valid && sMR.valid && sTL.valid && sTR.valid) {
        rasterizeLeafTriangle(fb, sML, vML, sMR, vMR, sTR, vTR, leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
        rasterizeLeafTriangle(fb, sML, vML, sTR, vTR, sTL, vTL, leafMorphologyId, leafCol, veinCol, stemColor, rustBackCol, isUnderside, splitSeed, effAlpha, isWireframe);
    }
}

} // namespace scene
} // namespace vegetation
